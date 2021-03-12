#pragma once
#include"../Utilities/throw_exception.h"

// todo(p3): #include<generator> // co_yield

namespace Standard::Algorithms::Strings::Inner
{
    template<class func_t>
    constexpr void string_enumerator(std::string &str, std::int32_t left, std::int32_t right, func_t callback,
        const char left_brace, const char right_brace)
    {
        if (left == right)
        {
            callback(str);
        }

        if (0 < left)
        {
            str += left_brace;
            string_enumerator(str, left - 1, right, callback, left_brace, right_brace);
            str.pop_back();
        }

        if (left < right)
        {
            str += right_brace;
            string_enumerator(str, left, right - 1, callback, left_brace, right_brace);
            str.pop_back();
        }
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    // Catalan number of all valid strings of length = "length".
    template<class func_t>
    constexpr void enumerate_all_valid_brace_strings(
        std::int32_t length, func_t callback, const char left_brace = '(', const char right_brace = ')')
    {
        if (constexpr auto max_size = 40; length < 0 || max_size < length) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "String length (" << length << ") must be between 0 and " << max_size << " error.";
            throw_exception(str);
        }

        if (const auto suin = static_cast<std::uint32_t>(length); 0U != (suin & 1U)) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "String length (" << length << ") must be an even integer.";
            throw_exception(str);
        }

        std::string str{};
        str.reserve(length);

        Inner::string_enumerator<func_t>(str, length / 2, length / 2, callback, left_brace, right_brace);
    }
} // namespace Standard::Algorithms::Strings
