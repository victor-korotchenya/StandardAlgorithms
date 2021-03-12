#include <memory>
#include <mutex>
#include <random>
#include <unordered_set>
#include "digit_and_sum_of_ab.h"
#include "digit_and_sum_of_ab_tests.h"
#include"factorials_cached.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    constexpr auto
#if _DEBUG
        n_max = 3, n_large = n_max,
#else
        n_max = 5, n_large = 3,
#endif
        attempts_max = 100,
        random_tests = 1;
    using number_t = unsigned;
    using number_t2 = uint64_t;
    using factorials_t = factorials_cached<number_t, number_t2>;

    shared_ptr<factorials_t> factorials_p;
    mutex mutex_factorials;

    // Do not call with 1,2,3... call with a larger number.
    shared_ptr<factorials_t> ensure_factorials(unsigned n)
    {
        if (n < 2)
            n = 2;

        {
            auto s_p = factorials_p;
            auto p = s_p.get();
            if (p && p->size() >= n)
                return s_p;
        }

        lock_guard<mutex> lock(mutex_factorials);
        auto s_p = factorials_p;
        auto p = s_p.get();
        if (!p || p->size() < n)
            factorials_p = s_p = make_shared<factorials_t>(n);
        return s_p;
    }

    shared_ptr<factorials_t> ensure_factorials_power10(unsigned n)
    {
        const auto p10 = power10_slow(n);
        const auto& f = ensure_factorials(p10);
        return f;
    }

    struct test_case final : base_test_case
    {
        const number_t n, expected;
        const char digit1, digit2, has_result;

        test_case(string&& name,
            number_t n, int digit1, int digit2, number_t expected, bool has_result = true)
            : base_test_case(forward<string>(name)),
            n(n),
            expected(expected),
            digit1(static_cast<char>(digit1)),
            digit2(static_cast<char>(digit2)),
            has_result(has_result)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(n, "n");
            RequirePositive(digit1, "digit1");
            RequirePositive(digit2, "digit2");
            Assert::GreaterOrEqual(9, digit1, "digit1");
            Assert::GreaterOrEqual(9, digit2, "digit2");
            RequireNonNegative(expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            PrintValue(str, "n", n);
            PrintValue(str, "digit1", int(digit1));
            PrintValue(str, "digit2", int(digit2));
            PrintValue(str, "has_result", int(has_result));
            if (has_result)
                PrintValue(str, "expected", expected);
        }
    };

    void gen_random(vector<test_case>& test_cases)
    {
        unordered_set<number_t> uniq;
        random_device rd;

        for (auto i = 0, attempt = 0; i < random_tests && attempt < attempts_max; ++attempt)
        {
            const auto a = static_cast<unsigned>(rd());
            const number_t n = a & 0x7;
            auto digit1 = char(a >> 3 & 15),
                digit2 = char(a >> 7 & 15);
            if (digit2 < digit1)
                swap(digit2, digit1);

            if (n <= 0 || n > n_max ||
                digit1 <= 0 || digit1 > 9 ||
                digit2 <= 0 || digit2 > 9)
                continue;

            const auto key = n ^
                static_cast<number_t>(digit1) << 20 ^
                static_cast<number_t>(digit2) << 21;
            if (!uniq.insert(key).second)
                continue;

            test_cases.emplace_back("random" + to_string(i), n, digit1, digit2, 0, false);
            ++i;
            attempt = 0;
        }
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        // n, digit1, digit2, expected)
#ifndef _DEBUG
        test_cases.emplace_back("base0", 6, 6, 3, 7); // ds(366_666)=33 +6, ds(666_666)=36 +1.
#endif
        test_cases.emplace_back("base1", 1, 1, 1, 1); // 1
        test_cases.emplace_back("base9", 1, 9, 9, 1); // 9
        test_cases.emplace_back("base49", 1, 4, 9, 2); // 4,9

        test_cases.emplace_back("base2", 3, 1, 1, 0);
        test_cases.emplace_back("base3", 3, 1, 3, 1); // 111
        test_cases.emplace_back("base4", 2, 1, 1, 0);

        //test_cases.emplace_back("base10", 10, 2, 3, 165); // 45 seconds in slow.
        //test_cases.emplace_back("base11", 11, 1, 1, 1); // 11111111111 todo: p4. make it run faster

        gen_random(test_cases);
    }

    void run_test_case(const test_case& test_case)
    {
        assert(test_case.n <= n_max);

        auto factorials_ptr = ensure_factorials_power10(static_cast<unsigned>(test_case.n));
        assert(factorials_ptr);
        const auto& factorials = *factorials_ptr.get();

        const auto actual = digit_and_sum_of_ab<number_t, number_t2, factorials_t>(factorials, test_case.n, test_case.digit1, test_case.digit2);
        if (test_case.has_result)
            Assert::AreEqual(test_case.expected, actual, "digit_and_sum_of_ab");

        const auto actual2 = digit_and_sum_of_ab_slow_still<number_t, number_t2, factorials_t>(factorials, test_case.n, test_case.digit1, test_case.digit2);
        Assert::AreEqual(actual, actual2, "digit_and_sum_of_ab_slow_still");

        if (test_case.n > n_large)
            return;

        const auto slow = digit_and_sum_of_ab_slow<number_t2>(test_case.n, test_case.digit1, test_case.digit2);
        Assert::AreEqual(actual, slow, "digit_and_sum_of_ab_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::digit_and_sum_of_ab_tests()
{
    ensure_factorials_power10(n_max);

    test_utilities<test_case>::run_tests<0>(run_test_case, generate_test_cases);
}