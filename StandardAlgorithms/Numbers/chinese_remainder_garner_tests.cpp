#include "chinese_remainder_garner_tests.h"
#include "chinese_remainder_garner.h"
#include "../Assert.h"
using namespace std;

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void chinese_remainder_garner_tests()
                {
                    using number = int64_t;
                    const vector<number> mods{ 2,5,11,7 }, remainders{ 1,4,6,0 };

                    number x = -1;
                    chinese_remainder_garner(mods, remainders, x);
                    Assert::AreEqual(259, x, "x");
                }
            }
        }
    }
}