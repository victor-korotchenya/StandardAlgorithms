#include"Utilities/console_color.h"
#include"Utilities/ert.h"
#include"Utilities/is_debug.h"
#include"Utilities/is_little_endian.h"
#include"Utilities/localize_stream.h"
#include"Utilities/print_utilities.h"
#include"Utilities/tests.h"
#include"Utilities/three_numpunct.h"
#include<iostream>
#include<memory>

namespace
{
    // Its ctor/constructor might throw - no, thanks.
    // NOLINTNEXTLINE
    const std::locale *default_locale{};

    void init_unsafe()
    {
        std::cout << "__cplusplus " << (__cplusplus) << std::endl;

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
        {
            const auto is_little = Standard::Algorithms::Utilities::is_little_endian();

            ::Standard::Algorithms::ert::are_equal(true, is_little, "is little Endian");
        }
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

// todo(p3): modules.

auto main() -> std::int32_t
{
    std::int32_t attempt{};
    std::int32_t result{};

    try
    {
        init_unsafe();

        constexpr auto is_debugger_attached = false;
        constexpr auto max_attempts = 1;

        while (++attempt <= max_attempts)
        {
            result = Standard::Algorithms::run_all_tests(is_debugger_attached);
        }

        return result;
    }
    catch (const std::exception &exc)
    {
        const Standard::Algorithms::Colors::error_color color;
        std::cout << "\nAttempt " << attempt << ". Main error: " << exc.what() << '\n';
        return 1;
    }
    catch (...)
    {
        const Standard::Algorithms::Colors::error_color color;
        std::cout << "\nAttempt " << attempt << ". Unknown main error.\n";
        return 2;
    }
}
