#pragma once
#include<algorithm>
#include<cassert>
#include<concepts>
#include<type_traits>
#include<unordered_map>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given a signed circular ring, having zero total sum,
    // return minimum number of moves to make all values zero.
    // In every step, either positive or negative sum can be carried between neighbor points.
    // E.g. given {1, 0, -1}, there is one move of 1 from 0-th to second position.
    // Example two: {10, 1, -4, 2, 4, -3, -6, -4} requires 6 moves: 10 to -4 and to -6;
    //  then 1 to -4; -3 to 2; -1 to 4; 3 to -3.
    //
    // Warning: here is no test that the total sum of all numbers is zero.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto array_min_moves_zero_sum(const std::vector<int_t> &ring) -> std::size_t
    {
        const auto size = ring.size();

        std::size_t result{};
        int_t sum{};

        std::unordered_map<int_t, std::size_t> prefix_sums;

        for (std::size_t index{}; index < size; ++index)
        {
            sum += ring[index];

            auto &cnt = prefix_sums[sum];
            ++cnt;
            result = std::max(result, cnt);
        }

        assert(sum == 0);

        return size - result;
    }
} // namespace Standard::Algorithms::Numbers
