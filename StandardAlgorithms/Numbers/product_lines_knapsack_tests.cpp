#include"product_lines_knapsack_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/remove_duplicates.h"
#include"../Utilities/test_utilities.h"
#include"product_lines_knapsack.h"

namespace
{
    using int_t = std::int16_t;
    using long_int_t = std::int64_t;

    constexpr long_int_t no_solution = -1;
    constexpr long_int_t not_computed = -2;

    constexpr void verify_choices(const std::string &name, const long_int_t &max_total_price,
        const std::vector<std::vector<int_t>> &line_prices, const std::pair<long_int_t, std::vector<int_t>> &sum_prices)
    {
        assert(!name.empty());

        const auto &[sum_expected, chosen_prices] = sum_prices;
        {
            constexpr long_int_t one = 1;

            Standard::Algorithms::require_between(one, sum_expected, max_total_price, name + " chosen sum");
        }

        const auto size = line_prices.size();
        ::Standard::Algorithms::ert::are_equal(size, chosen_prices.size(), name + " size");

        long_int_t sum{};

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &prices = line_prices[index];
            const auto &chosen = chosen_prices[index];

            if (const auto ite = std::find(prices.cbegin(), prices.cend(), chosen); ite == prices.cend()) [[unlikely]]
            {
                auto err = "Chosen price (" + std::to_string(chosen) + ") not found in the line prices at " +
                    std::to_string(index) + ". " + name;

                throw std::runtime_error(err);
            }

            const auto old = sum;
            sum += chosen;
            Standard::Algorithms::require_greater(sum, old, name + " partial sum");
        }

        ::Standard::Algorithms::ert::are_equal(sum_expected, sum, name + " chosen total sum");
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, long_int_t max_total_price,
            std::vector<std::vector<int_t>> &&line_prices, long_int_t expected_price,
            std::vector<int_t> &&chosen_prices = {}) noexcept
            : base_test_case(std::move(name))
            , Max_total_price(max_total_price)
            , Line_prices(std::move(line_prices))
            , Expected({ expected_price, std::move(chosen_prices) })
        {
        }

        [[nodiscard]] constexpr auto max_total_price() const &noexcept -> const long_int_t &
        {
            return Max_total_price;
        }

        [[nodiscard]] constexpr auto line_prices() const &noexcept -> const std::vector<std::vector<int_t>> &
        {
            return Line_prices;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::pair<long_int_t, std::vector<int_t>> &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::Numbers::check_prod_lin_price_total(Max_total_price);

            Standard::Algorithms::Numbers::check_prod_lin_prices_many(Line_prices);

            const auto &[sum_expected, chosen_prices] = Expected;
            if (sum_expected < long_int_t{})
            {
                return;
            }

            verify_choices("validate", Max_total_price, Line_prices, Expected);
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("max total price", str, Max_total_price);
            ::Standard::Algorithms::print("line prices", Line_prices, str);
            ::Standard::Algorithms::print_value("expected price sum", str, Expected.first);
            ::Standard::Algorithms::print("expected chosen prices", Expected.second, str);
        }

