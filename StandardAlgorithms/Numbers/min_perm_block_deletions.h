#pragma once
// #include<iostream>
#include"permutation.h"

namespace Standard::Algorithms::Numbers
{
    // Given a zero-based permutation,
    // compute min number of block deletions to get an empty set,
    // where each block has consecutive numbers e.g. 8,9,10.
    // No overflow check.
    // Space, time O(n*n).
    template<std::integral int_t, class from_to_t = std::pair<std::size_t, std::size_t>>
    requires(sizeof(int_t) <= sizeof(std::size_t))
    constexpr auto min_perm_block_deletions(const std::vector<int_t> &perm, const bool is_zero_start = true)
        // Count, erase sequence.
        -> std::pair<std::size_t, std::vector<from_to_t>>
    {
        const auto size = perm.size();

        if (0U == size)
        {
            return {};
        }

        if (!is_permutation_2(perm, is_zero_start)) [[unlikely]]
        {
            throw std::runtime_error("A zero-based permutation is expected in min_perm_block_deletions.");
        }

        if (size == 1U)
        {
            return { 1U, { { 0U, 1U } } };
        }

        // Given a permutation {3, 1, 0, 2}
        //
        // one_erase_cost[0][1] = 2
        // one_erase_cost[1][2] = 2
        // one_erase_cost[2][3] = 2. Last entry for length 2.
        // one_erase_cost[0][2] = 3
        //   several_erase_cost[1][3] = 2. In {1, 0, 2}, first erase {0}, then do {1, 2}; the cost is 2.
        // one_erase_cost[0][3] = 3
        //
        // buffers:
        // {1, 2, 3, 3}, //  The result is here in the right-most cell.
        // {0, 1, 2, 2},
        // {0, 0, 1, 2},
        // {0, 0, 0, 1}.

        std::vector<int_t> rev_perm(size + 1U);
        rev_permutation(perm, rev_perm, is_zero_start);

        // Store (max + 1) value to have fewer ifs below.
        rev_perm.push_back(size);

        std::vector<std::vector<std::size_t>> buffers(size, std::vector<std::size_t>(size, std::size_t{}));

        for (std::size_t index{}; index < size; ++index)
        {
            buffers[index][index] = 1;
        }

        for (std::size_t length = 2; length <= size; ++length)
        {
            for (std::size_t start{}; start <= size - length; ++start)
            {
                const auto next = static_cast<std::size_t>(perm[start]) + 1U;
                const auto index_of_next = static_cast<std::size_t>(rev_perm.at(next));

                const auto stop_incl = start + length - 1U;
                const auto one_erase_cost = 1U + // Cut one, then the rest.
                    buffers[start + 1U][stop_incl];

                auto &buf = buffers[start][stop_incl];

                if (start < index_of_next && index_of_next <= stop_incl)
                {
                    // Here (perm[start] + 1) == perm[index_of_next].
                    //
                    // perm[start], perm[start + 1], .., perm[index_of_next - 1], (perm[start] + 1), ..,
                    // perm[stop_incl]
                    //
                    // The {perm[start + 1], .., perm[index_of_next - 1]} portion is cut off
                    // in order to enable joining at least {perm[start], perm[start] + 1}.
                    const auto several_erase_cost =
                        buffers[start + 1U][index_of_next - 1U] + buffers[index_of_next][stop_incl];

                    if (several_erase_cost < one_erase_cost)
                    {
                        buf = several_erase_cost;
                        // std::cout << " start " << start << ", " << stop_incl << " = " << several_erase_cost << "\n";
                        continue;
                    }
                }

                buf = one_erase_cost;
                // std::cout << " start " << start << ", " << stop_incl << " = " << one_erase_cost << "\n";
            }
        }

        const auto &best = buffers[0U][size - 1U];
        return { best,
            // todo(p3): restore the actual erase sequence.
            {} };
    }
} // namespace Standard::Algorithms::Numbers
