#pragma once
#include"../Utilities/require_utilities.h"
#include<span>

namespace Standard::Algorithms::Numbers
{
    // The binary search, where a pointer is cast to a value.
    // nullptr is returned when nothing is found.
    template<class item_t, class search_item_t, class caster_t, std::size_t extent = std::dynamic_extent,
        class order_t = std::strong_ordering>
    [[nodiscard]] constexpr auto binary_search_with_cast(
        const std::span<item_t, extent> spa, const search_item_t &search_item, caster_t caster) -> const item_t *
    {
        if (spa.empty())
        {
            return nullptr;
        }

        constexpr auto has_3d_compare = std::three_way_comparable_with<item_t, search_item_t, order_t>;

        const auto *begin = spa.data();
        const auto *end_inclusive = begin + spa.size() - 1U;

        assert(begin != nullptr && begin <= end_inclusive);

        while (begin <= end_inclusive)
        {
            const auto *const middle = begin + ((end_inclusive - begin) / 2);

            const auto &middle_value = caster(*middle);

            if constexpr (has_3d_compare)
            {
                const auto comp = search_item <=> middle_value;

                if (comp < 0)
                {
                    end_inclusive = middle - 1;
                    continue;
                }

                if (0 < comp)
                {
                    begin = middle + 1;
                    continue;
                }
            }
            else
            {
                if (search_item < middle_value)
                {
                    end_inclusive = middle - 1;
                    continue;
                }
                if (middle_value < search_item)
                {
                    begin = middle + 1;
                    continue;
                }

                assert(middle_value == search_item);
            }

            return middle;
        }

        return nullptr;
    }

    // nullptr is returned when not found.
    template<class item_t, class search_item_t, std::size_t extent = std::dynamic_extent,
        class order_t = std::strong_ordering>
    [[nodiscard]] constexpr auto binary_search(const std::span<item_t, extent> spa, const search_item_t &search_item)
        -> const item_t *
    {
        const auto caster = [] [[nodiscard]] (const item_t &value1) -> const item_t &
        {
            return value1;
        };

        const auto *const result =
            binary_search_with_cast<item_t, search_item_t, decltype(caster), extent, order_t>(spa, search_item, caster);

        return result;
    }

    // The binary search, returning a region of values [start, end_incl].
    // See also "occurrences_in_sorted_array_t.h".
} // namespace Standard::Algorithms::Numbers
