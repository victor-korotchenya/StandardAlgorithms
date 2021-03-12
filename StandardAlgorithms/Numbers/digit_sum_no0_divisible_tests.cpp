#include <random>

//#define sorted_slow 1

#if sorted_slow
#include <map>
#else
#include <unordered_map>
#include "hash_utilities.h"
#endif

#include "digit_sum_no0_divisible.h"
#include "digit_sum_no0_divisible_tests.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using number_t = int64_t;
    using tuple_t = tuple<number_t, number_t, int, char>;

#if sorted_slow
    using map_t = map<tuple_t, number_t>;
#else
    using map_t = unordered_map<tuple_t, number_t, tuple_hash>;
#endif

    struct test_case final : base_test_case
    {
        const number_t n, sum, divisor, expected;

        test_case(string&& name,
            number_t n, number_t sum, number_t divisor, number_t expected)
            : base_test_case(forward<string>(name)),
            n(n),
            sum(sum),
            divisor(divisor),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(n, "n");
            RequirePositive(sum, "sum");
            RequirePositive(divisor, "divisor");
            RequireNonNegative(expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            PrintValue(str, "n", n);
            PrintValue(str, "sum", sum);
            PrintValue(str, "divisor", divisor);
            PrintValue(str, "expected", expected);
        }
    };

    void gen1(map_t& uniq, random_device& rd,
        vector<char>& digits, map_t& m,
        number_t& n, number_t& sum, number_t& divisor, number_t& expected)
    {
        number_t attempts = 0;
        for (;;)
        {
            const auto a = static_cast<unsigned>(rd());
            if (n <= 0)
            {
                n = a & 0x3fFf;
                continue;
            }

            if (sum <= 0)
            {
                sum = a & 0xf;
                continue;
            }

            if (divisor <= 0)
            {
                divisor = a & 0x3f;
                continue;
            }

            expected = digit_sum_no0_divisible<number_t>(m, digits, n, sum, divisor);
            const tuple_t key = { n, sum, static_cast<int>(divisor), char(0) };
            if (expected > 0 && uniq.insert({ key, expected }).second)
                break;

            if (++attempts > 100)
                throw runtime_error("Too many random gen attempts " + to_string(attempts));

            n = sum = divisor = 0;
        }
    }

    void gen_random(vector<test_case>& test_cases)
    {
        map_t uniq;

        random_device rd;
        vector<char> digits;
        map_t m;

        for (auto i = 0; i < 1; ++i)
        {
            number_t n = 0, sum = 0, divisor = 0, expected = 0;
            gen1(uniq, rd, digits, m, n, sum, divisor, expected);

            test_cases.emplace_back("random" + to_string(i), n, sum, divisor, expected);
        }
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        // n, sum, divisor, expected)
        test_cases.emplace_back("base1", 100, 9, 4, 2);// 36, 72
        test_cases.emplace_back("base2", 100, 8, 2, 4);// 8, 26, 44, 62
        test_cases.emplace_back("base3", 200, 4, 11, 2);// 22, 121

        gen_random(test_cases);
    }

    void run_test_case(const test_case& test_case)
    {
        vector<char> digits;
        map_t m;
        const auto actual = digit_sum_no0_divisible(m, digits, test_case.n, test_case.sum, test_case.divisor);
        Assert::AreEqual(test_case.expected, actual, "digit_sum_no0_divisible");

        const auto slow = digit_sum_no0_divisible_slow(test_case.n, test_case.sum, test_case.divisor);
        Assert::AreEqual(test_case.expected, slow, "digit_sum_no0_divisible_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::digit_sum_no0_divisible_tests()
{
    test_utilities<test_case>::run_tests<0>(run_test_case, generate_test_cases);
}