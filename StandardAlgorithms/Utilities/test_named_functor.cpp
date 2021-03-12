#include"test_named_functor.h"
#include"console_color.h"
#include"elapsed_time_ns.h"
#include"is_debug.h"
#include"launch_exception.h"
#include"require_utilities.h"
#include<cmath>
#include<iomanip>
#include<iostream>

namespace
{
    using mutex_t = Standard::Algorithms::Tests::test_run_context::mutex_t;
    using lock_t = std::lock_guard<mutex_t>;

    constexpr auto shall_print_test_names = true;

    [[nodiscard]] constexpr auto log10up(const std::int32_t val) -> std::int32_t
    {
        constexpr auto base1 = 10;

        const auto log = base1 < val ? std::log10(val) : 1;
        const auto cei = std::ceil(log);
        auto res = static_cast<std::int32_t>(cei);
        assert(0 < res);

        return res;
    }

    inline void print_new_line(::Standard::Algorithms::Tests::test_run_context &context)
    {
        if (context.shall_print_line)
        {
            std::cout << '\n';
        }
        else
        {
            context.shall_print_line = true;
        }
    }
} // namespace

Standard::Algorithms::Utilities::test_named_functor::test_named_functor(std::string &&name,
    Standard::Algorithms::Utilities::test_named_functor::void_func_t test_function, const std::int32_t total_tests)
    : test_named_functor::parent_t(std::move(name), test_function)
    , total_tests_log10(log10up(require_positive(total_tests, "total tests")))
{
}

void Standard::Algorithms::Utilities::test_named_functor::operator() (
    Standard::Algorithms::Tests::test_run_context *context) &
{
    if constexpr (shall_print_test_names)
    {
        const lock_t add_lock(context->console_mutex);
        print_new_line(*context);

        std::cout << "  Starting the test '" << name() << "'..\n";
    }

    auto has_error = false;
    std::string error_message;
    std::int64_t elapsed_ns{};

    const elapsed_time_ns clo{};

    try
    {
        func()();
        elapsed_ns = clo.elapsed();
    }
    catch (const std::exception &exc)
    {
        elapsed_ns = clo.elapsed();
        error_message = exc.what();
        has_error = true;

        const lock_t add_lock(context->console_mutex);
        print_new_line(*context);

        const Standard::Algorithms::Colors::error_color color;

        std::cout << "The test '" << name() << "' has failed:\n" << error_message << '\n';
    }

    // NOLINTNEXTLINE
    constexpr auto max_duration_ns = 1'000'000 * (::Standard::Algorithms::is_debug ? 50 : 5);
    static_assert(0 < max_duration_ns);

    if (max_duration_ns < elapsed_ns)
    {
        const lock_t add_lock(context->console_mutex);
        // PrintNewLine(*context);
        ++(context->long_runing_tests);

        constexpr auto ns_in_millisecond = 1'000;

        std::cout << " Test_" << std::setfill('0') << std::setw(total_tests_log10)
                  << static_cast<std::size_t>(context->long_runing_tests) << " '" << name()
                  << "' took too long: " << elapsed_ns / ns_in_millisecond << " micro-second(s).\n";
    }
    else if (!has_error)
    {
        if constexpr (shall_print_test_names)
        {
            const lock_t add_lock(context->console_mutex);
            print_new_line(*context);

            std::cout << " Done the test '" << name() << "'.\n";
        }
    }

    if (has_error) [[unlikely]]
    {
        throw launch_exception(error_message);
    }
}
