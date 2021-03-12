#include <tuple>
#include "ArrayConcatenateMinimum.h"
#include "ArrayConcatenateMinimumTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned long long int;

    void PredicateTest()
    {
        using TT = tuple<int, int, bool>;

        const string data[] = { "1", "12", "123", "0", "32", };
        const TT tuples[] = {
          TT(0, 1, true), TT(0, 2, true), TT(1, 2, true),
          //Zero
          TT(0, 3, false), TT(3, 0, true),
          //32
          TT(0, 4, true), TT(4, 0, false),
          TT(2, 4, true), TT(4, 2, false),
          //self
          TT(2, 2, true),
        };

        size_t index = 0;
        for (const TT& t : tuples)
        {
            const bool actual = ArrayConcatenateMinimum::SumPredicate(
                data[get<0>(t)], data[get<1>(t)]);

            Assert::AreEqual(get<2>(t), actual,
                "PredicateTest at index=" + to_string(index));

            ++index;
        }
    }

    void MainTest()
    {
        const Number dataArray[] = { 159, 16, 4, 3, 1, 15, 153, };
        const size_t dataArraySize = sizeof(dataArray) / sizeof(Number);

        const vector< Number > data(dataArray, dataArray + dataArraySize);

        string actual = "2";
        ArrayConcatenateMinimum::Calc(data, actual);

        const string expected = "1151531591634";
        Assert::AreEqual(expected, actual, "Result");
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
                void ArrayConcatenateMinimumTests()
                {
                    PredicateTest();
                    MainTest();
                }
            }
        }
    }
}