private:
        long_int_t Max_total_price;
        std::vector<std::vector<int_t>> Line_prices;
        std::pair<long_int_t, std::vector<int_t>> Expected;
    };

    constexpr void verify_result(const char *const name, const std::pair<long_int_t, std::vector<int_t>> &expected,
        const test_case &test, const std::pair<long_int_t, std::vector<int_t>> &actual)
    {
        ::Standard::Algorithms::ert::are_equal(expected.first, actual.first, name);

        if (expected.first < long_int_t{})
        {
            return;
        }

        verify_choices(name + std::string(" expected"), test.max_total_price(), test.line_prices(), expected);

        verify_choices(name + std::string(" actual"), test.max_total_price(), test.line_prices(), actual);
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("too large values", // NOLINTNEXTLINE
            4,
            std::vector<std::vector<int_t>> // NOLINTNEXTLINE
            { { 10, 5, 7 } },
            no_solution);

        test_cases.emplace_back("too large values2", // NOLINTNEXTLINE
            9,
            std::vector<std::vector<int_t>> // NOLINTNEXTLINE
            { { 5, 6, 7 }, { 7, 6, 5 } },
            no_solution);

        test_cases.emplace_back("tre", // NOLINTNEXTLINE
            23,
            std::vector<std::vector<int_t>> // NOLINTNEXTLINE
            { { 3, 10, 4 }, { 3, 10, 5 }, { 2, 20 } }, // NOLINTNEXTLINE
            22, std::vector<int_t>{ 10, 10, 2 });

        test_cases.emplace_back("exact min", // NOLINTNEXTLINE
            10,
            std::vector<std::vector<int_t>> // NOLINTNEXTLINE
            { { 5, 6, 7 }, { 7, 6, 5 } },
            // NOLINTNEXTLINE
            10, std::vector<int_t>{ 5, 5 });

        test_cases.emplace_back("exact max", // NOLINTNEXTLINE
            15,
            std::vector<std::vector<int_t>> // NOLINTNEXTLINE
            { { 2, 6, 7 }, { 8, 6, 5 } }, // NOLINTNEXTLINE
            15, std::vector<int_t>{ 7, 8 });

        test_cases.emplace_back("in between", // NOLINTNEXTLINE
            12,
            std::vector<std::vector<int_t>> // NOLINTNEXTLINE
            { { 2, 6, 7 }, { 8, 6, 20 } }, // NOLINTNEXTLINE
            12, std::vector<int_t>{ 6, 6 });

        test_cases.emplace_back("less", // NOLINTNEXTLINE
            11,
            std::vector<std::vector<int_t>> // NOLINTNEXTLINE
            { { 2, 6, 7 }, { 8, 6, 20 } }, // NOLINTNEXTLINE
            10, std::vector<int_t>{ 2, 8 });

        test_cases.emplace_back("less2", // NOLINTNEXTLINE
            20,
            std::vector<std::vector<int_t>> // NOLINTNEXTLINE
            { { 6, 2, 7 }, { 8, 6, 20 } }, // NOLINTNEXTLINE
            15, std::vector<int_t>{ 7, 8 });

        constexpr int_t value_min = 1;
        constexpr int_t value_max =
            ::Standard::Algorithms::is_debug ? 10 : Standard::Algorithms::Numbers::prod_lin_up_price;

        static_assert(int_t{} < value_min && value_min <= value_max &&
            value_max <= Standard::Algorithms::Numbers::prod_lin_up_price);

        Standard::Algorithms::Utilities::random_t<int_t> rnd(value_min, value_max);
        auto gena = [&rnd]
        {
            return rnd();
        };

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr int_t min_size = 1;
            constexpr int_t max_size = 6;

            static_assert(int_t{} < min_size && min_size <= max_size &&
                max_size <= static_cast<int_t>(Standard::Algorithms::Numbers::prod_lin_max_size));

            constexpr int_t sum_max = value_max * max_size;

            static_assert(value_max <= sum_max && max_size <= sum_max &&
                sum_max <= Standard::Algorithms::Numbers::prod_lin_up_price_total);

            const auto line_count = rnd(min_size, max_size);
            const auto sum = rnd(value_min, static_cast<int_t>(value_max * line_count));

            std::vector<std::vector<int_t>> line_prices(line_count);

            for (auto &prices : line_prices)
            {
                const auto prices_size = rnd(min_size, max_size);
                prices.resize(prices_size);
                std::generate(prices.begin(), prices.end(), gena);

                Standard::Algorithms::sort_remove_duplicates(prices);
                Standard::Algorithms::Utilities::shuffle_data(prices);
            }

            test_cases.emplace_back("Random" + std::to_string(att), sum, std::move(line_prices), not_computed);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::product_lines_knapsack<int_t, long_int_t>(
            test.line_prices(), test.max_total_price());

        if (test.expected().first != not_computed)
        {
            verify_result("product_lines_knapsack", test.expected(), test, fast);
        }

        const auto slow = Standard::Algorithms::Numbers::product_lines_knapsack_slow<int_t, long_int_t>(
            test.line_prices(), test.max_total_price());

        verify_result("product_lines_knapsack_slow", fast, test, slow);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::product_lines_knapsack_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
