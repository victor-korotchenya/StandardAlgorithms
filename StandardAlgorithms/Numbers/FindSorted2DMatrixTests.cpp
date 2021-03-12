#include <limits>
#include <vector>
#include "FindSorted2DMatrix.h"
#include "FindSorted2DMatrixTests.h"
#include "../test_utilities.h"
#include "../Utilities/VectorUtilities.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/StreamUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Item = short;
    using Result = std::pair<size_t, size_t>;
    using TData = std::vector< std::vector< Item > >;

    struct TestCase final : base_test_case
    {
        TData Data{};
        Item Key{};
        Result Expected{};

        //TODO: p2. add another data members.
        explicit TestCase(std::string&& name)
            : base_test_case(forward<string>(name))
        {
        }

        bool ShouldNotFound() const
        {
            const bool result = (SIZE_MAX) == Expected.first
                //TODO: p2. What was the second check supposed to be?
                //|| SIZE_MAX == Expected.first
                ;

            return result;
        }

        void Validate() const override
        {
            base_test_case::Validate();

            Result firstFailure;
            const bool isMatrixSorted = IsMatrixSorted(firstFailure);
            if (!isMatrixSorted)
            {
                ostringstream ss;
                ss << "The matrix of must be sorted at " << firstFailure << ".";

                StreamUtilities::throw_exception(ss);
            }

            const bool notFound = ShouldNotFound();
            if (notFound)
            {
                return;
            }

            const size_t size = Data.size();
            if (size <= Expected.first)
            {
                ostringstream ss;
                ss << "The first index (" << Expected.first
                    << ") must not exceed size(" << size << ").";

                StreamUtilities::throw_exception(ss);
            }

            const size_t rowSize = Data[0].size();
            if (rowSize <= Expected.second)
            {
                ostringstream ss;
                ss << "The second index (" << Expected.second
                    << ") must not exceed row size(" << rowSize << ").";

                StreamUtilities::throw_exception(ss);
            }
        }

        void Print(std::ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", Data=", Data, str);

            str << " Key=" << Key
                << ", Expected=" << Expected;
        }

        bool IsMatrixSorted(Result& firstFailure) const
        {
            const size_t size = Data.size();
            if (0 == size)
            {
                return true;
            }

            const size_t rowSize = Data[0].size();

            for (size_t row = 0; row < size; ++row)
            {
                const vector< Item >& dataRow = Data[row];
                const size_t rowSize2 = dataRow.size();

                if (rowSize != rowSize2)
                {
                    ostringstream ss;
                    ss << "The row (" << row
                        << ") size is " << rowSize2 << " != default row size(" << rowSize << ").";
                    StreamUtilities::throw_exception(ss);
                }

                const vector< Item >& previousDataRow = 0 < row ? Data[row - 1] : dataRow;

                for (size_t column = 0; column < rowSize; ++column)
                {
                    const bool isError = 0 < column && dataRow[column] < dataRow[column - 1]
                        || 0 < row && dataRow[column] < previousDataRow[column];

                    if (isError)
                    {
                        firstFailure = Result(row, column);
                        return false;
                    }
                }
            }

            return true;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        const Result notFound = Result(SIZE_MAX, SIZE_MAX);
        {
            TestCase testCase("Simple");

            testCase.Key = 9;
            testCase.Expected = Result(3, 1);

            const size_t columns = 5;
            const Item dataArray[][columns] = {
              {1, 3, 5, 7, 8},
              {2, 3, 5, 8, 10},
              {3, 5, 8, 8, 10},
              {8, testCase.Key, 10, 123, 345}
            };
            const size_t rows = sizeof(dataArray) / sizeof(Item) / columns;

            VectorUtilities::Copy2D< Item, columns >(
                dataArray, rows, testCase.Data);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase = VectorUtilities::First(testCases);
            testCase.set_Name("MinValue");

            testCase.Key = testCase.Data[0][0];
            testCase.Expected = Result(0, 0);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase = VectorUtilities::First(testCases);
            testCase.set_Name("MaxValue");

            testCase.Key = VectorUtilities::Last(VectorUtilities::Last(testCase.Data));
            testCase.Expected = Result(testCase.Data.size() - 1, testCase.Data[0].size() - 1);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase = VectorUtilities::First(testCases);
            testCase.set_Name("NotExist");

            testCase.Key = 4;
            testCase.Expected = notFound;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase = VectorUtilities::First(testCases);
            testCase.set_Name("OneRow");

            testCase.Data.erase(testCase.Data.begin() + 1, testCase.Data.end());

            testCase.Expected = Result(0, testCase.Data[0].size() >> 1);
            testCase.Key = testCase.Data[0][testCase.Expected.second];

            testCases.push_back(testCase);
        }
        {
            TestCase testCase = VectorUtilities::Last(testCases);
            testCase.set_Name("OneValue");

            testCase.Data[0].erase(testCase.Data[0].begin() + 1, testCase.Data[0].end());

            testCase.Expected = Result(0, 0);
            testCase.Key = testCase.Data[0][0];

            testCases.push_back(testCase);
        }
        {
            TestCase testCase = VectorUtilities::Last(testCases);
            testCase.set_Name(testCase.get_Name() + "_NotFound");

            testCase.Expected = notFound;
            testCase.Key = testCase.Data[0][0] + 10;

            testCases.push_back(testCase);
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        FindSorted2DMatrix< Item > alg;

        const Result actual = alg.Search(testCase.Data, testCase.Key);
        Assert::AreEqual(testCase.Expected, actual, "Expected");
    }
}

void Privet::Algorithms::Numbers::Tests::FindSorted2DMatrixTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}