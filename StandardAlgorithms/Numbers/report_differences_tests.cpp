#include "report_differences.h"
#include "report_differences_tests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void report_differences_tests()
                {
                    vector<int> actual;
                    auto rep = [&actual](const int a) -> void {
                        actual.push_back(a);
                    };

                    constexpr auto count = 5, mi = numeric_limits<int>::min();
                    vector<int> diffs(1);
                    for (auto i = 0; i <= count; ++i)
                    {
                        const auto curr = 2 * i * i + 7 * i + 8;
                        rep(curr);
                        report_diffs(rep, diffs, curr);
                        rep(mi);
                    }

                    const vector<int> expected{
                        // 0
                        8, 8, mi,
                        17, 9, 1, mi,
                        30, 13, 4, 3, mi,
                        47, 17, 4, mi,
                        68, 21, 4, mi,
                        93, 25, 4, mi,
                    };
                    Assert::AreEqual(expected, actual, "report_diffs");
                }
            }
        }
    }
}