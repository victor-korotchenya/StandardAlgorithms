#include"two_repetitions_interleaving.h"

void Standard::Algorithms::Numbers::Inner::two_rep_cross_backtrack(std::size_t length_0,
    const std::vector<std::vector<bool>> &possibles, std::vector<std::size_t> &chosen_indexes_0, std::size_t length_1)
{
    constexpr auto one = 1U;

    assert(0U < length_0 && length_0 < possibles.size() && length_1 < possibles.size());

    while (0U < length_0)
    {
        assert(possibles[length_0][length_1]);

        if (0U < length_1 && possibles[length_0][length_1 - one])
        {
            --length_1;
            continue;
        }

        assert(possibles[length_0 - one][length_1]);

        chosen_indexes_0.push_back(length_1 + --length_0);
    }

    std::reverse(chosen_indexes_0.begin(), chosen_indexes_0.end());
}

[[nodiscard]] auto Standard::Algorithms::Numbers::Inner::two_rep_cross_try_find_solution(const std::size_t source_size,
    const std::vector<std::vector<bool>> &possibles, std::vector<std::size_t> &chosen_indexes_0) -> bool
{
    assert(0U < source_size && source_size + 1U == possibles.size());

    for (std::size_t length_0{}; length_0 <= source_size; ++length_0)
    {
        const auto length_1 = source_size - length_0;

        if (possibles[length_0][length_1])
        {
            if (0U < length_0)
            {
                // Provide noexcept guarantee in two_rep_cross_backtrack.
                chosen_indexes_0.reserve(length_0);

                two_rep_cross_backtrack(length_0, possibles, chosen_indexes_0, length_1);
            }

            return true;
        }
    }

    return false;
}
