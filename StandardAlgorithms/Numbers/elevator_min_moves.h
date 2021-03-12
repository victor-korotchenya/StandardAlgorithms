#pragma once

#include <cstdint>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Min moves to moves all people by an elevator to the
            // top floor given up to max_weight kilos in a single move.
            int elevator_min_moves(const std::vector<int>& weights, const int64_t max_weight);

            namespace Tests
            {
                void elevator_min_moves_test();
            }
        }
    }
}