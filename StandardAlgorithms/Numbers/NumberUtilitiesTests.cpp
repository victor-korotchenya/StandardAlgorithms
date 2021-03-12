#include <array>
#include <tuple>
#include <string>
#include "SumOfMin2ConsecutiveNumbers.h"
#include "NumberUtilities.h"
#include "NumberUtilitiesTests.h"
#include "../Assert.h"
#include "../Utilities/Random.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    void SumTest()
    {
        using Tuple = tuple<size_t, size_t, size_t>;

        const Tuple tests[] = { Tuple{ 0,0,0 },
            Tuple{ 0,1,1 },
            Tuple{ 0,2,3 },
            Tuple{ 0,9,45 },
            Tuple{ 0,10,55 },
            Tuple{ 1,10,55 },
            Tuple{ 2,10,54 },
            Tuple{ 3,10,52 },
            Tuple{ 4,10,49 },
            Tuple{ 5,10,45 },
        };

        for (const auto& test : tests)
        {
            const auto from = get<0>(test);
            const auto to = get<1>(test);
            const auto expected = get<2>(test);

            const auto actual = SumToN(from, to);

            const string name = "Sum" + to_string(from) + "_" + to_string(to);

            Assert::AreEqual(expected, actual, name);
        }
    }

    void modulo_inverse_simple_tests()
    {
        constexpr auto prime_number = 11;

        //number, expected.
        const vector<pair<int, int>> tests{
            make_pair(1, 1), make_pair(2, 6), make_pair(3, 4),
            make_pair(4, 3), make_pair(5, 9), make_pair(6, 2),
            make_pair(7, 8), make_pair(8, 7), make_pair(9, 5),
            make_pair(10, 10), make_pair(prime_number * 123456 + 7, 8)
        };
        vector<int> prime_factors;

        for (const auto& test : tests)
        {
            {
                const auto actual = modulo_inverse_simple<long long>(test.first, prime_number);
                const auto name = "modulo_inverse_simple(" + to_string(test.first) + ")";
                Assert::AreEqual(test.second, actual, name);
            } {
                const auto actual = modulo_inverse_slow_but_checked<int64_t, int, prime_number>(test.first, prime_factors);
                const auto name = "modulo_inverse_slow_but_checked(" + to_string(test.first) + ")";
                Assert::AreEqual(test.second, actual, name);
            } {
                const auto actual = modulo_inverse<long long>(test.first, prime_number);
                const auto name = "modulo_inverse(" + to_string(test.first) + ")";
                Assert::AreEqual(test.second, actual, name);
            }
        }
    }

    void Euler_phi_totient_tests()
    {
        //number, expected.
        const vector<pair<int, int>> tests{
            make_pair(1, 1), make_pair(2, 1), make_pair(3, 2),
            make_pair(4, 2), make_pair(36, 12),{ 12, 4 }, make_pair(50, 20),
            make_pair(51, 32), make_pair(52, 24), make_pair(53, 52),
            make_pair(54, 18), make_pair(55, 40), make_pair(56, 24)
        };

        const auto n_max = max_element(tests.begin(), tests.end())->first;
        const auto euler_phi_totients = euler_phi_totient_all<int64_t>(n_max);

        vector<int> prime_factors;

        for (const auto& test : tests)
        {
            const auto actual = Euler_phi_totient<int64_t>(test.first, prime_factors);
            const auto name = "Euler_phi_totient(" + to_string(test.first) + ")";
            Assert::AreEqual(test.second, actual, name);

            const auto& actual2 = euler_phi_totients[test.first];
            Assert::AreEqual(test.second, actual2, name + " _all");
        }
    }

    void get_sqrt_tests()
    {
        IntRandom r;
        const array data{ 999999824000007648LL,
            static_cast<int64_t>(r(0, 1000 * 1000 * 1000)) << 30 | r(0, 1000 * 1000 * 1000) };

        for (const auto& datum : data)
        {
            const auto q = get_sqrt(datum);
            const auto q2 = get_sqrt_slow(datum);
            const auto ok = q == q2 && q >= 0 && q <= datum && q * q <= datum && (q + 1ll) * (q + 1ll) > datum;
            Assert::AreEqual(true, ok, "datum=" + to_string(datum) + ", q=" + to_string(q) + ", q2=" + to_string(q2));
        }
    }

    void mobius_tests()
    {
        const auto mob = mobius(20);
        const vector<signed char> expected{ 0, 1, -1, -1, 0,//4
            -1,1,-1,0,0,1,//10
            -1,0,-1,1,1,0,-1,0,-1,0,//20
        };
        Assert::AreEqual(expected, mob, "Mobius");
    }

    void all_divisors_test()
    {
        vector<int> factors, factors2;
        unordered_set<int> s2;

        for (auto n = 1; n <= 64; ++n)
        {
            all_divisors(n, factors);

            all_divisors_slow(n, s2);
            factors2.clear();
            factors2.insert(factors2.begin(), s2.begin(), s2.end());

            sort(factors.begin(), factors.end());
            sort(factors2.begin(), factors2.end());

            Assert::AreEqual(factors, factors2, "all_divisors_test_" + to_string(n));
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void NumberUtilitiesTests()
                {
                    SumTest();
                    modulo_inverse_simple_tests();
                    Euler_phi_totient_tests();
                    get_sqrt_tests();
                    mobius_tests();
                    all_divisors_test();
                }
            }
        }
    }
}