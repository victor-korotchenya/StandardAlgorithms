#include <algorithm>
#include <cassert>
#include <utility>
#include "elevator_min_moves.h"
#include "../Utilities/ExceptionUtilities.h"
#include "../test_utilities.h"

using namespace std;

int Privet::Algorithms::Numbers::elevator_min_moves(
    const vector<int>& weights,
    const int64_t max_weight)
{
    RequirePositive(weights.size(), "weights");
    RequirePositive(max_weight, "max_weight");
    if (30 <= weights.size())
        throw runtime_error("The number of people ("
            + to_string(weights.size()) + ") is too large.");

    //For a subset: min weight in a single elevator move, min moves.
    using mw = pair<int64_t, int>;
    const auto size = static_cast<int>(weights.size());
    const auto up_mask = 1 << size;
    vector<mw> opt(up_mask, mw(size + 1, 0));

    for (auto m = 1; m < up_mask; ++m)
    {// for all not-empty subsets.
        for (auto j = 0; j < size; ++j)
        {
            const auto shifted = 1 << j;
            if (0 == (m & shifted))
                continue;
            auto best = opt[m ^ shifted];
            if (best.first + weights[j] <= max_weight)
                best.first += weights[j];
            else
            {
                best.first = weights[j];
                best.second++;
            }
            opt[m] = min(best, opt[m]);
        }
    }

    auto result = opt.back().second;
    return result;
}

void Privet::Algorithms::Numbers::Tests::elevator_min_moves_test()
{
    const vector<int> weights{
      8,6, 4,1,1,//2
      2,3,4,2,4 };//2
    const auto actual = elevator_min_moves(weights, 10);
    Assert::AreEqual(4, actual, "elevator_min_moves");
}