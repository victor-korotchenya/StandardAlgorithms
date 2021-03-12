#include <sstream>
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/VectorUtilities.h"
#include "../Utilities/ConvertUtilities.h"
#include "MaxSubMatrixIn2DMatrix.h"
#include "../Utilities/StreamUtilities.h"
#include "../Assert.h"
#include "MaxSubMatrixIn2DMatrixTests.h"

using namespace std;
using namespace Privet::Algorithms;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Tests;

namespace
{
    static void BoolTest();
    static void SumTest();

    using TNumber = int;
    using TNumberSquared = long long int;

    using TSize = unsigned int;
    using TSizeSquared = size_t;

    using TPoint = Geometry::point2d<TSize>;
    using TRectangle = Geometry::rectangle2d<TSize, TSizeSquared, TPoint>;

    using AreaFunction = TRectangle(*)(const std::vector< std::vector< bool > >& data1);

    struct TestCase final : ::base_test_case
    {
        std::vector< std::vector< bool > > Data;

        //The second point is exclusive.
        TRectangle Rectangle, Square;

        //TODO: p2. add another data members.
        explicit TestCase(std::string&& name = "");

        void Print(std::ostream& str) const override;
    };

    struct TestCaseSum final : ::base_test_case
    {
        std::vector< std::vector< TNumber > > Data;

        TNumberSquared ExpectedResult{};

        //The second point is exclusive.
        TRectangle ExpectedRectangle{};

        //TODO: p2. add another data members.
        explicit TestCaseSum(std::string&& name);

        void Print(std::ostream& str) const override;
    };

    static void GenerateTestCases(std::vector<TestCase>& testCases);
    static void GenerateTestCasesSum(std::vector< TestCaseSum >& testCases);

    static void RunTestCase(const TestCase& testCase);

    static void RunTestCaseSum(const TestCaseSum& testCase);

    static void AddTransposedTestCases(std::vector<TestCase>& testCases);

    static TRectangle TransposeRectangle(const TRectangle& rectangle);

    static void RunOnce(
        const bool isSquared,
        const std::string& nameSuffix,
        const TestCase& testCase,
        AreaFunction areaFunction);

    inline static TNumber String2Int(const std::string& s)
    {
        const TNumber result = std::stoi(s);
        return result;
    }

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        using TAlgorithm = MaxSubMatrixIn2DMatrix< TSize, TSizeSquared, TPoint, TRectangle >;

        vector< bool > row;

        const TPoint zeroPoint = TPoint(0, 0);

