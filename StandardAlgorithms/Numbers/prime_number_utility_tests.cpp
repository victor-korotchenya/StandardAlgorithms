#include "../Utilities/Random.h"
#include "prime_number_utility_tests.h"
#include "prime_number_utility.h"
#include "eratosthenes_sieve_cache.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    constexpr auto n_max = 10007;

    void factorial_prime_max_power_tests()
    {
        auto s_ptr = eratosthenes_sieve_cache(n_max);
        assert(s_ptr);

        const auto& prime_numbers = s_ptr.get()->first;
        const auto size_minus_one = static_cast<int>(prime_numbers.size()) - 1;
        assert(size_minus_one == 1230 - 1);

        IntRandom r;
        string name;

        int n, prime, expected;

        for (auto att = 0, max_att = 2; att < max_att; ++att)
        {
            if (att)
            {
                n = r(1, n_max);

                const auto index = r(0, size_minus_one);
                prime = prime_numbers[index];
                assert(prime >= 2);
            }
            else
                n = 1000, prime = 5, expected = 249;

            name = "factorial_prime_max_power(" + to_string(n) + ","
                + to_string(prime) + ")";

            const auto actual = factorial_prime_max_power(n, prime);
            if (!att)
                Assert::AreEqual(expected, actual, name);

            const auto slow = factorial_prime_max_power_slow<int64_t>(n, prime);
            Assert::AreEqual(actual, slow, name + " slow");
        }
    }
}

void Privet::Algorithms::Numbers::Tests::prime_number_utility_tests()
{
    factorial_prime_max_power_tests();
}