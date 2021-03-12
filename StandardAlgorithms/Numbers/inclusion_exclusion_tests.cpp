#include "inclusion_exclusion_tests.h"
#include "inclusion_exclusion.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    void all_subsets_tests()
    {
        const vector<short> data{ 2,7,3 };
        const vector<vector<short>> expected = {
            {},
            {2},
            {7},
            { 2,7 },
            {3},
            {2,3},
            {7,3},
            {2,7,3},
        };
        const auto actual = all_subsets(data);
        Assert::AreEqual(expected, actual, "all_subsets");
    }

    void coprimes_in_interval_tests()
    {
        vector<int> factors;
        for (auto n = 1; n <= 10; ++n)
        {
            const auto s = to_string(n) + "_coprimes_in_interval_";
            for (auto r = 1; r <= 10; ++r)
            {
                const auto expected = coprimes_in_interval_slow(n, r);
                const auto actual = coprimes_in_interval<int64_t>(factors, n, r);
                Assert::AreEqual(expected, actual, s + to_string(r));
            }
        }
    }
}

void Privet::Algorithms::Numbers::Tests::inclusion_exclusion_tests()
{
    coprimes_in_interval_tests();
    all_subsets_tests();
}