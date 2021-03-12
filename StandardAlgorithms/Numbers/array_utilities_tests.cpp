#include <array>
#include "array_utilities.h"
#include "array_utilities_tests.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using val_t = short;
    using array_t = array<val_t, 6>;

    void normalize_cycle_tests()
    {
        const array_t ar0{ 3,7,100,5,9,8 };
        const vector<array_t> arrays{
            ar0,
            {8,3,7,100,5,9},
            {9,8,3,7,100,5},
            {5,9,8,3,7,100},
            {100,5,9,8,3,7},
            {7,100,5,9,8,3},
        };

        vector<val_t> temp;
        auto shift = 0;
        for (const auto& ar : arrays)
        {
            const auto shift_str = to_string(shift);
            {
                auto cop = ar;
                rotate_array_left_slow(cop, temp, shift);
                Assert::AreEqual(ar0, cop, "rotate_array_left_slow, shift=" + shift_str);
            }
            {
                auto cop = ar;
                rotate_array_left(cop, shift);
                Assert::AreEqual(ar0, cop, "rotate_array_left, shift=" + shift_str);
            }
            {
                auto cop = ar;
                normalize_cycle_slow(cop, temp);
                Assert::AreEqual(ar0, cop, "normalize_cycle_slow, shift=" + shift_str);
            }
            {
                auto cop = ar;
                normalize_cycle(cop);
                Assert::AreEqual(ar0, cop, "normalize_cycle, shift=" + shift_str);
            }
            ++shift;
        }
    }
}

void Privet::Algorithms::Numbers::Tests::array_utilities_tests()
{
    normalize_cycle_tests();
}