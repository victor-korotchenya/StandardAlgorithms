#include <iomanip> // setprecision
#include <sstream>

#include "Arithmetic.h"
#include "../Assert.h"
#include "NumberComparer.h"
#include "NumberComparerTests.h"
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/TestNameUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

using Number = double;
using Comparer = NumberComparer< Number >;

struct NumCompTestCase final
{
    string Name;
    inline const std::string& get_Name() const
    {
        return Name;
    }
    Number A{};
    Number B{};
    bool ExpectedAreEqual{};

    friend ostream& operator <<
        (ostream& str, NumCompTestCase const& b)
    {
        const auto prec = StreamUtilities::GetMaxDoublePrecision();

        str << setprecision(prec)
            << "Name='" << b.Name
            << "', A=" << b.A
            << "', B=" << b.B
            << "', ExpectedAreEqual=" << b.ExpectedAreEqual;
        return str;
    }
};

void BuildTestCases(
    const Number epsilon,
    vector< NumCompTestCase >& testCases)
{
    constexpr auto infinity = numeric_limits< double >::infinity();

    {
        NumCompTestCase testCase;
        testCase.Name = "0 == epsilon/2";
        testCase.A = 0;
        testCase.B = epsilon / 2;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "0 == -epsilon/2";
        testCase.A = 0;
        testCase.B = epsilon / (-2);
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "0 != 2 * epsilon";
        testCase.A = 0;
        testCase.B = 2 * epsilon;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "0 != -2 * epsilon";
        testCase.A = 0;
        testCase.B = (-2) * epsilon;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "epsilon != 2 * epsilon";
        testCase.A = epsilon;
        testCase.B = 2 * epsilon;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "epsilon != 3 * epsilon";
        testCase.A = epsilon;
        testCase.B = 3.0 * epsilon;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "0 == epsilon";
        testCase.A = 0;
        testCase.B = epsilon;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "0 == -epsilon";
        testCase.A = 0;
        testCase.B = -epsilon;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "1 == 1 + epsilon";
        testCase.A = 1;
        testCase.B = 1 + epsilon;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "1 == 1 - epsilon";
        testCase.A = 1;
        testCase.B = 1 - epsilon;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "1 != epsilon";
        testCase.A = 1;
        testCase.B = epsilon;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        //Both small values.
        NumCompTestCase testCase;
        testCase.Name = "epsilon == epsilon * epsilon";
        testCase.A = epsilon;
        testCase.B = epsilon * epsilon;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.A = 1;
        testCase.B = 1;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.A = -1;
        testCase.B = -1;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.A = -1;
        testCase.B = 1;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.A = 12345;
        testCase.B = 12346;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.A = 123456;
        testCase.B = 123457;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.A = 123456789;
        testCase.B = 123456788;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.A = 500;
        testCase.B = 501;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "500 == 500 + epsilon";
        testCase.A = 500;
        testCase.B = 500 + epsilon;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "500 == 500 - epsilon";
        testCase.A = 500;
        testCase.B = 500 - epsilon;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "500 != 500 * epsilon";
        testCase.A = 500;
        testCase.B = 500 * epsilon;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "2 close numbers to be ==";
        testCase.A = 0.12345678912345678;
        testCase.B = 0.1234567891234568;

        testCase.ExpectedAreEqual = true;

        Assert::NotEqual(testCase.A, testCase.B, testCase.Name);

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.A = epsilon;
        testCase.B = epsilon;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "epsilon == -epsilon";
        testCase.A = epsilon;
        testCase.B = -epsilon;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "1.1 * epsilon != epsilon";
        testCase.A = 1.1 * epsilon;
        testCase.B = epsilon;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "1.1 * epsilon != -epsilon";
        testCase.A = 1.1 * epsilon;
        testCase.B = -epsilon;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "infinity == infinity";
        testCase.A = infinity;
        testCase.B = infinity;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "-infinity == -infinity";
        testCase.A = -infinity;
        testCase.B = -infinity;
        testCase.ExpectedAreEqual = true;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "0 != infinity";
        testCase.A = 0;
        testCase.B = infinity;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "10 != infinity";
        testCase.A = 10;
        testCase.B = infinity;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "-10 != infinity";
        testCase.A = -10;
        testCase.B = infinity;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
    {
        NumCompTestCase testCase;
        testCase.Name = "epsilon != infinity";
        testCase.A = epsilon;
        testCase.B = infinity;
        testCase.ExpectedAreEqual = false;

        testCases.push_back(testCase);
    }
}

void FixEmptyTestName(NumCompTestCase& testCase)
{
    testCase.Name.append(to_string(testCase.A));
    testCase.Name += testCase.ExpectedAreEqual ? " == " : " != ";
    testCase.Name.append(to_string(testCase.B));
}

void CheckTestName(const NumCompTestCase& testCase)
{
    const size_t eqIndex = testCase.Name.find("==");
    const bool hasEq = string::npos != eqIndex;

    const size_t neqIndex = testCase.Name.find("!=");
    const bool hasNeq = string::npos != neqIndex;

    if (testCase.ExpectedAreEqual)
    {
        if (!hasEq)
        {
            ostringstream ss;
            ss << "There must be equal in test case: " << testCase;
            StreamUtilities::throw_exception(ss);
        }

        if (hasNeq)
        {
            ostringstream ss;
            ss << "There is an unexpected not equal in test case: " << testCase;
            StreamUtilities::throw_exception(ss);
        }
    }
    else
    {
        if (!hasNeq)
        {
            ostringstream ss;
            ss << "There must be not equal in test case: " << testCase;
            StreamUtilities::throw_exception(ss);
        }

        if (hasEq)
        {
            ostringstream ss;
            ss << "There is an unexpected equal in test case: " << testCase;
            StreamUtilities::throw_exception(ss);
        }
    }
}

void CheckAndFixTestName(NumCompTestCase& testCase)
{
    if (0 == testCase.Name.length())
    {
        FixEmptyTestName(testCase);
        return;
    }

    CheckTestName(testCase);
}

void NumberComparerTests::Test()
{
    const Number epsilon = 1.0e-5;

    Comparer comparer(epsilon, AbsoluteValue);

    vector< NumCompTestCase > testCases;
    BuildTestCases(epsilon, testCases);

    const size_t count = testCases.size();
    for (size_t i = 0; i < count; ++i)
    {
        NumCompTestCase& testCase = testCases[i];
        CheckAndFixTestName(testCase);
    }
    TestNameUtilities::RequireUniqueNames(testCases, "testCases");

    for (size_t i = 0; i < count; ++i)
    {
        const NumCompTestCase& testCase = testCases[i];

        const bool actual = comparer.AreEqual(testCase.A, testCase.B);
        Assert::AreEqual(testCase.ExpectedAreEqual, actual, testCase.Name);

        string message = testCase.Name + "_Symmetric";
        const bool symmetric = comparer.AreEqual(testCase.B, testCase.A);
        Assert::AreEqual(testCase.ExpectedAreEqual, symmetric, message.c_str());
    }
}