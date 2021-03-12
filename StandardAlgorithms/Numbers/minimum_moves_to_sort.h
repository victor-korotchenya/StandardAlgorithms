#pragma once
#include"../Utilities/require_utilities.h"
#include"longest_increasing_subsequence.h"

namespace Standard::Algorithms::Numbers
{
    // Given an array, count the minimum number of moves to sort it.
    // See also "array_inversion_count.h".
    // E.g. given { 4, 1, 2, 3, }, the "4" must be moved to the end,
    // thus there is 1 move.
    // { 4, 3, 2, 1 } has min 3 moves.
    template<class element_t, class less_t>
    [[nodiscard]] constexpr auto minimum_moves_to_sort(const std::vector<element_t> &data, less_t comparer = {})
        -> std::size_t
    {
        const auto size = data.size();

        if (size <= 1U)
        {
            return {};
        }

        const auto longest_increasing =
            longest_increasing_subsequence<element_t, std::vector<element_t>, less_t>::fast(data, comparer);

        const auto increasing_size = longest_increasing.size();
        require_less_equal(increasing_size, size, "increasingSize");

        auto result = size - increasing_size;
        return result;
    }
} // namespace Standard::Algorithms::Numbers
