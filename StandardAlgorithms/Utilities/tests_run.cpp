#include"console_color.h"
#include"elapsed_time_ns.h"
#include"find_first_repetition.h"
#include"has_openmp.h"
#include"task_runner.h"
#include"test_named_functor.h"
#include"test_run_context.h"
#include"tests.h"
#include<algorithm>
#include<functional>
#include<iostream>
#include<optional>
#include<tuple>

namespace
{
    using functor_t = Standard::Algorithms::Utilities::test_named_functor;
    using full_function_t = functor_t::full_function_t;

    [[nodiscard]] [[gnu::pure]] constexpr auto to_functors(const std::vector<Standard::Algorithms::test_function> &tests)
    {
        std::vector<full_function_t> result(tests.size());

        const auto total_tests = static_cast<std::int32_t>(tests.size());

        std::transform(tests.cbegin(), tests.cend(), result.begin(),
            [total_tests](const auto &test)
            {
                return functor_t(test.name + "", test.func, total_tests);
            });

        return result;
    }

    [[nodiscard]] [[gnu::pure]] constexpr auto to_func_pointers(
        const std::vector<Standard::Algorithms::test_function> &tests) -> std::vector<const void *>
    {
        std::vector<const void *> func_pointers(tests.size());

        std::transform(tests.cbegin(), tests.cend(), func_pointers.begin(),
            [] [[nodiscard]] (const auto &test)
            {
                // Need to compare by pointers to find duplicates.
                // NOLINTNEXTLINE
                return reinterpret_cast<const void *>(test.func);
            });

        return func_pointers;
    }

    constexpr void require_unique_function_pointers(const std::vector<Standard::Algorithms::test_function> &tests)
    {
        const auto func_pointers = to_func_pointers(tests);

        // No constant.
        // NOLINTNEXTLINE
        std::size_t firstRepeatIndex{};

        const auto *const func_name = static_cast<const char *>(__FUNCTION__); // todo(p3): Clang source location.

        if (Standard::Algorithms::Utilities::find_first_repetition<const void *>(
                func_pointers.cbegin(), firstRepeatIndex, func_pointers.cend(), func_name)) [[unlikely]]
        {
            // No constant.
            // NOLINTNEXTLINE
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "FunctionPointers are repeating at index " << firstRepeatIndex;

            if (firstRepeatIndex < tests.size())
            {
                str << ", name '" << tests[firstRepeatIndex].name << "'";
            }

            str << ".";

            throw std::runtime_error(str.str());
        }
    }

    [[nodiscard]] auto tests_prepared(const bool is_debugger_attached = false)
        -> std::vector<Standard::Algorithms::test_function>
    {
        auto result = Standard::Algorithms::gather_all_tests();
        // todo(p3): fix - use a flat_map. test_name_utilities::require_unique_names(result, "tests");
        // todo(p4): #include"test_name_utilities.h"

        require_unique_function_pointers(result);

        if (!is_debugger_attached)
        {
            std::sort(result.begin(), result.end(),
                [] [[nodiscard]] (
                    const Standard::Algorithms::test_function &one, const Standard::Algorithms::test_function &two)
                {
                    return one.name < two.name;
                });
        }

        return result;
    }

    void print_errors_safe(const std::vector<Standard::Algorithms::test_function> &tests,
        const Standard::Algorithms::Utilities::task_runner<>::result_vector_t &errors)
    {
        const Standard::Algorithms::Colors::error_color color;

        try
        {
            std::cout << "\n" << errors.size() << " test errors have occurred.\n\n";

            for (std::size_t index{}; const auto &error : errors)
            {
                // todo(p3): check it?
                // if (Standard::Algorithms::Utilities::run_result_utilities::has_printable_error(std::get<1>(error)))
                {
                    const auto &name = tests.at(std::get<0>(error)).name;
                    const auto &err = std::get<2>(error);

                    std::cout << index << ". '" << name << "' has an error:\n" << err << "\n\n";

                    ++index;
                }
            }
        }
        catch (const std::exception &exc)
        {
            const auto *const func_name = static_cast<const char *>(__FUNCTION__);

            std::cout << "Error in " << func_name << ": " << exc.what() << '\n';
        }
    }

    [[nodiscard]] auto run_tests_safe_two(const std::size_t cpu_count, const bool is_debugger_attached)
        -> std::tuple<std::size_t, std::size_t, bool, std::int64_t>
    {
        const Standard::Algorithms::elapsed_time_ns start_time{};
        std::optional<Standard::Algorithms::ns_elapsed_type> elapsed{};

        Standard::Algorithms::Tests::test_run_context context{};
        std::size_t test_count{};
        std::size_t error_count{};
        auto has_unknown_error = false;

        try
        {
            const auto tests = tests_prepared(is_debugger_attached);
            test_count = tests.size();

            const auto test_functors = to_functors(tests);

            const auto errors = Standard::Algorithms::Utilities::task_runner<full_function_t,
                Standard::Algorithms::Tests::test_run_context *>::execute(nullptr, cpu_count, has_unknown_error,
                test_functors, &context);

            error_count = errors.size();
            elapsed = start_time.elapsed();

            if (0U < error_count)
            {
                print_errors_safe(tests, errors);
            }
        }
        catch (const std::exception &exc)
        {
            if (!elapsed.has_value())
            {
                elapsed = start_time.elapsed();
            }

            error_count = std::max(error_count, static_cast<decltype(error_count)>(1));

            const Standard::Algorithms::Colors::error_color color;

            std::cout << "Exit the tests run abnormally. Error: " << exc.what() << '\n';
        }

        if (!elapsed.has_value()) [[unlikely]]
        {
            throw std::runtime_error("Elapsed has not been set in the tests run.");
        }

        return { test_count, error_count, has_unknown_error, elapsed.value() };
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::run_all_tests(const bool is_debugger_attached) -> std::int32_t
{
    const auto omp_good = has_openmp_computer();
    if (!omp_good)
    {
        const Standard::Algorithms::Colors::warn_color color;

        std::cout << "OPENMP (omp) is not found warning.\n";
    }

    const auto cpu_count = is_debugger_attached ? 1U : 0U;

    const auto &[test_count, error_count, has_unknown_error, elapsed_ns] =
        run_tests_safe_two(cpu_count, is_debugger_attached);

    if (!has_unknown_error && 0U == error_count)
    {
        if (omp_good)
        {
            const Standard::Algorithms::Colors::success_color color;

            std::cout << "All " << test_count << " tests are successful.\n";
        }
        else
        {
            std::cout << "Almost all " << test_count << " tests are successful. Please check the OMP settings.\n";
        }
    }
    else
    {
        const Standard::Algorithms::Colors::error_color color;

        std::cout << error_count << " errors in " << test_count << " tests.\n";

        if (has_unknown_error)
        {
            std::cout << "At least one unknown error has occurred!\n";
        }
    }

    std::cout << " Elapsed time " << elapsed_ns << " nanoseconds.\n";

    return 0;
}
