#include "maximum_share_speculation.h"
#include "maximum_share_speculation_tests.h"
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"
#include "../elapsed_time_ns.h"

//#define print_maximum_share_speculation_tests 1

#if print_maximum_share_speculation_tests
#include<iostream>
#endif

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = int;
    using int_t2 = int64_t;
    constexpr auto not_computed = -1;

    struct test_case final : base_test_case
    {
        vector<int_t> prices;
        size_t allowed_transactions;
        int_t2 expected_profit;

        test_case(string&& name, vector<int_t>&& prices, size_t allowed_transactions, int_t2 expected_profit = not_computed)
            : base_test_case(forward<string>(name)),
            prices(forward<vector<int_t>>(prices)),
            allowed_transactions(allowed_transactions),
            expected_profit(expected_profit)
        {
            Assert::GreaterOrEqual(allowed_transactions, 0, "allowed_transactions");
            Assert::GreaterOrEqual(expected_profit, not_computed, "expected_profit");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print(", prices=", prices, str);
            str << "allowed_transactions=" << allowed_transactions
                << ", expected_profit=" << expected_profit;
        }
    };

    void generate_test_cases(vector<test_case>& tests)
    {
        tests.emplace_back("base1 2 tr", vector<int_t>{100, 5, 40, 1, 9, 11, 5, 26, 15}, 2, 60);

        tests.emplace_back("empty 0", vector<int_t>{}, 0, 0);
        tests.emplace_back("empty 1", vector<int_t>{}, 1, 0);
        tests.emplace_back("1 price", vector<int_t>{3}, 1, 0);
        tests.emplace_back("2 prices 0 tr", vector<int_t>{3, 40}, 0, 0);
        tests.emplace_back("2 prices 1 tr", vector<int_t>{3, 40}, 1, 37);
        tests.emplace_back("2 prices 2 tr", vector<int_t>{3, 40}, 2, 37);
        tests.emplace_back("2 prices many tr", vector<int_t>{3, 40}, 5, 37);
        tests.emplace_back("-2 prices 1 tr", vector<int_t>{40, 3}, 1, 0);
        tests.emplace_back("-2 prices 2 tr", vector<int_t>{40, 3}, 2, 0);
        tests.emplace_back("5 prices 0 tr", vector<int_t>{40, 3, 10, 5, 25}, 0, 0);
        tests.emplace_back("5 prices 1 tr", vector<int_t>{40, 3, 10, 5, 25}, 1, 22);
        tests.emplace_back("5 prices 2 tr", vector<int_t>{40, 3, 10, 5, 25}, 2, 27);
        tests.emplace_back("5 prices 3 tr", vector<int_t>{40, 3, 10, 5, 25}, 3, 27);
        tests.emplace_back("6 prices 0 tr", vector<int_t>{40, 0, 9, 10, 5, 25}, 0, 0);
        tests.emplace_back("6 prices 1 tr", vector<int_t>{40, 0, 9, 10, 5, 25}, 1, 25);
        tests.emplace_back("6 prices 2 tr", vector<int_t>{40, 0, 9, 10, 5, 25}, 2, 30);
        tests.emplace_back("6 prices 3 tr", vector<int_t>{40, 0, 9, 10, 5, 25}, 3, 30);
        tests.emplace_back("6 prices 10 tr", vector<int_t>{40, 0, 9, 10, 5, 25}, 10, 30);
        tests.emplace_back("7 prices 1 tr", vector<int_t>{5, 40, 0, 9, 10, 5, 25}, 1, 35);
        tests.emplace_back("7 prices 3 tr", vector<int_t>{5, 40, 0, 9, 10, 5, 25}, 3, 65);
        tests.emplace_back("7 prices 4 tr", vector<int_t>{5, 40, 0, 9, 10, 5, 25}, 4, 65);
        tests.emplace_back("7 prices 8 tr", vector<int_t>{5, 40, 0, 9, 10, 5, 25}, 8, 65);

        constexpr auto min_size = 0, max_size = 40, max_tran = 20;
        IntRandom r;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            auto prices = random_vector<int_t>(r, min_size, max_size);
            const auto m = r(0, max_tran);

            tests.emplace_back("random" + to_string(att), move(prices), m);
        }
    }

    void run_test_case(const test_case& test)
    {
        int_t2 actual_profit{};
        vector<pair<size_t, size_t>> actual_indexes;

        elapsed_time_ns t0;
        maximum_share_speculation<int_t, int_t2>(test.prices, test.allowed_transactions, actual_profit, actual_indexes);
        const auto elapsed0 = t0.elapsed();

        if (test.expected_profit >= 0)
        {
            Assert::AreEqual(test.expected_profit, actual_profit, "maximum_share_speculation");
            // todo: p2 indexes.
        }

        remove_cvref_t<decltype(elapsed0)> elapsed1;
        {
            int_t2 slow{};
            vector<pair<size_t, size_t>> indexes;

            elapsed_time_ns t1;
            maximum_share_speculation_simple<int_t, int_t2>(test.prices, test.allowed_transactions, slow, indexes);
            elapsed1 = t1.elapsed();

            Assert::AreEqual(actual_profit, slow, "maximum_share_speculation_simple");
            // todo: p2 indexes.
        }

#if print_maximum_share_speculation_tests
        {
            const auto ratio = elapsed0 > 0 ? static_cast<double>(elapsed1) / elapsed0 : 0.0;
            const auto report = "  _size "s + to_string(test.prices.size()) +
                ", tran " + to_string(test.allowed_transactions) +
                ", t1 " + to_string(elapsed1) +
                ", t0 " + to_string(elapsed0) +
                ", t1/t0 " + to_string(ratio) +
                "\n";
            cout << report;
        }
#endif

        if (test.prices.size() > 50)
            return;

        {
            int_t2 slow{};
            vector<pair<size_t, size_t>> indexes;

            maximum_share_speculation_slow<int_t, int_t2>(test.prices, test.allowed_transactions, slow, indexes);

            Assert::AreEqual(actual_profit, slow, "maximum_share_speculation_slow");
            // todo: p2 indexes.
        }
    }
}

void Privet::Algorithms::Numbers::Tests::maximum_share_speculation_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}