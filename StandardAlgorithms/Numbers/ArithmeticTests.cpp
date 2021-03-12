#include <limits>
#include <sstream>
#include <vector>
#include <tuple>
#include <utility>
#include "Arithmetic.h"
#include "NumberUtilities.h"
#include "factorials_cached.h"
#include "../Utilities/RandomGenerator.h"
#include "ArithmeticTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms;

namespace
{
    bool IsSigned;
    bool ShallTestReverse;
    void* add_function_p = nullptr;
    long long int ArgumentA, ArgumentB;

    void test_modular_inverses_first_n()
    {
        const string name = "test_modular_inverses_till_";
        const int data[] = { 3, 5,7,11,13, 17, 97 };

        vector<int> actual, expected{ 0, 1 };
        for (const auto& n : data)
        {
            expected.resize(n + 1ll);
            for (auto i = 2; i < n; ++i)
                expected[i] = static_cast<int>(modulo_inverse_simple<int64_t>(i, n));

            actual.clear();
            modular_inverses_first_n<int, int64_t>(n, n, actual);
            Assert::AreEqual(expected, actual, name + to_string(n));
        }
    }
}

static void FailRun()
{
    if (nullptr == add_function_p)
    {
        throw exception("nullptr == add_function_p");
    }

    if (IsSigned)
    {
        ArithmeticTests::AddFunction< ArithmeticTests::TSigned > addFunction =
            static_cast<ArithmeticTests::AddFunction< ArithmeticTests::TSigned >>(add_function_p);

        const auto a = static_cast<ArithmeticTests::TSigned>(ArgumentA);
        const auto b = static_cast<ArithmeticTests::TSigned>(ArgumentB);

        addFunction(a, b);
    }
    else
    {
        ArithmeticTests::AddFunction< ArithmeticTests::TUnsigned > addFunction =
            static_cast<ArithmeticTests::AddFunction< ArithmeticTests::TUnsigned >>(add_function_p);

        const auto a = static_cast<ArithmeticTests::TUnsigned>(ArgumentA);
        const auto b = static_cast<ArithmeticTests::TUnsigned>(ArgumentB);

        addFunction(a, b);
    }
}

template <typename T >
void ArithmeticTests::Success(
    const vector< Triple< T > >& testCases,
    const string& prefixName,
    AddFunction< T > aAddFunction)
{
    if (nullptr == aAddFunction)
    {
        throw exception("nullptr == aAddFunction");
    }

    const size_t testCasesSize = testCases.size();

    for (size_t i = 0; i < testCasesSize; ++i)
    {
        const Triple< T >& testCase = testCases[i];

        ostringstream ssName;
        ssName << prefixName << "_" << testCase;
        const string name = ssName.str();

        const T actual = aAddFunction(testCase.A, testCase.B);
        Assert::AreEqual(testCase.Expected, actual, name);

        if (ShallTestReverse)
        {
            const T actual2 = aAddFunction(testCase.B, testCase.A);
            Assert::AreEqual(testCase.Expected, actual2, name + "_Reverse");
        }
    }
}

template <typename T >
void ArithmeticTests::Fail(
    const vector< Triple< T > >& testCases,
    const string& prefixName,
    const string& badPart1,
    const string& badPart2,
    AddFunction< T > addFunction)
{
    ThrowIfNull(addFunction, "addFunction");

    const size_t testCasesSize = testCases.size();

    for (size_t i = 0; i < testCasesSize; ++i)
    {
        const Triple< T >& testCase = testCases[i];

        ostringstream ssName;
        ssName << prefixName << "_" << testCase;
        const string name = ssName.str();

        {
            ostringstream ss;
            //TODO: p1. Remove copy-paste.
            ss << badPart1 << testCase.A << badPart2 << testCase.B << " will produce an overflow.";
            const string expectedMessage = ss.str();

            ArgumentA = testCase.A;
            ArgumentB = testCase.B;
            Assert::ExpectException< ArithmeticException >(FailRun, expectedMessage, name);
        }
        if (ShallTestReverse)
        {
            ostringstream ss;
            //TODO: p1. Remove copy-paste.
            ss << badPart1 << testCase.B << badPart2 << testCase.A << " will produce an overflow.";
            const string expectedMessage = ss.str();

            ArgumentA = testCase.B;
            ArgumentB = testCase.A;
            Assert::ExpectException< ArithmeticException >(FailRun, expectedMessage, name);
        }
    }
}

