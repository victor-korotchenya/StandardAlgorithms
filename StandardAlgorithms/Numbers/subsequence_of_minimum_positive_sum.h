#pragma once
#include<cstddef>
#include<cstdint>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Minimum positive subsequence sum.
    // Return -1 when no positive number.
    // Given {-9, -8, 10}, return 1; chosen_indexes = {0, 2}.
    [[nodiscard]] auto subsequence_of_minimum_positive_sum_slow(
        const std::vector<std::int32_t> &data, std::vector<std::size_t> &chosen_indexes) -> std::int32_t;

    // [[nodiscard]] auto subsequence_of_minimum_positive_sum(const std::vector<std::int32_t> &data,
    //    std::vector<std::size_t> &chosen_indexes) -> std::int32_t;
} // namespace Standard::Algorithms::Numbers
