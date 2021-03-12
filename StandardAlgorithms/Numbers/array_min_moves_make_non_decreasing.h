#pragma once
#include"../Utilities/require_utilities.h"
#include<queue>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an array, any element can be increased by 0,1,-1.
    // The goal is the make the array NOT strictly increasing
    // by making the minimum possible number of operations.
    // E.g. given
    // {5, 9, 1, 4, 10, 8}, the answer is 11:
    // {5, 5, 5, 5,  8, 8}
    //     4 +4 +1  +2 == 11.
    // To make it strictly increasing,
    // one must subtract the index from each element,
    // then run this O(n*log(n)) code.
    template<std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(std::int64_t) && std::convertible_to<int_t, std::int64_t>)
    [[nodiscard]] constexpr auto array_min_moves_make_non_decreasing(const std::vector<int_t> &data) -> std::int64_t
    {
        const auto size = require_positive(data.size(), "data size");

        std::int64_t result{};
        std::priority_queue<std::int64_t> que{};
        que.push(data[0]);

        for (std::size_t index = 1; index < size; ++index)
        {
            const auto &item = data[index]
                // To make it strictly increasing:
                //- index
                ;
            que.push(item);

            if (!(item < que.top()))
            {
                continue;
            }

            result += que.top() - item; // todo(p3): overflow?
            que.pop();
            que.push(item);
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
