#include "../Utilities/VectorUtilities.h"
#include "SubSetsNumberTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned int;

    struct TestCase final : base_test_case
    {
        using SubSet = SubSet<Number>;
        using SubSets = SubSet::TSubSets;
        using SubSequence = SubSet::TSubSequence;

        Number SetSize;
        SubSets Expected;

        explicit TestCase(
            std::string&& name = "",
            const Number& setSize = 0, const SubSets& expected = SubSets());

        void Validate() const override;

        void Print(std::ostream& str) const override;

        void fix_name();
    };

    static void GenerateTestCases(std::vector<TestCase>& testCases);

    static void RunTestCase(const TestCase& testCase);

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        using SubSet = SubSet<Number>;
        using SubSets = SubSet::TSubSets;
        using SubSequence = SubSet::TSubSequence;

        {
            //testCases.push_back(TestCase());
        }
        {
            TestCase testCase;
            testCase.SetSize = 1;
            testCase.Expected = SubSets{
              SubSet{ SubSequence{1} },
            };
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.SetSize = 2;
            testCase.Expected = SubSets{
              SubSet{ SubSequence{ 1 } },
              SubSet{ SubSequence{ 2 } },
              SubSet{ SubSequence{ 1, 2 } },
              //
              SubSet{ SubSets{
              SubSet{ SubSequence{ 1 } },
              SubSet{ SubSequence{ 2 } }, } },
            };
            testCases.push_back(testCase);
        }
        {
            TestCase testCase = VectorUtilities::Last(testCases);
            testCase.SetSize = 3;
            //    testCase.Expected = {
            // Sub-sequences:
            // { 1 }, { 2 }, { 3 },
            // { 1, 2 }, { 1, 3 }, { 2, 3 },
            // { 1, 2, 3 },
            // Sub-sets:
            //  size 2:
            // { {1}, {2} }, { {1}, {3} }, { {2}, {3} },
            // { {1}, {2,3} }, { {2}, {1,3} }, { {3}, {1,2} },
            //  size 3:
            // { {1}, {2}, {3} }
            //};
            testCases.push_back(testCase);
        }

        //Must be the last line.
        fix_names(testCases);
    }

    TestCase::TestCase(
        string&& name, const Number& setSize, const SubSets& expected)
        : base_test_case(forward<string>(name)),
        SetSize(setSize), Expected(expected)
    {
    }

    void TestCase::Validate() const
    {
        base_test_case::Validate();

        Assert::GreaterOrEqual<Number>(64, SetSize, get_Name() + "_N is too big");

        const size_t size = Expected.size();
        const size_t one = size_t(1);
        const size_t sizeLowerBound = (one << SetSize) - one;
        Assert::GreaterOrEqual(size, sizeLowerBound, get_Name() + "_SubSets.LowerBound");
    }

    void TestCase::Print(ostream& str) const
    {
        base_test_case::Print(str);

        const SubSet expectedSubSet{ Expected };

        str << ", SetSize=" << SetSize
            << ", Expected=" << expectedSubSet;
    }

    void TestCase::fix_name()
    {
        if (get_Name().empty())
        {
            set_Name(to_string(SetSize));
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        const SubSet expectedSubSet{ testCase.Expected };

        const auto actuals = SubSetsNumber<Number>::AllNonEmptySubSets(testCase.SetSize);
        const SubSet actual{ actuals };
        Assert::AreEqual(expectedSubSet, actual, testCase.get_Name());
    }
}

void SubSetsNumberTests::Test()
{
    //TODO: finish. p1. test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}