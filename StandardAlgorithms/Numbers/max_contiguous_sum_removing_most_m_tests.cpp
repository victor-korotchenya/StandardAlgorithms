#include<chrono>
#include "../Utilities/Random.h"
#include "max_contiguous_sum_removing_most_m_tests.h"
#include "max_contiguous_sum_removing_most_m.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "../elapsed_time_ns.h"

using namespace std::chrono;
using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

//#define load_tests 1

namespace
{
    using int_t = int;
    using long_int_t = int64_t;
    constexpr long_int_t not_computed = -1;

#if load_tests
    constexpr auto power2_max = 10;

    atomic<int64_t> me_win, they_win;
    atomic<double> me_ratio_max, they_ratio_max;
#else
    constexpr auto power2_max = 10;
#endif

    struct test_case final : base_test_case
    {
        const vector<int_t> ar;
        const int_t remove_max;
        const long_int_t expected;

        test_case(string&& name,
            vector<int_t>&& ar,
            int_t remove_max,
            long_int_t expected)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<int_t>>(ar)),
            remove_max(remove_max),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(ar.size(), "ar.size");
            RequirePositive(remove_max, "remove_max");
            if (expected > not_computed)
                RequireNonNegative(expected, "expected");
            RequireNotGreater(remove_max, static_cast<int_t>(ar.size()), "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("ar", ar, str);
            PrintValue(str, "remove_max", remove_max);
            PrintValue(str, "expected", expected);
        }
    };

    void gen_add_random_test(vector<test_case>& test_cases, const string& prefix, IntRandom& r, const int att, const int n)
    {
        assert(n > 0);

        vector<int_t> ar(n);
        for (auto i = 0; i < n; ++i)
        {
            const auto value_max =
#if load_tests
                1000;
#else
                30;
#endif
            ar[i] = r(-value_max, value_max);
        }

        const auto remove_max = r(1, n);
        test_cases.push_back({ prefix + to_string(att), move(ar), remove_max, not_computed });
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "remove not consecutive <0 numbers, 3",
        { 6, -100, 5, -2, -3, 10, -5, -10, -100, 150 }, 3, 161 });
        //6,     , 5, -2, -3, 10, -5,    ,     , 150 }

        test_cases.push_back({ "remove not consecutive <0 numbers",
        { 3, -100, 6, -1, 10, -100, 2 }, 2, 20 });
        //3,     , 6, -1, 10,     , 2 }

        test_cases.push_back({ "0", { 0 }, 1, 0 });
        test_cases.push_back({ "1", { 1 }, 1, 1 });
        test_cases.push_back({ "-1", { -1 }, 1, 0 });
        test_cases.push_back({ "all < 0", { -1, -12, -34 }, 3, 0 });
        test_cases.push_back({ "all >= 0", { 1, 0, 34 }, 3, 35 });
        test_cases.push_back({ "inside > 0", { -1, 0, 1, 34, 0, -2 }, 1, 35 });
        test_cases.push_back({ "inside > 0, 2", { -1, 0, 1, 34, 0, -2 }, 2, 35 });
        test_cases.push_back({ "negative <= remove_max", { -1, 2, -5, -17, 7, 0, -2 }, 2, 9 });

        test_cases.push_back({ "base1", { -2, 1, 3, -2, 4, -7, 20 }, 1, 26 });
        test_cases.push_back({ "base2", { -2, 1, 3, -2, 4, -7, 20 }, 2, 28 });
        test_cases.push_back({ "base3", { -2, 1, 3, -2, 4, -7, 20 }, 3, 28 });
        test_cases.push_back({ "base4", { -2, 1, 3, -2, 4, -7, 20 }, 4, 28 });
        test_cases.push_back({ "base7", { -2, 1, 3, -2, 4, -7, 20 }, 7, 28 });

        test_cases.push_back({ "base11", { -1, 1, -1, -1, 1, 1 }, 1, 2 });
        test_cases.push_back({ "base12", { -1, 1, -1, -1, 1, 1 }, 2, 3 });
        test_cases.push_back({ "base16", { -1, 1, -1, -1, 1, 1 }, 6, 3 });

        IntRandom r;
#if load_tests
        constexpr auto n_max = 10000;
        {
            const string stat_prefix = "stat";
            for (int_t n = 10; n <= n_max; n += 10)
            {
                gen_add_random_test(test_cases, stat_prefix, r, n, n);
            }
        }
#endif

        const string random_prefix = "random";
        for (auto att = 0, att_max =
#if load_tests
            1000
#else
            1
#endif
            ; att < att_max; ++att)
        {
            const auto n = r(
#if load_tests
                n_max / 10, n_max
#else
                1, 8
#endif
            );
            gen_add_random_test(test_cases, random_prefix, r, att, n);
        }

#if load_tests
        puts(" max_contiguous_sum_removing_most_m_tests gen completed.");
#endif
    }

    void run_test_case(const test_case& test_case)
    {
        elapsed_time_ns t0;
        const auto actual = max_contiguous_sum_removing_most_m<long_int_t, int_t>(test_case.ar, test_case.remove_max);
        const auto elapsed0 = t0.elapsed();

        if (test_case.expected > not_computed)
            Assert::AreEqual(test_case.expected, actual, "max_contiguous_sum_removing_most_m");

        elapsed_time_ns t1;
        const auto they = max_contiguous_sum_removing_most_m_they<long_int_t, int_t>(test_case.ar, test_case.remove_max);
        const auto elapsed1 = t1.elapsed();

        Assert::AreEqual(actual, they, "max_contiguous_sum_removing_most_m_they");

        if (elapsed0 > 0 && elapsed1 > 0)
        {
#if load_tests
            const auto ratio = elapsed1 * 1.0 / elapsed0;
            if (ratio > 1.0)
            {
                me_win.fetch_add(1, memory_order_relaxed);

                double m;
                do
                {
                    m = me_ratio_max.load();
                    if (ratio <= m)
                        break;
                }                 while (!me_ratio_max.compare_exchange_weak(m, ratio, memory_order_release, memory_order_relaxed));
            }
            else if (ratio < 1.0)
            {
                they_win.fetch_add(1, memory_order_relaxed);

                const auto ratio_rev = 1.0 / ratio;
                double m;
                do
                {
                    m = they_ratio_max.load();
                    if (ratio_rev <= m)
                        break;
                } while (!they_ratio_max.compare_exchange_weak(m, ratio_rev, memory_order_release, memory_order_relaxed));
            }

            const auto report = "n" + to_string(test_case.ar.size())
                + ", de " + to_string(test_case.remove_max)

                + ", t/m " + to_string(ratio)
                + ", t " + to_string(they_win.fetch_add(0, memory_order_relaxed))
                + ", m " + to_string(me_win.fetch_add(0, memory_order_relaxed))

                + "; th r " + to_string(they_ratio_max.load(memory_order_relaxed))
                + ", m r " + to_string(me_ratio_max.load(memory_order_relaxed))
                + ", t t " + to_string(elapsed1)
                + ", m t " + to_string(elapsed0)
                ;
            puts(report.data());
#endif
        }

        if (test_case.ar.size() > power2_max)
            return;

        const auto slow = max_contiguous_sum_removing_most_m_slow<long_int_t, int_t>(test_case.ar, test_case.remove_max);
        Assert::AreEqual(actual, slow, "max_contiguous_sum_removing_most_m_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::max_contiguous_sum_removing_most_m_tests()
{
    test_utilities<test_case>::run_tests
#if load_tests
        <0>
#endif
        (run_test_case, generate_test_cases);
}