void ArithmeticTests::TestAddUnsigned()
{
    const string prefixName = "AddUnsigned";
    AddFunction< TUnsigned > addFunction = AddUnsigned< TUnsigned >;
    IsSigned = false;
    add_function_p = addFunction;

    {
        vector< TU3 > testCases;

        testCases.push_back({ 1, 3, 4 });
        testCases.push_back({ 0, USHRT_MAX, USHRT_MAX });
        testCases.push_back({ 0, 0, 0 });

        Success< TUnsigned >(testCases, prefixName, addFunction);
    }
    {
        vector< TU3 > testCases;

        testCases.push_back({ USHRT_MAX, USHRT_MAX, USHRT_MAX });

        Fail< TUnsigned >(testCases, prefixName, "Adding ", " and ", addFunction);
    }
}

void ArithmeticTests::TestAddSigned()
{
    const string prefixName = "AddSigned";
    AddFunction< TSigned > addFunction = AddSigned< TSigned >;
    IsSigned = true;
    add_function_p = addFunction;

    {
        vector< TS3 > testCases;

        testCases.push_back({ 1, 3, 4 });
        testCases.push_back({ 0, SHRT_MAX, SHRT_MAX });
        testCases.push_back({ 10, SHRT_MAX - 10, SHRT_MAX });
        testCases.push_back({ SHRT_MAX >> 1, SHRT_MAX - (SHRT_MAX >> 1), SHRT_MAX });
        testCases.push_back({ 0, 0, 0 });

        Success< TSigned >(testCases, prefixName, addFunction);
    }
    {
        vector< TS3 > testCases;

        testCases.push_back({ SHRT_MIN, SHRT_MIN, 0 });
        testCases.push_back({ SHRT_MAX, SHRT_MAX, 0 });
        testCases.push_back({ 1, SHRT_MAX, 0 });
        testCases.push_back({ -1, SHRT_MIN, 0 });

        Fail< TSigned >(testCases, prefixName, "Adding ", " and ", addFunction);
    }
}

void ArithmeticTests::TestSubtractSigned()
{
    const string prefixName = "SubtractSigned";
    AddFunction< TSigned > addFunction = SubtractSigned< TSigned >;
    IsSigned = true;
    add_function_p = addFunction;

    {
        vector< TS3 > testCases;

        testCases.push_back({ 1, 3, -2 });
        testCases.push_back({ 3, 1, 2 });
        testCases.push_back({ 3, 0, 3 });
        testCases.push_back({ 0, SHRT_MAX, -SHRT_MAX });
        testCases.push_back({ SHRT_MAX, 0, SHRT_MAX });
        testCases.push_back({ -SHRT_MAX, -SHRT_MAX, 0 });
        testCases.push_back({ SHRT_MAX, SHRT_MAX, 0 });
        testCases.push_back({ SHRT_MIN, SHRT_MIN, 0 });
        testCases.push_back({ SHRT_MIN, 0, SHRT_MIN });
        testCases.push_back({ SHRT_MIN, -500, -32268 });
        testCases.push_back({ -10, SHRT_MAX - 10, -SHRT_MAX });
        testCases.push_back({ 0, 0, 0 });
        testCases.push_back({ -1, SHRT_MAX, SHRT_MIN });
        testCases.push_back({ -1, SHRT_MIN, SHRT_MAX });

        Success< TSigned >(testCases, prefixName, addFunction);
    }
    {
        vector< TS3 > testCases;

        testCases.push_back({ SHRT_MAX, -SHRT_MAX, 0 });
        testCases.push_back({ 0, SHRT_MIN, 0 });
        testCases.push_back({ -2, SHRT_MAX, 0 });
        testCases.push_back({ 1, SHRT_MIN, 0 });

        Fail< TSigned >(testCases, prefixName, "Subtracting ", " minus ", addFunction);
    }
}

