#pragma once
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Strings/string_utilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Compute the max length of a palindromic substring of a string,
            // returning {start, stop} positions, where start < stop.
            // Time O(n), space O(n).
            //
            // Also see ukkonen_suffix_tree_utilities.h
            template<class pair_t, class string_t>
            pair_t longest_palindromic_substring_manacher(const string_t& s)
            {
                const auto size0 = static_cast<int>(s.size());
                RequirePositive(size0, "size");

                // abbc -> |a|b|b|c| where '|' is an invisible delimeter.
                // 0123    012345678
                const auto size = size0 << 1 | 1;
                assert(size >= 3);

                auto center_max = 1, length_max = 1;

                std::vector<int> substr_len_max(size);
                substr_len_max[1] = 1;

                for (auto i = 2, //current right
                    center = 1, right = 2; i < size; ++i)
                {
                    auto& cur_max = substr_len_max[i];

                    const auto is_within_window = i < right;
                    if (is_within_window)
                    {
                        // center-left = right-center
                        const auto left = (center << 1) - i;
                        assert(left >= 0);
                        cur_max = std::min(substr_len_max[left], right - i);
                    }
                    else assert(!cur_max);

                    // '|' is at even indexes - always increment.
                    // If real chars at odd indexes match, then increment.
                    while (cur_max < i && i + cur_max < size &&
                        (!((i + cur_max + 1) & 1) ||
                            (s[(i - cur_max - 1) >> 1] == s[(i + cur_max + 1) >> 1])))
                    {
                        ++cur_max;
                    }

                    const auto is_outside = right < i + cur_max;
                    if (is_outside)
                        right = i + cur_max, center = i;

                    if (length_max < cur_max)
                        length_max = cur_max, center_max = i;

                    assert(center_max >= length_max);
                }

                const auto start = (center_max - length_max) >> 1,
                    stop = start + length_max;
                assert(0 <= start && start < stop&& stop <= size);
                return pair_t(start, stop);
            }

            // Slow time, space O(n*n).
            template<class pair_t, class string_t>
            pair_t longest_palindromic_substring_slow(const string_t& s)
            {
                const auto size = static_cast<int>(s.size());
                RequirePositive(size, "size");

                if (Privet::Algorithms::Strings::is_palindrome(&s[0], size))
                    return { 0, size };

                auto start = 0, stop = 1;
                for (auto i = 0; i < size; ++i)
                {
                    {//Odd palindrome
                        const auto odd_length = std::min(i, size - 1 - i);
                        assert(odd_length >= 0);
                        auto cand = 0;

                        for (auto x = 1; x <= odd_length && s[i - x] == s[i + x]; ++x)
                            ++cand;

                        if (stop - start < (cand << 1 | 1))
                        {
                            start = i - cand;
                            stop = i + cand + 1;
                            assert(start >= 0 && start < stop&& stop <= size);
                        }
                    }
                    {//Even
                        const auto even_length = std::min(i, size - 2 - i);
                        assert(even_length >= -1);

                        auto cand = 0;
                        for (auto x = 0; x <= even_length && s[i - x] == s[i + x + 1]; ++x)
                            ++cand;

                        if (stop - start < cand << 1)
                        {
                            start = i - cand + 1;
                            stop = i + cand + 1;
                            assert(start >= 0 && start < stop&& stop <= size);
                        }
                    }
                }

                assert(0 <= start && start < stop&& stop <= size && stop - start < size);
                return pair_t(start, stop);
            }
        }
    }
}