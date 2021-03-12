#pragma once
#include <vector>

namespace Privet::Algorithms::Numbers
{
    // Treating an array element x as x jumps around the array,
    // check whether the array is a cycle starting at 0 index, visiting all items, and getting back to index 0.
    // Time O(n), space O(1).
    template<class int_t>
    bool array_is_cycle(const std::vector<int_t>& ar)
    {
        const auto size = static_cast<int>(ar.size());
        if (size <= 1)
            return true;

        auto count = 0, pos = 0;
        for (;;)
        {
            assert(count < size);

            pos = (pos + ar[pos]) % size;
            if (pos < 0)
                pos += size;
            assert(pos >= 0 && pos < size);

            if (++count == size || !pos)
                return !pos && count == size;
        }
    }

    // Slow time, space O(n).
    template<class int_t>
    bool array_is_cycle_slow(const std::vector<int_t>& ar)
    {
        const auto size = static_cast<int>(ar.size());
        if (size <= 1)
            return true;

        std::vector<bool> visited(size);

        for (auto count = 0, pos = 0;;)
        {
            visited[pos] = true;
            ++count;

            pos = static_cast<int>(
                (static_cast<int64_t>(pos) + ar[pos]) % size);
            if (pos < 0)
                pos += size;
            assert(pos >= 0 && pos < size);

            if (visited[pos])
            {
                const auto r = !pos && count == size;
#if _DEBUG
                auto h = !pos;
                for (auto i = 0; i < size && h; ++i)
                    h = visited[i];
                assert(h == r);
#endif
                return r;
            }
        }
    }
}