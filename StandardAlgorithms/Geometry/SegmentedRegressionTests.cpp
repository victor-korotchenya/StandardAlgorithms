#include <limits>
#include <vector>
#include "SegmentedRegression.h"
#include "SegmentedRegressionOld.h"
#include "SegmentedRegressionTests.h"
#include "../Numbers/NumberUtilities.h"
#include "../Numbers/NumberComparer.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/VectorUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Coordinate = float;
    using Number = double;
    using TResult = SegmentResult<Number>;
    using TPoint = point2d<Coordinate>;

    NumberComparer<Number> Comparer;

    bool AreEqualNumbers(const Number& a, const Number& b)
    {
        const bool result = Comparer.AreEqual(a, b);
        return result;
    }

    bool AreEqualResults(const TResult* const aPtr, const TResult* const bPtr)
    {
        const TResult& a = *aPtr;
        const TResult& b = *bPtr;

        const SegmentInfo< Number > infoA = a.Info,
            infoB = b.Info;

        const bool result = a.Point1 == b.Point1
            && a.Point2 == b.Point2
            && Comparer.AreEqual(infoA.Slope, infoB.Slope)
            && Comparer.AreEqual(infoA.Intercept, infoB.Intercept)
            && Comparer.AreEqual(infoA.Error, infoB.Error);
        return result;
    }

    struct TestCase final : base_test_case
    {
        Number SegmentCost{};
        std::vector< TPoint > Points;

        Number ExpectedCost{};
        std::vector<SegmentResult< Number > > ExpectedSegments;

        //TODO: p2. add another data members.
        explicit TestCase(std::string&& name)
            : base_test_case(forward<string>(name))
        {
        }

        void Print(std::ostream& str) const override
        {
            base_test_case::Print(str);

            str << " SegmentCost=" << SegmentCost;
            ::Print(", Points=", Points, str);

            ::Print(", ExpectedSegments=", ExpectedSegments, str);

            str << " ExpectedCost=" << ExpectedCost;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        {//Put the longest running one first.
            TestCase testCase("Parabola");
            testCase.SegmentCost = 10 * 1000;

            const int length = 20;
            for (int i = 0; i < length; ++i)
            {
                const Coordinate x = static_cast<Coordinate>(i);
                const Coordinate y = static_cast<Coordinate>(x * x * 7) + 3.4F;
                testCase.Points.push_back(TPoint(x, y));
            }

            testCase.ExpectedCost = 40061.323095023399;

            testCase.ExpectedSegments.push_back(
                SegmentResult< Number >(SegmentInfo< Number >(
                    41.999998177800862, -31.599995936666215, 4115.9994459152786), 0, 6));

            testCase.ExpectedSegments.push_back(
                SegmentResult< Number >(SegmentInfo< Number >(
                    133.00002615792411, -607.93356759207597, 1829.3321940172568), 7, 12));

            testCase.ExpectedSegments.push_back(
                SegmentResult< Number >(SegmentInfo< Number >(
                    223.99996512276786, -1760.5994524274554, 4115.9914550908661), 13, 19));

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("3Lines");
            testCase.SegmentCost = 0.1;

            const TPoint pointsC[] = {
              TPoint(1, 1),
              TPoint(2, 1),
              TPoint(3, 1),

              TPoint(4, 2),
              TPoint(5, 3),
              TPoint(6, 4),
              TPoint(7, 5),

              TPoint(8, 3),
              TPoint(9, 1),
              TPoint(10, -1),
              TPoint(11, -3),
              TPoint(12, -5)
            };
            const size_t length = sizeof(pointsC) / sizeof(TPoint);
            Assert::AreEqual<size_t>(12, length, "sizeof test");

            VectorUtilities::Append(length, pointsC, testCase.Points);

            testCase.ExpectedCost = testCase.SegmentCost * 3;

            testCase.ExpectedSegments.push_back(SegmentResult< Number >(SegmentInfo< Number >(0.0, 1.0, 0.0), 0, 1));
            testCase.ExpectedSegments.push_back(SegmentResult< Number >(SegmentInfo< Number >(1.0, -2.0, 0.0), 2, 5));
            testCase.ExpectedSegments.push_back(SegmentResult< Number >(SegmentInfo< Number >(-2.0, 19.0, 0.0), 6, 11));

            testCases.push_back(testCase);
        }
        {//This test case must go right after the previous one.
            TestCase testCase = testCases[testCases.size() - 1];
            testCase.set_Name("3LinesAs2");
            testCase.SegmentCost = 50.0;

            const Number error = 1.0857142857142859;
            testCase.ExpectedCost = 2 * testCase.SegmentCost + error;

            testCase.ExpectedSegments.clear();
            testCase.ExpectedSegments.push_back(SegmentResult< Number >(SegmentInfo< Number >(0.62857142857142856, -0.19999999999999987, error), 0, 5));
            testCase.ExpectedSegments.push_back(SegmentResult< Number >(SegmentInfo< Number >(-2.0, 19.0, 0.0), 6, 11));

            testCases.push_back(testCase);
        }
        {//This test case must go right after the previous one.
            TestCase testCase = testCases[testCases.size() - 1];
            testCase.set_Name("3LinesAs1");
            testCase.SegmentCost = 500.0;

            const Number error = 64.825174825174827;
            testCase.ExpectedCost = testCase.SegmentCost + error;

            testCase.ExpectedSegments.clear();
            testCase.ExpectedSegments.push_back(SegmentResult< Number >(SegmentInfo< Number >(-0.41958041958041958, 3.7272727272727271, error), 0, 11));

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("2Points");
            testCase.SegmentCost = 0.1;

            const TPoint pointsC[] = {
              TPoint(3, 1),
              TPoint(6, 4)
            };
            const size_t length = sizeof(pointsC) / sizeof(TPoint);
            VectorUtilities::Append(length, pointsC, testCase.Points);

            testCase.ExpectedCost = testCase.SegmentCost;

            testCase.ExpectedSegments.push_back(SegmentResult< Number >(SegmentInfo< Number >(1.0, -2.0, 0.0), 0, 1));

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Zigzag");
            testCase.SegmentCost = 0.001;

            const TPoint pointsC[] = {
              TPoint(1, 1),
              TPoint(2, 3),
              TPoint(3, 5),
              TPoint(4, 3),

              TPoint(5, 1),
              TPoint(6, 3),
              TPoint(7, 5),
              TPoint(8, 3),

              TPoint(9, 1),
              TPoint(10, 3),
              TPoint(11, 5),
              TPoint(12, 3),

              TPoint(13, 1),
              TPoint(14, 3),
              TPoint(15, 5),
              TPoint(16, 3),

              TPoint(17, 1),
              TPoint(18, 3),
              TPoint(19, 5),
              TPoint(20, 3)
            };
            const size_t length = sizeof(pointsC) / sizeof(TPoint);
            VectorUtilities::Append(length, pointsC, testCase.Points);

            testCase.ExpectedCost = 0.010000000000000002;

            const size_t maxIndex = 5;
            for (size_t i = 0; i < maxIndex; ++i)
            {
                const size_t mult4 = i << 2;
                const Number mult8 = static_cast<Number>(i << 3);
                testCase.ExpectedSegments.push_back(
                    SegmentResult< Number >(SegmentInfo< Number >(2.0, -1.0 - mult8, 0.0), mult4, 1 + mult4));

                testCase.ExpectedSegments.push_back(
                    SegmentResult< Number >(SegmentInfo< Number >(-2.0, 11.0 + mult8, 0.0), 2 + mult4, 3 + mult4));
            }

            testCases.push_back(testCase);
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        constexpr auto infinity = numeric_limits< Number >::infinity();

        //TODO: p1. Call non-static method. Comparer.AreEqual,
      //#pragma omp parallel sections default(none) shared(testCase, infinity)
        {
            //#pragma omp section
            {
                vector< SegmentResult< Number > > actualSegments;
                const Number actualCost = SegmentedRegressionOld< Coordinate >::Solve(
                    infinity,
                    testCase.Points, testCase.SegmentCost, actualSegments);

                string costMessage = "Old_" + testCase.get_Name() + ". Total cost";
                Assert::AreEqual< Number >(
                    testCase.ExpectedCost,
                    actualCost,
                    costMessage.c_str());

                string segmentsMessage = "Old_" + testCase.get_Name() + ". Segments";
                Assert::AreEqual(
                    testCase.ExpectedSegments,
                    actualSegments,
                    segmentsMessage.c_str());
            }
            //#pragma omp section
            {
                vector< SegmentResult< Number > > actualSegments;
                const Number actualCost = SegmentedRegression< Coordinate >::Solve(
                    infinity,
                    AbsoluteValue,
                    testCase.Points, testCase.SegmentCost, actualSegments);

                string costMessage = "Approximate_" + testCase.get_Name() + ". Total cost";
                Assert::AreEqualWithEpsilon<Number, Number>(
                    testCase.ExpectedCost,
                    actualCost,
                    AreEqualNumbers,
                    costMessage.c_str());

                string segmentsMessage = "Approximate_" + testCase.get_Name() + ". Segments";
                Assert::AreEqual(
                    testCase.ExpectedSegments,
                    actualSegments,
                    segmentsMessage.c_str(),
                    AreEqualResults);
            }
        }//sections
    }
}

void SegmentedRegressionTests()
{
    const Number epsilon = 0.9e-6;

    Comparer.Epsilon = epsilon;
    Comparer.AbsoluteValuePtr = AbsoluteValue;

    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}