#pragma once
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Numbers
{
    //The binary search, where a pointer is cast to a value.
    // null is returned when nothing is found.
    template <typename T, typename value_t, typename caster_t>
    T* binary_search_inclusive_with_cast(
        const T* const begin,
        const T* const end_inclusive,
        const value_t& value,
        caster_t caster)
    {
        ThrowIfNull(begin, "begin");
        ThrowIfNull(end_inclusive, "end_inclusive");

        T* left = const_cast<T*>(begin);
        T* right_inclusive = const_cast<T*>(end_inclusive);

        while (left <= right_inclusive)
        {
            T* middle = left + ((right_inclusive - left) >> 1);

            const auto middle_value = caster(*middle);
            if (value == middle_value)
            {
                return middle;
            }

            if (value < middle_value)
            {
                right_inclusive = middle - 1;
            }
            else
            {
                left = middle + 1;
            }
        }

        return nullptr;
    }

    // null is returned when not found.
    template <typename T, typename value_t>
    T* binary_search_inclusive(
        const T* const begin, const T* const end_inclusive,
        const value_t& value)
    {
        const auto caster = [](const T& value1)
            -> const T& { return value1; };

        auto result = binary_search_inclusive_with_cast<T, value_t, decltype(caster)>(
            begin, end_inclusive, value, caster);
        return result;
    }

    //The binary search, returning a region of values [start, end],
    //where a pointer is cast to a value.
    //See file OccurrencesInSortedArray.h
}