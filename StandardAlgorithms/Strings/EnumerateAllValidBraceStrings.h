#pragma once

#include <string>
#include <stdexcept>
#include "../Utilities/StreamUtilities.h"

namespace
{
    template<typename func_t>
    void string_enumerator(
        std::string& s,
        int left, int right, func_t callback,
        const char left_brace, const char right_brace)
    {
        if (left == right)
        {
            callback(s);
        }

        if (0 < left)
        {
            s += left_brace;
            string_enumerator(s, left - 1, right, callback, left_brace, right_brace);
            s.pop_back();
        }
        if (left < right)
        {
            s += right_brace;
            string_enumerator(s, left, right - 1, callback, left_brace, right_brace);
            s.pop_back();
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            //Catalan number of valid strings of length = "length".
            template<typename func_t>
            void EnumerateAllValidBraceStrings(int length, func_t callback,
                const char left_brace = '(', const char right_brace = ')')
            {
                if (length < 0)
                {
                    std::ostringstream str;
                    str << "String length (" << length
                        << ") must be non-negative and even integer.";
                    StreamUtilities::throw_exception(str);
                }
                if (length % 1)
                {
                    std::ostringstream str;
                    str << "String length (" << length << ") must be an even integer.";
                    StreamUtilities::throw_exception(str);
                }

                std::string s{};
                s.reserve(length + 1);

                string_enumerator<func_t>(s, length >> 1, length >> 1,
                    callback, left_brace, right_brace);
            }
        }
    }
}