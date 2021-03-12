#pragma once
#include"compute_core_count.h"
#include"require_utilities.h"
#include"run_result_kind.h"
#include"run_result_utilities.h"
#include<atomic>
#include<mutex>
#include<new> // std::hardware_destructive_interference_size
#include<semaphore>
#include<thread>
#include<tuple>
#include<vector>

#if defined(__clang__)
// todo(p2): remove when Clang has C++20 std::jthread class.
#include"jthread_not.h"
#endif

namespace Standard::Algorithms::Utilities
{
    template<class function_t = void (*)(), class... arguments_t>
    struct task_runner final
    {
        task_runner() = delete;

        using result_vector_t = std::vector<std::tuple<std::size_t, run_result_kind, std::string>>;

        // There must be at least one item in "functions".
        [[nodiscard]] static constexpr auto execute(std::counting_semaphore<> *sem, std::size_t cpu_count,
            bool &has_unknown_error, const std::vector<function_t> &functions, arguments_t... arguments)
            -> result_vector_t
        {
            throw_if_empty("functions", functions);

            const auto size = functions.size();
            const auto actual_core_count = compute_core_count(size, cpu_count);

            // NOLINTNEXTLINE
            const auto is_single = actual_core_count <= single_core || true; // todo(p2): fix

            auto result = is_single ? launch_sequential(sem, has_unknown_error, functions,
                                          // no std::forward<arguments_t>(arguments)...
                                          arguments...)
                                    : launch_in_parallel(sem, actual_core_count, has_unknown_error, functions,
                                          // no std::forward<arguments_t>(arguments)...
                                          arguments...);

            return result;
        }

private:
        struct alignas( // False sharing is bad.
#if defined(__cpp_lib_hardware_interference_size)
            std::hardware_destructive_interference_size
#else // NOLINTNEXTLINE
            64
#endif
            ) run_context final
        {
            constexpr run_context(std::counting_semaphore<> *sem, const std::size_t cpu_count, bool &has_unknown_error)
                : Semaphore(sem)
                , Has_unknown_error(has_unknown_error)
                , Threads(std::vector<std::jthread>(cpu_count))
            {
            }

            run_context(const run_context &) = delete;
            auto operator= (const run_context &) & -> run_context & = delete;
            run_context(run_context &&) noexcept = delete;
            auto operator= (run_context &&) &noexcept -> run_context & = delete;

            constexpr ~run_context() noexcept = default;

            [[nodiscard]] constexpr auto threads() &noexcept -> std::vector<std::jthread> &
            {
                return Threads;
            }

            [[nodiscard]] constexpr auto result() &noexcept -> result_vector_t &
            {
                return Result;
            }

            [[nodiscard]] constexpr auto has_unknown_error() &noexcept -> bool &
            {
                return Has_unknown_error;
            }

            [[nodiscard]] constexpr auto semaphore() &noexcept -> std::counting_semaphore<> *
            {
                return Semaphore;
            }

            [[nodiscard]] constexpr auto atomic_counter() &noexcept -> std::atomic<std::size_t> &
            {
                return Atomic_counter;
            }

            [[nodiscard]] constexpr auto mutex() &noexcept -> std::mutex &
            {
                return Mutex;
            }

            [[nodiscard]] constexpr auto has_jobs(const std::size_t total_count) const noexcept -> bool
            {
                assert(0U < total_count);

                const auto cur = static_cast<std::size_t>(Atomic_counter);
                return cur < total_count;
            }

            constexpr void join_threads() noexcept
            {
                for (auto &thr : Threads)
                {
                    if (thr.joinable())
                    {
                        thr.join();
                    }
                }

                Threads.clear();
            }

    private:
            result_vector_t Result{};
            std::counting_semaphore<> *Semaphore;
            bool &Has_unknown_error;

            // One thread can run several tasks.
            std::atomic<std::size_t> Atomic_counter{};

            // To lock on task reports.
            std::mutex Mutex{};
            std::vector<std::jthread> Threads;
        };

        [[nodiscard]] static constexpr auto launch_sequential(std::counting_semaphore<> *sem, bool &has_unknown_error,
            const std::vector<function_t> &functions, arguments_t... arguments) -> result_vector_t
        {
            const auto size = functions.size();

            result_vector_t result{};

            if (sem != nullptr)
            {
                sem->acquire();
            }

            try
            {
                for (std::size_t index{}; index < size; ++index)
                {
                    run_function(has_unknown_error, index, nullptr, result, functions[index],
                        // no std::forward<arguments_t>(arguments)...
                        arguments...);
                }
            }
            catch (...)
            {
                if (sem != nullptr)
                {
                    sem->release();
                }

                throw;
            }

            if (sem != nullptr)
            {
                sem->release();
            }

            return result;
        }

