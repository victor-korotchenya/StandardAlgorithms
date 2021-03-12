#include <vector>
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/VectorUtilities.h"
#include "../Utilities/Random.h"
#include "../Assert.h"
#include "../Utilities/TestNameUtilities.h"
#include "../Utilities/StreamUtilities.h"
#include "MaximumIndexDifference2Items.h"
#include "MaximumIndexDifference2ItemsTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Number = int;
    using TSize = short;
    using TPair = pair<TSize, TSize>;

    struct TestCase final
    {
        string Name;
        inline const std::string& get_Name() const
        {
            return Name;
        }
        vector< Number > Data;

        TPair ExpectedResult;

        //To print the "Data", use a separate method.
        friend ostream& operator <<
            (ostream& str, TestCase const& b)
        {
            str << b.Name
                << ", ExpectedResult=" << (b.ExpectedResult);

            return str;
        }

        void Validate() const;
    };

    using Service = MaximumIndexDifference2Items< Number, TSize, TPair >;

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        {
            TestCase testCase;
            testCase.Name = "Simple1.";

            testCase.Data.push_back(31236);
            testCase.Data.push_back(-1548);
            testCase.Data.push_back(920);
            testCase.Data.push_back(-1685);
            testCase.Data.push_back(1376);
            testCase.Data.push_back(2148);

            testCase.ExpectedResult = TPair(1, 5);

            testCases.push_back(testCase);
        } {//This case must follow the previous one.
            TestCase testCase = VectorUtilities::Last(testCases);
            testCase.Name = testCase.Name + "1.";

            testCase.Data.push_back(3 + testCase.Data[0]);

            testCase.ExpectedResult = TPair(0, TSize(testCase.Data.size() - 1));

            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Name = "Decreasing.";

            testCase.Data.push_back(12345);
            testCase.Data.push_back(1234);
            testCase.Data.push_back(123);
            testCase.Data.push_back(12);
            testCase.Data.push_back(1);

            testCase.ExpectedResult = TPair(0, 0);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Name = "Non-increasing.";

            testCase.Data.push_back(1234);
            testCase.Data.push_back(1234);
            testCase.Data.push_back(12);
            testCase.Data.push_back(12);
            testCase.Data.push_back(1);

            testCase.ExpectedResult = TPair(0, 0);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Name = "ZigZag.";

            testCase.Data.push_back(10000);
            testCase.Data.push_back(9999);
            testCase.Data.push_back(10000);
            testCase.Data.push_back(12);
            testCase.Data.push_back(9999);

            testCase.ExpectedResult = TPair(1, 2);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Name = "Many decreasing.";

            testCase.Data.push_back(12);
            testCase.Data.push_back(7);
            testCase.Data.push_back(10);
            testCase.Data.push_back(5);
            testCase.Data.push_back(8);
            testCase.Data.push_back(3);
            testCase.Data.push_back(11);
            testCase.Data.push_back(1);

            testCase.ExpectedResult = TPair(1, 6);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.Name = "Random test.";

            const TSize size = 10;
            const Number cutter = Number(SHRT_MAX >> 1);
            FillRandom(testCase.Data, size, cutter);

            Service service;

            testCase.ExpectedResult = service.Slow(testCase.Data.data(), size);

            testCases.push_back(testCase);
        }
    }

    void TestCase::Validate() const
    {
        const string indexesName = Name + " Min_Max_Indexes";

        Assert::GreaterOrEqual(ExpectedResult.second,
            ExpectedResult.first, indexesName);
    }
}

void Privet::Algorithms::Numbers::MaximumIndexDifference2ItemsTests()
{
    using runFunction = TPair(Service::*)(
        const Number* const data, const TSize size) const;

    using TNameFunctionPair = pair < string, runFunction >;

    const TNameFunctionPair subCases[] = {
        make_pair("_Slow", &Service::Slow),
        make_pair("_Fast", &Service::Fast)
    };
    const TSize subCasesSize = sizeof(subCases) / sizeof(TNameFunctionPair);

    vector< TestCase > testCases;
    GenerateTestCases(testCases);
    TestNameUtilities::RequireUniqueNames(testCases, "testCases");

    Service service;

    const TSize length = TSize(testCases.size());

    for (TSize i = 0; i < length; ++i)
    {
        const TestCase& testCase = testCases[i];
        testCase.Validate();

        const string resultName = testCase.Name + "_Result";

        const Number* const numbers = testCase.Data.data();
        const TSize numbersSize = TSize(testCase.Data.size());

        for (TSize j = 0; j < subCasesSize; j++)
        {
            const TNameFunctionPair& subCase = subCases[j];
            const string& subCaseName = subCase.first;

            try
            {
                const TPair actual = (service.*(subCase.second))(numbers, numbersSize);
                Assert::AreEqual(testCase.ExpectedResult, actual, resultName + subCaseName);
            }
            catch (...)
            {
                ostringstream ss;
                ss << testCase << '\n';

                const string errorMessage = "The failed '"
                    + subCaseName + "' testCase.Data=";
                Print(errorMessage, testCase.Data, ss);

                StreamUtilities::throw_exception(ss);
            }
        }
    }
}