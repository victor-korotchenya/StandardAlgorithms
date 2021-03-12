#include"zu_string_tests.h"
#include"test_utilities.h"
#include"zu_string.h"
#include<limits>

namespace
{
    // NOLINTNEXTLINE
    constexpr std::int64_t signed_max_value = 9'223'372'036'854'775'807LL;

    static_assert(0 < signed_max_value && -signed_max_value < 0);

    template<std::integral int_t>
    constexpr void run_zste(const int_t &value, const std::string &expected)
    {
        Standard::Algorithms::throw_if_empty("expected", expected);

        const auto actual = Standard::Algorithms::Utilities::zu_string<int_t>(value);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "zu_string");
    }

    constexpr void typical_zst(const std::int64_t &value, const std::string &expected)
    {
        Standard::Algorithms::require_positive(value, "value");

        run_zste(value, expected);
        run_zste(-value, Standard::Algorithms::Utilities::negative_sign + expected);

        run_zste<std::uint64_t>(value, expected);
    }

    [[nodiscard]] constexpr auto typical_zu_string_tests() -> bool
    {
        typical_zst(1, "1"); // NOLINTNEXTLINE
        typical_zst(12, "12"); // NOLINTNEXTLINE
        typical_zst(124, "124"); // NOLINTNEXTLINE
        typical_zst(142, "142"); // NOLINTNEXTLINE
        typical_zst(7'142, "7,142"); // NOLINTNEXTLINE
        typical_zst(87'142, "87,142"); // NOLINTNEXTLINE
        typical_zst(987'142, "987,142"); // NOLINTNEXTLINE
        typical_zst(5'987'142, "5,987,142"); // NOLINTNEXTLINE
        typical_zst(5'987'102, "5,987,102"); // NOLINTNEXTLINE
        typical_zst(5'987'140, "5,987,140"); // NOLINTNEXTLINE
        typical_zst(5'987'100, "5,987,100"); // NOLINTNEXTLINE
        typical_zst(5'980'100, "5,980,100"); // NOLINTNEXTLINE
        typical_zst(5'900'100, "5,900,100"); // NOLINTNEXTLINE
        typical_zst(5'000'100, "5,000,100"); // NOLINTNEXTLINE
        typical_zst(5'987'000, "5,987,000"); // NOLINTNEXTLINE
        typical_zst(5'980'000, "5,980,000"); // NOLINTNEXTLINE
        typical_zst(5'900'000, "5,900,000"); // NOLINTNEXTLINE
        typical_zst(5'000'000, "5,000,000"); // NOLINTNEXTLINE
        typical_zst(8'046'744'073'000'051'615LLU, "8,046,744,073,000,051,615"); // NOLINTNEXTLINE

        {
            constexpr auto mini = std::numeric_limits<std::int64_t>::min();
            constexpr auto maxima = std::numeric_limits<std::int64_t>::max();

            static_assert(mini < 0 && 0 < maxima);
            static_assert(signed_max_value == maxima);
            static_assert(-signed_max_value - 1LL == mini);
            static_assert(-signed_max_value == mini + 1LL);
        }

        typical_zst(signed_max_value, "9,223,372,036,854,775,807"); // NOLINTNEXTLINE

        return true;
    }

    [[nodiscard]] constexpr auto special_zu_string_tests() -> bool
    {
        run_zste<std::uint8_t>(0, "0"); // NOLINTNEXTLINE
        run_zste<std::uint32_t>(0, "0"); // NOLINTNEXTLINE
        run_zste<std::uint8_t>(124, "124"); // NOLINTNEXTLINE
        run_zste<std::int8_t>(124, "124"); // NOLINTNEXTLINE
        run_zste<std::int8_t>(-124, "-124"); // NOLINTNEXTLINE
        run_zste<std::uint16_t>(124, "124"); // NOLINTNEXTLINE
        run_zste<std::int16_t>(124, "124"); // NOLINTNEXTLINE
        run_zste<std::int16_t>(-124, "-124"); // NOLINTNEXTLINE
        run_zste(-signed_max_value - 1LL, "-9,223,372,036,854,775,808"); // NOLINTNEXTLINE

        {
            constexpr auto uns_maxima = std::numeric_limits<std::uint64_t>::max();

            static_assert(0U < uns_maxima);

            run_zste<std::uint64_t>(uns_maxima, "18,446,744,073,709,551,615"); // NOLINTNEXTLINE
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Utilities::Tests::zu_string_tests()
{
    static_assert(typical_zu_string_tests());
    static_assert(special_zu_string_tests());
}
