#include"maximum_share_speculation_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"maximum_share_speculation.h"
#include<iostream>

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;
    using vec_t = std::vector<int_t>;

    constexpr auto print_maximum_share_speculation_tests = false;
    constexpr auto not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::size_t allowed_transactions, vec_t &&prices,
            long_int_t expected_profit = not_computed) noexcept
            : base_test_case(std::move(name))
            , Allowed_transactions(allowed_transactions)
            , Prices(std::move(prices))
            , Expected_profit(expected_profit)
        {
        }

        [[nodiscard]] constexpr auto allowed_transactions() const noexcept -> std::size_t
        {
            return Allowed_transactions;
        }

        [[nodiscard]] constexpr auto prices() const &noexcept -> const vec_t &
        {
            return Prices;
        }

        [[nodiscard]] constexpr auto expected_profit() const &noexcept -> const long_int_t &
        {
            return Expected_profit;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            ::Standard::Algorithms::ert::greater_or_equal(Expected_profit, not_computed, "expected profit");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("prices", Prices, str);
            str << "allowed_transactions " << Allowed_transactions << ", expected_profit " << Expected_profit;
        }

private:
        std::size_t Allowed_transactions;
        vec_t Prices;
        long_int_t Expected_profit;
    };

    void generate_test_cases(std::vector<test_case> &tests)
    {
        tests.emplace_back("base 2 transactions", // NOLINTNEXTLINE
            2, vec_t{ 100, 5, 40, 1, 9, 11, 5, 26, 15 }, // NOLINTNEXTLINE
            60); // -5+40 -1+26 = 35 + 25.

        tests.emplace_back("empty 0", 0, vec_t{}, 0);
        tests.emplace_back("empty 1", 1, vec_t{}, 0);
        tests.emplace_back("1 price", 1, vec_t{ 3 }, 0);

        tests.emplace_back("2 prices 0 tr", // NOLINTNEXTLINE
            0, vec_t{ 3, 40 }, 0);

        tests.emplace_back("2 prices 1 tr", // NOLINTNEXTLINE
            1, vec_t{ 3, 40 }, // NOLINTNEXTLINE
            37);

        tests.emplace_back("2 prices 2 tr", // NOLINTNEXTLINE
            2, vec_t{ 3, 40 }, // NOLINTNEXTLINE
            37);

        tests.emplace_back("2 prices many tr", // NOLINTNEXTLINE
            5, vec_t{ 3, 40 }, // NOLINTNEXTLINE
            37);

        tests.emplace_back("-2 prices 1 tr", // NOLINTNEXTLINE
            1, vec_t{ 40, 3 }, 0);

        tests.emplace_back("-2 prices 2 tr", // NOLINTNEXTLINE
            2, vec_t{ 40, 3 }, 0);

        tests.emplace_back("5 prices 0 tr", // NOLINTNEXTLINE
            0, vec_t{ 40, 3, 10, 5, 25 }, 0);

        tests.emplace_back("5 prices 1 tr", // NOLINTNEXTLINE
            1, vec_t{ 40, 3, 10, 5, 25 }, // NOLINTNEXTLINE
            22); // -3+25.

        tests.emplace_back("5 prices 2 tr", // NOLINTNEXTLINE
            2, vec_t{ 40, 3, 10, 5, 25 }, // NOLINTNEXTLINE
            27); // -3+10 -5+25 = 7 + 20.

        tests.emplace_back("5 prices 3 tr", // NOLINTNEXTLINE
            3, vec_t{ 40, 3, 10, 5, 25 }, // NOLINTNEXTLINE
            27);

        tests.emplace_back("6 prices 0 tr", // NOLINTNEXTLINE
            0, vec_t{ 40, 0, 9, 10, 5, 25 }, 0);

        tests.emplace_back("6 prices 1 tr", // NOLINTNEXTLINE
            1, vec_t{ 40, 0, 9, 10, 5, 25 }, // NOLINTNEXTLINE
            25); // -0+25 = 25.

        tests.emplace_back("6 prices 2 tr", // NOLINTNEXTLINE
            2, vec_t{ 40, 0, 9, 10, 5, 25 }, // NOLINTNEXTLINE
            30); // -0+10 -5+25 = 10 + 20.

        tests.emplace_back("6 prices 3 tr", // NOLINTNEXTLINE
            3, vec_t{ 40, 0, 9, 10, 5, 25 }, // NOLINTNEXTLINE
            30);

        tests.emplace_back("6 prices 10 tr", // NOLINTNEXTLINE
            10, vec_t{ 40, 0, 9, 10, 5, 25 }, // NOLINTNEXTLINE
            30);

        tests.emplace_back("7 prices 1 tr", // NOLINTNEXTLINE
            1, vec_t{ 5, 40, 0, 9, 10, 5, 25 }, // NOLINTNEXTLINE
            35); // -5+40 = 35.

        tests.emplace_back("7 prices 3 tr", // NOLINTNEXTLINE
            3, vec_t{ 5, 40, 0, 9, 10, 5, 25 }, // NOLINTNEXTLINE
            65); // -5+40 -0+10 -5+25 = 35 + 10 + 20.

        tests.emplace_back("7 prices 4 tr", // NOLINTNEXTLINE
            4, vec_t{ 5, 40, 0, 9, 10, 5, 25 }, // NOLINTNEXTLINE
            65);

        tests.emplace_back("7 prices 8 tr", // NOLINTNEXTLINE
            8, vec_t{ 5, 40, 0, 9, 10, 5, 25 }, // NOLINTNEXTLINE
            65);

        constexpr auto min_size = 0;
        constexpr auto max_size = 30;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            auto prices = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size);
            auto trans = rnd();

            tests.emplace_back("Random" + std::to_string(att), trans, std::move(prices));
        }
    }

    void run_test_case(const test_case &test)
    {
        std::vector<std::pair<std::size_t, std::size_t>> indexes;

        const Standard::Algorithms::elapsed_time_ns tim0;

        const auto fast_profit = Standard::Algorithms::Numbers::maximum_share_speculation<int_t, long_int_t>(
            test.prices(), test.allowed_transactions(), indexes);

        [[maybe_unused]] const auto elapsed0 = tim0.elapsed();

        if (0 <= test.expected_profit())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected_profit(), fast_profit, "maximum_share_speculation");
            // todo(p3): check indexes.
        }

        [[maybe_unused]] std::remove_cvref_t<decltype(elapsed0)> elapsed1 = 0;

        {
            const Standard::Algorithms::elapsed_time_ns tim1;

            const auto slow_profit = Standard::Algorithms::Numbers::maximum_share_speculation_simple<int_t, long_int_t>(
                test.prices(), test.allowed_transactions(), indexes);

            if constexpr (print_maximum_share_speculation_tests)
            {
                elapsed1 = tim1.elapsed();
            }

            ::Standard::Algorithms::ert::are_equal(fast_profit, slow_profit, "maximum_share_speculation_simple");
            // todo(p3): check indexes.
        }

        if constexpr (print_maximum_share_speculation_tests)
        {
            const auto ratio = ::Standard::Algorithms::ratio_compute(elapsed0, elapsed1);

            const auto report = "  size " + std::to_string(test.prices().size()) + ", tran " +
                std::to_string(test.allowed_transactions()) + ", t1 " + std::to_string(elapsed1) + ", t0 " +
                std::to_string(elapsed0) + ", t1/t0 " + std::to_string(ratio) + "\n";
            std::cout << report;
        }

        if (constexpr auto large = 50U; large < test.prices().size())
        {
            return;
        }

        {
            const auto slow_profit = Standard::Algorithms::Numbers::maximum_share_speculation_slow<int_t, long_int_t>(
                test.prices(), test.allowed_transactions(), indexes);

            ::Standard::Algorithms::ert::are_equal(fast_profit, slow_profit, "maximum_share_speculation_slow");
            // todo(p3): check indexes.
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::maximum_share_speculation_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
