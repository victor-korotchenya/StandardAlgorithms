#pragma once
#include"arithmetic.h"
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto covered_floors(const std::pair<int_t, int_t> &balls_floors, const int_t &attempts)
        -> int_t
    {
        const auto &balls = balls_floors.first;
        assert(balls < attempts);

        const auto &floors = balls_floors.second;

        int_t sum{};
        auto binomial = attempts;

        for (int_t bal = 1; bal <= balls; ++bal)
        {
            sum += binomial;

            if (floors <= sum)
            {// fertig
                return floors;
            }

            binomial = static_cast<int_t>(binomial * (attempts - bal) / (bal + 1U));
        }

        return sum;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given balls>0 and floors>0, compute the minimum attempts
    // in order to identify an edge floor where a ball begins to break.
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto drop_floor(const std::pair<int_t, int_t> &balls_floors) -> int_t
    {
        using large_t = std::uint64_t;

        static_assert(sizeof(int_t) <= sizeof(large_t));

        const auto &floors = require_positive(balls_floors.second, "floors");

        const auto balls = std::min(floors,
            // Excessive balls are not needed.
            require_positive(balls_floors.first, "balls"));

        if (balls == 1U || // With only 1 ball, one must start from the first floor,
                           // and then go up one-by-one until the ball breaks.
            floors == 1U) // 1 floor - one try is enough.
        {
            return floors;
        }

        const auto binary_search_count = static_cast<int_t>(1 + most_significant_bit64(static_cast<large_t>(floors)));

        if (binary_search_count <= balls)
        {// There are enough balls to use the binary search.
            return binary_search_count;
        }

        {// Some overflow checks.
            const auto plus_1 = static_cast<int_t>(balls + 1LLU);
            require_greater(plus_1, balls, "total balls + 1");

            const auto plus_2 = static_cast<int_t>(balls + 2LLU);
            require_greater(plus_2, plus_1, "total balls + 2");
        }

        // The attempts cannot be less than that of the binary search.
        auto left = binary_search_count;

        // Here are minimum 2 balls, and minimum 2 floors,
        // so one can start from the 2nd floor.
        auto right = static_cast<int_t>(floors - 1U);

        // Cannot make the left one larger as it won't work for (2, 4).
        assert(left <= right);

        do
        {
            const auto mid_attempts = static_cast<int_t>(left + ((right - left) >> 1U));

            const auto covered = Inner::covered_floors<int_t>(balls_floors, mid_attempts);

            if (covered < floors)
            {
                left = static_cast<int_t>(mid_attempts + 1U);
            }
            else
            {
                assert(0U < mid_attempts);

                right = static_cast<int_t>(mid_attempts - 1U);
            }
        } while (!(right < left));

        return right + 1U;
    }

    constexpr auto drop_floor_slow_maxim = 101U;

    // Slow time O(balls * floors * floors).
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto drop_floor_slow(const std::pair<int_t, int_t> &balls_floors) -> int_t
    {
        const auto &floors = require_positive(balls_floors.second, "floors");

        const auto balls = std::min(floors, require_positive(balls_floors.first, "balls"));

        require_greater(drop_floor_slow_maxim, balls, "balls");
        require_greater(drop_floor_slow_maxim, floors, "floors");

        std::vector<std::vector<int_t>> buffer(balls + 1LLU, std::vector<int_t>(floors + 1LLU, int_t{}));

        for (int_t flo = 1; flo <= floors; ++flo)
        {
            constexpr auto bal = 1U;

            buffer[bal][flo] = flo;
        }

        for (int_t bal = 2; bal <= balls; ++bal)
        {
            {
                constexpr auto flo1 = 1U;

                buffer[bal][flo1] = 1U;
            }

            for (int_t flo = 2; flo <= floors; ++flo)
            {
                auto &val = buffer[bal][flo];
                val = std::numeric_limits<int_t>::max();

                for (int_t sub_flo = 1; sub_flo < flo; ++sub_flo)
                {
                    const auto &not_broken = buffer[bal][flo - sub_flo];

                    const auto &broken = buffer[bal - 1U][sub_flo - 1U];

                    const auto cand = static_cast<int_t>(1U + std::max(not_broken, broken));

                    assert(not_broken < cand && broken < cand);

                    val = std::min(val, cand);
                }
            }
        }

        const auto res = buffer[balls][floors];
        assert(0U < res);

        return res;
    }
} // namespace Standard::Algorithms::Numbers
