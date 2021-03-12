#include <random>
#include "digit_sum.h"
#include "digit_sum_tests.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using number_t = int64_t;

    void digit_sum_1(vector<vector<number_t>>& cache,
        vector<char>& digits,
        const vector<number_t>& prepared_sums,
        const string& n_str, const number_t n)
    {
        const auto actual = digit_sum(prepared_sums, n);
        const auto actual_slow = digit_sum_slow(n);
        Assert::AreEqual(actual, actual_slow, "digit_sum " + n_str);

        const auto actual2 = digit_sum2<number_t, number_t>(cache, digits, n);
        Assert::AreEqual(actual, actual2, "digit_sum2 " + n_str);
    }

    void digit_sum_8(const vector<number_t>& prepared_sums,
        const string& n_str, const string& to_str,
        const number_t n, const number_t to)
    {
        const auto actual = digit_sum_range(prepared_sums, n, to);
        const auto actual_slow = digit_sum_range_slow(n, to);
        Assert::AreEqual(actual, actual_slow, "digit_sum_range " + n_str + " ..  " + to_str);
    }

    void digit_sum_small(const vector<number_t>& prepared_sums,
        vector<vector<number_t>>& cache, vector<char>& digits, random_device& rd)
    {
        for (auto att = 0, max_atts = 3; att < max_atts; ++att)
        {
            constexpr auto value_max = 10 * 1000u;
            const auto n = static_cast<number_t>(att
                ? static_cast<unsigned>(rd()) % value_max
                : value_max);

            const auto n_str = to_string(n);
            digit_sum_1(cache, digits, prepared_sums, n_str, n);

            const auto to = n + static_cast<number_t>(static_cast<unsigned>(rd()) % value_max);
            const auto to_str = to_string(n);
            digit_sum_8(prepared_sums, n_str, to_str, n, to);
        }
    }

    void digit_sum_large(const vector<number_t>& prepared_sums,
        vector<vector<number_t>>& cache, vector<char>& digits, random_device& rd)
    {
        for (auto att = 0, max_atts = 3; att < max_atts; ++att)
        {
            constexpr unsigned del_sign_mask = 0x7fFfFfFf;
            const unsigned t0 = static_cast<unsigned>(rd()) & del_sign_mask,
                t1 = rd();
            const auto n = static_cast<number_t>(t0) << 25 | t1;
            assert(n >= 0);

            const auto actual = digit_sum(prepared_sums, n);
            const auto actual2 = digit_sum2<number_t, number_t>(cache, digits, n);
            const auto n_str = to_string(n);
            Assert::AreEqual(actual, actual2, "digit_sum2 " + n_str);
#if _DEBUG
            digit_sum2_rec_calls = 0;
            digit_sum2<number_t, number_t>(cache, digits, n);
            Assert::Greater(10 * max_number_digits, digit_sum2_rec_calls, "digit_sum2_rec_calls " + n);
#endif
        }
    }
}

void Privet::Algorithms::Numbers::Tests::digit_sum_tests()
{
    const auto prepared_sums = prepare_digit_sum<number_t>();
    vector<char> digits;
    random_device rd;
    auto cache = digit_sum2_cache<number_t>();

    digit_sum_small(prepared_sums, cache, digits, rd);
    digit_sum_large(prepared_sums, cache, digits, rd);
}