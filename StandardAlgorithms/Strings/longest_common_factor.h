#pragma once
#include"../Utilities/is_debug.h"
// "longest_common_factor.h"
#include"demand_char_unique.h" // demand_item_unique
#include"suffix_array.h" // suffix_array_other
#include<cassert>
#include<cstddef>
#include<cstdint>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Strings
{
    // Find the longest common factor or substring of 2 strings: start and stop in each string.
    // If nothing is found, return 0s.
    // Time O(m+n), space O(min(m, n)).
    // See also longest_common_factor in ukkonen_suffix_tree_utilities.h

    // The separator must not occur in either string
    // Time, space O(m + n).
    template<class string_t, class char_t, class pair_t>
    constexpr void lcp_longest_common_factor(
        const string_t &eins, const char_t &abscheider, const string_t &zwei, pair_t &result)
    {
        result = {};

        const auto eins_size = eins.size();

        if (eins_size == 0U || zwei.size() == 0U)
        {
            return;
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            auto err = std::string("The separator '") + abscheider + "' must be unique in";
            demand_item_unique(err + " eins", eins, abscheider);
            demand_item_unique(err + " zwei", zwei, abscheider);
        }

        const string_t polizei = eins + abscheider + zwei;
        const auto polizei_size = polizei.size();

        assert(0U < polizei_size && polizei_size == eins_size + 1ZU + zwei.size());

        std::vector<std::int32_t> tsar{};
        std::vector<std::int32_t> lcp{};
        suffix_array_other(polizei, tsar, &lcp);

        assert(polizei_size == tsar.size() && polizei_size == lcp.size());

        std::int32_t length_max{};

        for (std::size_t index{ 1 }; index < polizei_size; ++index)
        {
            const auto &len = lcp[index];
            if (!(length_max < len))
            {
                continue;
            }

            const auto row = static_cast<std::size_t>(tsar[index - 1]);
            const auto col = static_cast<std::size_t>(tsar[index]);

            // 012 s3 456789A ; 3 + 1 + 7
            if (row < eins_size && eins_size < col)
            {
                result.first.first = row;
                result.second.first = col - eins_size - 1;
            }
            else if (col < eins_size && eins_size < row)
            {
                result.first.first = col;
                result.second.first = row - eins_size - 1;
            }
            else
            {
                continue;
            }

            length_max = len;
            result.first.second = result.first.first + len;
            result.second.second = result.second.first + len;
        }
    }

    // Slow time O(m*n), space O(min(m, n)).
    template<class string_a, class string_b, class pair_t>
    constexpr void longest_common_factor_slow_still2(
        const string_a &source, const string_b &destination, pair_t &result)
    {
        result = {};

        const auto source_size = static_cast<std::int32_t>(source.size());
        const auto destination_size = static_cast<std::int32_t>(destination.size());
        if (source_size == 0 || destination_size == 0)
        {
            return;
        }

        const auto use_src = source_size <= destination_size;
        const auto &one = use_src ? source : destination;
        const auto one_size = use_src ? source_size : destination_size;

        const auto &two = use_src ? destination : source;
        const auto two_size = use_src ? destination_size : source_size;

        std::vector<std::int32_t> cur(one_size + 1LL);
        std::vector<std::int32_t> prev(one_size + 1LL);

        std::int32_t row_max{};
        std::int32_t col_max{};
        std::int32_t length_max{};

        for (std::int32_t col{}; col < two_size; ++col)
        {
            std::swap(prev, cur);

            for (std::int32_t row{}; row < one_size; ++row)
            {
                auto &len = cur[row + 1];

                if (one[row] != two[col])
                {
                    len = 0;
                    continue;
                }

                len = prev[row] + 1;

                if (length_max < len)
                {
                    length_max = len;
                    row_max = row;
                    col_max = col;
                }
            }
        }

        if (length_max == 0)
        {
            return;
        }

        assert(!(row_max < 0) && row_max < one_size && !(col_max < 0) && col_max < two_size);

        row_max -= length_max - 1;
        col_max -= length_max - 1;

        assert(!(row_max < 0) && row_max < one_size && !(col_max < 0) && col_max < two_size);

        result.first.first = use_src ? row_max : col_max;
        result.second.first = use_src ? col_max : row_max;

        result.first.second = result.first.first + length_max;
        result.second.second = result.second.first + length_max;
    }

    // Slow time, space O(m*n).
    template<class string_a, class string_b, class pair_t>
    constexpr void longest_common_factor_slow_still(const string_a &one, const string_b &two, pair_t &result)
    {
        result = {};

        const auto one_size = static_cast<std::int32_t>(one.size());
        const auto two_size = static_cast<std::int32_t>(two.size());
        if (one_size == 0 || two_size == 0)
        {
            return;
        }

        std::vector<std::vector<std::int32_t>> buf(one_size + 1LL, std::vector<std::int32_t>(two_size + 1LL, 0));

        std::int32_t row_max{};
        std::int32_t col_max{};
        std::int32_t length_max{};

        for (std::int32_t row{}; row < one_size; ++row)
        {
            const auto &prev = buf[row];
            auto &cur = buf[row + 1];

            for (std::int32_t col{}; col < two_size; ++col)
            {
                if (one[row] != two[col])
                {
                    continue;
                }

                auto &len = cur[col + 1];
                len = prev[col] + 1;

                if (length_max < len)
                {
                    length_max = len;
                    row_max = row;
                    col_max = col;
                }
            }
        }

        if (length_max == 0)
        {
            return;
        }

        assert(!(row_max < 0) && row_max < one_size && !(col_max < 0) && col_max < two_size);

        row_max -= length_max - 1;
        col_max -= length_max - 1;

        assert(!(row_max < 0) && row_max < one_size && !(col_max < 0) && col_max < two_size);

        result.first.first = row_max;
        result.first.second = row_max + length_max;

        result.second.first = col_max;
        result.second.second = col_max + length_max;
    }

    // Slow time O(n**3), space(1).
    template<class string_a, class string_b, class pair_t>
    constexpr void longest_common_factor_slow(const string_a &one, const string_b &two, pair_t &result) noexcept
    {
        result = {};

        const auto one_size = static_cast<std::int32_t>(one.size());
        const auto two_size = static_cast<std::int32_t>(two.size());
        if (one_size == 0 || two_size == 0)
        {
            return;
        }

        for (std::int32_t row{}, length_max{}; row + length_max < one_size; ++row)
        {
            for (std::int32_t col{}; col + length_max < two_size; ++col)
            {
                if (one[row] != two[col])
                {
                    continue;
                }

                const auto tail = std::min(one_size - row, two_size - col);
                auto len = 1;
                while (len < tail && one[row + len] == two[col + len])
                {
                    ++len;
                }

                if (length_max < len)
                {
                    length_max = len;
                    result.first.first = row;
                    result.first.second = row + len;
                    result.second.first = col;
                    result.second.second = col + len;

                    if (len == std::min(one_size, two_size))
                    {
                        return;
                    }
                }
            }
        }
    }
} // namespace Standard::Algorithms::Strings
