#include "z_alg.h"
#include "../test_utilities.h"
#include "../Utilities/Random.h"

using namespace std;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            namespace Tests
            {
                void z_array_test()
                {
                    {
                        const string s = "abcabda";
                        const vector<int> expected{ 7,0,0, 2,0,0,1 };
                        Assert::AreEqual(s.size(), expected.size(), "size");
                        {
                            const auto actual = z_array_slow(s);
                            Assert::AreEqual(expected, actual, "z_array_slow");
                        }
                        {
                            const auto actual = z_array(s);
                            Assert::AreEqual(expected, actual, "z_array");
                        }
                    }
                    IntRandom r;
                    {
                        const auto s = random_string(r);
                        const auto slow = z_array_slow(s),
                            fast = z_array(s);
                        Assert::AreEqual(slow, fast, "random");
                    }
                }

                void string_border_test()
                {
                    {
                        const string s = "abacabacaba";
                        const vector<int> expected{ 1,3,7 };
                        {
                            const auto actual = string_borders_slow(s);
                            Assert::AreEqual(expected, actual, "string_borders_slow");
                        }
                        {
                            const auto actual = string_borders(s);
                            Assert::AreEqual(expected, actual, "string_borders");
                        }
                    }
                    IntRandom r;
                    {
                        const auto s = random_string(r);
                        const auto slow = string_borders_slow(s),
                            fast = string_borders(s);
                        Assert::AreEqual(slow, fast, "random=" + s);
                    }
                }
            }
        }
    }
}