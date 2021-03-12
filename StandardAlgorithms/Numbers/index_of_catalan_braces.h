#pragma once
#include <algorithm>
#include <vector>
#include "enum_catalan_braces.h" // is_catalan_string

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Prepare once.
            template<class number_t = int>
            std::vector<std::vector<number_t>> compute_catalan_brace_table(const number_t size)
            {
                RequirePositive(size, "size");

                // To use index -1.
                constexpr auto x1 = 1;

                std::vector<std::vector<number_t>> t(size + 1);
                t[0] = { 0,1,0,0 }; // empty string.

                for (number_t len = 1; len <= size; ++len)
                {
                    const auto& prev = t[len - 1];
                    auto& cur = t[len];

                    // Too large balance does not matter.
                    const auto bal_max = std::min<number_t>(len, size + 1 - len);
                    cur.resize(bal_max + 3 + x1);

                    for (number_t bal = len & 1; bal <= bal_max; bal += 2)
                    {
                        const auto ad = prev[bal - 1 + x1] + prev[bal + 1 + x1];
                        //bal:  0 1 2 3
                        //len
                        //  0   1
                        //  1     1
                        //  2   1   1
                        //  3     2   1
                        //  4   2   3
                        //  5     5
                        //  6   5
                        cur[bal + x1] += ad;
                    }
                }

                return t;
            }

            // Count how many valid Catalan strings stand before.
            // E.g. "[[][]]" has index 1 in the sorted array:
            //"[[[]]]",
            //"[[][]]",
            //"[[]][]",
            //"[][[]]",
            //"[][][]".
            template<class number_t = int, char open_c = '[', char close_c = ']'>
            number_t index_of_catalan_braces(const std::vector<std::vector<number_t>>& table, const std::string& str)
            {
                static_assert(open_c != close_c);

                assert(str.size() + 1 == table.size());
#if _DEBUG
                {
                    //assert(is_catalan_string<open_c, close_c>(str));
                    // C4002 too many arguments for function-like macro invocation 'assert'
                    const auto isc = is_catalan_string<open_c, close_c>(str);
                    assert(isc);
                }
#endif

                const auto size = static_cast<number_t>(str.size());
                if (!size)
                    return 0;

                assert(str.front() == open_c && str.back() == close_c);

                // To use index -1.
                constexpr auto x1 = 1;
                number_t r = 0;
                for (number_t pos = 0, bal = 0; pos < size; ++pos)
                {
                    const auto& c = str[pos];
                    if (c == open_c)
                    {
                        ++bal;
                        continue;
                    }

                    assert(c == close_c && bal > 0);
                    const number_t ind = size - 1 - pos,
                        & ad = table[ind][bal + 1 + x1];
                    assert(ad >= 0);

                    r += ad;
                    assert(r >= 0);
                    --bal;
                }
                return r;
            }

            // Given the index, build the valid Catalan string with that index. The string length is table.size - 1.
            // E.g. index 1 of length 6 has the string "[[][]]".
            template<class number_t = int, char open_c = '[', char close_c = ']'>
            void brace_string_by_index_of_catalan(const std::vector<std::vector<number_t>>& table,
                std::string& str,
                const number_t index)
            {
                static_assert(open_c != close_c);

                const auto size = static_cast<number_t>(table.size()) - 1;
                assert(size >= 0 && !(size & 1));

                str.resize(size, open_c);
                if (!size)
                {
                    assert(!index);
                    return;
                }

                // To use index -1.
                constexpr auto x1 = 1;
                number_t bal = 0, left = index;
                for (number_t pos = 0; pos < size; ++pos)
                {
                    assert(bal >= 0);
                    const number_t ind = size - 1 - pos,
                        & gr = table[ind][bal + 1 + x1];

                    auto& c = str[pos];
                    if (left < gr)
                    {
                        ++bal;
                        c = open_c;
                        continue;
                    }

                    assert(bal > 0);
                    c = close_c;
                    --bal;
                    left -= gr;
                    assert(left >= 0);
                }

                assert(!bal && !left && str.front() == open_c && str.back() == close_c);
#if _DEBUG
                {
                    //assert(is_catalan_string<open_c, close_c>(str));
                    // C4002 too many arguments for function-like macro invocation 'assert'
                    const auto isc = is_catalan_string<open_c, close_c>(str);
                    assert(isc);
                }
#endif
            }
        }
    }
}