void ArithmeticTests::Test_numbers_having_reminder()
{
    using t = unsigned;
    constexpr t divisor = 5;

    const auto test_cases = vector<tuple<t, t, t>>{
      make_tuple(0, 1, 0),
      make_tuple(0, 2, 0),
      make_tuple(0, 3, 0),
      make_tuple(0, 4, 0),
      make_tuple(0, 5, 1),
      make_tuple(0, 6, 1),
      make_tuple(0, 9, 1),
      make_tuple(0, 10, 2),
      make_tuple(0, 11, 2),
      //
      make_tuple(1, 1, 1),
      make_tuple(1, 2, 1),
      make_tuple(1, 3, 1),
      make_tuple(1, 4, 1),
      make_tuple(1, 5, 1),
      make_tuple(1, 6, 2),
      make_tuple(1, 9, 2),
      make_tuple(1, 10, 2),
      make_tuple(1, 11, 3),
      //
      make_tuple(2, 1, 0),
      make_tuple(2, 2, 1),
      make_tuple(2, 3, 1),
      make_tuple(2, 4, 1),
      make_tuple(2, 5, 1),
      make_tuple(2, 6, 1),
      make_tuple(2, 7, 2),
      make_tuple(2, 9, 2),
      make_tuple(2, 11, 2),
      make_tuple(2, 12, 3),
      //
      make_tuple(3, 1, 0),
      make_tuple(3, 2, 0),
      make_tuple(3, 3, 1),
      make_tuple(3, 4, 1),
      make_tuple(3, 7, 1),
      make_tuple(3, 8, 2),
      make_tuple(3, 9, 2),
      make_tuple(3, 12, 2),
      make_tuple(3, 13, 3),
      //
      make_tuple(4, 1, 0),
      make_tuple(4, 2, 0),
      make_tuple(4, 3, 0),
      make_tuple(4, 4, 1),
      make_tuple(4, 5, 1),
      make_tuple(4, 8, 1),
      make_tuple(4, 9, 2),
      make_tuple(4, 10, 2),
      make_tuple(4, 13, 2),
      make_tuple(4, 14, 3),
    };

    for (const auto& test_case : test_cases)
    {
        const auto actual = numbers_having_reminder<t, divisor>(
            get<0>(test_case), get<1>(test_case));

        Assert::AreEqual(get<2>(test_case), actual,
            "numbers_having_reminder("
            + to_string(get<0>(test_case)) + ", "
            + to_string(get<1>(test_case)) + ")");
    }
}

namespace
{
    void TestGcd()
    {
        using Number = uint64_t;
        using TestCase = tuple<Number, Number, Number>;

        const vector<TestCase> tests{
          TestCase(0, 0, 0),
          TestCase(0, 1, 1),
          TestCase(0, 10, 10),
          TestCase(10, 0, 10),
          TestCase(1, 1, 1),
          TestCase(1, 2, 1),
          TestCase(2, 2, 2),
          TestCase(2, 4, 2),
          TestCase(21, 14, 7),
          TestCase(11 * 13 * 7 * 19, 39 * 13 * 7 * 89, 13 * 7),
        };

        for (const auto& testCase : tests)
        {
            const auto& a = get<0>(testCase);
            const auto& b = get<1>(testCase);
            const auto& expected = get<2>(testCase);

            const auto name = "GCD(" + to_string(a) + ", " + to_string(b) + ")";
            {
                const auto actual = gcd_unsigned(a, b);
                Assert::AreEqual(expected, actual, name);
            }
            {
                const auto actual = gcd_unsigned(b, a);
                Assert::AreEqual(expected, actual, name + "_reverse");
            }

            const auto name2 = "2_" + name;
            const auto name3 = "3_" + name;
            {
                Number x, y;
                const auto actual = gcd_extended(a, b, x, y);
                Assert::AreEqual(expected, actual, name2);

                if (0 != a && 0 != b)
                {
                    const auto actual3 = a * x + b * y;
                    Assert::AreEqual(expected, actual3, name3);

                    x = y = 0;
                    const auto g = gcd_extended_simple(a, b, x, y);
                    const auto name3g = "3_gcde_" + name;
                    Assert::AreEqual(expected, g, name3g);

                    const auto actual3g = a * x + b * y;
                    Assert::AreEqual(expected, actual3g, name3g + "_sum");
                }
            }
            {
                Number x, y;
                const auto actual = gcd_extended(b, a, y, x);
                Assert::AreEqual(expected, actual, name2 + "_reverse");

                if (0 != a && 0 != b)
                {
                    const auto actual3 = a * x + b * y;
                    Assert::AreEqual(expected, actual3, name3);

                    x = y = 0;
                    const auto g = gcd_extended_simple(b, a, y, x);
                    const auto name3g = "3_gcde_rev_" + name;
                    Assert::AreEqual(expected, g, name3g);

                    const auto actual3g = a * x + b * y;
                    Assert::AreEqual(expected, actual3g, name3g + "_sum");
                }
            }
        }
    }

