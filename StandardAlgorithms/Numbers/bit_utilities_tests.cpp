#include <vector>
#include "bit_utilities.h"
#include "bit_utilities_tests.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    vector<pair<int, int>> get_tests()
    {
        vector<pair<int, int>> tests{
            {1, 2},
            {2, 4},
            {5, 6},
            {1 << 30, 1 << 31},
            {(1 << 30) | (1 << 29) | (1 << 28), (1 << 31) | 3},
            {(1 << 31) | 3, (1 << 31) | 5},
        };
        const vector<int> values{
            0b1100011111000,
            0b1100100001111,
            0b1100100010111,
            0b1100100011011,
            0b1100100011101,
            0b1100100011110,
            0b1100100100111,
        };
        for (auto i = 0; i < static_cast<int>(values.size() - 1); ++i)
            tests.emplace_back(values[i], values[i + 1]);

        return tests;
    }

    void lowest_bit_tests()
    {
        const initializer_list<pair<int, int>> tests{
            { 0, 0 },
            { -1, 1 },
            { 1, 1 },
            { 2, 2 },
            { 3, 1 },
            { 4, 4 },
            { 5, 1 },
            { 6, 2 },
            { 12, 4 },
            { 100, 4 },
            { 104, 8 },
        };
        for (const auto& test : tests)
        {
            const auto actual = lowest_bit(test.first);
            Assert::AreEqual(test.second, actual, "lowest_bit " + to_string(test.first));
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
                void bit_utilities_tests()
                {
                    lowest_bit_tests();
                    {
                        const auto actual = enumerate_all_subsets_of_mask<unsigned>(0b100110);
                        const vector<unsigned> expected{
                            //0b100110, //3
                            //0b100100, 0b100010, 0b110, //2
                            //0b100, 0b100000, 0b10, //1
                            0b100110,
                            0b100100,
                            0b100010,
                            0b100000,
                            0b110,
                            0b100,
                            0b10,
                        };
                        Assert::AreEqual(expected, actual, "enumerate_all_subsets_of_mask");
                    }

                    const auto  tests = get_tests();
                    for (const auto& p : tests)
                    {
                        Assert::NotEqual(p.first, p.second, "In/out test values must be different.");
                        {
                            const auto actual = next_greater_same_pop_count<unsigned>(p.first);
                            const auto str = to_string(p.first);
                            Assert::AreEqual(static_cast<unsigned>(p.second), actual, "next_greater_same_pop_count" + str);
                        }

                        {
                            const auto actual = next_smaller_same_pop_count<unsigned>(p.second);
                            const auto str = to_string(p.second);
                            Assert::AreEqual(static_cast<unsigned>(p.first), actual, "next_smaller_same_pop_count" + str);
                        }
                    }
                }
            }
        }
    }
}