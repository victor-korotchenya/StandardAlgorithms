#pragma once
#include"../Utilities/require_utilities.h"
#include<functional>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Input: key to search; array sorted by each column,
    //  and each row in increasing order.
    // Output: a pair of array indexes or (std::numeric_limits<std::size_t>::max(),
    // (std::numeric_limits<std::size_t>::max()) when not found.
    template<class item_t, class less_t = std::less<item_t>>
    struct find_sorted_2d_matrix final
    {
        constexpr explicit find_sorted_2d_matrix(less_t less1 = {})
            : Less(less1)
        {
        }

        // Running time is O(rows + columns).
        [[nodiscard]] constexpr auto search(const std::vector<std::vector<item_t>> &items, const item_t &key) const
            -> std::pair<std::size_t, std::size_t>
        {
            const auto size = require_positive(items.size(), "items size");

            const auto row_size = require_positive(items[0].size(), "items[0] size");

            std::size_t row{};
            auto column = row_size - 1U;

            for (;;)
            {
                const auto &cur = items[row].at(column);

                if (Less(key, cur))
                {
                    if (0U == column--)
                    {
                        break;
                    }
                }
                else if (Less(cur, key))
                {
                    if (size == ++row)
                    {
                        break;
                    }

                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        const auto row_size2 = items[row].size();

                        require_equal(row_size, "Size of row " + std::to_string(row), row_size2);
                    }
                }
                else
                {
                    return { row, column };
                }
            }

            constexpr auto none = std::numeric_limits<std::size_t>::max();

            return { none, none };
        }

private:
        mutable less_t Less;
    };
} // namespace Standard::Algorithms::Numbers
