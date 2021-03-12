#pragma once
#include <algorithm>
#include <vector>
#include "enum_catalan_braces.h" // is_catalan_string

namespace
{
    template<char open_c, char close_c, char question_c>
    int count_catalan_braces_rec_slow(std::string& str, const int pos, const int balance = 0)
    {
        if (balance > 0)
            // going from right.
            return 0;

        if (pos < 0)
        {
#if _DEBUG
            for (const auto& c : str) assert(c == open_c || c == close_c);
#endif
            const auto is = !balance && Privet::Algorithms::Numbers::is_catalan_string<open_c, close_c>(str);
            return is;
        }

        const auto c = str[pos];
        if (c != question_c)
            return count_catalan_braces_rec_slow<open_c, close_c, question_c>(str, pos - 1, balance + (c == open_c ? 1 : -1));

        str[pos] = open_c;
        const auto r = count_catalan_braces_rec_slow<open_c, close_c, question_c>(str, pos - 1, balance + 1);

        str[pos] = close_c;
        const auto l = count_catalan_braces_rec_slow<open_c, close_c, question_c>(str, pos - 1, balance - 1);

        str[pos] = c;
        return r + l;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Given "[[??][??", count all possible valid Catalan strings.
            // '?' can be [ or ].
            template<class number_t = int, number_t mod = 1000 * 1000 * 1000, char open_c = '[', char close_c = ']', char question_c = '?'>
            number_t count_catalan_braces(const std::string& str)
            {
                static_assert(mod >= 2 && open_c != close_c && open_c != question_c && close_c != question_c);
                if (str.empty())
                    return 1;

                const auto size = static_cast<int>(str.size());
                if (size & 1 || str.front() == close_c || str.back() == open_c)
                    return 0;

                // To use index -1.
                constexpr auto x1 = 1;

                // A valid substring of length + x1, balance + x1 has count.
                std::vector<number_t> prev((size >> 1) + 3 + x1), cur(prev.size());
                cur[x1] = 1; // empty string.
#if _DEBUG
                std::vector<std::vector<number_t>> v_debug{ prev, cur };
#endif
                for (auto len = 1; len <= size; ++len)
                {
                    std::swap(prev, cur);

                    // Too large balance does not matter.
                    const auto bal_max = std::min(len, size + 1 - len);
                    cur.assign(bal_max + 3ll + x1, 0);

                    const auto& c = str[len - 1ll];
                    assert(open_c == c || close_c == c || question_c == c);

                    for (auto bal = len & 1; bal <= bal_max; bal += 2)
                    {
                        number_t ad;
                        if (open_c == c)
                            ad = prev[bal - 1 + x1];
                        else if (close_c == c)
                            ad = prev[bal + 1 + x1];
                        else
                        {
                            ad = prev[bal - 1 + x1] + prev[bal + 1 + x1];
                            if (ad >= mod)
                                ad -= mod;
                        }
                        //bal:  0 1 2 3
                        //len
                        //  0   1
                        //  1     1
                        //  2   1   1
                        //  3     2   1
                        //  4   2   3
                        //  5     5
                        //  6   5
                        auto& val = cur[bal + x1];
                        val += ad;
                        if (val >= mod)
                            val -= mod;
                    }
#if _DEBUG
                    v_debug.push_back(cur);
#endif
                }

                const auto& r = cur[x1];
                assert(r >= 0 && r < mod);
                return r;
            }

            template<char open_c = '[', char close_c = ']', char question_c = '?'>
            int count_catalan_braces_slow(const std::string& source)
            {
                static_assert(open_c != close_c && open_c != question_c && close_c != question_c);
                if (source.empty())
                    return 1;

                const auto size = static_cast<int>(source.size());
                if (size & 1)
                    return 0;

                auto str = source;
                const auto r = count_catalan_braces_rec_slow<open_c, close_c, question_c>(str, size - 1);
                return r;
            }
        }
    }
}