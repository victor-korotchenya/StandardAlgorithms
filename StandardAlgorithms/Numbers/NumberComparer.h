#pragma once

#include <stdexcept>
#include <sstream>

//#ifndef isinf
//#define isinf(x) (!_finite(x))
//#endif

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template <typename Number = double>
            struct NumberComparer final
            {
                Number Epsilon;

                //typed ef bool (*ComparerFunction)(const Number &a);
                using TransformFunction = Number(*)(const Number& a);

                //It must be like "isinf".
                //ComparerFunction IsInfinitePtr;

                //It must be like "abs".
                TransformFunction AbsoluteValuePtr;

                explicit NumberComparer(
                    const Number& epsilon = 0,

                    //It must be like "isinf".
                    //ComparerFunction isInfinitePtr = nullptr,

                    //It must be like "abs".
                    TransformFunction absoluteValuePtr = nullptr)
                    :
                    Epsilon(epsilon),
                    //IsInfinitePtr(isInfinitePtr),
                    AbsoluteValuePtr(absoluteValuePtr)
                {
                }

                inline bool AreEqual(const Number& a, const Number& b)
                {
#ifdef _DEBUG
                    //if (nullptr == IsInfinitePtr)
                    //{
                    //  throw exception("nullptr == IsInfinitePtr");
                    //}
                    if (nullptr == AbsoluteValuePtr)
                    {
                        throw std::runtime_error("nullptr == AbsoluteValuePtr");
                    }
                    if (Epsilon <= static_cast<Number>(0))
                    {
                        std::ostringstream ss;
                        ss << "Epsilon(" << Epsilon << ") must be positive.";
                        StreamUtilities::throw_exception(ss);
                    }
#endif
                    if (a == b)
                    {
                        return true;
                    }

                    //const bool infA = IsInfinitePtr(a);
                    //const bool infB = IsInfinitePtr(b);
                    //if (infA)
                    //{
                    //  return infB;
                    //}

                    const Number delta = AbsoluteValuePtr(a - b);

                    const Number absoluteA = AbsoluteValuePtr(a);
                    const Number absoluteB = AbsoluteValuePtr(b);

                    //It does not work to compare 0 and small number, but it must!
                    //
                    //const Number maxValue = max(absoluteA, absoluteB);
                    //const bool result = delta <= maxValue * Epsilon;
                    //return result;

                    const Number sum = absoluteA + absoluteB;
                    const Number div = delta / sum;
                    const bool result = div <= Epsilon;
                    if (result)
                    {
                        return true;
                    }

                    const bool result2 = absoluteA <= Epsilon && absoluteB <= Epsilon;
                    return result2;
                }
            };
        }
    }
}