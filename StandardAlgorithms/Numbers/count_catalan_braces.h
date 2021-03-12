#pragma once
#include"../Utilities/is_debug.h"
#include"bit_utilities.h"
#include"default_modulus.h"
#include"enum_catalan_braces.h" // is_catalan_string
#include<algorithm>
#include<string>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    constexpr auto default_question_c = '?';
    constexpr auto default_open_c = '[';
    constexpr auto default_close_c = ']';
} // namespace Standard::Algorithms::Numbers

namespace Standard::Algorithms::Numbers::Inner
{
    template<class int_t, int_t mod, char open_c, char close_c, char question_c>
    requires(open_c != close_c && open_c != question_c && close_c != question_c)
    [[nodiscard]] constexpr auto count_catalan_braces_rec_slow(
        std::string &str, const int_t &pos, const int_t &balance = int_t{}) -> int_t
    {
        if (0 < balance)
        {// going from right.
            return {};
        }

        if (pos < 0)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                for (const auto &cha1 : str)
                {
                    assert(cha1 == open_c || cha1 == close_c);
                }
            }

            const auto has = 0 == balance && ::Standard::Algorithms::Numbers::is_catalan_string<open_c, close_c>(str);

            return has ? 1 : 0;
        }

        const auto old_cha = str[pos];

        if (old_cha != question_c)
        {
            return count_catalan_braces_rec_slow<int_t, mod, open_c, close_c, question_c>(
                str, pos - 1, balance + (old_cha == open_c ? 1 : -1));
        }

        str[pos] = open_c;

        auto right = count_catalan_braces_rec_slow<int_t, mod, open_c, close_c, question_c>(str, pos - 1, balance + 1);

        str[pos] = close_c;

        right += count_catalan_braces_rec_slow<int_t, mod, open_c, close_c, question_c>(str, pos - 1, balance - 1);

        str[pos] = old_cha;

        if (!(right < mod))
        {
            right -= mod;
        }

        assert(0 <= right && right < mod);

        return right;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given "[[??][??", count all possible valid Catalan strings.
    // '?' can be [ or ].
    template<std::integral int_t = std::int32_t, int_t mod = default_modulus, char open_c = default_open_c,
        char close_c = default_close_c, char question_c = default_question_c>
    requires(2 <= mod && open_c != close_c && open_c != question_c && close_c != question_c)
    [[nodiscard]] constexpr auto count_catalan_braces(const std::string &str) -> int_t
    {
        if (str.empty())
        {
            return 1;
        }

        const auto size = static_cast<int_t>(str.size());

        if (has_zero_bit(size) || str.front() == close_c || str.back() == open_c)
        {
            return 0;
        }

        // To use index -1.
        constexpr auto ix1 = 1;

        // A valid substring of length + ix1, balance + ix1 has count.
        std::vector<int_t> prev((size >> 1U) + 3LL + ix1);

        std::vector<int_t> cur(prev.size());
        cur[ix1] = 1; // empty string.

#if _DEBUG
        std::vector<std::vector<int_t>> v_debug{ prev, cur };
#endif

        for (int_t len = 1; len <= size; ++len)
        {
            std::swap(prev, cur);

            // Too large balance does not matter.
            const auto bal_max = std::min(len, size + 1 - len);

            cur.assign(bal_max + 3LL + ix1, 0);

            const auto &cha = str[len - 1];

            assert(open_c == cha || close_c == cha || question_c == cha);

            for (int_t bal = len & 1; bal <= bal_max; bal += 2)
            {
                int_t add{};

                if (open_c == cha)
                {
                    add = prev[bal - 1 + ix1];
                }
                else if (close_c == cha)
                {
                    add = prev[bal + 1 + ix1];
                }
                else
                {
                    add = prev[bal - 1 + ix1] + prev[bal + 1 + ix1];

                    if (!(add < mod))
                    {
                        add -= mod;
                    }
                }

                // bal:  0 1 2 3
                // len
                //   0   1
                //   1     1
                //   2   1   1
                //   3     2   1
                //   4   2   3
                //   5     5
                //   6   5
                auto &val = cur[bal + ix1];
                val += add;

                if (!(val < mod))
                {
                    val -= mod;
                }
            }

#if _DEBUG
            v_debug.push_back(cur);
#endif
        }

        const auto &res = cur[ix1];
        assert(!(res < 0) && res < mod);

        return res;
    }

    template<std::integral int_t = std::int32_t, int_t mod = default_modulus, char open_c = default_open_c,
        char close_c = default_close_c, char question_c = default_question_c>
    requires(2 <= mod && open_c != close_c && open_c != question_c && close_c != question_c)
    [[nodiscard]] constexpr auto count_catalan_braces_slow(const std::string &source) -> int_t
    {
        if (source.empty())
        {
            return 1;
        }

        const auto size = static_cast<int_t>(source.size());

        if (const auto is_odd = has_zero_bit(size); is_odd)
        {
            return 0;
        }

        auto str = source;

        auto res = Inner::count_catalan_braces_rec_slow<int_t, mod, open_c, close_c, question_c>(
            str, static_cast<int_t>(size - 1));

        assert(0 <= res && res < mod);

        return res;
    }
} // namespace Standard::Algorithms::Numbers
