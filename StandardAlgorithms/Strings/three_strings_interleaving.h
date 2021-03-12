#pragma once
// "three_strings_interleaving.h"
#include<cstddef>
#include<vector>

namespace Standard::Algorithms::Strings::Inner
{
    template<class string_t>
    constexpr void calc_matrix(
        const string_t &one, const string_t &two, const string_t &both, std::vector<std::vector<bool>> &data)
    {
        const auto one_size = one.size();
        const auto two_size = two.size();
        data[0][0] = true;

        for (std::size_t index{}; index < one_size && one[index] == both[index]; ++index)
        {
            data[index + 1U][0] = true;
        }

        for (std::size_t index{}; index < two_size && two[index] == both[index]; ++index)
        {
            data[0][index + 1U] = true;
        }

        //(ab, cde, cadbe)
        //    0    c   d     e
        // 0  y    1c  0     0
        //
        // a  0    2ca 3cad  0
        // b  0    0   4cadb 5cadbe
        for (std::size_t row = 1; row <= one_size; ++row)
        {
            const auto &pre = data[row - 1U];
            auto &curr = data[row];

            for (std::size_t col = 1; col <= two_size; ++col)
            {
                if ((pre[col] && one[row - 1U] == both[row + col - 1U]) ||
                    (curr[col - 1U] && two[col - 1U] == both[row + col - 1U]))
                {
                    curr[col] = true;
                }
            }
        }
    }

    template<class string_t>
    constexpr void backtrack(const string_t &one, const std::vector<std::vector<bool>> &data, const string_t &two,
        std::vector<bool> &chosen_positions, const string_t &both)
    {
        const auto one_size = one.size();
        const auto two_size = two.size();
        const auto both_size = both.size();
        chosen_positions.assign(both_size, false);

        for (auto row = one_size, col = two_size;;)
        {
            if (0U < col && data[row][col - 1U] && two[col - 1U] == both[row + col - 1U])
            {
                --col;
            }
            else if (0U < row && data[row - 1U][col] && one[row - 1U] == both[row + col - 1U])
            {
                --row;
                chosen_positions[row + col] = true;
            }
            else
            {
                return;
            }
        }
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    // true for a char from the first string "one",
    // false - second "two".
    template<class string_t>
    [[nodiscard]] constexpr auto is_interleaving(
        const string_t &one, const string_t &two, const string_t &both, std::vector<bool> &chosen_positions) -> bool
    {
        const auto one_size = one.size();
        const auto two_size = two.size();
        const auto both_size = both.size();

        if (one_size + two_size != both_size)
        {
            return false;
        }

        if (0U == both_size)
        {
            chosen_positions.clear();
            return true;
        }

        if (0U == one_size)
        {
            chosen_positions.assign(both_size, false);
            return two == both;
        }

        if (0U == two_size)
        {
            chosen_positions.assign(both_size, true);
            return one == both;
        }

        if ((one[0] != both[0] && two[0] != both[0]) ||
            (one[one_size - 1U] != both[both_size - 1U] && two[two_size - 1U] != both[both_size - 1U]))
        {// quick test.
            return false;
        }

        std::vector<std::vector<bool>> data(one_size + 1U, std::vector<bool>(two_size + 1U, false));

        Inner::calc_matrix(one, two, both, data);
        if (!data[one_size][two_size])
        {
            return false;
        }

        Inner::backtrack(one, data, two, chosen_positions, both);
        return true;
    }
} // namespace Standard::Algorithms::Strings
