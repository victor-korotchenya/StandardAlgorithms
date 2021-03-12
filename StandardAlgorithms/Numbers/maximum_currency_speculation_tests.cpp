#include"maximum_currency_speculation_tests.h"
#include"../Utilities/test_utilities.h"
#include"maximum_currency_speculation.h"

namespace
{
    using floating_t = Standard::Algorithms::floating_point_type;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<std::vector<floating_t>> &&exchange_rates,
            floating_t expected_profit = {}, std::vector<std::size_t> &&expected_cycle = {}) noexcept
            : base_test_case(std::move(name))
            , Exchange_rates(std::move(exchange_rates))
            , Expected_profit(expected_profit)
            , Expected_cycle(std::move(expected_cycle))
        {
        }

        [[nodiscard]] constexpr auto exchange_rates() const &noexcept -> const std::vector<std::vector<floating_t>> &
        {
            return Exchange_rates;
        }

        [[nodiscard]] constexpr auto expected_profit() const &noexcept -> const floating_t &
        {
            return Expected_profit;
        }

        [[nodiscard]] constexpr auto expected_cycle() const &noexcept -> const std::vector<std::size_t> &
        {
            return Expected_cycle;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("exchange rates", Exchange_rates, str);
            ::Standard::Algorithms::print_value("expected profit", str, Expected_profit);
            ::Standard::Algorithms::print("expected cycle", Expected_cycle, str);
        }

private:
        std::vector<std::vector<floating_t>> Exchange_rates;
        floating_t Expected_profit;
        std::vector<std::size_t> Expected_cycle;
    };

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        // todo(p3): slow alg.
        // todo(p3): more tests including random.

        // R
        // o
        // w
        // _ 0 1 2 ; column
        // 0 | 1 2 3
        // 1 | 5 1 7
        // 2 | 11 13 1
        // Exchange rates.
        //
        // mid=0
        // 0 | 1 2 3
        // 1 | 5 10 15 ; 1-0-1 costs 5*2=10. 1-0-2 costs 5*3=15.
        // 2 | 11 22 33 ; 2-0-1 costs 11*2=22. 2-0-2 costs 11*3=33 (best rate).
        //
        // mid=1
        // 0 | 10 2 30 ; 0-1-2 costs 2*15.
        // 1 | 5 10 15
        // 2 | 110 22 330 ; 2-1-0 costs 22*5.
        //
        // mid=2
        // 0 | 3300 660 30 ; 0-2-0 costs 30*110.
        // 1 | 1650 330 15
        // 2 | 110 22 330
        //
        // 0-2-0 costs 30 * 110 = 3300.
        // 0-1-2-0 costs 2*15 * 110.
        // 0-1-0-2-0 costs 2*5*3 * 110.
        // 0-1-0-2-1-0 costs 2*5*3 * 22*5.
        // 0-1-0-2-0-1-0 costs 2*5*3 * 11*2*5.
        tests.push_back({ "size 3 prime rates", // NOLINTNEXTLINE
            { { 1, 2, 3 }, { 5, 1, 7 }, { 11, 13, 1 } }, // NOLINTNEXTLINE
            3300, { 0, 1, 0, 2, 0, 1, 0 } });

        tests.push_back({ "size 1", { { 1 } } });

        tests.push_back({ "size 2 trivial", { { 1, 1 }, { 1, 1 } }, 1, {} });

        tests.push_back({ "size 2 rate 2 3", // NOLINTNEXTLINE
            { { 1, 2 }, { 3, 1 } }, // NOLINTNEXTLINE
            6, { 1, 0, 1 } });

        tests.push_back({ "size 3 float rates", // NOLINTNEXTLINE
            { { 1, 1.1, 1 }, { 0.499, 1, 1.01 }, { 1.001, 1, 1 } }, // NOLINTNEXTLINE
            1.001 * 1.1 * 1.01, { 2, 0, 1, 2 } });

        constexpr auto small = 0.1;

        tests.push_back({ "size 4 float rates", // NOLINTNEXTLINE
            { { 1, small, small, small }, // NOLINTNEXTLINE
                { small, 1, small, 0.4999 }, // NOLINTNEXTLINE
                { small, 0.997, 1, small }, // NOLINTNEXTLINE
                { small, small, 2.1, 1 } }, // NOLINTNEXTLINE
            0.4999 * 2.1 * 0.997, { 3, 2, 1, 3 } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        constexpr floating_t inf = 1E50;

        std::vector<std::size_t> cycle;

        const auto actual = Standard::Algorithms::Numbers::maximum_currency_speculation_simple<floating_t>(
            test.exchange_rates(), inf, cycle);

        ::Standard::Algorithms::ert::are_equal_with_epsilon(test.expected_profit(), actual, "expected profit");

        ::Standard::Algorithms::ert::are_equal(test.expected_cycle(), cycle, "expected cycle");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::maximum_currency_speculation_tests()
{
    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
