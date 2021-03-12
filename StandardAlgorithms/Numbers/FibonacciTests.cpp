#include <unordered_map>
#include <vector>
#include "InfInt.h"
#include "../test_utilities.h"
#include "Fibonacci.h"
#include "FibonacciTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = InfInt;
    using TNumberCache = std::unordered_map< int_t, int_t, int_t >;

    struct TestCase final : base_test_case
    {
        unsigned long long int N;
        int_t Expected;

        TNumberCache* NumberCache;

        explicit TestCase(
            std::string&& name = "",
            const unsigned long long int n = 0,
            const int_t& expected = int_t(0),
            TNumberCache* numberCache = nullptr);

        void Print(std::ostream& str) const override;

        bool IsSmall() const;

        void fix_name(TNumberCache* numberCache);
    };

    void GenerateTestCases(vector<TestCase>& testCases, TNumberCache* numberCache)
    {
        ThrowIfNull(numberCache, "numberCache");

        const int someValues[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144 };
        const size_t size = sizeof(someValues) / sizeof(int);

        for (size_t i = 0; i < size; ++i)
        {
            TestCase testCase;
            testCase.N = i;
            testCase.Expected = someValues[i];
            testCases.push_back(testCase);
        }
#ifndef _DEBUG
        {
            TestCase testCase;
            testCase.N = 38;
            testCase.Expected = 39088169;
            testCases.push_back(testCase);
        }
        {
            TestCase testCase;
            testCase.N = 100;
            testCase.set_Name("Big number_" + to_string(testCase.N));
            testCase.Expected = int_t("354224848179261915075");
            testCases.push_back(testCase);
        }
        //  {
        //    TestCase testCase;
        //    testCase.N = 1024;
        //    testCase.set_Name("Big number_" + to_string(testCase.N);
        //    testCase.Expected = int_t("45066996336778198131043832357288860\
        //49367860596218604830803023149600030645708721396248792609141030396244\
        //87326658034501121953020936742558101987106764609420026228520234665586\
        //8899711089246778413354004103631553925405243");
        //    testCases.push_back(testCase);
        //  }
#endif

  //It must be the last line.
        fix_names(testCases, numberCache);
    }

    TestCase::TestCase(
        string&& name,
        const unsigned long long n,
        const int_t& expected,
        TNumberCache* numberCache)
        : base_test_case(forward<string>(name)),
        N(n), Expected(expected), NumberCache(numberCache)
    {
    }

    void TestCase::Print(ostream& str) const
    {
        base_test_case::Print(str);

        str << ", N=" << N
            << ", Expected=" << Expected;
    }

    bool TestCase::IsSmall() const
    {
        return N < 10;
    }

    void TestCase::fix_name(TNumberCache* numberCache)
    {
        NumberCache = numberCache;
        if (get_Name().empty())
        {
            set_Name(to_string(N));
        }
    }

    void RunTestCase(const TestCase& testCase)
    {
        int_t actual = -1;
        if (testCase.IsSmall())
        {
            Fibonacci< int_t >::Slow(testCase.N, actual);
            Assert::AreEqual(testCase.Expected, actual, "Slow");
            actual = -1;
        }

        Fibonacci< int_t >::Fast< int_t >(testCase.N, *(testCase.NumberCache), actual);
        Assert::AreEqual(testCase.Expected, actual, "Fast");
    }
}

void Privet::Algorithms::Numbers::Tests::FibonacciTests()
{
    TNumberCache numberCache;

    test_utilities<TestCase, TNumberCache*>::run_tests(RunTestCase, GenerateTestCases, &numberCache);
}