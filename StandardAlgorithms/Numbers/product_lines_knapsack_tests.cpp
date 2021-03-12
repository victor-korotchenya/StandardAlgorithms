#include "../Utilities/Random.h"
#include "product_lines_knapsack_tests.h"
#include "product_lines_knapsack.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/VectorUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = short;
    using int_t2 = int64_t;
    constexpr auto not_computed = -2, no_solution = -1;

    struct test_case final : base_test_case
    {
        vector<vector<int_t>> line_prices;
        int_t2 max_total_price;

        pair<int_t2, vector<int_t>> expected;

        test_case(string&& name,
            vector<vector<int_t>>&& line_prices,
            int_t2 max_total_price,
            int_t2 expected_price = not_computed,
            vector<int_t>&& chosen_prices = {})
            : base_test_case(forward<string>(name)),
            line_prices(forward<vector<vector<int_t>>>(line_prices)),
            max_total_price(max_total_price),
            expected({ expected_price, forward<vector<int_t>>(chosen_prices) })
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = line_prices.size();
            RequirePositive(size, "line_prices.size");

            for (auto i = 0u; i < size; ++i)
            {
                const auto& prices = line_prices[i];
                RequireAllPositive(prices, "line_prices[i]");
            }

            RequirePositive(max_total_price, "max_total_price");
            assert(max_total_price <= 1000);

            if (expected.first < 0)
                return;

            const auto& chosen_prices = expected.second;
            Assert::AreEqual(size, chosen_prices.size(), "chosen_prices.size");
            RequireAllPositive(chosen_prices, "chosen_prices");

            int_t2 sum{};
            for (auto i = 0u; i < size; ++i)
            {
                const auto& line = line_prices[i];
                const auto& price = chosen_prices[i];
                const auto it = find(line.begin(), line.end(), price);
                if (it == line.end())
                    throw runtime_error("Cannot find chosen price (" + to_string(price) +
                        ") in line_prices at " + to_string(i));
                sum += price;
            }

            Assert::AreEqual(sum, expected.first, "chosen price.");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("line_prices", line_prices, str);
            PrintValue(str, "max_total_price", max_total_price);
            PrintValue(str, "expected price sum", expected.first);
            ::Print("chosen_prices", expected.second, str);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "too large values",
            vector<vector<int_t>>{ {5,6,7} }, 4,
            no_solution, vector<int_t>{} });

        test_cases.push_back({ "too large values2",
            vector<vector<int_t>>{ {5,6,7}, {7,6,5} }, 9,
            no_solution, vector<int_t>{} });

        test_cases.push_back({ "exact min",
            vector<vector<int_t>>{ {5,6,7}, {7,6,5} }, 10,
            10, vector<int_t>{5, 5} });
        test_cases.push_back({ "exact max",
            vector<vector<int_t>>{ {2,6,7}, {8,6,5} }, 15,
            15, vector<int_t>{7, 8} });

        test_cases.push_back({ "in between",
            vector<vector<int_t>>{ {2,6,7}, {8,6,20} }, 12,
            12, vector<int_t>{6, 6} });

        test_cases.push_back({ "less",
            vector<vector<int_t>>{ {2,6,7}, {8,6,20} }, 11,
            10, vector<int_t>{2, 8} });

        test_cases.push_back({ "less2",
            vector<vector<int_t>>{ {6,2,7}, {8,6,20} }, 20,
            15, vector<int_t>{7, 8} });

        ShortRandom r;
        vector<vector<int_t>> line_prices;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 6, value_max = 10;
            const auto n = r(1, size_max);
            const auto sum = r(1, int_t(value_max * n));

            line_prices.resize(n);
            for (auto i = 0; i < n; ++i)
            {
                const auto m = r(1, size_max);
                auto& prices = line_prices[i];
                prices.resize(m);

                for (auto j = 0; j < m; ++j)
                    prices[j] = r(1, value_max);

                VectorUtilities::sort_remove_duplicates(prices);
                ShuffleArray(prices);
            }

            test_cases.emplace_back("random" + to_string(att), move(line_prices), sum);
        }
    }

    void verify_choices(const test_case& test, const pair<int_t2, vector<int_t>>& actual, const string& name)
    {
        const auto& sum_expected = actual.first;
        Assert::Greater(sum_expected, 0, name);

        const auto size = test.line_prices.size();
        const auto& chosen_prices = actual.second;
        Assert::AreEqual(size, chosen_prices.size(), name + ".size");

        const auto name_price = name + ".price at ";
        int_t2 sum{};

        for (auto i = 0u; i < size; ++i)
        {
            const auto& price = chosen_prices[i];
            const auto i_str = to_string(i);
            Assert::Greater(price, 0, name_price + i_str);

            const auto& prices = test.line_prices[i];
            assert(prices.size());

            const auto it = find(prices.begin(), prices.end(), price);
            if (it == prices.end())
                throw runtime_error("price (" + to_string(price) +
                    ") not found in line_prices at " + i_str);

            sum += price;
        }

        Assert::AreEqual(sum_expected, sum, name + ".sum");
    }

    void verify_result(const test_case& test, const pair<int_t2, vector<int_t>>& expected, const pair<int_t2, vector<int_t>>& actual, const char* const name)
    {
        Assert::AreEqual(expected.first, actual.first, name);
        if (expected.first < 0)
            return;

        assert(expected.first > 0);
        verify_choices(test, expected, name + string(".expected"));
        verify_choices(test, actual, name + string(".actual"));
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = product_lines_knapsack<int_t, int_t2>(test.line_prices, test.max_total_price);
        if (test.expected.first != not_computed)
            verify_result(test, test.expected, actual, "product_lines_knapsack.sum");

        const auto slow = product_lines_knapsack_slow<int_t, int_t2>(test.line_prices, test.max_total_price);
        verify_result(test, actual, slow, "product_lines_knapsack_slow.sum");
    }
}

void Privet::Algorithms::Numbers::Tests::product_lines_knapsack_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}