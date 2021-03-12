#include "../Utilities/Random.h"
#include "../test_utilities.h"
#include "eratosthenes_sieve.h"
#include "eratosthenes_sieve_tests.h"
#include "prime_number_utility.h"
#include "NumberUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    template<class int_t2, class int_t, class pair_t>
    void test_factoring(const vector<int_t>& min_prime_factors,
        vector<pair_t>& factor_powers,
        vector<pair_t>& factor_powers2,
        vector<int_t>& temp,
        vector<int_t>& temp2,
        const int_t a)
    {
        factor_powers.clear();
        decompose_eratosthenes_sieve_factoring(min_prime_factors, factor_powers, a);

        factor_powers2.clear();
        decompose_into_divisors_with_powers<int_t2, int_t, pair_t>(a, factor_powers2);

        Assert::AreEqual(factor_powers, factor_powers2, "decompose_eratosthenes_sieve_factoring " + to_string(a));

        assert(factor_powers2.size());
        temp.resize(factor_powers2.size());

        transform(factor_powers.begin(), factor_powers.end(), temp.begin(), [](const pair_t& p)
            {
                return p.first;
            });

        decompose_into_prime_divisors<int_t2, int_t>(a, temp2);
        Assert::AreEqual(temp, temp2, "decompose_into_prime_divisors " + to_string(a));
    }
}

namespace Privet::Algorithms::Numbers::Tests
{
    void eratosthenes_sieve_tests()
    {
        using int_t = int64_t;
        using long_int_t = int64_t;
        using pair_t = pair<int_t, int_t>;

        IntRandom r;
        const auto n = r(2, 100);
        const auto prime_factors = eratosthenes_sieve<long_int_t, int_t>(n);
        const auto& primes = prime_factors.first;
        const auto flags_slow = eratosthenes_sieve_slow<long_int_t, int_t>(n);
        const auto prefix = "eratosthenes_sieve " + to_string(n) + " ";

        vector<pair_t> factor_powers, factor_powers2;
        vector<int_t> temp, temp2;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const int_t a = r(2, n);
            const auto expected = is_prime_simple(a);
            const auto name = prefix + to_string(a);
            {
                const auto prime = prime_factors.second[a] == a;
                Assert::AreEqual(expected, prime, name);

                const auto it = lower_bound(primes.begin(), primes.end(), a);
                const auto has = it != primes.end() && *it == a;
                Assert::AreEqual(expected, has, "lower " + name);

                test_factoring<long_int_t, int_t>(prime_factors.second, factor_powers, factor_powers2, temp, temp2, a);
            }

            const auto slow = flags_slow[a];
            Assert::AreEqual(expected, slow, "slow " + name);
        }
    }

    void prime_number_count_tests()
    {
        using int_t = int64_t;
        using long_int_t = int_t;

        IntRandom r;
        unordered_map<int_t, int_t> m;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(2, 100);
            const auto simple = prime_number_count_simple<long_int_t, int_t>(n);
            const auto expected = simple;//todo: p2. faster.
            Assert::AreEqual(expected, simple, "prime_number_count_simple " + to_string(n));

            const auto slow = prime_number_count_slow<long_int_t, int_t>(m, n);
            Assert::AreEqual(expected, slow, "prime_number_count_slow " + to_string(n));
        }
    }
}