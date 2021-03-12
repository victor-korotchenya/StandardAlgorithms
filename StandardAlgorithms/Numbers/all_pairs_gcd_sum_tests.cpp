#include "all_pairs_gcd_sum_tests.h"
#include "all_pairs_gcd_sum.h"
#include "../Utilities/Random.h"
#include "../test_utilities.h"
#include "NumberUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

void Privet::Algorithms::Numbers::Tests::all_pairs_gcd_sum_tests()
{
    constexpr auto size = 10;
    vector<uint32_t> v(size);
    vector<uint64_t> t0;
    FillRandom(v, size, 20u);
    sort(v.begin(), v.end());

    const auto g_slow = all_pairs_gcd_sum_slow<uint64_t>(v, gcd_unsigned<uint32_t>);
    const auto g = all_pairs_gcd_sum<uint64_t>(v, gcd_unsigned<uint32_t>, t0);
    if (g != g_slow)
    {
        stringstream ss;
        for (const auto& a : v)
            ss << a << ',';

        auto str = ss.str();
        Assert::AreEqual(g, g_slow, str);
    }
}