#pragma once
#include <atomic>
#include <mutex>
#include <semaphore>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>
#include "CpuUtilities.h"
#include "RunResultKind.h"
#include "RunResultUtilities.h"
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Utilities
{
    template <typename function_t = void(*)(), typename ... arguments_t>
    class task_runner final
    {
        task_runner() = delete;

    public:
        using result_vector_t = std::vector<std::tuple<size_t, RunResultKind, std::string>>;

        //There must be at least one item in "functions".
        static result_vector_t execute(std::counting_semaphore<>* sem,
            size_t cpuCount, bool& has_unknown_error,
            const std::vector<function_t>& functions, arguments_t ... arguments)
        {
            has_unknown_error = false;
            ThrowIfEmpty(functions, "functions");

            const auto size = functions.size();
            const auto actual_core_count = get_core_count(size, cpuCount);
            const auto is_single = actual_core_count <= 1;

            const auto result = is_single
                ? LaunchSequential(sem, has_unknown_error, functions, arguments...)
                : LaunchInParallel(sem, actual_core_count, has_unknown_error, functions, arguments...);
            return result;
        }

    private:
        struct run_context final
        {
            result_vector_t Result;

            std::counting_semaphore<>* semaphore;

            bool* HasUnknownError;

            //One thread can run several tasks.
            std::atomic<size_t> AtomicCounter;

            //To lock on task reports.
            std::mutex Mutex;

            std::vector<std::thread> Threads;

            run_context(std::counting_semaphore<>* sem, const size_t cpuCount, bool* pHasUnknownError)
                : Result({}), semaphore(sem), HasUnknownError(ThrowIfNull(pHasUnknownError, "pHasUnknownError")),
                AtomicCounter(0), Mutex(), Threads(std::vector<std::thread>(cpuCount))
            {
            }

            ~run_context()
            {
                JoinThreads();
            }

            bool has_jobs(const size_t total_count) const noexcept
            {
                assert(total_count > 0);
                const auto cur = static_cast<size_t>(AtomicCounter);
                return cur < total_count;
            }

            void JoinThreads()
            {
                for (auto& th : Threads)
                    if (th.joinable())
                        th.join();

                Threads.clear();
            }
        };

        static result_vector_t LaunchSequential(
            std::counting_semaphore<>* sem,
            bool& has_unknown_error,
            const std::vector<function_t>& functions, arguments_t ... arguments)
        {
            const size_t size = functions.size();

            result_vector_t result;
            if (sem)
                sem->acquire();

            try
            {
                for (size_t i = 0; i < size; ++i)
                {
                    run_function(has_unknown_error,
                        i, nullptr, result,
                        functions[i], arguments ...);
                }
            }
            catch (...)
            {
                if (sem)
                    sem->release();
                throw;
            }

            return result;
        }

        static result_vector_t LaunchInParallel(
            std::counting_semaphore<>* sem,
            const size_t cpuCount,
            bool& has_unknown_error,
            const std::vector<function_t>& functions, arguments_t ... arguments)
        {
            if (cpuCount < 2)
            {
                throw std::out_of_range("cpuCount must be at least 2.");
            }

            run_context context(sem, cpuCount, &has_unknown_error);
            auto debt = false;
            try
            {
                for (size_t i = 0; i < cpuCount &&
                    context.has_jobs(functions.size()); ++i)
                {
                    if (sem)
                    {
                        sem->acquire();
                        debt = true;
                    }

                    context.Threads[i] = std::thread(
                        //Use pointers for a thread function.
                        &RunInThread,
                        &context, &functions, arguments ...);
                    debt = false;
                }
            }
            catch (...)
            {
                has_unknown_error = true;
                if (sem && debt)
                    sem->release();
            }

            context.JoinThreads();
            return context.Result;
        }

        static void run_function(
            bool& has_unknown_error,
            const size_t index,
            std::mutex* pMutex,
            result_vector_t& result,
            const function_t& function, arguments_t ... arguments)
        {
            const RunResultUtilities::RunResult_Message runResult =
                RunResultUtilities::
                RunSafe<function_t, arguments_t ...>(function, arguments ...);

            if (RunResultUtilities::HasErrorUncaught(runResult.first))
            {
                has_unknown_error = true;
            }

            if (RunResultUtilities::HasError(runResult.first))
            {
                const auto entry = std::make_tuple(index, runResult.first, runResult.second);
                if (nullptr == pMutex)
                {
                    result.push_back(entry);
                }
                else
                {
                    std::lock_guard<std::mutex> addLock(*pMutex);
                    result.push_back(entry);
                }
            }
        }

        static void RunInThread(run_context* pRunContext,
            //Use pointers to be called from a thread.
            const std::vector<function_t>* pFunctions, arguments_t ... arguments)
        {
            assert(pRunContext && pRunContext->HasUnknownError && pFunctions);
            try
            {
                ThrowIfNull(pRunContext, "pRunContext");
                ThrowIfNull(pRunContext->HasUnknownError, "pRunContext->HasUnknownError");
                ThrowIfNull(pFunctions, "pFunctions");
                ThrowIfEmpty(*pFunctions, "*pFunctions");

                ExecuteFunctionCycleUnsafe(pRunContext, *pFunctions, arguments ...);
            }
            catch (...)
            {
                try
                {
                    if (pRunContext->HasUnknownError)
                        *(pRunContext->HasUnknownError) = true;
                }
                catch (...)
                {//Nothing can be done.
                }
            }

            if (pRunContext->semaphore)
                pRunContext->semaphore->release();
        }

        static void ExecuteFunctionCycleUnsafe(run_context* pRunContext,
            const std::vector<function_t>& functions, arguments_t ... arguments)
        {
            const size_t size = functions.size();
            for (;;)
            {
                size_t counterOld = pRunContext->AtomicCounter;
                if (size <= counterOld)
                {//All tasks have been taken.
                    break;
                }

                const auto counterNew = counterOld + 1;
                if (!(pRunContext->AtomicCounter.compare_exchange_strong(counterOld, counterNew)))
                {//Should retry to take a task.
                    continue;
                }

                run_function(*(pRunContext->HasUnknownError),
                    counterOld,
                    &(pRunContext->Mutex),
                    pRunContext->Result,
                    functions[counterOld], arguments ...);
            }
        }
    };
}