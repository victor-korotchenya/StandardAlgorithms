#pragma once
#include <cassert>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class proc_t, char open_c, char close_c, class check_t>
    void enum_catalan_braces_rec(proc_t proc, std::string& str,
        const int open, const int close, check_t check)
    {
        static_assert(open_c != close_c);

        assert(open >= 0 && close > 0 && open <= close);
        if (open > 0)
        {
            str.push_back(open_c);
            enum_catalan_braces_rec<proc_t, open_c, close_c, check_t>(proc, str, open - 1, close, check);
            str.pop_back();
        }
        else if (close == 1)
        {
            str.push_back(close_c);
            check(str);
            proc(str);
            str.pop_back();
            return;
        }

        if (open == close)
            return;

        str.push_back(close_c);
        enum_catalan_braces_rec<proc_t, open_c, close_c, check_t>(proc, str, open, close - 1, check);
        str.pop_back();
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template<char open_c = '[', char close_c = ']'>
            bool is_catalan_string(const std::string& str)
            {
                static_assert(open_c != close_c);
                const auto size = static_cast<int>(str.size());
                if (size & 1)
                    return false;//The size must be even.

                auto open = 0, close = 0;
                for (const auto& c : str)
                {
                    if (c == open_c)
                        ++open;
                    else if (c == close_c)
                    {
                        if (++close > open)
                            return false;
                    }
                    else
                        return false;
                }

                return open == close;
            }

            // Enumerate all valid/balanced Catalan strings.
            template<class proc_t, char open_c = '[', char close_c = ']'>
            void enum_catalan_braces(proc_t proc, const int n)
            {
                static_assert(open_c != close_c);
                RequirePositive(n, "n");

                std::string str;
                str.reserve(n << 1);

                enum_catalan_braces_rec<proc_t, open_c, close_c>(proc, str, n, n, &is_catalan_string<open_c, close_c>);
            }

            // Method 2: find the rightmost imbalanced [ e.g. "[ ]]] [][]",
            // replace [ with ],
            // print [[[
            // and ]]]].
            // to build "] [[[ ]]]]".
            template<char open_c = '[', char close_c = ']'>
            bool next_catalan_string(std::string& str)
            {
                static_assert(open_c != close_c);
                assert(!(str.size() & 1));

                if (str.empty() || !is_catalan_string<open_c, close_c>(str))
                    return false;

                assert(!(str.size() & 1));

                auto i = static_cast<int>(str.size()) - 1, open = 0, close = 0;
                for (;;)
                {
                    assert(i > 0 && close >= open);
                    const auto& c = str[i];
                    // ...  [ ]] [][]
                    // ...  ] [[[ ]]]
                    //
                    // ...  [ ]]
                    // ...  ] []
                    if (c == open_c)
                    {
                        assert(open < close);
                        if (++open < close)
                        {
                            str[i] = close_c;

                            do str[++i] = open_c;
                            while (--open > 0);

                            --close;
                            do str[++i] = close_c;
                            while (--close > 0);

                            return true;
                        }
                    }
                    else
                    {
                        assert(c == close_c);
                        ++close;
                        assert(close > open);
                    }

                    if (!--i)
                    {
                        // Build the initial minimal string.
                        const auto half = static_cast<int>(str.size() >> 1);
                        i = half;
                        assert(i > 0);
                        do
                        {
                            str[i - 1] = open_c;
                            str[i - 1 + half] = close_c;
                        } while (--i);

                        return false;
                    }
                }
            }
        }
    }
}