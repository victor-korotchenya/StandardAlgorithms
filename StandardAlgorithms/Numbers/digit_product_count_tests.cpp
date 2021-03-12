#include <random>

#define sorted_slow 1

#if sorted_slow
#include <map>
#else
#include <unordered_map>
#include "hash_utilities.h"
#endif

#include "digit_product_count.h"
#include "digit_product_count_tests.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using number_t = int64_t;
    using tuple_t = tuple<number_t, int, char>;

#if sorted_slow
    using map_t = map<tuple_t, number_t>;
#else
    using map_t = unordered_map<tuple_t, number_t, tuple_hash>;
#endif

    number_t run1(vector<char>& digits, map_t& temp,
        const string& n_str, const number_t n, const number_t prod,
        const bool use_slow = true)
    {
        if (!prod) // todo: p2. Solve for 0 product.
            return 0;

        const auto prod_str = to_string(prod);
        const auto actual = digit_product_count<number_t>(n, prod, digits, temp);
        if (!use_slow)
            return actual;

        const auto slow = digit_product_count_slow(n, prod);
        Assert::AreEqual(actual, slow, "digit_product_count " + n_str + ", " + prod_str);
        return actual;
    }

    int64_t eval_prod(random_device& rd, const number_t n, const number_t hi)
    {
        assert(n >= 0 && hi > 0);
        if (n <= 9)
            return n * hi;

        number_t pr = 1, cop = n;
        do
        {
            if (cop <= 9)
            {
                pr *= cop > 1 ? cop - 1 : 1;
                break;
            }

            const auto r = rd() & 3;
            if (!r)
                break;

            pr *= r > 1 ? 9 : cop % 9 + 1;
            cop /= 10;
        } while (cop > 0);

        return pr * hi;
    }

    void specific_subtests(
        const vector<pair<pair<number_t, number_t>, number_t>> n_results,
        vector<char>& digits, map_t& temp)
    {
        for (const auto& n_r : n_results)
        {
            const auto& n = n_r.first.first,
                & prod = n_r.first.second,
                & expected = n_r.second;
            const auto n_str = to_string(n);
            const auto actual = run1(digits, temp, n_str, n, prod, n < 10000);
            Assert::AreEqual(expected, actual, n_str);
        }
    }

    void run_specific(vector<char>& digits, map_t& temp)
    {
        const vector<pair<pair<number_t, number_t>, number_t>> n_results{
            { { 100, 5 }, 3 }, // 5, 15, 51
            { { 9876, 10 }, 20 },//25,52,125,152,215,251,512,521,
            //1125,1152,1215,1251,1512,1521,
            //2115,2151,2511,
            //5112,5121,5211.
        };
        specific_subtests(n_results, digits, temp);
    }

    void run_many(vector<char>& digits, map_t& temp)
    {
        for (auto n = 0; n <= 12; ++n)
        {
            const auto n_str = to_string(n);
            for (auto prod = 0; prod <= 10; ++prod)
                run1(digits, temp, n_str, n, prod);
        }
    }

    void run_random(vector<char>& digits, map_t& temp)
    {
        random_device rd;

        for (auto att = 0, att_max = 3; att < att_max; ++att)
        {
            constexpr unsigned mask = 0xFfFf;
            const auto a = static_cast<unsigned>(rd());
            const auto n = static_cast<number_t>(a & mask),
                hi = static_cast<number_t>((a >> 28) & 7 | 1),
                prod = eval_prod(rd, n, hi);
            const auto n_str = to_string(n);
            run1(digits, temp, n_str, n, prod);
        }
    }

    void run_large(vector<char>& digits, map_t& temp)
    {
        constexpr number_t prod0 = 8 * 125,
#if !_DEBUG
            prod2 = 840000,
#endif
            k3 = 1000, k18 = k3 * k3 * k3 * k3 * k3 * k3,
            som = k18 / 10 - 1234567;
        const vector<pair<pair<number_t, number_t>, number_t>> n_results{
            {{k18, prod0 }, 1596912 },
            {{som, prod0 }, 1042032 },
#if !_DEBUG
            {{k18, prod2 }, 14867793360 },
#endif
        };
        specific_subtests(n_results, digits, temp);
    }
}

void Privet::Algorithms::Numbers::Tests::digit_product_count_tests()
{
    vector<char> digits;
    map_t temp;

    run_specific(digits, temp);
    run_random(digits, temp);

#if !_DEBUG
    run_many(digits, temp);
    run_large(digits, temp);
#endif
}