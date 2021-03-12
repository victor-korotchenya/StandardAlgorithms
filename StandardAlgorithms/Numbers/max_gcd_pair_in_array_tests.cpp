#include "max_gcd_pair_in_array.h"
#include "max_gcd_pair_in_array_tests.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/Random.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using int_t = unsigned;
    using int_t2 = uint64_t;
    constexpr auto min_size = 2, max_size = 20, min_value = 0, max_value = 100;
}

void Privet::Algorithms::Numbers::Tests::max_gcd_pair_in_array_tests()
{
    UnsignedIntRandom r;
    vector<int_t> v;
    try
    {
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            v.clear();
            v = random_vector<int_t>(r, min_size, max_size, min_value, max_value);

            const auto actual = max_gcd_pair_in_array<int_t2, int_t>(v.data(), v.size());
            const auto slow = max_gcd_pair_in_array_slow(v.data(), v.size());
            Assert::AreEqual(actual, slow, "max_gcd_pair_in_array.");
        }
    }
    catch (const std::exception& e)
    {
        stringstream ss;
        ss << e.what();
        Print("data", v, ss);

        const auto s = ss.str();
        throw runtime_error(s);
    }
}