#pragma once
#include <algorithm>
#include <cassert>
#include <vector>
#include "../Strings/string_utilities.h"

namespace
{
    template<class string_t>
    int longest_subsequence_palindrome_slow_rec(const string_t& s, string_t& buf, const int pos)
    {
        if (pos < 0)
        {
            const auto size = static_cast<int>(buf.size());
            if (Privet::Algorithms::Strings::is_palindrome(&buf[0], size))
                return size;
            return 0;
        }

        const auto without = longest_subsequence_palindrome_slow_rec<string_t>(s, buf, pos - 1);

        buf.push_back(s[pos]);
        const auto with = longest_subsequence_palindrome_slow_rec<string_t>(s, buf, pos - 1);
        buf.pop_back();

        const auto result = without >= with ? without : with;
        assert(result >= 0 && result <= static_cast<int>(s.size()));
        return result;
    }

    template <class p_t>
    int longest_subsequence_palindrome_almost_impl(const p_t s, const int size)
    {
        assert(s && size > 0);

        std::vector<std::vector<int>> buf(size, std::vector<int>(size));

        for (auto i = 0; i < size; ++i) buf[i][i] = 1;

        for (auto i = 0; i < size - 1; ++i)
            buf[i][i + 1] = (s[i] == s[i + 1]) + 1;

        // Cycle len 3..n; 2nd cycle start = 0..n-len+1
        //
        // index  0  1  2  3  4
        // len    a  b  c  b  b  ; Comments
        // 1      1  1 +1  1  1
        // 2         1  1 *1  2  ; Got 2 as b = b.
        // 3            1  3+ 2  ; 3+ for "bcb" as b = b, and c is in between.
        // 4               3  3* ; 3* for "bcbb" needs penult buf[3] for "cb" which is 1!
        // 5                  3  ; Take the max of the previous and current ones.

        for (auto len1 = 2; len1 < size; ++len1)
        {
            for (auto start = 0; start < size - len1; ++start)
            {
                const auto stop = start + len1;
                auto& val = buf[start][stop];
                if (s[start] == s[stop])
                    val = buf[start + 1][stop - 1] + 2;
                else
                    val = std::max(buf[start + 1][stop], buf[start][stop - 1]);
            }
        }

        const auto& result = buf[0][size - 1];
        return  result;
    }

    template<class p_t>
    int longest_subsequence_palindrome_impl_they(const p_t s, const int size)
    {
        assert(size >= 2 && s[0] != s[size - 1]);

        // [i..to_incl] -> max length, here i <= to_incl.
        std::vector<int> buf(size, 1);

        //// Length 1, 2
        //for (auto i = 0; i < size - 1; ++i)
        //    buf[i + 1] = (s[i] == s[i + 1]) + 1;
        //
        // Don't need the above to set old = *1

        // Cycle b n-2..0; 2nd cycle e b+2..n.
        //
        // index    0  1  2  3  4  SubStr Comments
        // max_len  a  b  c  b  b
        // 1        1  1 +1  1  1
        // 2                 ___2     bb ; Got 2 as b = b.
        // 3              __*1__2    cbb
        // 4           ___1__3+_3*  bcbb ; 3+ for "bcb" needs old buf[2] for "c" which is +1.
        //                               ; 3* for "bcbb" needs old buf[3] for "cb" which is *1!
        // 5        ___1__1__3__3  abcbb ; So, previous row and previous col value is needed.

        // Length 2!,  3, 4, ... size
        for (auto from = size - 2; from >= 0; --from)
        {
            const auto& c = s[from];
            for (auto to_incl = from + 1, old = 0; to_incl < size; ++to_incl)
            {
                auto& val = buf[to_incl];
                const auto cand = old + 2;

                // Prev row, prev col.
                old = val;

                if (c == s[to_incl])
                {
                    assert(val <= cand && cand < size);
                    val = cand;
                }
                else
                {
                    const auto& prev = buf[to_incl - 1];
                    if (val < prev)
                        val = prev;
                }
            }
        }

        const auto& result = buf.back();
        assert(0 < result && result < size);
        return result;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Compute the max length of a palindromic subsequence of a string.
            // Time O(n*n), space O(n).

            // Bad idea - caching problems when going in ar[--i].
            //
            // end 2..n; 2nd cycle b = end-1..0
            //
            // index      0  1  2  3  4  SubStr Comments
            // max_len    a  b  c  b  b
            // 1          1  1 *1  1  1
            // 2          1___           ab
            // 3          1/_1___        abc
            // 4          3+_3*t1___     abcb  ; 3* = 2 + *1
            //                                 ; 3+ = max(1/, 3*)
            // 5          3__3t_2__2___  abcbb ; 3t = 2 + t1, t1 = "cb" = 1

            // Good idea - they 1) i = n..0 2) main cycle j = i..n

            template<class string_t>
            int longest_subsequence_palindrome_they(const string_t& s0)
            {
                const auto size0 = static_cast<int>(s0.size()),
                    half_size0 = size0 >> 1;
                auto start0 = 0;
                while (start0 < half_size0 && s0[start0] == s0[size0 - 1 - start0])
                    ++start0;

                if (start0 == half_size0)
                    return size0; // entire string

                // todo: p3. string_view
                const auto size = size0 - (start0 << 1);
                const auto s = &s0[0] + start0;
                assert(size >= 2 && s[0] != s[size - 1]);

                const auto sub_res = longest_subsequence_palindrome_impl_they(s, size),
                    result = sub_res + (start0 << 1);
                assert(0 < sub_res && sub_res <= result && result < size0);
                return result;
            }

            // Time, space O(n*n).
            template<class string_t>
            int longest_subsequence_palindrome_almost(const string_t& s0)
            {
                const auto size0 = static_cast<int>(s0.size()),
                    half_size0 = size0 >> 1;
                auto start0 = 0;
                while (start0 < half_size0 && s0[start0] == s0[size0 - 1 - start0])
                    ++start0;

                if (start0 == half_size0)
                    return size0; // entire string

                // todo: p3. string_view
                const auto size = size0 - (start0 << 1);
                const auto s = &s0[0] + start0;
                assert(size >= 2 && s[0] != s[size - 1]);

                const auto sub_res = longest_subsequence_palindrome_almost_impl(s, size),
                    result = sub_res + (start0 << 1);
                assert(0 < sub_res && sub_res <= result && result < size0);
                return result;
            }

            // Slow time O(2**n)
            template<class string_t>
            int longest_subsequence_palindrome_slow(const string_t& s)
            {
                const auto size = static_cast<int>(s.size());
                assert(size <= 20);
                if (size <= 1)
                    return size;

                string_t buf;
                buf.reserve(size);

                const auto result = longest_subsequence_palindrome_slow_rec<string_t>(s, buf, size - 1);
                assert(result > 0 && result <= size);
                return result;
            }
        }
    }
}