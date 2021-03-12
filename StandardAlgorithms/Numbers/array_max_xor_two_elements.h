#pragma once
#include"../Graphs/binary_trie.h"
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an array of n>=2 unsigned integers,
    // return the maximum XOR of 2 elements, and their distinct indexes.
    // Time O(n*log(max_value)).
    template<std::unsigned_integral int_t, class tree_t = ::Standard::Algorithms::Trees::binary_trie<int_t>>
    [[nodiscard]] constexpr auto array_max_xor_two_elements(const std::vector<int_t> &values) -> int_t
    {
        require_greater(values.size(), 1U, "values size");

        tree_t tree{};

        for (const auto &val : values)
        {
            tree.add(val);
        }

        auto result = tree.max_xor();
        return result;
    }

    // Slow time O(n*n).
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto array_max_xor_two_elements_slow(const std::vector<int_t> &values) -> int_t
    {
        const auto size = require_greater(values.size(), 1ZU, "values size");

        int_t result{};

        for (std::size_t index{}; index < size - 1ZU; ++index)
        {
            const auto &datum = values[index];

            for (auto ind_2 = index; ++ind_2 < size;)
            {
                const auto &datum_2 = values[ind_2];
                const auto cand = static_cast<int_t>(datum ^ datum_2);
                if (result < cand)
                {
                    result = cand;
                }
            }
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
