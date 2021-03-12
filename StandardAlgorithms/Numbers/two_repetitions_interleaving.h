#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    // possibles[i][j] means there is a valid interleaving of
    //  (sequence0**inf).substr(0, i)
    //  (sequence1**inf).substr(0, j),
    // equal to sourceSignal.substr(0, i + j).

    template<class item_t>
    constexpr void two_rep_cross_fill_matrix(const std::vector<item_t> &source_signal,
        std::vector<std::vector<bool>> &possibles, const std::vector<item_t> &sequence_0,
        const std::vector<item_t> &sequence_1) noexcept
    {
        const auto source_size = source_signal.size();
        const auto size_0 = sequence_0.size();
        const auto size_1 = sequence_1.size();

        assert(0U < source_size && 0U < size_0 && 0U < size_1 && 1U < possibles.size());

        possibles[0][0] = true; // An empty string is always valid.

        for (std::size_t lena{}; lena < source_size; ++lena)
        {
            const auto &sochar = source_signal[lena];

            for (std::size_t length_0{}; length_0 <= lena; ++length_0)
            {
                const auto length_1 = lena - length_0;

                // There can be many ways to truthfulness.
                possibles[length_0][length_1 + 1U] = possibles[length_0][length_1 + 1U] ||
                    (possibles[length_0][length_1] && sochar == sequence_1[length_1 % size_1]);

                // todo(p4): avoid "%" division.

                possibles[length_0 + 1U][length_1] = possibles[length_0 + 1U][length_1] ||
                    (possibles[length_0][length_1] && sochar == sequence_0[length_0 % size_0]);
            }
        }
    }

    void two_rep_cross_backtrack(std::size_t length_0, const std::vector<std::vector<bool>> &possibles,
        std::vector<std::size_t> &chosen_indexes_0, std::size_t length_1);

    [[nodiscard]] auto two_rep_cross_try_find_solution(std::size_t source_size,
        const std::vector<std::vector<bool>> &possibles, std::vector<std::size_t> &chosen_indexes_0) -> bool;
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Two non-empty sequences are repeated infinitely.
    // If possible, find an interleaving, recreating the source signal,
    // such that each source bit is cut off from either sequence head.
    // Time, space O(|source|**2).
    template<class item_t>
    [[nodiscard]] constexpr auto two_repetitions_interleaving(const std::vector<item_t> &source_signal,
        // The "chosenIndexes_1" can be inferred from the "chosenIndexes_0"
        // provided the returned value is true.
        // Because (chosenIndexes_0 + chosenIndexes_1) = {0, 1, .. , source.size() - 1}
        std::vector<std::size_t> &chosen_indexes_0, const std::vector<item_t> &sequence_0,
        const std::vector<item_t> &sequence_1) -> bool
    {
        const auto source_size = require_positive(source_signal.size(), "source signal size");
        require_positive(sequence_0.size(), "sequence 0 size");
        require_positive(sequence_1.size(), "sequence 1 size");

        chosen_indexes_0.clear();

        if (const auto has_first_symbol_match = source_signal[0] == sequence_0[0] || source_signal[0] == sequence_1[0];
            !has_first_symbol_match)
        {
            return false;
        }

        const auto source_size_plus_one = source_size + 1LLU;

        std::vector<std::vector<bool>> possibles(source_size_plus_one, std::vector<bool>(source_size_plus_one, false));

        Inner::two_rep_cross_fill_matrix<item_t>(source_signal, possibles, sequence_0, sequence_1);

        auto result = Inner::two_rep_cross_try_find_solution(source_signal.size(), possibles, chosen_indexes_0);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
