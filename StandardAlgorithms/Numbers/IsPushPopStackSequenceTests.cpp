#include "../Utilities/PrintUtilities.h"
#include "IsPushPopStackSequence.h"
#include "../test_utilities.h"
#include "IsPushPopStackSequenceTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = short;
    using Data = vector<Number>;

    class TestCase final : public base_test_case
    {
        Data _Array1;
        Data _Array2;
        bool _Expected;

    public:

        TestCase(
            string&& name,
            Data&& array1,
            Data&& array2,
            const bool expected)
            : base_test_case(forward<string>(name)),
            _Array1(forward<Data>(array1)), _Array2(forward<Data>(array2)),
            _Expected(expected)
        {
        }

        inline const Data& get_Array1() const { return _Array1; }
        inline const Data& get_Array2() const { return _Array2; }
        inline bool get_Expected() const { return _Expected; }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            AppendSeparator(str);
            ::Print("Array1", _Array1, str);
            ::Print("Array2", _Array2, str);
            PrintValue(str, "Expected", _Expected);
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        testCases.push_back({ "Equal",{ 1, 2, 3 },{ 1, 2, 3 }, true });
        testCases.push_back({ "Reversed",{ 1, 2, 3 },{ 3, 2, 1 }, true });
        testCases.push_back({ "213",{ 1, 2, 3 }, { 2, 1, 3 }, true });

        testCases.push_back({ "different last item", { 1, 2, 3 }, { 2, 1, 2 }, false });
        testCases.push_back({ "smaller size", { 1, 2, 3 }, { 2, 1 }, false });
        testCases.push_back({ "greater size", { 1, 2, 3 }, { 2, 1, 3, 4 }, false });
        testCases.push_back({ "all same items", { 1, 2, 3 }, { 2, 2, 2 }, false });
        testCases.push_back({ "empty2", { 1, 2, 3 }, {}, {} });
        testCases.push_back({ "impossible 312", { 1, 2, 3 }, { 3, 1, 2}, false });
    }

    void RunTestCase(const TestCase& testCase)
    {
        const auto actual = IsPushPopStackSequence<Number>(
            testCase.get_Array1(), testCase.get_Array2());

        Assert::AreEqual(testCase.get_Expected(), actual, testCase.get_Name());
    }
}

void Privet::Algorithms::Numbers::Tests::IsPushPopStackSequenceTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}