        {
            const size_t dim = 4;
            const bool ladder[][dim] = {
              { 0, 0, 0, 1, },
              { 0, 0, 1, 1, },
              { 0, 1, 1, 1, },
              { 1, 1, 1, 1, },
            };
            const size_t sizeAll = sizeof(ladder) / sizeof(bool);
            const size_t rows = sizeAll / dim;

            {
                TestCase testCase("Ladder3");

                //{ 0, 0, 1, },
                //{ 0, 1, 1, },
                //{ 1, 1, 1, },

                for (size_t i = 0; i < rows - 1; ++i)
                {
                    const bool* const line = ladder[i];

                    row.clear();
                    row.insert(row.begin(), line + 1, line + dim);
                    testCase.Data.push_back(row);
                }

                testCase.Rectangle = TRectangle(TPoint(1, 1), TPoint(3, 3));
                testCase.Square = testCase.Rectangle;

                testCases.push_back(testCase);
            }
            {
                TestCase testCase("Ladder4");

                for (size_t i = 0; i < rows; ++i)
                {
                    const bool* const line = ladder[i];

                    row.clear();
                    row.insert(row.begin(), line, line + dim);
                    testCase.Data.push_back(row);
                }

                testCase.Rectangle = TRectangle(TPoint(2, 1), TPoint(4, 4));
                testCase.Square = TRectangle(TPoint(1, 2), TPoint(3, 4));

                testCases.push_back(testCase);
            }
        }
        {
            TestCase testCase("Simple1");

            // 0110000
            // 0111110
            ConvertUtilities::StringToBooleanVector("0110000", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("0111110", row);
            testCase.Data.push_back(row);

            testCase.Rectangle = TRectangle(TPoint(1, 1), TPoint(2, 6));
            testCase.Square = TRectangle(TPoint(0, 1), TPoint(2, 3));

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Simple1_Rotated");

            // 00
            // 11
            // 11
            // 10
            // 10
            // 10
            // 00
            ConvertUtilities::StringToBooleanVector("00", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("11", row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("10", row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("00", row);
            testCase.Data.push_back(row);

            testCase.Rectangle = TRectangle(TPoint(1, 0), TPoint(6, 1));
            testCase.Square = TRectangle(TPoint(1, 0), TPoint(3, 2));

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Cross");

            // 0110000
            // 1111101
            // 0101101
            ConvertUtilities::StringToBooleanVector("0110000", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("1111101", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("0101101", row);
            testCase.Data.push_back(row);

            testCase.Rectangle = TRectangle(TPoint(1, 0), TPoint(2, 5));
            testCase.Square = TRectangle(TPoint(0, 1), TPoint(2, 3));

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("All 1");

            // 11111
            // 11111
            // 11111
            ConvertUtilities::StringToBooleanVector("11111", row);

            const int count = 3;
            for (int i = 0; i < count; i++)
            {
                testCase.Data.push_back(row);
            }

            const TPoint leftTop = TPoint(0, 0);
            testCase.Rectangle = TRectangle(leftTop, TPoint(3, 5));
            testCase.Square = TRectangle(leftTop, TPoint(3, 3));

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("One 1");

            // 00000
            // 00010
            // 00000
            ConvertUtilities::StringToBooleanVector("00000", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("00010", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("00000", row);
            testCase.Data.push_back(row);

            testCase.Rectangle = TRectangle(TPoint(1, 3), TPoint(2, 4));
            testCase.Square = testCase.Rectangle;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Two 1");

            // 00000
            // 00110
            // 00000
            ConvertUtilities::StringToBooleanVector("00000", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("00110", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("00000", row);
            testCase.Data.push_back(row);

            const TPoint point = TPoint(1, 2);
            testCase.Rectangle = TRectangle(point, TPoint(2, 4));
            testCase.Square = TRectangle(point, TPoint(2, 3));

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("All 0");

            // 00000
            // 00000
            ConvertUtilities::StringToBooleanVector("00000", row);

            const int count = 2;
            for (int i = 0; i < count; i++)
            {
                testCase.Data.push_back(row);
            }

            testCase.Rectangle = TRectangle(zeroPoint, zeroPoint);
            testCase.Square = testCase.Rectangle;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Simple2");

            // 0010010100
            // 0000000000
            // 0101110110
            // 1001110110
            // 1101110110
            // 1101110110
            // 0000000000

            ConvertUtilities::StringToBooleanVector("0010010100", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("0000000000", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("0101110110", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("1001110110", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("1101110110", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("1101110110", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToBooleanVector("0000000000", row);
            testCase.Data.push_back(row);

            testCase.Rectangle = TRectangle(TPoint(2, 3), TPoint(6, 6));
            testCase.Square = TRectangle(TPoint(2, 3), TPoint(5, 6));

            testCases.push_back(testCase);
        }
        {
            using Pair = pair<size_t, size_t>;

#ifdef _DEBUG
            const size_t minValue = 3;
            const size_t maxValue = 6;
#else
            const size_t minValue = 5;
            const size_t maxValue = 50;
#endif

            SizeRandom sizeRandom;

            const size_t random1 = sizeRandom(minValue, maxValue);
            const size_t random2 = sizeRandom(minValue, maxValue);

            const Pair pairs[] = {
              Pair(random1, random2),
        #ifndef _DEBUG
              Pair(5, 3),
              Pair(11, 18),
              Pair(35, 46),
        #endif
            };
            const size_t pairsSize = sizeof(pairs) / sizeof(Pair);

            for (size_t pairIndex = 0; pairIndex < pairsSize; ++pairIndex)
            {
                const auto& p = pairs[pairIndex];

                const size_t& size = p.first;
                const size_t& rowSize = p.second;

                TestCase testCase("Random test" + to_string(pairIndex + 1));

                for (size_t i = 0; i < size; ++i)
                {
                    FillRandom(row, rowSize);
                    testCase.Data.push_back(row);
                }

                testCase.Rectangle = TAlgorithm::MaxAreaMethod2(testCase.Data);

                //TODO: p2. Implement another faster method to test.
                testCase.Square = TAlgorithm::MaxSquare(testCase.Data);

                testCases.push_back(testCase);
            }
        }

        //It must be the last line.
        AddTransposedTestCases(testCases);
    }

    void GenerateTestCasesSum(
        vector< TestCaseSum >& testCases)
    {
        using TAlgorithm = MaxSubMatrixIn2DMatrix< TSize, TSizeSquared, TPoint, TRectangle >;
        using CastFunction = TNumber(*)(const string& s);

        CastFunction castFunction = &String2Int;

        vector< TNumber > row;

        {//Long-running test cases goes first.
            using Pair = pair< size_t, size_t>;

#ifdef _DEBUG
            const size_t minValue = 3;
            const size_t maxValue = 6;
#else
            const size_t minValue = 5;
            const size_t maxValue = 20;
#endif

            SizeRandom sizeRandom;

            const size_t random1 = sizeRandom(minValue, maxValue);
            const size_t random2 = sizeRandom(minValue, maxValue);

            const Pair pairs[] = {
        #ifdef _DEBUG
              Pair(random1, random2),
        #else
              Pair(3, 5),
              Pair(15, 13),
              Pair(random1, random2),
        #endif
            };
            const size_t pairsSize = sizeof(pairs) / sizeof(Pair);

            for (size_t pairIndex = 0; pairIndex < pairsSize; ++pairIndex)
            {
                const auto& p = pairs[pairIndex];

                const size_t& size = p.first;
                const size_t& rowSize = p.second;

                TestCaseSum testCase("Random test" + to_string(pairIndex + 1));

                for (size_t i = 0; i < size; ++i)
                {
                    FillRandom(row, rowSize);

                    testCase.Data.push_back(row);
                }

                testCase.ExpectedResult = TAlgorithm::MaxSumFast<TNumber, TNumberSquared>(
                    testCase.Data, testCase.ExpectedRectangle);

                testCases.push_back(testCase);
            }
        }
        {
            TestCaseSum testCase("Simple1");

            ConvertUtilities::StringToNumberVector(castFunction, "-500 10 20", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToNumberVector(castFunction, "2 - 8  \t 0 ", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToNumberVector(castFunction, "1 , -    1  \n 11;", row);
            testCase.Data.push_back(row);

            testCase.ExpectedResult = 32;
            testCase.ExpectedRectangle = TRectangle(TPoint(0, 1), TPoint(3, 3));

            testCases.push_back(testCase);
        }
        {
            TestCaseSum testCase("Only one positive");

            ConvertUtilities::StringToNumberVector(castFunction, "-500 -10 -20", row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToNumberVector(castFunction, "-2 -1 789 ", row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToNumberVector(castFunction, "-2 - 9 0 ", row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);

            testCase.ExpectedResult = 789;
            const TPoint p1 = TPoint(4, 2);
            testCase.ExpectedRectangle = TRectangle(p1, TPoint(p1.X + 1, p1.Y + 1));

            testCases.push_back(testCase);
        }
        {
            TestCaseSum testCase("All negative");

            ConvertUtilities::StringToNumberVector(castFunction, "-500 -10 -20", row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);

            testCase.ExpectedResult = 0;
            const TPoint p0 = TPoint(0, 0);
            testCase.ExpectedRectangle = TRectangle(p0, p0);

            testCases.push_back(testCase);
        }
        {
            TestCaseSum testCase("All positive");

            ConvertUtilities::StringToNumberVector(castFunction, "1 3 8 20", row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);

            testCase.ExpectedResult = 3 * (1 + 3 + 8 + 20);
            const TPoint p0 = TPoint(0, 0);
            testCase.ExpectedRectangle = TRectangle(p0, TPoint(3, 4));

            testCases.push_back(testCase);
        }
        {
            TestCaseSum testCase("Left top positive, right bottom zeros");

            ConvertUtilities::StringToNumberVector(castFunction, "1 3 8 0", row);
            testCase.Data.push_back(row);
            testCase.Data.push_back(row);

            ConvertUtilities::StringToNumberVector(castFunction, "0 0 0 0", row);
            testCase.Data.push_back(row);

            testCase.ExpectedResult = 2 * (1 + 3 + 8);
            const TPoint p0 = TPoint(0, 0);
            testCase.ExpectedRectangle = TRectangle(p0, TPoint(2, 3));

            testCases.push_back(testCase);
        }
    }

    TRectangle TransposeRectangle(
        const TRectangle& rectangle)
    {
        const TPoint top = rectangle.LeftTop();

        const TSize height = rectangle.Height();
        const TSize width = rectangle.Width();

        const TRectangle result = TRectangle(
            TPoint(top.Y, top.X),
            TPoint(top.Y + width, top.X + height));
        return result;
    }

    void AddTransposedTestCases(vector<TestCase>& testCases)
    {
        const size_t size = testCases.size();
        Assert::Greater(size, 0, "testCases before transposing");

        testCases.resize(size << 1);

        const string transposed = "_Transposed";

        for (size_t i = 0; i < size; ++i)
        {
            const TestCase& oldTestCase = testCases[i];

            const auto& name = oldTestCase.get_Name();

            const size_t height = oldTestCase.Data.size();
            Assert::Greater(height, 0, name + "_size");

            const size_t width = oldTestCase.Data[0].size();
            Assert::Greater(width, 0, name + "_size_of_row");

            //New one.
            TestCase& testCase = testCases[i + size];

            testCase.set_Name(name + transposed);

            VectorUtilities::Transpose(oldTestCase.Data, testCase.Data);

            testCase.Rectangle = TransposeRectangle(oldTestCase.Rectangle);
            testCase.Square = TransposeRectangle(oldTestCase.Square);
        }
    }

    TestCase::TestCase(std::string&& name)
        : base_test_case(forward<string>(name))
    {
    }

    void TestCase::Print(ostream& str) const
    {
        base_test_case::Print(str);

        ::Print(", Data=", Data, str);

        str << " Rectangle=" << Rectangle
            << ", Square=" << Square;
    }

    void TestCaseSum::Print(std::ostream& str) const
    {
        base_test_case::Print(str);
        str << ", ExpectedResult=" << ExpectedResult
            << ", ExpectedRectangle=" << ExpectedRectangle;
    }

    void RunTestCase(const TestCase& testCase)
    {
        //TODO: p3. del? const string matrixSuffix = "_Matrix_";
        const string matrixSuffix2 = "_MatrixMethod2_";
        const string prefixSquare = "_SquareMatrix_";

        using TAlgorithm = MaxSubMatrixIn2DMatrix< TSize, TSizeSquared, TPoint, TRectangle >;

        // TODO: p2. Exclude the buggy code for now.
        //RunOnce(false, matrixSuffix, testCase, &TAlgorithm::MaxArea);

        RunOnce(false, matrixSuffix2, testCase, &TAlgorithm::MaxAreaMethod2);
        RunOnce(true, prefixSquare, testCase, &TAlgorithm::MaxSquare);
    }

    void RunTestCaseSum(const TestCaseSum& testCase)
    {
        using TAlgorithm = MaxSubMatrixIn2DMatrix< TSize, TSizeSquared, TPoint, TRectangle >;

        const string name = "SumTest_" + testCase.get_Name();

        try
        {
            {
                TRectangle actualRectangle;

                const TNumberSquared actual = TAlgorithm::MaxSum< TNumber, TNumberSquared>(
                    testCase.Data, actualRectangle);

                Assert::AreEqual(testCase.ExpectedResult, actual, name + "_Result");
                Assert::AreEqual(testCase.ExpectedRectangle, actualRectangle, name + "_Rectangle");
            }
            {
                TRectangle actualRectangle;

                const TNumberSquared actual = TAlgorithm::MaxSumFast< TNumber, TNumberSquared>(
                    testCase.Data, actualRectangle);

                Assert::AreEqual(testCase.ExpectedResult, actual, name + "_Result" + "_Fast");
                Assert::AreEqual(testCase.ExpectedRectangle, actualRectangle, name + "_Rectangle" + "_Fast");
            }
        }
        catch (...)
        {
            ostringstream ss;
            ss << "Failed test case: " << testCase << '\n';

            Print("Test case data", testCase.Data, ss, 100);

            StreamUtilities::throw_exception(ss);
        }
    }

    void RunOnce(
        const bool isSquared,
        const string& nameSuffix,
        const TestCase& testCase,
        AreaFunction areaFunction)
    {
        if (nameSuffix.empty())
        {
            throw exception("The 'nameSuffix must' be not empty.");
        }

        if (nullptr == areaFunction)
        {
            throw exception("nullptr == areaFunction");
        }

        const string name = testCase.get_Name() + nameSuffix;

        try
        {
            const TRectangle actual = areaFunction(testCase.Data);

            const TNumberSquared actualArea = actual.Area();
            if (actualArea)
            {
                const TPoint leftTop = actual.LeftTop();
                const TPoint rightBottom = actual.RightBottom();

                TRectangle::Check(leftTop, rightBottom);

                const TSize x1 = leftTop.X;
                const TSize y1 = leftTop.Y;

                const TSize x2 = rightBottom.X;
                const TSize y2 = rightBottom.Y;

                VectorUtilities::CheckAreaHasAllTrueValues(
                    testCase.Data,
                    x1, y1,
                    x2, y2,
                    name);
            }

            const TRectangle& expectedRectangle = isSquared ? testCase.Square : testCase.Rectangle;

            const bool areEqual = expectedRectangle == actual;
            if (!areEqual)
            {
                //The area must be exactly the same,
                //but the coordinates are not always the same.
                const TNumberSquared expectedArea = expectedRectangle.Area();
                Assert::AreEqual(
                    expectedArea,
                    actualArea, name + "_Area");
            }

            if (isSquared)
            {
                Assert::AreEqual(testCase.Square.Width(),
                    testCase.Square.Height(),
                    name + "_ExpectedSides");

                Assert::AreEqual(actual.Width(),
                    testCase.Square.Width(),
                    name + "_Width");

                Assert::AreEqual(actual.Height(),
                    testCase.Square.Height(),
                    name + "_Height");
            }
        }
        catch (...)
        {
            ostringstream ss;
            ss << "Failed test case: " << testCase << '\n';

            Print("Test case data", testCase.Data, ss, 100);

            StreamUtilities::throw_exception(ss);
        }
    }

    void BoolTest()
    {
        test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
    }

    void SumTest()
    {
        test_utilities<TestCaseSum>::run_tests(RunTestCaseSum, GenerateTestCasesSum);
    }
    TestCaseSum::TestCaseSum(std::string&& name)
        : base_test_case(forward<string>(name))
    {
    }
}

void MaxSubMatrixIn2DMatrixTests::Test()
{
    BoolTest();
    SumTest();
}