    void Test_ModuloInverse()
    {
        using Number = long long;
        constexpr auto Modulo = 1000 * 1000 * 1000 + 7;
        using TestCase = tuple<Number, Number>;

        TestCase randomTestCase;
        {
            IntRandom random_generator;
            const Number someNumber = random_generator.operator()(1, Modulo - 1);

            const auto actual = modulo_inverse<Number>(someNumber, Modulo);
            randomTestCase = TestCase(someNumber, actual);
        }

        const vector<TestCase> tests{
          TestCase(1, 1),
          TestCase(2, 500000004),
          randomTestCase
        };

        for (const auto& testCase : tests)
        {
            const auto& a = get<0>(testCase);
            const auto expected = get<1>(testCase);
            const auto name = "modulo_inverse " + to_string(a);

            const auto actual = modulo_inverse<Number>(a, Modulo);
            Assert::AreEqual(expected, actual, name);
            Assert::GreaterOrEqual(actual, 0, name);
            Assert::Greater(Modulo, actual, name);

            const auto prod = (a * actual) % Modulo;
            Assert::AreEqual(1, prod, "prod_" + name);
        }
    }

    void Test_find_first_byte()
    {
        using N = unsigned long long;
        using P = pair<N, unsigned>;
        const P tests[] = {
          { 0x1234567890ABCDEF, 8 },
          { 0, 7 },
          { 0xFE2F30797E7F8081, 8 },
          { 0xFE2F03797E7F8081, 5 },
          { 0xFFFF, 7 },
          { 0x001029FFEE000001, 7 },
          { 0x27E847F0FF303139, 8 },
          { 0x2E2F30797E7F0410, 1 },
          //
          { 0xFFFFFFFFFFFFFFFF, 8 },
          { 0xFF09FFFFFFFFFFFF, 6 },
          { 0xFF0AFFFFFFFFFFFF, 8 },
          { 0xFFFF07FFFFFFFFFF, 5 },
          { 0xFFFF0700FFFFFF00, 5 },
          { 0xFFFFFFFFFFFFFF04, 0 },
          { 0xFFFFFFFFFFFF0304, 1 },
        };

        for (const auto& test : tests)
        {
            const auto actual = find_first_byte(test.first);

            const auto name = "Test_find_first_byte(" + to_string(test.first);
            Assert::AreEqual(test.second, actual, name);
        }
    }

    void Test_multi_byte_subtract()
    {
        using N = unsigned long long;
        using P = pair<N, N>;
        const P tests[] = {
          { 0, 0xD0D0D0D0D0D0D0D0 },
          { 0x1234567890ABCDEF, 0xE2042648607B9DBF },
          { 0x001029FFEE000001, 0xD0E0F9CFBED0D0D1 },
          { 0x27E847F0FF303139, 0xF7B817C0CF000109 },
          { 0x2E2F30797E7F8081, 0xFEFF00494E4F5051 }
        };

        for (const auto& test : tests)
        {
            constexpr N subtrahend = 0x3030303030303030ull;

            auto actual = test.first;
            multi_byte_subtract<subtrahend>(actual);

            const auto name = "multi_byte_subtract(" + to_string(test.first);
            Assert::AreEqual(test.second, actual, name);
        }
    }

    void Test_powers_sum()
    {
        using N = unsigned long long;
        constexpr unsigned mod = 1000 * 1000 * 1000 + 7;

        //base, power, actual.
        const auto tests = vector<tuple<N, N, N>>{
          make_tuple(0, 0, 0),
          make_tuple(0, 1, 0),
          make_tuple(0, 2, 0),
          make_tuple(0, 235, 0),
          //
          make_tuple(1, 0, 0),
          make_tuple(2, 0, 0),
          make_tuple(3, 0, 0),
          make_tuple(mod - 2, 0, 0),
          make_tuple(mod - 1, 0, 0),
          make_tuple(mod, 0, 0),
          make_tuple(mod + 1, 0, 0),
          make_tuple(mod + 2, 0, 0),
          make_tuple(mod + 3, 0, 0),
          //
          make_tuple(1, 1, 1),
          make_tuple(1, 2, 2),
          make_tuple(1, 5, 5),
          make_tuple(1, mod + 1234, 1234),
          //
          make_tuple(2, 1, 1),
          make_tuple(2, 2, 3),
          make_tuple(2, 3, 7),
          make_tuple(2, 4, 15),
          make_tuple(2, 5, 31),
          make_tuple(2, 6, 63),
          make_tuple(2, 32, 294967267),//4294967295
          //
          make_tuple(3, 1, 1),//1
          make_tuple(3, 2, 4),//3
          make_tuple(3, 3, 13),//9
          make_tuple(3, 4, 40),//27
          make_tuple(3, 5, 121),//81
          make_tuple(3, 6, 121 + 243),//243
          make_tuple(3, 32, 87940350),//Medium complexity to check.
        };

        for (const auto& test : tests)
        {
            const auto actual = powers_sum<N, mod>(get<0>(test), get<1>(test));

            const auto name = "powers_sum(" + to_string(get<0>(test))
                + ", " + to_string(get<1>(test));
            Assert::AreEqual(get<2>(test), actual, name);

            if (!(0 <= get<2>(test) && get<2>(test) < mod))
            {
                const string s = "The reminder (" + to_string(get<2>(test))
                    + ") is bad in powers_sum test.";
                throw runtime_error(s.c_str());
            }
        }
    }
}

