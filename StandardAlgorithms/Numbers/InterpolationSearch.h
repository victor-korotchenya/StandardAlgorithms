#pragma once
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Numbers
{
    struct InterpolationSearch final
    {
        InterpolationSearch() = delete;

    public:

        template <typename T>
        static inline T* Search(
            const T* const begin, const T* const endInclusive, const T& value)
        {
            using SameCastFunction = T(*)(const T& value1);

            const SameCastFunction caster = [](const T& value2) { return value2; };

            T* result = SearchWithCast<T, T, SameCastFunction>(
                begin, endInclusive,
                value,
                caster);

            return result;
        }

        template <typename T,
            typename CastValueType,
            typename Caster>
            static inline T* SearchWithCast(
                const T* const begin,
                const T* const endInclusive,
                const CastValueType& castValue,
                const Caster caster)
        {
            ThrowIfNull(begin, "begin");
            ThrowIfNull(endInclusive, "endInclusive");

#ifdef _DEBUG
            if (begin > endInclusive)
            {
                throw std::runtime_error("_Debug. begin > endInclusive.");
            }
#endif
            T* left = const_cast<T*>(begin);
            CastValueType leftValue = caster(*left);

            T* rightInclusive = const_cast<T*>(endInclusive);
            CastValueType rightValue = caster(*rightInclusive);

            while (leftValue <= castValue && castValue <= rightValue)
            {
                const CastValueType delta = rightValue - leftValue;
                if (0 == delta)
                {
                    if (castValue == leftValue)
                    {
                        return left;
                    }

                    break;
                }

                T* middle = left +
                    (castValue - leftValue) * (rightInclusive - left) / delta;

                const CastValueType middleValue = caster(*middle);
                if (castValue == middleValue)
                {
                    return middle;
                }

                if (middleValue < castValue)
                {
                    left = middle + 1;
                    leftValue = caster(*left);
                }
                else
                {
                    rightInclusive = middle - 1;
                    rightValue = caster(*rightInclusive);
                }
            }

            return nullptr;
        }
    };
}