#pragma once
#include<algorithm>
#include<cassert>
#include<cstddef>
#include<utility>

namespace Standard::Algorithms::Numbers
{
    // A simple in-place copy-merge into a resizeable class such as std::vector.
    constexpr void inplace_merge(const auto &src, auto &dest)
    {
        assert(std::is_sorted(src.begin(), src.end()) && std::is_sorted(dest.begin(), dest.end()));

        if (src.empty())
        {
            return;
        }

        if (dest.empty())
        {
            dest.resize(src.size());
            std::copy(src.cbegin(), src.cend(), dest.begin());
            return;
        }

        const std::size_t src_size = src.size();
        const std::size_t dest_size = dest.size();
        auto new_size = src_size + dest_size;
        assert(0ZU < src_size && src_size < new_size);
        assert(0ZU < dest_size && dest_size < new_size);

        dest.resize(new_size);

        if (const auto &dest_back = dest[dest_size - 1ZU]; dest_back < src.front())
        {// src {10, 20, 30, 40} + dest {1, 2, 9}
            // Append the source to the tail.
            std::copy(src.cbegin(), src.cend(), dest.begin() + dest_size);

            assert(std::is_sorted(dest.begin(), dest.end()));
            return;
        }

        for (auto index = dest_size; 0ZU < index--;)
        {// Copy the destination to the tail.
            --new_size;
            assert(index < new_size);

            dest[new_size] = dest[index];
        }

        if (!(dest.front() < src.back()))
        {// src {1, 2, 9, 10} + dest {10, 20, 20, 20, 30}
            std::copy(src.cbegin(), src.cend(), dest.begin());

            assert(std::is_sorted(dest.begin(), dest.end()));
            return;
        }

        const auto src_end = src.cend();
        auto src_it = src.cbegin();

        const auto dest_end = dest.end();
        auto dest_it = dest.begin() + src_size;

        // src {10, 20, 30, 40} + dest {1, 2, 10, 20, 40, 50}
        auto merge_it = dest.begin();

        // merge_it 1, 2, 10, 20, dest_it 1, 2, 10, 20, 40, 50
        for (;;)
        {
            assert(src_end != src_it && dest_end != dest_it && merge_it != dest_it);

            const auto &src_value = *src_it;
            const auto &dest_value = *dest_it;

            if (!(dest_value < src_value))
            {
                *merge_it = src_value;

                if (++src_it == src_end)
                {
                    break;
                }

                ++merge_it;
                continue;
            }

            *merge_it = dest_value;
            ++merge_it;

            if (++dest_it != dest_end)
            {
                continue;
            }

            std::copy(src_it, src_end, merge_it);
            break;
        }

        assert(std::is_sorted(dest.begin(), dest.end()));
    }
} // namespace Standard::Algorithms::Numbers
