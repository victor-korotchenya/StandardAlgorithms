#include <algorithm>
#include <exception>
#include <vector>
#include "../Utilities/Random.h"
#include "../Utilities/RandomGenerator.h"
#include "../Utilities/PrintUtilities.h"
#include "../Assert.h"
#include "CrossPointOTwofIncrDecrLines.h"
#include "CrossPointOTwofIncrDecrLinesTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    void one_run_CrossPointTests()
    {
        using number_x = int;
        using number_y = long long int;

#ifdef _DEBUG
        constexpr auto mi = -64, ma = -mi;
        constexpr auto cutter = 64;
#else
        constexpr auto mi = -641, ma = -mi;
        constexpr auto cutter = 0;
#endif
        IntRandom intRandom;
        const auto a = intRandom(mi, ma), b = intRandom(mi, ma),
            beginning = min(a, b), ending = max(a, b);

        const auto size = ending - beginning + 1;
        vector<number_y> inc_v, dec_v;

        FillRandom<number_y, number_x>(inc_v, size, cutter);
        sort(inc_v.begin(), inc_v.end());

        FillRandom<number_y, number_x>(dec_v, size, cutter);
        sort(dec_v.begin(), dec_v.end(),
            [](const number_y& a1, const number_y& b1)
            {//reverse
                return b1 < a1;
            });

        auto incr_line = [&](const number_x& x)
        {
            return inc_v[x - beginning];
        };
        auto decr_line = [&](const number_x& x)
        {
            return dec_v[x - beginning];
        };

        try
        {
            const auto expected = CrossPointOTwofIncrDecrLines_slow<number_x,
                decltype(incr_line),
                number_y,
                decltype(decr_line)>(
                    incr_line, decr_line, beginning, ending);

            Assert::GreaterOrEqual(expected, beginning, "beginning");
            Assert::GreaterOrEqual(ending, expected, "ending");

            const auto actual = CrossPointOTwofIncrDecrLines<number_x,
                decltype(incr_line),
                number_y,
                decltype(decr_line)>(
                    incr_line, decr_line, beginning, ending);

            if (expected != actual)
            {
                const auto i1 = incr_line(expected),
                    d1 = decr_line(expected),
                    dif1 = abs_diff(d1, i1);

                const auto i2 = incr_line(actual),
                    d2 = decr_line(actual),
                    diff2 = abs_diff(d2, i2);

                auto message = "diff, expected=" + to_string(expected)
                    + ", actual=" + to_string(actual);

                Assert::AreEqual(dif1, diff2, message);
            }
        }
        catch (const exception& ex)
        {
            ostringstream ss;
            ss << "Error: " << ex.what()
                << "\nbeginning=" << beginning
                << ", ending=" << ending
                << '\n';

            ::Print("inc_v", inc_v, ss);
            ::Print("dec_v", dec_v, ss);

            StreamUtilities::throw_exception(ss);
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
                void CrossPointOTwofIncrDecrLinesTests()
                {
                    const auto length = 10;
                    for (auto i = 0; i < length; i++)
                    {
                        one_run_CrossPointTests();
                    }
                }
            }
        }
    }
}