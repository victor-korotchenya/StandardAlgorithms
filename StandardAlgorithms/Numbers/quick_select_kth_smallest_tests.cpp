#include "quick_select_kth_smallest_tests.h"
#include "quick_select_kth_smallest.h"
#include "../test_utilities.h"
#include "../Utilities/Random.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using value_t = int64_t;
}

void Privet::Algorithms::Numbers::Tests::quick_select_kth_smallest_tests()
{
    const vector<value_t> a_sorted{ 0,1,2,2,2,5,7 };
    auto temp = a_sorted;

    for (auto rank = 0u; rank < a_sorted.size(); ++rank)
    {
        temp.resize(a_sorted.size());
        copy(a_sorted.cbegin(), a_sorted.cend(), temp.begin());
        ShuffleArray(temp);

        const auto ind = quick_select_kth_smallest(temp, rank);
        Assert::AreEqual(temp[ind], a_sorted[rank], "quick_select_kth_smallest_" + to_string(rank));
    }
}