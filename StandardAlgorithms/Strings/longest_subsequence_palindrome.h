#pragma once
#include"string_utilities.h" // is_palindrome
#include<span>
#include<vector>

namespace Standard::Algorithms::Strings::Inner
{
    template<class string_t>
    [[nodiscard]] constexpr auto longest_subsequence_palindrome_slow_rec(
        const string_t &str, const std::int32_t pos, string_t &buf) -> std::size_t
    {
        if (pos < 0)
        {
            const auto size = buf.size();

            if (0U < size && is_palindrome(&buf[0], size))
            {
                return size;
            }

            return {};
        }

        const auto without = longest_subsequence_palindrome_slow_rec<string_t>(str, pos - 1, buf);

        buf.push_back(str.at(pos));

        const auto with = longest_subsequence_palindrome_slow_rec<string_t>(str, pos - 1, buf);

        assert(!buf.empty());

        buf.pop_back();

        const auto &result = std::max(with, without);
        assert(result <= str.size());

        return result;
    }

    template<class item_t, std::size_t extent = std::dynamic_extent>
    [[nodiscard]] constexpr auto longest_subsequence_palindrome_almost_impl(const std::span<item_t, extent> spa)
        -> std::size_t
    {
        constexpr std::size_t one = 1;

        const auto size = spa.size();
        assert(spa.data() != nullptr && one < size && spa[0] != spa[size - one]);

        std::vector<std::vector<std::size_t>> buf(size, std::vector<std::size_t>(size, 0U));

        for (std::size_t index{}; index < size; ++index)
        {
            buf[index][index] = one;
        }

        for (std::size_t index{}; index < size - one; ++index)
        {
            buf[index][index + one] = one + (spa[index] == spa[index + one] ? one : 0U);
        }

        // Cycle len 3..n; 2nd cycle start = 0..n-len+1
        //
        // index  0  1  2  3  4
        // len    a  b  c  b  b  ; Comments
        // 1      1  1 +1  1  1
        // 2         1  1 *1  2  ; Got 2 as b = b.
        // 3            1  3+ 2  ; 3+ for "bcb" as b = b, and c is in between.
        // 4               3  3* ; 3* for "bcbb" needs penult buf[3] for "cb" which is 1!
        // 5                  3  ; Take the max of the previous and current ones.

        for (std::size_t len1 = 2; len1 < size; ++len1)
        {
            for (std::size_t start{}; start < size - len1; ++start)
            {
                const auto stop = start + len1;
                auto &val = buf[start][stop];

                if (spa[start] == spa[stop])
                {
                    const auto &prev = buf[start + one][stop - one];

                    val = prev + 2U;
                }
                else
                {
                    const auto &val_1 = buf[start + one][stop];
                    const auto &val_2 = buf[start][stop - one];

                    val = std::max(val_1, val_2);
                }
            }
        }

        const auto &result = buf[0][size - one];
        return result;
    }

