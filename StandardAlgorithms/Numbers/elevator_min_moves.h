#pragma once
#include<cstdint>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Min moves to moves all people by an elevator to the
    // top floor raising up to max_weight kilos in a single move.
    [[nodiscard]] auto elevator_min_moves(const std::vector<std::int32_t> &weights, std::int64_t max_weight)
        -> std::int32_t;
} // namespace Standard::Algorithms::Numbers