void Test_factorials_cached()
{
    using factorial_t = factorials_cached<>;
    const vector<factorial_t> fcs{
      factorial_t(1),
      factorial_t(5),
      factorial_t(10000)
    };

    //index, n,k, result.
    using t_t = tuple<unsigned, unsigned, unsigned, unsigned>;

    const vector<t_t> tests{
      make_tuple(0, 0, 0, 1),
      make_tuple(0, 1, 0, 1),
      make_tuple(0, 1, 1, 1),
      //
      make_tuple(1, 0, 0, 1),
      make_tuple(1, 1, 0, 1),
      make_tuple(1, 1, 1, 1),
      make_tuple(1, 3, 2, 3),
      make_tuple(1, 3, 1, 3),
      make_tuple(1, 3, 3, 1),
      make_tuple(1, 5, 3, 10),
      make_tuple(1, 5, 2, 10),
      make_tuple(1, 5, 0, 1),
      make_tuple(1, 5, 1, 5),
    };

    auto i = 0;
    for (const auto& test : tests)
    {
        const auto& a0 = get<0>(test);
        const auto& a1 = get<1>(test);
        const auto& a2 = get<2>(test);
        const auto& fc = fcs[a0];
        const auto actual = fc.choose_modulo(a1, a2);

        const auto& expected = get<3>(test);
        Assert::AreEqual(expected, actual,
            "Test factorials_cached n_" + std::to_string(i)
            + " failed.");
        ++i;
    }

    for (auto j = 1; j <= 2; ++j)
    {
        const auto name = std::to_string(j);
        Assert::AreEqual(120u, fcs[j].factorial_modulo(5), "factorial_modulo_" + name);
        Assert::AreEqual(808333339u, fcs[j].inverse_factorial_modulo(5), "inverse_factorial_modulo_" + name);
    }

    Assert::AreEqual(531950728u, fcs[2].factorial_modulo(10000), "factorial_modulo");
    Assert::AreEqual(556156297u, fcs[2].inverse_factorial_modulo(10000), "inverse_factorial_modulo");

    Assert::AreEqual(8365757u, fcs[2].factorial_modulo(1234), "factorial_modulo");
    Assert::AreEqual(53320459u, fcs[2].inverse_factorial_modulo(1234), "inverse_factorial_modulo");
}

void Test_decompose_into_divisors_with_powers()
{
    using number_t = long long;
    using pair_t = pair<number_t, int>;

    constexpr auto a = static_cast<number_t>(4) * 625 * 25 * 243 * 243 * 243;
    const vector<pair_t> expected{
      { 2,2 },
      { 3,15 },
      { 5,6 }
    };

    vector<pair_t> components;
    decompose_into_divisors_with_powers<number_t, number_t>(a, components);
    Assert::AreEqual(expected, components, __FUNCTION__);

    components.clear();
    decompose_into_divisors_with_powers_slow<number_t>(a, components);
    Assert::AreEqual(expected, components, __FUNCTION__ + string("_slow"));
}

void ArithmeticTests::Test()
{
    ShallTestReverse = true;
    TestAddUnsigned();
    TestAddSigned();

    ShallTestReverse = false;
    TestSubtractSigned();
    Test_numbers_having_reminder();
    TestGcd();
    Test_ModuloInverse();
    Test_multi_byte_subtract();
    Test_find_first_byte();
    Test_powers_sum();
    Test_factorials_cached();
    Test_decompose_into_divisors_with_powers();
    test_modular_inverses_first_n();
}