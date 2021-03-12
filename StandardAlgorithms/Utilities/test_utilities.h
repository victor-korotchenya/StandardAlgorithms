#pragma once
// "test_utilities.h"
#include"base_test_case.h"
#include"ert.h"
#include"get_sub_test_semaphore.h"
#include"run_result_utilities.h"
#include"task_runner.h"
#include<algorithm>
#include<functional>

namespace Standard::Algorithms::Tests::Inner
{
    template<class test_case_t, class function_t>
    constexpr void execute_template_method(const test_case_t &test_case, function_t func)
    {
        try
        {
            test_case.validate();
            func(test_case);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << exc.what() << "\nTest details: " << test_case;
            throw_exception(str);
        }
        catch (...)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "\nA non-standard error has occurred.";

            try
            {
                str << " Test case details: " << test_case;
            }
            catch (...)
            {// Ignore.
            }

            throw_exception(str);
        }
    }

    template<class test_case_t, class function_t>
    constexpr void run_sequentially(const std::vector<test_case_t> &test_cases, function_t func)
    {
        [[maybe_unused]] std::size_t index{};

        for (const auto &test_case : test_cases)
        {
            execute_template_method(test_case, func);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                ++index;
            }
        }
    }

    template<class errors_t>
    [[noreturn]] constexpr void rethrow_exception(const bool has_unknown_error, const errors_t &errors)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        if (has_unknown_error)
        {
            str << "An unknown error has occurred in at least one sub-test.\n";
        }

        try
        {
            for (const auto &error : errors)
            {
                if (Standard::Algorithms::Utilities::run_result_utilities::has_error(std::get<1>(error)))
                {
                    str << std::get<2>(error) << '\n';
                }
            }
        }
        catch (const std::exception &exc)
        {
            const auto *const func_name = static_cast<const char *>(__FUNCTION__);

            str << "Error in " << func_name << ": " << exc.what() << '\n';
        }

        throw_exception(str);
    }

    // To launch a function e.g. "void(*)(void)".
    template<class test_case_t, class function_t>
    class run_functor final
    {
        const test_case_t &Test_case;
        function_t Function;

public:
        constexpr explicit run_functor(const test_case_t &test_case, function_t function1 = {})
            : Test_case(test_case)
            , Function(function1)
        {
        }

        constexpr void operator() ()
        {
            execute_template_method(Test_case, Function);
        }
    };

    template<class test_case_t, class function_t>
    constexpr void run_in_parallel(
        const std::vector<test_case_t> &test_cases, function_t func, const std::size_t cpu_count)
    {
        assert(1U < cpu_count);

        using void_func_t = std::function<void()>;
        std::vector<void_func_t> functions(test_cases.size());

        std::transform(test_cases.cbegin(), test_cases.cend(), functions.begin(),
            [&](const test_case_t &test)
            {
                return run_functor<test_case_t, function_t>(test, func);
            });

        auto &sema = Standard::Algorithms::Tests::get_sub_test_semaphore();
        auto has_unknown_error = false;

        const auto errors = Standard::Algorithms::Utilities::task_runner<void_func_t>::execute(
            &sema, cpu_count, has_unknown_error, functions);

        if (has_unknown_error || !errors.empty())
        {
            rethrow_exception(has_unknown_error, errors);
        }
    }
} // namespace Standard::Algorithms::Tests::Inner

namespace Standard::Algorithms::Tests
{
    template<class test_case_t, class... arguments_t>
    struct test_utilities final
    {
        test_utilities() = delete;

        // Return true on success, false on failure.
        template<std::size_t cpu_count = single_core>
        static constexpr auto run_tests(void (*run_test)(const test_case_t &),
            void (*generate_test_cases)(std::vector<test_case_t> &, arguments_t...),
            arguments_t &&...generate_arguments) -> bool
        {
            throw_if_null("generate_test_cases", generate_test_cases);
            throw_if_null("run_test", run_test);

            std::vector<test_case_t> test_cases;
            generate_test_cases(test_cases, std::forward<arguments_t>(generate_arguments)...);

            check_test_names(test_cases);
            assert(!test_cases.empty());

            if constexpr (cpu_count == single_core)
            {
                Inner::run_sequentially(test_cases, run_test);
                return true;
            }

            const auto size = test_cases.size();
            const auto actual_cpu_count = Standard::Algorithms::Utilities::compute_core_count(size, cpu_count);

            if (actual_cpu_count <= single_core)
            {
                Inner::run_sequentially(test_cases, run_test);
            }
            else
            {
                Inner::run_in_parallel(test_cases, run_test, actual_cpu_count);
            }

            return true;
        }

private:
        static constexpr void check_test_names(const std::vector<test_case_t> &test_cases)
        {
            const auto size = require_positive(test_cases.size(), "test cases size");

            std::vector<std::string> names;
            names.reserve(size);

            std::transform(test_cases.cbegin(), test_cases.cend(), std::back_inserter(names),
                [] [[nodiscard]] (const auto &test) // todo(p3): "-> const std::string &" ?
                {
                    return test.name();
                });

            std::sort(names.begin(), names.end());

            if (const auto iter = std::adjacent_find(names.cbegin(), names.cend()); iter != names.cend()) [[unlikely]]
            {
                auto err = "A duplicate test name is found: " + *iter;

                throw std::runtime_error(err);
            }
        }
    };
} // namespace Standard::Algorithms::Tests
