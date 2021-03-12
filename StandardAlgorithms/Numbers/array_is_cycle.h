#pragma once
#include"../Utilities/is_debug.h"
#include<cassert>
#include<concepts>
#include<cstdint>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Treating an array element x as x jumps around the array,
    // check whether the array is a cycle starting at 0 index, visiting all items, and getting back to index 0.
    // Time O(n), space O(1).
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto array_is_cycle(const std::vector<int_t> &arr) -> bool
    {
        const auto size = static_cast<std::int32_t>(arr.size());
        if (size <= 1)
        {
            return true;
        }

        std::int32_t count{};
        std::int32_t pos{};

        for (;;)
        {
            assert(count < size);

            pos = static_cast<std::int32_t>((static_cast<std::int64_t>(pos) + arr.at(pos)) % size);

            if (pos < 0)
            {
                pos += size;
            }

            assert(!(pos < 0) && pos < size);

            ++count;

            if (0 == pos || count == size)
            {
                return 0 == pos && count == size;
            }
        }
    }

    // Slow time, space O(n).
    template<class int_t>
    [[nodiscard]] constexpr auto array_is_cycle_slow(const std::vector<int_t> &arr) -> bool
    {
        const auto size = static_cast<std::int32_t>(arr.size());

        if (size <= 1)
        {
            return true;
        }

        std::vector<bool> visited(size);

        for (std::int32_t count{}, pos{};;)
        {
            visited[pos] = true;
            ++count;

            pos = static_cast<std::int32_t>((static_cast<std::int64_t>(pos) + arr[pos]) % size);

            if (pos < 0)
            {
                pos += size;
            }

            assert(!(pos < 0) && pos < size);

            if (!visited[pos])
            {
                continue;
            }

            const auto res = 0 == pos && count == size;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                auto flag = 0 == pos;

                for (std::int32_t index{}; index < size && flag; ++index)
                {
                    flag = visited[index];
                }

                assert(flag == res);
            }

            return res;
        }
    }
} // namespace Standard::Algorithms::Numbers
