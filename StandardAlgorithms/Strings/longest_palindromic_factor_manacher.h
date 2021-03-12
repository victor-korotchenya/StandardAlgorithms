#pragma once
#include"string_utilities.h" // is_palindrome
#include<vector>

namespace Standard::Algorithms::Strings
{
    // Compute the max length of a palindromic factor (substring) of a non-empty string,
    // returning [start, stop), where start < stop.
    // Time, space O(n).
    //
    // Also see ukkonen_suffix_tree_utilities.h
    template<class string_t>
    [[nodiscard]] constexpr auto longest_palindromic_factor_manacher(const string_t &str)
        -> std::pair<std::size_t, std::size_t>
    {
        {
            constexpr auto max_size = std::numeric_limits<std::size_t>::max() / 2 - 6U;

            require_greater(max_size, str.size(), "string size");
        }

        constexpr std::size_t one = 1;

        // abbc -> |a|b|b|c| where '|' is an invisible delimiter.
        // 0123    012345678
        const auto size = (require_positive(str.size(), "string size") << 1U) | one;
        assert(3U <= size);

        auto center_max = one;
        auto length_max = one;

        std::vector<std::size_t> substr_len_max(size);
        substr_len_max.at(1) = one;

        for (std::size_t center = 1, current_right = 2, right = 2; current_right < size; ++current_right)
        {
            auto &cur_max = substr_len_max[current_right];

            if (const auto is_within_window = current_right < right; is_within_window)
            {
                // center - left == current_right - center
                const auto left = (center << 1U) - current_right;

                assert(current_right <= (center << 1U));
                assert(left <= center && center <= current_right);

                cur_max = std::min(substr_len_max.at(left), right - current_right);
            }
            else
            {
                assert(0U == cur_max);
            }

            // '|' is at even indexes - always increment.
            // If real chars at odd indexes match, then increment.
            while (cur_max < current_right && cur_max + current_right < size &&
                (0U == ((cur_max + current_right + one) & one) ||
                    (str[(current_right - cur_max - one) >> 1U] == str[(cur_max + current_right + one) >> 1U])))
            {
                ++cur_max;
            }

            if (const auto is_outside = right < current_right + cur_max; is_outside)
            {
                right = current_right + cur_max, center = current_right;
            }

            if (length_max < cur_max)
            {
                length_max = cur_max, center_max = current_right;
            }

            assert(!(center_max < length_max));
        }

        const auto start = (center_max - length_max) >> 1U;
        const auto stop = start + length_max;

        assert(start < stop && stop <= size);

        return { start, stop };
    }

    // Slow time, space O(n*n).
    template<class string_t>
    [[nodiscard]] constexpr auto longest_palindromic_factor_slow(const string_t &str)
        -> std::pair<std::size_t, std::size_t>
    {
        constexpr std::size_t one = 1;

        const auto size = require_positive(str.size(), "string size");

        if (Standard::Algorithms::Strings::is_palindrome(&str[0], size))
        {
            return { 0U, size };
        }

        assert(one < size);

        std::size_t start{};
        auto stop = one;

        for (std::size_t index{}; index < size; ++index)
        {
            {// Odd length palindrome
                const auto odd_length = std::min(index, size - one - index);

                std::size_t cand{};

                for (auto ind_2 = one; ind_2 <= odd_length && str[index - ind_2] == str[index + ind_2]; ++ind_2)
                {
                    ++cand;
                }

                if (stop - start < ((cand << 1U) | one))
                {
                    start = index - cand;
                    stop = index + cand + one;

                    assert(start < stop && stop <= size);
                }
            }

            if (index <= size - 2U)
            {// Even length
                const auto even_length = std::min(index, size - 2U - index);
                std::size_t cand{};

                for (std::size_t ind_2{}; ind_2 <= even_length && str[index - ind_2] == str[index + ind_2 + one];
                     ++ind_2)
                {
                    ++cand;
                }

                if (stop - start < (cand << 1U))
                {
                    start = index - cand + one;
                    stop = index + cand + one;

                    assert(start < stop && stop <= size);
                }
            }
        }

        assert(start < stop && stop <= size);

        return { start, stop };
    }
} // namespace Standard::Algorithms::Strings
