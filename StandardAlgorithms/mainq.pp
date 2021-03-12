#include"Graphs/red_black_tree_tests.h"
#include"Numbers/merge_tests.h"
#include"Utilities/console_color.h"
#include"Utilities/elapsed_time_ns.h"
#include"Utilities/ert.h"
#include"Utilities/has_openmp.h"
#include"Utilities/is_debug.h"
#include"Utilities/is_little_endian.h"
#include"Utilities/localize_stream.h"
#include"Utilities/print_utilities.h"
#include"Utilities/three_numpunct.h"
#include<iostream>
#include<memory>

namespace
{
    void launch_tests()
    {
        // ::Privet::Algorithms::Print("Actual output", output, std::cout);
        Standard::Algorithms::Trees::Tests::red_black_tree_tests();
        Standard::Algorithms::Numbers::Tests::merge_tests();
    }

    // Its ctor/constructor might throw - no, thanks.
    // NOLINTNEXTLINE
    const std::locale *default_locale{};

    [[nodiscard]] auto init_unsafe() -> bool
    {
        std::cout << "__cplusplus " << (__cplusplus) << "\nSingle test run...\n" << std::endl;

        {
            auto empty_locale = std::make_unique<std::locale>("");
            auto punct = std::make_unique<Standard::Algorithms::three_numpunct>();
            auto uni_local = std::make_unique<std::locale>(*(empty_locale.get()), punct.get());

            default_locale = uni_local.release();

            // NOLINTNEXTLINE
            punct.release(); // punct is not just copied in std::locale. // todo(p4): leak?
        }

        // todo(p4): Why does it make everything 3 times slower? Commented out.
        // std::locale::global(*default_locale);

        Standard::Algorithms::Utilities::localize_stream(std::cout);

        if (const auto is_little = Standard::Algorithms::Utilities::is_little_endian(); !is_little)
        {
            std::cout << "A little-endian computer must have been used warning.\n";
            return false;
        }

        if (const auto omp_good = Standard::Algorithms::has_openmp_computer(); !omp_good)
        {
            std::cout << "OPENMP (omp) is not found warning.\n";
            return false;
        }

        return true;
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Utilities::fetch_locale() noexcept -> const std::locale &
{
    if constexpr (::Standard::Algorithms::is_debug)
    {
        assert(default_locale != nullptr);
    }

    return *default_locale;
}

auto main() -> std::int32_t
{
    std::int32_t attempt{};

    try
    {
        const auto harasho = init_unsafe();
        if (!harasho)
        {
            const Standard::Algorithms::Colors::warn_color color;
        }

        constexpr auto max_attempts = 512;
        static_assert(0 < max_attempts);

        Standard::Algorithms::ns_elapsed_type prev_elaps{};

        while (++attempt <= max_attempts)
        {
            const Standard::Algorithms::elapsed_time_ns tima;
            launch_tests();
            const auto elapsed = tima.elapsed();

            const auto ratio = ::Standard::Algorithms::ratio_compute(prev_elaps, elapsed);

            std::cout << " Main attempt " << attempt << " out of " << max_attempts << ", elapsed " << elapsed
                      << ", ratio " << ratio << std::endl;

            prev_elaps = elapsed;
        }

        if (harasho)
        {
            const Standard::Algorithms::Colors::success_color color;
            std::cout << "Success." << std::endl;
        }
        else
        {
            std::cout << "Warnings." << std::endl;
        }

        constexpr auto success = 0;
        return success;
    }
    catch (const std::exception &exc)
    {
        const Standard::Algorithms::Colors::error_color color;
        std::cout << "\nAttempt " << attempt << ". Main error: " << exc.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        const Standard::Algorithms::Colors::error_color color;
        std::cout << "\nAttempt " << attempt << ". Unknown main error." << std::endl;
        return 2;
    }
}
