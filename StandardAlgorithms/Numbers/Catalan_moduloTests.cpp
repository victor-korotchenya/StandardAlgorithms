#include <limits>
#include <utility>
#include <vector>
#include "Catalan_modulo.h"
#include "Catalan_moduloTests.h"
#include "../test_utilities.h"

namespace
{
    using namespace std;
    using namespace Privet::Algorithms::Numbers::Tests;
    using namespace Privet::Algorithms::Numbers;
    using namespace Privet::Algorithms::Tests;
    using namespace Privet::Algorithms;

    using Number = unsigned char;
    using LongNumber = unsigned short;
    using input_output_t = pair<Number, LongNumber>;

    struct TestCase final : base_test_case
    {
        Number MaxNumber;
        Number Modulus;
        vector<input_output_t> SubCases;

        TestCase(
            string&& name = "",
            Number maxNumber = {},
            Number modulus = {},
            vector<input_output_t>&& subCases = {});

        void Validate() const override;

        void Print(ostream& str) const override;
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        const vector<unsigned long long> firstFewNumbers{
          1, 1, 2, 5, 14,
          //5
          42, 132, 429, 1430,
          //9
          4862, 16796, 58786, 208012,
          //13
          742900, 2674440, 9694845, 35357670,
          //17
          129644790, 477638700, 1767263190, 6564120420,
          //21
          24466267020, 91482563640, 343059613650, 1289904147324,
          //25
          4861946401452 };
        const auto size = firstFewNumbers.size();

        constexpr auto zero = static_cast<Number>(0);
        constexpr auto one = static_cast<Number>(1);
        constexpr auto two = static_cast<Number>(2);
        constexpr auto three = static_cast<Number>(3);

        testCases.push_back({ "Simplest", one, two,
        {
          { zero, one},
          { one, one},
        } });
        testCases.push_back({ "Simplest2", two, two,
        {
          { zero, one },
          { one, one },
          { two, zero },
        } });
        testCases.push_back({ "Simplest3", three, three,
        {
          { zero, one },
          { one, one },
          { two, two },
          { three, two },
        } });
        {
            constexpr Number somePrime = 251;

            const auto someIndex = static_cast<Number>(size - 1);
            const auto someValue = static_cast<Number>(
                firstFewNumbers[someIndex] % somePrime);

            constexpr auto beforePrimeIndex = static_cast<Number>(somePrime - 1);
            constexpr auto beforePrimeValue = beforePrimeIndex;

            constexpr auto lastIndex = numeric_limits<Number>::max();
            constexpr Number lastValue = 28;

            testCases.push_back({ "Greatest", lastIndex, somePrime,
            {
              {zero, one},
              { one, one },
              {two, two},
              { three, static_cast<Number>(5)},
              { someIndex, someValue },
              { beforePrimeIndex, beforePrimeValue },
              { lastIndex, lastValue },
            } });
        }

        {
            const auto maxNumber = static_cast<Number>(size - 1);

            vector<input_output_t> subCases(size);

            //constexpr Number somePrime = 65521;
            constexpr Number somePrime = 251;

            for (Number i = 0; i <= maxNumber; i++)
            {
                const auto expected = static_cast<Number>(
                    firstFewNumbers[i] % somePrime);

                subCases[i] = { i,expected };
            }

            testCases.push_back({ "Large", maxNumber, somePrime, move(subCases) });
        }
    }

    TestCase::TestCase(
        std::string&& name,
        Number maxNumber,
        Number modulus,
        vector<input_output_t>&& subCases)
        :
        base_test_case(forward<string>(name)),
        MaxNumber(maxNumber),
        Modulus(modulus),
        SubCases(forward<vector<input_output_t>>(subCases))
    {
    }

    void TestCase::Validate() const
    {
        base_test_case::Validate();
        RequirePositive(MaxNumber, "MaxNumber");
        RequirePositive(Modulus, "Modulus");
        RequirePositive(SubCases.size(), "SubCases.size()");

        static_assert(is_unsigned<Number>::value,
            "The Number must be unsigned.");

        const auto maxPossibleModulus = static_cast<Number>(
            static_cast<Number>(0) - static_cast<Number>(1));
        Assert::GreaterOrEqual(maxPossibleModulus, Modulus, "maxPossibleModulus");

        for (const auto& subCase : SubCases)
        {
            Assert::GreaterOrEqual(MaxNumber, subCase.first, "MaxNumber");
            Assert::Greater(Modulus, subCase.second, "Modulus");
        }
    }

    void TestCase::Print(ostream& str) const
    {
        base_test_case::Print(str);

        str << ", MaxNumber=" << static_cast<size_t>(MaxNumber)
            << ", Modulus=" << static_cast<size_t>(Modulus);
    }

    void RunTestCase(const TestCase& testCase)
    {
        Catalan_modulo<Number, LongNumber> catalan(
            testCase.MaxNumber,
            testCase.Modulus);

        const auto prefix = testCase.get_Name() + "_";

        for (const auto& subCase : testCase.SubCases)
        {
            const auto actual = catalan.get(subCase.first);
            const auto name = prefix + to_string(subCase.first);

            Assert::AreEqual(subCase.second, actual, name);
        }
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
                void Catalan_moduloTests()
                {
                    test_utilities<TestCase>::run_tests<0>(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}