#pragma once
#include<cassert>
#include<cstddef>
#include<string>
#include<vector>

namespace Standard::Algorithms::Strings
{
    // Given a string s of size n, z array or z function has size n,
    // where z[i] is the longest common prefix LCP(s, s[i:]) of the suffix s[i:] and the whole string.
    //
    // Examples.
    // "AbAcAbAcAbAe" of size 12 == 0xC == z[0],
    // {01234 56789 ab} indexes,
    // {c0107 01030 10} z-array in hex,
    // z[8] == length("AbA"), where "AbA" == s[:3] == s[8:11].
    //
    // String matching example: pattern "aab", text "baabaa", unique symbol '#'.
    // The concatenated string of size 10 == 0xA:
    //  "aab#baabaa"
    // "0123456789" indexes,
    // "a100031021" Z array in hex.
    // The value 3 (pattern size) in Z array at index 5 means
    // there is a matching of length 3
    // between the suffix "aabaa" and the whole string.
    //
    // See also "knuth_morris_pratt.h".
    [[nodiscard]] inline constexpr auto z_array(const std::string &str) -> std::vector<std::size_t>
    {
        if (str.empty())
        {
            return {};
        }

        const auto size = str.size();
        std::vector<std::size_t> zar(size);
        zar[0] = size;

        std::size_t left{};
        std::size_t right{}; // The right inclusive position of the rightmost sub-border.

        for (std::size_t index = 1; index < size; ++index)
        {
            if (right < index)
            {
                right = index;
            }
            else
            {
                // The already computed sub-border [left .. right] of the str assumes that
                // str[0 .. (right - left)] ==== str[left .. index .. right],
                // where (left <= index and index <= right).
                const auto &precomputed = zar[index - left];
                const auto bound = right - index + 1LLU;
                const auto is_within_right_bound = precomputed < bound;
                if (is_within_right_bound)
                {
                    zar[index] = precomputed;
                    assert(zar[index] <= size - index);
                    continue;
                }
            }

            while (right < size && str[right - index] == str[right])
            {
                ++right;
            }

            zar[index] = right - index;
            left = index;
            --right;
        }

        return zar;
    }

    [[nodiscard]] inline constexpr auto z_array_slow(const std::string &str) -> std::vector<std::size_t>
    {
        const auto size = str.size();
        std::vector<std::size_t> zar(size);

        for (std::size_t index{}; index < size; ++index)
        {
            auto &matches = zar[index];

            while (index + matches < size && str[matches] == str[index + matches])
            {
                ++matches;
            }
        }

        return zar;
    }

    // A border is a non-empty prefix of length L, that is equal to a suffix,
    //  and is shorter then the whole string.
    //  The string "abacabacaba" has 3 borders:
    //  1 "a"
    //  3 "aba". The whole string starts and ends with "aba" which has size 3.
    //  7 "abacaba"
    [[nodiscard]] inline constexpr auto string_borders(const std::string &str) -> std::vector<std::size_t>
    {
        const auto zar = z_array(str);
        const auto size = str.size();
        assert(zar.size() == size);

        std::vector<std::size_t> result;

        for (std::size_t index = 1U; index < size; ++index)
        {
            if (index == zar[size - index])
            {
                result.push_back(index);
            }
        }

        return result;
    }

    [[nodiscard]] inline constexpr auto string_borders_slow(const std::string &str) -> std::vector<std::size_t>
    {
        const auto size = str.size();
        std::vector<std::size_t> result;

        for (std::size_t index = 1; index < size; ++index)
        {
            auto okk = true;

            for (std::size_t ind2{}; ind2 < index && okk; ++ind2)
            {
                const auto right = size - index + ind2;
                okk = str[ind2] == str[right];
            }

            if (okk)
            {
                result.push_back(index);
            }
        }

        return result;
    }
} // namespace Standard::Algorithms::Strings
