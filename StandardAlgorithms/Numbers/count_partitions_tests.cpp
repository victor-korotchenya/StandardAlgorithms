#include "count_partitions.h"
#include "count_partitions_tests.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using long_t = int64_t;
    using int_t = int32_t;
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void count_partitions_tests()
                {
                    const vector<int_t> expected{ 1,1,
                        2,// 2,11
                        3,// 3,21,111
                        5,// 4,31,22,211,1111
                        7, 11, 15, 22, 30, 42, // 10
                        56, 77, 101, 135, 176, 231, 297, 385, 490, 627,//20
                    };
                    const auto n_max = static_cast<int_t>(expected.size() - 1);

                    const auto actual = count_partitions<long_t, int_t>(n_max);
                    Assert::AreEqual(expected, actual, "count_partitions");

                    {
                        const auto actual2 = count_partitions_still_slow<long_t, int_t>(n_max);
                        Assert::AreEqual(expected, actual2, "count_partitions_still_slow");
                    }

                    const auto n_max_slow = min(int_t(5), n_max);
                    const auto slow = count_partitions_slow<long_t, int_t>(n_max_slow);
                    Assert::AreEqual(expected.data(), n_max_slow + 1ll, slow, "count_partitions_slow");
                }
            }
        }
    }
}