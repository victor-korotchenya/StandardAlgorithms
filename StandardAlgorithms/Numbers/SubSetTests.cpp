#include <vector>
#include "../test_utilities.h"
#include "SubSet.h"
#include "SubSetTests.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Item = unsigned short;

    struct TestCase final : base_test_case
    {
        using SubSet = SubSet<Item>;
        using SubSets = SubSet::TSubSets;
        using SubSequence = SubSet::TSubSequence;

        SubSet Input;

        explicit TestCase(
            std::string&& name = "",
            const SubSet& subSet = SubSet());

        void Validate(void) const override;

        void Print(std::ostream& str) const override;
    };

    static void GenerateTestCases(std::vector<TestCase>& testCases);

    static void RunTestCase(const TestCase& testCase);

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        using SubSet = SubSet<Item>;
        using SubSets = SubSet::TSubSets;
        using SubSequence = SubSet::TSubSequence;

        //The empty set.
        {
            testCases.push_back(TestCase{ "{}" });
        }
        //Singles.
        {
            testCases.push_back(TestCase{ "{83}", SubSet(SubSequence{ 83 }) });
        }
        {
            testCases.push_back(TestCase{ "{134}", SubSet(SubSet{ SubSequence{ 134 } }) });
        }
        {
            testCases.push_back(TestCase{ "{275}", SubSet(SubSet{ SubSet(SubSequence{ 275 }) }) });
        }
        {
            testCases.push_back(TestCase{ "{2178}",
              SubSet(SubSet{ SubSet{ SubSet(SubSequence{ 2178 }) } }) });
        }
        //Doubles.
        {
            testCases.push_back(TestCase{ "{2, 73}", SubSet(SubSequence{
                //Notice that the order is reversed.
                73, 2 }) });
        }
        {
            testCases.push_back(TestCase{ "{4, 730}", SubSet(SubSet(SubSequence{ 730, 4 })) });
        }
        {
            //TODO: p3. printout is not sorted.
            testCases.push_back(TestCase{ "{{72}, {5}}",
              SubSet(SubSequence{ 72 }, { SubSet{ SubSequence{ 5 } } }) });
        }
        {
            testCases.push_back(TestCase{ "{{51}, {832}}",
              SubSet{ SubSets{ SubSet(SubSequence{ 832 }), SubSet(SubSequence{ 51 }) } } });
        }
        //Triplets.
        {
            testCases.push_back(TestCase{ "{5, 8, 19}", SubSet{ SubSequence{ 19, 5, 8 } } });
        }
        {
            testCases.push_back(TestCase{ "{{2}, {9}, {87}}",
              SubSet(SubSets{
              SubSet{ SubSequence{ 9, } },
              SubSet{ SubSequence{ 87, } },
              SubSet{ SubSequence{ 2, } },
            }) });
        }
        {
            testCases.push_back(TestCase{ "{{9}, {2}, {8}}",
              SubSet(SubSequence{ 9 }, SubSets{
              SubSet{ SubSequence{ 8, } },
              SubSet{ SubSequence{ 2, } },
            }) });
        }
        {
            testCases.push_back(TestCase{ "{{9}, {5, 8}}",
              SubSet(SubSequence{ 9 }, SubSets{ SubSet{ SubSequence{ 8, 5 } } }) });
        }
        //4-tuples
        {
            testCases.push_back(TestCase{ "{{9}, {20}, {51, 83}}",
              SubSet(SubSequence{ 9 }, SubSets{
              SubSet{ SubSequence{ 83, 51 } },
              SubSet{ SubSequence{ 20 } },
            }) });
        }
        {
            testCases.push_back(TestCase{ "{{5137}, {{54, 83}, {17}}}",
              SubSet(SubSequence{ 5137 }, SubSets{
              SubSet{ SubSequence{ 83, 54 }, SubSets{ SubSet(SubSequence{ 17 }) } },
            }) });
        }

        //TODO: P0. end.
    }

    TestCase::TestCase(string&& name, const SubSet& subSet)
        : base_test_case(forward<string>(name)),
        Input(subSet)
    {
    }

    void TestCase::Validate() const
    {
        base_test_case::Validate();

        bool isOk = false;
        size_t count = 0;

        auto name = get_Name();

        for (const char& c : name)
        {
            if ('{' == c)
            {
                isOk = true;
                ++count;
            }
            else if ('}' == c)
            {
                Assert::Greater(count, 0, name + "_count closing parenthesis");

                --count;
            }
        }

        Assert::AreEqual<size_t>(0, count, name + "_count");
        Assert::AreEqual(true, isOk, name + "_missing parenthesis");
    }

    void TestCase::Print(ostream& str) const
    {
        base_test_case::Print(str);

        str << ", Input=" << Input;
    }

    void RunTestCase(const TestCase& testCase)
    {
        ostringstream ss;
        ss << testCase.Input;
        const string actual = ss.str();
        Assert::AreEqual(testCase.get_Name(), actual, "equal");
    }
}

void Privet::Algorithms::Numbers::Tests::SubSetTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}