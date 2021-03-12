#include <cmath>
#include "Assert.h"

using namespace std;
using namespace Privet::Algorithms;

namespace
{
    bool are_equal(const double a, const double b, const double epsilon)
    {
#ifdef _DEBUG
        RequirePositive(epsilon, "epsilon");
#endif
        if (a == b)
            return true;
        const auto absoluteA = fabs(a), absoluteB = fabs(b);
        const auto result1 = absoluteA <= epsilon && absoluteB <= epsilon;
        if (result1)
            return result1;

        //const bool infA = IsInfinitePtr(a);
        //const bool infB = IsInfinitePtr(b);
        //if (infA)
        //{
        //  return infB;
        //}

        //It does not work to compare 0 and small number, but it must!
        //
        //const Number maxValue = max(absoluteA, absoluteB);
        //const bool result = delta <= maxValue * Epsilon;
        //return result;

        const auto sum = absoluteA + absoluteB;
        if (0 == sum)
            //todo: check. p0. div by 0, overflow.
            return false;

        const auto delta = fabs(a - b);
        const auto div = delta / sum;
        const auto result = div <= epsilon;
        return result;
    }
}

void Assert::AreEqualWithEpsilon(const double& a, const double& b, const char* message, const double epsilon)
{
    if (are_equal(a, b, epsilon))
        return;

    NotEqualThrowException<double, double>(a, b, message);
}