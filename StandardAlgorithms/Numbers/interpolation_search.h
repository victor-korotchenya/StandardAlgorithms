#pragma once
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include<span>

namespace Standard::Algorithms::Numbers
{
    // The data must be sorted.
    // Watch out for an overflow when computing add_x.
    // Average time O(log(log(n)), but worst O(n).
    template<std::integral long_int_t, std::integral int_t, class search_item_t,
        std::size_t extent = std::dynamic_extent, class order_t = std::strong_ordering>
    requires(same_sign_leq_size<int_t, long_int_t>)
    constexpr auto interpolation_search(const std::span<int_t, extent> spa, const search_item_t &search_item)
        -> const int_t *
    {
        if (spa.empty())
        {
            return nullptr;
        }

        constexpr long_int_t zero{};
        constexpr auto has_3d_compare = std::three_way_comparable_with<int_t, search_item_t, order_t>;

        const auto *begin = spa.data();
        const auto *end_inclusive = begin + spa.size() - 1U;

        if constexpr (::Standard::Algorithms::is_debug)
        {
            require_less_equal(begin, end_inclusive, "interpolation_search DEBUG error: endInclusive < begin.");
            require_less_equal(
                *begin, *end_inclusive, "interpolation_search DEBUG not sorted data: *endInclusive < *begin.");
        }

        const auto long_search_item = static_cast<long_int_t>(search_item);

        while (!(search_item < *begin || *end_inclusive < search_item))
        {
            const auto delta_y = static_cast<long_int_t>(*end_inclusive) - static_cast<long_int_t>(*begin);

            if (delta_y == zero)
            {
                return search_item == *begin ? begin : nullptr;
            }

            const auto delta_x = end_inclusive - begin;

            const auto delta_search_y = long_search_item - static_cast<long_int_t>(*begin);

            // tan = delta_y / delta_x = delta_search_y / add_x
            // add_x = delta_search_y * delta_x / delta_y
            const auto add_x = delta_search_y * delta_x / delta_y;

            const auto *const middle = begin + add_x;
            assert(zero < delta_y && begin < end_inclusive && begin <= middle && middle <= end_inclusive);

            if constexpr (has_3d_compare)
            {
                const auto comp = search_item <=> *middle;

                if (0 < comp)
                {
                    begin = middle + 1;
                }
                else if (comp < 0)
                {
                    end_inclusive = middle - 1;
                }
                else
                {
                    return middle;
                }
            }
            else
            {
                if (search_item == *middle)
                {
                    return middle;
                }

                if (*middle < search_item)
                {
                    begin = middle + 1;
                    continue;
                }

                end_inclusive = middle - 1;
            }
        }

        return nullptr;
    }
} // namespace Standard::Algorithms::Numbers
