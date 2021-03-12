#include<unordered_map>
#include "quadratic_residue.h"
#include "quadratic_residue_tests.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    unordered_map<int, int> precalc_roots(const int odd_prime)
    {
        unordered_map<int, int> m;
        for (auto i = 1; i < odd_prime; ++i)
        {
            const auto p = i * i, r = p % odd_prime;
            auto& v = m[r];
            if (!v)
                v = i;
        }
        return m;
    }

    void euler_criterion_tests(const int odd_prime)
    {
        const auto qs = precalc_roots(odd_prime);
        const auto suffix = ", odd_prime=" + to_string(odd_prime);

        auto ys = 0, ns = 0;
        for (auto i = 1; i < odd_prime; ++i)
        {
            const auto name = to_string(i) + suffix + " ";
            const auto actual = euler_criterion<int64_t>(i, odd_prime);

            const auto it = qs.find(i);
            const auto expected = it != qs.end();
            Assert::AreEqual(expected, actual, "euler_criterion" + name);
            ++(expected ? ys : ns);

            const auto root = find_quadratic_residue_modulo<int64_t>(i, odd_prime);
            if (expected)
            {
                const auto root2 = odd_prime - root;
                assert(root2 > 0 && root2 < odd_prime);
                assert(root > 0 && root < odd_prime);

                const auto r = min(root, root2);
                Assert::AreEqual(it->second, r, "find_quadratic_residue_modulo" + name);
            }
            else
            {
                Assert::AreEqual(-1, root, "find_quadratic_residue_modulo" + name);
            }

            {
                const auto s = legendre_symbol<int64_t>(i, odd_prime);
                assert(s == 1 || s == -1);
                Assert::AreEqual(expected, s > 0, "legendre_symbol" + name);
            }
            {
                const auto s = jacobi_symbol<int64_t>(i, odd_prime);
                assert(s == 1 || s == -1);
                Assert::AreEqual(expected, s > 0, "jacobi_symbol" + name);
            }
        }

        Assert::AreEqual(odd_prime >> 1, ys, "euler_criterion ys must be half" + suffix);
        Assert::AreEqual(odd_prime >> 1, ns, "euler_criterion ns must be half" + suffix);
    }
}

void Privet::Algorithms::Numbers::Tests::quadratic_residue_tests()
{
    for (const auto& a : { 3,5,7,11,13,17,19 })
        euler_criterion_tests(a);
}