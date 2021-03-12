#pragma once
#include <vector>
#include "../Strings/string_utilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            // Find the longest common substring of 2 strings: start and stop in each string.
            // If nothing is found, return 0s.
            // Time O(m+n), space O(min(n, m)).
            //
            // See ukkonen_suffix_tree_utilities.h

            // Slow time O(n*m), space O(min(n, m)).
            template<class string_a, class string_b, class pair_t>
            void longest_common_substring_slow_still2(const string_a& a0, const string_b& b0,
                pair_t& result)
            {
                result = {};

                const auto m0 = static_cast<int>(a0.size()),
                    n0 = static_cast<int>(b0.size());
                if (!m0 || !n0)
                    return;

                const auto use_a = m0 <= n0;
                const auto m = use_a ? m0 : n0, n = !use_a ? m0 : n0;
                const auto& a = use_a ? a0 : b0, & b = !use_a ? a0 : b0;

                std::vector<int> cur(m + 1), prev;
                auto row_max = 0, col_max = 0, length_max = 0;

                for (auto col = 0; col < n; ++col)
                {
                    std::swap(prev, cur);
                    cur.assign(m + 1, 0);

                    for (auto row = 0; row < m; ++row)
                    {
                        if (a[row] != b[col])
                            continue;

                        auto& len = cur[row + 1];
                        len = prev[row] + 1;

                        if (length_max < len)
                        {
                            length_max = len;
                            row_max = row;
                            col_max = col;
                        }
                    }
                }

                if (!length_max)
                    return;

                assert(row_max >= 0 && row_max < m&& col_max >= 0 && col_max < n);
                row_max -= length_max - 1;
                col_max -= length_max - 1;
                assert(row_max >= 0 && row_max < m&& col_max >= 0 && col_max < n);

                result.first.first = use_a ? row_max : col_max;
                result.second.first = !use_a ? row_max : col_max;

                result.first.second = result.first.first + length_max;
                result.second.second = result.second.first + length_max;
            }

            // Slow time, space O(n*m).
            template<class string_a, class string_b, class pair_t>
            void longest_common_substring_slow_still(const string_a& a, const string_b& b,
                pair_t& result)
            {
                result = {};

                const auto m = static_cast<int>(a.size()),
                    n = static_cast<int>(b.size());
                if (!m || !n)
                    return;

                std::vector<std::vector<int>> buf(m + 1, std::vector<int>(n + 1));
                auto row_max = 0, col_max = 0, length_max = 0;

                for (auto row = 0; row < m; ++row)
                {
                    const auto& prev = buf[row];
                    auto& cur = buf[row + 1];

                    for (auto col = 0; col < n; ++col)
                    {
                        if (a[row] != b[col])
                            continue;

                        auto& len = cur[col + 1];
                        len = prev[col] + 1;

                        if (length_max < len)
                        {
                            length_max = len;
                            row_max = row;
                            col_max = col;
                        }
                    }
                }

                if (!length_max)
                    return;

                assert(row_max >= 0 && row_max < m&& col_max >= 0 && col_max < n);
                row_max -= length_max - 1;
                col_max -= length_max - 1;
                assert(row_max >= 0 && row_max < m&& col_max >= 0 && col_max < n);

                result.first.first = row_max;
                result.first.second = row_max + length_max;

                result.second.first = col_max;
                result.second.second = col_max + length_max;
            }

            // Slow time O(n**3), space(1).
            template<class string_a, class string_b, class pair_t>
            void longest_common_substring_slow(const string_a& a, const string_b& b,
                pair_t& result)
            {
                result = {};

                const auto m = static_cast<int>(a.size()),
                    n = static_cast<int>(b.size());
                if (!m || !n)
                    return;

                for (auto row = 0, length_max = 0; row + length_max < m; ++row)
                {
                    for (auto col = 0; col + length_max < n; ++col)
                    {
                        if (a[row] != b[col])
                            continue;

                        const auto tail = std::min(m - row, n - col);
                        auto len = 1;
                        while (len < tail && a[row + len] == b[col + len])
                            ++len;

                        if (length_max < len)
                        {
                            length_max = len;
                            result.first.first = row;
                            result.first.second = row + len;
                            result.second.first = col;
                            result.second.second = col + len;

                            if (len == std::min(m, n))
                                return;
                        }
                    }
                }
            }
        }
    }
}