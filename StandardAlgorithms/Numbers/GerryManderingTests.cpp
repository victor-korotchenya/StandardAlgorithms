#include <vector>
#include "GerryMandering.h"
#include "GerryManderingTests.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    struct TestCase final : base_test_case
    {
        std::vector<Precinct> Precincts;
        std::vector<size_t> ExpectedIndexes;
        bool Expected;

        TestCase(std::string&& name = "",
            std::vector<Precinct>&& precincts = {},
            std::vector<size_t>&& expectedIndexes = {},
            bool expected = {})
            : base_test_case(forward<string>(name)),
            Precincts(forward<vector<Precinct>>(precincts)),
            ExpectedIndexes(forward<vector<size_t>>(expectedIndexes)),
            Expected(expected)
        {
        }

        bool IsValid() const
        {
            const size_t indexesSize = ExpectedIndexes.size();

            const bool result = Expected && 0 < indexesSize
                || !(Expected) && 0 == indexesSize;

            return result;
        }

        void Validate() const override
        {
            base_test_case::Validate();

            Assert::AreEqual(true, IsValid(), "IsValid");
        }

        void Print(std::ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", Precincts=", Precincts, str);

            ::Print(", ExpectedIndexes=", ExpectedIndexes, str);

            str << " Expected=" << Expected;
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
#ifndef _DEBUG
        {
            TestCase testCase("Simple");

            testCase.Precincts.push_back(Precinct(7, 4));
            testCase.Precincts.push_back(Precinct(3, 8));
            testCase.Precincts.push_back(Precinct(9, 2));
            testCase.Precincts.push_back(Precinct(5, 6));

            testCase.Expected = true;

            testCase.ExpectedIndexes.push_back(0);
            testCase.ExpectedIndexes.push_back(3);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Simple2");

            testCase.Precincts.push_back(Precinct(8, 4));
            testCase.Precincts.push_back(Precinct(4, 8));
            testCase.Precincts.push_back(Precinct(10, 2));
            testCase.Precincts.push_back(Precinct(6, 6));

            testCase.Expected = true;

            testCase.ExpectedIndexes.push_back(0);
            testCase.ExpectedIndexes.push_back(3);

            testCases.push_back(testCase);
        }
#endif
        {
            TestCase testCase("Trivial");

            testCase.Precincts.push_back(Precinct(4, 3));
            testCase.Precincts.push_back(Precinct(5, 2));

            testCase.Expected = true;

            testCase.ExpectedIndexes.push_back(0);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Trivial3");

            const size_t minLoose = 1;
            const size_t minWin = 3;

            testCase.Precincts.push_back(Precinct(minWin, minLoose));
            testCase.Precincts.push_back(Precinct(minWin, minLoose));

            testCase.Expected = true;

            testCase.ExpectedIndexes.push_back(0);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("Trivial2");

            const size_t minLoose = 1;
            const size_t minWin = minLoose + 1;

            testCase.Precincts.push_back(Precinct(minWin, minLoose));
            testCase.Precincts.push_back(Precinct(minWin, minLoose));

            testCase.Expected = true;

            testCase.ExpectedIndexes.push_back(0);

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("NoHalf");

            testCase.Precincts.push_back(Precinct(6, 4));
            testCase.Precincts.push_back(Precinct(5, 5));

            testCase.Expected = false;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("NoHalf2");

            testCase.Precincts.push_back(Precinct(3, 1));
            testCase.Precincts.push_back(Precinct(2, 2));

            testCase.Expected = false;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("NoHalf22");

            testCase.Precincts.push_back(Precinct(2, 2));
            testCase.Precincts.push_back(Precinct(2, 2));

            testCase.Expected = false;

            testCases.push_back(testCase);
        }
        {
            TestCase testCase("NoLess");

            testCase.Precincts.push_back(Precinct(5, 3));
            testCase.Precincts.push_back(Precinct(2, 6));

            testCase.Expected = false;

            testCases.push_back(testCase);
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        vector< size_t > chosenIndexes;

        const bool actual = GerryMandering::Partition(
            testCase.Precincts, chosenIndexes);

        Assert::AreEqual(testCase.Expected, actual, "Result");

        //TODO: p3. test indexes.
        //const string indexesName = "Indexes_" + iAsString;
        //Assert::AreEqual(testCase.ExpectedIndexes, chosenIndexes, indexesName);
    }
}

void Privet::Algorithms::Numbers::Tests::GerryManderingTests()
{
    test_utilities<TestCase>::run_tests<0>(RunTestCase, GenerateTestCases);
}