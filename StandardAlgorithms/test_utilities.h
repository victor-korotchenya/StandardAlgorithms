#pragma once
#include <algorithm>
#include <functional>
#include <vector>
#include "Assert.h"
#include "base_test_case.h"
#include "Utilities/sub_test_limits.h"
#include "Utilities/ExceptionUtilities.h"
#include "Utilities/RunResultUtilities.h"
#include "Utilities/StreamUtilities.h"
#include "Utilities/TestNameUtilities.h"
#include "Utilities/task_runner.h"

namespace
{
    template <typename test_case_t, typename function_t>
    void execute_test_case(const test_case_t& test_case, function_t func)
    {
        try
        {
            test_case.Validate();
            func(test_case);
        }
        catch (const std::exception& ex)
        {
            std::ostringstream ss;
            ss << ex.what()
                << "\nTest details: " << test_case;
            StreamUtilities::throw_exception(ss);
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << "\nAn unknown error has occurred.";
            try
            {
                ss << " Details: " << test_case;
            }
            catch (...)
            {//Ignore.
            }
            StreamUtilities::throw_exception(ss);
        }
    }

    template <typename test_case_t, typename function_t>
    void run_sequentially(const std::vector<test_case_t>& test_cases, function_t func)
    {
        for (const auto& test_case : test_cases)
        {
            execute_test_case(test_case, func);
        }
    }

    template <typename errors_t>
    void rethrow_exception(
        const bool has_unknown_error,
        const errors_t& errors)
    {
        std::ostringstream ss;
        if (has_unknown_error)
        {
            ss << "An unknown error has occurred in at least one of sub-test.\n";
        }

        try
        {
            for (auto it = errors.begin(); errors.end() != it; ++it)
            {
                const auto& t = *it;
                if (Privet::Algorithms::Utilities::RunResultUtilities::HasError(
                    std::get<1>(t)))
                {
                    ss << std::get<2>(t) << '\n';
                }
            }
        }
        catch (const std::exception& ex)
        {
            ss << "Error in " << __FUNCTION__ << ": " << ex.what() << '\n';
        }

        StreamUtilities::throw_exception(ss);
    }

    //To launch a "void(*)(void)".
    template <typename test_case_t, typename function_t>
    class run_functor final
    {
        test_case_t _test_case;
        function_t _function;

    public:
        explicit run_functor(
            const test_case_t& test_case = {},
            function_t _function = {})
            : _test_case(test_case), _function(_function)
        {
        }

        void operator()()
        {
            execute_test_case(_test_case, _function);
        }
    };

    template <typename test_case_t, typename function_t>
    void run_in_parallel(const std::vector<test_case_t>& test_cases, function_t func, const size_t cpu_count)
    {
        assert(cpu_count > 1);
        std::vector<std::function<void()>> functions(test_cases.size());

        std::transform(test_cases.cbegin(), test_cases.cend(), functions.begin(),
            [&](const test_case_t& t)
            {
                return run_functor<test_case_t, function_t>(t, func);
            });

        auto& sem = Privet::Algorithms::Tests::get_sub_test_semaphore();
        auto has_unknown_error = false;

        const auto errors = Privet::Algorithms::Utilities::task_runner<
            std::function<void()>>::execute(
                &sem, cpu_count, has_unknown_error, functions);

        if (has_unknown_error || !errors.empty())
        {
            rethrow_exception(has_unknown_error, errors);
        }
    }
}

namespace Privet::Algorithms::Tests
{
    template <typename test_case_t, typename ... arguments_t>
    void fix_names(std::vector<test_case_t>& test_cases, arguments_t ... arguments)
    {
        for (test_case_t& testCase : test_cases)
            testCase.fix_name(arguments ...);
    }

    template <typename test_case_t, typename ... arguments_t>
    struct test_utilities final
    {
        test_utilities() = delete;

        template <size_t cpu_count = 1>
        static void run_tests(void(*run_test)(const test_case_t&),
            void(*generate_test_cases)(std::vector<test_case_t>&, arguments_t ...),
            arguments_t ... generate_arguments)
        {
            ThrowIfNull(generate_test_cases, "generate_test_cases");
            ThrowIfNull(run_test, "run_test");

            std::vector<test_case_t> test_cases;
            generate_test_cases(test_cases, generate_arguments ...);
            TestNameUtilities::RequireUniqueNames(test_cases, "testCases");

            const auto size = test_cases.size();
            const auto actual_cpu_count = Privet::Algorithms::Utilities::get_core_count(size, cpu_count);

            if (actual_cpu_count <= 1)
                run_sequentially(test_cases, run_test);
            else
                run_in_parallel(test_cases, run_test, actual_cpu_count);
        }
    };
}