        [[nodiscard]] static constexpr auto launch_in_parallel(std::counting_semaphore<> *sem,
            const std::size_t cpu_count, bool &has_unknown_error, const std::vector<function_t> &functions,
            arguments_t... arguments) -> result_vector_t
        {
            if (cpu_count < 2U) [[unlikely]]
            {
                auto err = "CPU or core count (" + std::to_string(cpu_count) + ") must be at least 2.";

                throw std::out_of_range(err);
            }

            run_context context(sem, cpu_count, has_unknown_error);
            auto debt = false;

            try
            {
                for (std::size_t index{}; index < cpu_count && context.has_jobs(functions.size()); ++index)
                {
                    if (sem != nullptr)
                    {
                        sem->acquire();
                        debt = true;
                    }

                    context.threads()[index] = std::jthread(
                        // Use pointers for a thread function.
                        &run_in_thread, &context, &functions,
                        // no std::forward<arguments_t>(arguments)...
                        arguments...);

                    debt = false;
                }
            }
            catch (...)
            {
                has_unknown_error = true;

                if (sem != nullptr && debt)
                {
                    sem->release();
                }
            }

            context.join_threads();

            return context.result();
        }

        static constexpr void run_function(bool &has_unknown_error, const std::size_t index, std::mutex *ptr_mutex,
            result_vector_t &result, const function_t &function1, arguments_t... arguments)
        {
            const auto run_result = run_result_utilities::run_safe<function_t, arguments_t...>(function1,
                // no std::forward<arguments_t>(arguments)...
                arguments...);

            if (run_result_utilities::has_error_uncaught(run_result.first))
            {
                has_unknown_error = true;
            }

            if (run_result_utilities::has_error(run_result.first))
            {
                if (nullptr == ptr_mutex)
                {
                    result.emplace_back(index, run_result.first, run_result.second);
                }
                else
                {
                    const std::lock_guard lock(*ptr_mutex);

                    // todo(p3): del copy-paste.
                    result.emplace_back(index, run_result.first, run_result.second);
                }
            }
        }

        inline static constexpr void unknown_error_safe(run_context &context) noexcept
        {
            try
            {
                context.has_unknown_error() = true;
            }
            // todo(p3): log n catch (const std::exception &exc)
            catch (...)
            {// Nothing can be done.
            }
        }

        static constexpr void run_in_thread(run_context *ptr_run_context,
            // Use pointers to be called from a thread.
            const std::vector<function_t> *ptr_functions, arguments_t... arguments) noexcept
        {
            if (ptr_run_context == nullptr) [[unlikely]]
            {
                assert(0);

                // NOLINTNEXTLINE
                return;
            }

            if (ptr_functions == nullptr || ptr_functions->empty()) [[unlikely]]
            {
                unknown_error_safe(*ptr_run_context);

                assert(0);

                // NOLINTNEXTLINE
                return;
            }

            try
            {
                execute_function_cycle_unsafe(ptr_run_context, *ptr_functions,
                    // no std::forward<arguments_t>(arguments)...);
                    arguments...);
            }
            // todo(p3): log n catch (const std::exception &exc)
            catch (...)
            {
                unknown_error_safe(*ptr_run_context);
            }

            try
            {
                if (auto *const sema = ptr_run_context->semaphore(); sema != nullptr)
                {
                    sema->release();
                }
            }
            // todo(p3): log n catch (const std::exception &exc)
            catch (...)
            {
                unknown_error_safe(*ptr_run_context);
            }
        }

        static constexpr void execute_function_cycle_unsafe(
            run_context *const ptr_run_context, const std::vector<function_t> &functions, arguments_t... arguments)
        {
            const auto size = functions.size();

            for (;;)
            {
                std::size_t counter_old = ptr_run_context->atomic_counter();

                if (size <= counter_old)
                {// All tasks have been taken.
                    break;
                }

                const auto counter_new = counter_old + 1U;

                if (!(ptr_run_context->atomic_counter().compare_exchange_strong(counter_old, counter_new)))
                {// Should retry to take a task.
                    continue;
                }

                run_function(ptr_run_context->has_unknown_error(), counter_old, &(ptr_run_context->mutex()),
                    ptr_run_context->result(), functions[counter_old],
                    // no std::forward<arguments_t>(arguments)...);
                    arguments...);
            }
        }
    };
} // namespace Standard::Algorithms::Utilities
