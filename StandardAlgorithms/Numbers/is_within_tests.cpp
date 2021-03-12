#include"is_within_tests.h"
#include"../Numbers/arithmetic.h"
#include"../Utilities/test_utilities.h"
#include<vector>

namespace
{
    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, const std::pair<std::uint32_t, std::uint32_t> &min_max,
            std::uint32_t value, bool expected) noexcept
            : base_test_case(std::move(name))
            , Min(min_max.first)
            , Max(min_max.second)
            , Value(value)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto min() const &noexcept -> const std::uint32_t &
        {
            return Min;
        }

        [[nodiscard]] constexpr auto max() const &noexcept -> const std::uint32_t &
        {
            return Max;
        }

        [[nodiscard]] constexpr auto value() const &noexcept -> const std::uint32_t &
        {
            return Value;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> bool
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            str << " Min " << Min << ", Max " << Max << ", Value " << Value << ", Expected " << Expected;
        }

private:
        std::uint32_t Min;
        std::uint32_t Max;
        std::uint32_t Value;
        bool Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr auto maxx = static_cast<std::uint32_t>(std::numeric_limits<std::int8_t>::max());

        static_assert(0U < maxx);

        for (std::uint32_t value{}; value <= maxx; ++value)
        {
            constexpr std::uint32_t min_value = '0';
            constexpr std::uint32_t max_value = '9';

            const auto expected = min_value <= value && value <= max_value;

            test_cases.emplace_back(std::to_string(value), std::make_pair(min_value, max_value), value, expected);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual = Standard::Algorithms::Numbers::is_within_inclusive(test.value(), { test.min(), test.max() });

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "is_within_inclusive");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::is_within_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
