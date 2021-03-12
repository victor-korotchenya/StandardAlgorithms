#include"coin_change_count_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"coin_change_count.h"
#include"default_modulus.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    constexpr auto not_computed = -1;

    constexpr auto mod = Standard::Algorithms::Numbers::default_modulus;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, const int_t &sum, std::vector<int_t> &&coins,
            const int_t &expected = not_computed) noexcept
            : base_test_case(std::move(name))
            , Coins(std::move(coins))
            , Sum(sum)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto coins() const &noexcept -> const std::vector<int_t> &
        {
            return Coins;
        }

        [[nodiscard]] constexpr auto sum() const &noexcept -> const int_t &
        {
            return Sum;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const int_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(static_cast<int_t>(Coins.size()), "coins size");

            for (int_t index{}; index < size; ++index)
            {
                const auto &coin = Standard::Algorithms::require_positive(Coins[index], "coin");

                if (int_t{} < index)
                {
                    ::Standard::Algorithms::ert::greater(coin, Coins[index - 1], "coin");
                }
            }

            if (Expected == not_computed)
            {
                return;
            }

            Standard::Algorithms::require_non_negative(Expected, "expected");
            Standard::Algorithms::require_greater(mod, Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("coins", Coins, str);
            ::Standard::Algorithms::print_value("sum", str, Sum);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<int_t> Coins;
        int_t Sum;
        int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "too large coins",
            // NOLINTNEXTLINE
            4, { 5, 6, 7 }, 0 });

        test_cases.push_back({ "base1",
            // NOLINTNEXTLINE
            4, { 1, 2, 3 }, 4 }); // 1111, 112, 22, 13.

        test_cases.push_back({ "base2",
            // NOLINTNEXTLINE
            10, { 2, 3, 5, 6 }, 5 }); // 22222, 2233, 226, 235, 55.


        constexpr auto value_min = 1;
        constexpr auto value_max = 10;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(value_min, value_max);
        std::set<int_t> uniq{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 6;

            const auto size = rnd(min_size, max_size);

            constexpr auto sum_min = 1;
            constexpr auto sum_max = 12;
            const auto sum = rnd(sum_min, sum_max);

            uniq.clear();

            for (;;)
            {
                const auto coin = rnd();

                if (!uniq.insert(coin).second)
                {
                    continue;
                }

                if (size == static_cast<int_t>(uniq.size()))
                {
                    break;
                }
            }

            std::vector<int_t> coins(uniq.size()); // std::set sorts.

            std::copy(uniq.cbegin(), uniq.cend(), coins.begin());

            test_cases.emplace_back("Random" + std::to_string(att), sum, std::move(coins));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::coin_change_count<int_t, mod>(test.coins(), test.sum());

        if (test.expected() != not_computed)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "coin_change_count");
        }

        {
            const auto slow_still = Standard::Algorithms::Numbers::coin_change_count_slow_still<int_t, mod>(
                test.coins(), test.sum(), static_cast<int_t>(test.coins().size()) - 1);

            ::Standard::Algorithms::ert::are_equal(fast, slow_still, "coin_change_count_slow_still");
        }
        {
            const auto slow =
                Standard::Algorithms::Numbers::coin_change_count_slow<long_int_t, int_t, mod>(test.coins(), test.sum());
            ::Standard::Algorithms::ert::are_equal(fast, slow, "coin_change_count_slow");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::coin_change_count_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
