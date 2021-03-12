#include"logarithm_tests.h"
#include"../Utilities/test_utilities.h"
#include"logarithm.h"
#include<array>

namespace // logarithm_2_tests
{
    using int_t = std::uint64_t;
    using logarithm_result_t = std::int32_t;

    constexpr int_t one = 1;

    constexpr void logarithm_2_test(const auto &tupitsa)
    {
        const auto [value, exp_down, exp_up] = tupitsa;

        Standard::Algorithms::require_less_equal(exp_down, exp_up, "down up");

        {
            const auto actual = Standard::Algorithms::Numbers::log_base_2_down(value);

            ::Standard::Algorithms::ert::are_equal(exp_down, actual, "log_base_2_down");
        }
        {
            const auto actual = Standard::Algorithms::Numbers::log_base_2_up(value);

            ::Standard::Algorithms::ert::are_equal(exp_up, actual, "log_base_2_up");
        }
    }

    [[nodiscard]] constexpr auto logarithm_2_tests() -> bool
    {
        using value_down_up = std::tuple<int_t, logarithm_result_t, logarithm_result_t>;

        constexpr auto max_bit = (sizeof(int_t) << 3U) - 1U;
        constexpr auto largest_pure_power_of_2 = one << max_bit;
        static_assert(int_t{} < max_bit && int_t{} < largest_pure_power_of_2);

        constexpr auto maxi = std::numeric_limits<int_t>::max();
        static_assert(int_t{} < maxi && int_t{} - one == maxi && int_t{} == maxi + one);

        const std::array tests{// NOLINTNEXTLINE
            value_down_up{ 0, -1, 0 }, value_down_up{ 1, 0, 0 }, // NOLINTNEXTLINE
            value_down_up{ 2, 1, 1 }, value_down_up{ 3, 1, 2 }, // NOLINTNEXTLINE
            value_down_up{ 4, 2, 2 }, value_down_up{ 5, 2, 3 }, // NOLINTNEXTLINE
            value_down_up{ 6, 2, 3 }, value_down_up{ 7, 2, 3 }, // NOLINTNEXTLINE
            value_down_up{ 8, 3, 3 }, value_down_up{ 9, 3, 4 }, // NOLINTNEXTLINE
            value_down_up{ 10, 3, 4 }, value_down_up{ 14, 3, 4 }, // NOLINTNEXTLINE
            value_down_up{ 15, 3, 4 }, value_down_up{ 16, 4, 4 }, // NOLINTNEXTLINE
            value_down_up{ 17, 4, 5 }, value_down_up{ 18, 4, 5 }, // NOLINTNEXTLINE
            value_down_up{ 31, 4, 5 }, value_down_up{ 32, 5, 5 }, // NOLINTNEXTLINE
            value_down_up{ 33, 5, 6 }, value_down_up{ 34, 5, 6 }, // NOLINTNEXTLINE
            value_down_up{ largest_pure_power_of_2 - one, max_bit - 1, max_bit },
            value_down_up{ largest_pure_power_of_2, max_bit, max_bit },
            value_down_up{ largest_pure_power_of_2 + one, max_bit, max_bit + 1 },
            value_down_up{ maxi - one, max_bit, max_bit + 1 }, value_down_up{ maxi, max_bit, max_bit + 1 }
        };

        for (const auto &test : tests)
        {
            logarithm_2_test(test);
        }

        return true;
    }
} //  namespace

void Standard::Algorithms::Numbers::Tests::logarithm_tests()
{
    static_assert(logarithm_2_tests());
}