    template<class item_t, std::size_t extent = std::dynamic_extent>
    [[nodiscard]] constexpr auto longest_subsequence_palindrome_impl_other(const std::span<item_t, extent> spa)
        -> std::size_t
    {
        constexpr std::size_t one = 1;

        const auto size = spa.size();
        assert(spa.data() != nullptr && one < size && spa[0] != spa[size - one]);

        // [i..to_incl] -> max length, here i <= to_incl.
        std::vector<std::size_t> buf(size, one);

        // Length 1, 2
        // for (std::size_t index{}; index < size - one; ++index)
        //{
        //    buf[index + one] = one +
        //        (spa[index] == spa[index + one] ? one : 0U);
        //}
        // Don't need the above to set old = *1

        // Cycle beg n-2..0; 2nd cycle end beg+2..n.
        //
        // index    0  1  2  3  4  SubStr Comments
        // max_len  a  b  c  b  b
        // 1        1  1 +1  1  1
        // 2                 ___2     bb ; Got 2 as b = b.
        // 3              __*1__2    cbb
        // 4           ___1__3+_3*  bcbb ; 3+ for "bcb" needs old buf[2] for "c" which is +1.
        //                               ; 3* for "bcbb" needs old buf[3] for "cb" which is *1!
        // 5        ___1__1__3__3  abcbb ; So, previous row and previous col value is needed.

        // Length: 2, 3, 4, .. size
        auto from = size - 2U;

        do
        {
            const auto &cha = spa[from];

            for (std::size_t old{}, to_incl = from + one; to_incl < size; ++to_incl)
            {
                auto &val = buf[to_incl];

                const auto cand = old + 2U;

                // Prev row, prev col.
                old = val;

                if (cha == spa[to_incl])
                {
                    assert(val <= cand && cand < size);

                    val = cand;
                }
                else if (const auto &prev = buf[to_incl - one]; val < prev)
                {
                    val = prev;
                }
            }
        } while (0U < from--);

        const auto &result = buf.back();

        assert(0U < result && result < size);

        return result;
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    // Compute the max length of a palindromic subsequence of a string.
    // Time O(n*n), space O(n).

    // Bad idea with caching problems when going in ar[--i].
    //
    // end 2..n; 2nd cycle beg = end-1..0
    //
    // index      0  1  2  3  4  SubStr Comments
    // max_len    a  b  c  b  b
    // 1          1  1 *1  1  1
    // 2          1___           ab
    // 3          1/_1___        abc
    // 4          3+_3*t1___     abcb  ; 3* = 2 + *1
    //                                 ; 3+ = max(1/, 3*)
    // 5          3__3t_2__2___  abcbb ; 3t = 2 + t1, t1 = "cb" = 1

    // Good idea. 1) i = n..0 2) main cycle j = i..n
    template<class string_t>
    [[nodiscard]] constexpr auto longest_subsequence_palindrome_other(const string_t &str) -> std::size_t
    {
        constexpr std::size_t one = 1U;

        const auto size = str.size();
        const auto half_size = size >> 1U;
        std::size_t start{};

        while (start < half_size && str[start] == str[size - one - start])
        {
            ++start;
        }

        if (const auto is_entire_string = start == half_size; is_entire_string)
        {
            return size;
        }

        const auto left_size = size - (start << 1U);
        assert(one < left_size && left_size <= size);

        const auto spa = std::span(str.data() + start, left_size);

        assert(spa.data() != nullptr && spa[0] != spa[left_size - one]);

        const auto sub_res = Inner::longest_subsequence_palindrome_impl_other(spa);

        auto result = sub_res + (start << 1U);

        assert(0U < sub_res && sub_res <= result && result < size);

        return result;
    }

    // Time, space O(n*n).
    template<class string_t>
    [[nodiscard]] constexpr auto longest_subsequence_palindrome_almost(const string_t &str) -> std::size_t
    {
        constexpr std::size_t one = 1U;

        const auto size = str.size();
        const auto half_size = size >> 1U;

        std::size_t start{};

        while (start < half_size && str[start] == str[size - one - start])
        {
            ++start;
        }

        if (const auto is_entire_string = start == half_size; is_entire_string)
        {
            return size;
        }

        // todo(p3): string_view
        const auto left_size = size - (start << 1U);
        assert(one < left_size && left_size <= size);

        const auto spa = std::span(str.data() + start, left_size);

        assert(spa.data() != nullptr && spa[0] != spa[left_size - one]);

        const auto sub_res = Inner::longest_subsequence_palindrome_almost_impl(spa);

        auto result = sub_res + (start << 1U);

        assert(0U < sub_res && sub_res <= result && result < size);

        return result;
    }

    // Slow time O(n * 2**n).
    template<class string_t>
    [[nodiscard]] constexpr auto longest_subsequence_palindrome_slow(const string_t &str) -> std::size_t
    {
        const auto size = str.size();
        {
            constexpr auto max_size = 20U;

            require_less_equal(size, max_size, "string size");
        }

        if (size <= 1U)
        {
            return size;
        }

        string_t buf;
        buf.reserve(size);

        auto result =
            Inner::longest_subsequence_palindrome_slow_rec<string_t>(str, static_cast<std::int32_t>(size) - 1, buf);

        assert(result <= size);

        return result;
    }
} // namespace Standard::Algorithms::Strings
