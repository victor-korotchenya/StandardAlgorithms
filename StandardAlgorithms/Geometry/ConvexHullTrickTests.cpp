#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "ConvexHullTrick.h"
#include "ConvexHullTrickTests.h"
#include "../Utilities/Random.h"

using namespace std;
using namespace Privet::Algorithms::Geometry::Tests;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = int;
    using LongNumber = long long;
    using Size = unsigned;
    using P = pair<Number, Number>;

    struct TestCase final : base_test_case
    {
        vector<P> Rectangles;
        LongNumber Expected;

        TestCase(string&& name,
            vector<P>&& rectangles,
            LongNumber expected)
            : base_test_case(forward<string>(name)),
            Rectangles(forward<vector<P>>(rectangles)),
            Expected(expected)
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            ::AppendSeparator(str);
            ::Print("Rectangles", Rectangles, str);

            ::PrintValue(str, "Expected", Expected);
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        testCases.push_back({ "One group better than two ones", {
          { 3,5 },
          { 5,3 },
        }, 25 });

        testCases.push_back({ "One group over many",{
          { 3,10 },
          { 5,11 },
          { 8,9 },
          { 10,4 },
        }, 110 });
        testCases.push_back({ "Trivial", {{ 3,5 }}, 15 });

        testCases.push_back({ "Two groups",{
          { 3,100 },
          { 500,4 },
        }, 300 + 2000 });

        testCases.push_back({ "Covered by bigger", {
          { 3,5 },
          { 2,3 },
          { 1,1 },
          { 2,4 },
          { 2,5 },
          { 3,3 },
          { 1,1 },
        }, 15 });

        testCases.push_back({ "3 groups", {
          { 3,100 },//300
          { 5,11 },{ 8,9 },//88
          { 4,7 },{ 8,8 },//0
          { 20,4 },//80
        }, 468 });

        testCases.push_back({ "2 groups test 2",{
            { 100, 105 },{ 111, 8 },
            // useless
            { 97, 18 }
        }, 11388 });

        const auto siz = 8;
        vector<short> v(siz * 2);

        const auto max_attempts = 2;
        for (auto att = 0; att < max_attempts; ++att)
        {
            vector<P> rectangles(siz);
            FillRandom(v, v.size());
            for (auto i = 0; i < siz; ++i)
            {
                rectangles[i].first = abs(static_cast<char>(v[i << 1])) + 1;
                rectangles[i].second = abs(static_cast<char>(v[i << 1 | 1])) + 1;
            }

            const auto best = ConvexHullTrick<Number, LongNumber, Size>(rectangles);
            testCases.push_back({ "Random conv hull tr" + to_string(att), move(rectangles), best });
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        const auto& rectangles = testCase.Rectangles;
        const auto actual_slow = ConvexHullTrickSlow<Number, LongNumber, Size>(rectangles);
        Assert::AreEqual(testCase.Expected, actual_slow, "slow_" + testCase.get_Name());

        const auto actual = ConvexHullTrick<Number, LongNumber, Size>(rectangles);
        Assert::AreEqual(testCase.Expected, actual, "fast_" + testCase.get_Name());
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            namespace Tests
            {
                void ConvexHullTrickTests()
                {
                    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}