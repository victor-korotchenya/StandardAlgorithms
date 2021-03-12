#pragma once
#include <vector>

namespace
{
    template<typename t>
    void calc_matrix(const t& a, const t& b, const t& ab,
        std::vector<std::vector<bool>>& data)
    {
        const auto s1 = a.size(), s2 = b.size();
        data[0][0] = true;
        for (size_t i = 0; i < s1 && a[i] == ab[i]; ++i)
        {
            data[i + 1][0] = true;
        }
        for (size_t i = 0; i < s2 && b[i] == ab[i]; ++i)
        {
            data[0][i + 1] = true;
        }

        //(ab, cde, cadbe)
        //    0    c   d     e
        // 0  y    1c  0     0
        //
        // a  0    2ca 3cad  0
        // b  0    0   4cadb 5cadbe
        for (size_t r = 1; r <= s1; ++r)
        {
            for (size_t c = 1; c <= s2; ++c)
            {
                if ((data[r - 1][c] && a[r - 1] == ab[r + c - 1]) ||
                    (data[r][c - 1] && b[c - 1] == ab[r + c - 1]))
                {
                    data[r][c] = true;
                }
            }
        }
    }

    template<typename t>
    void backtrack(const t& a, const t& b, const t& ab,
        const std::vector<std::vector<bool>>& data,
        std::vector<bool>& chosen_positions)
    {
        const auto s1 = a.size(), s2 = b.size(), s3 = ab.size();
        chosen_positions.assign(s3, false);
        for (size_t r = s1, c = s2;;)
        {
            if (c && data[r][c - 1] && b[c - 1] == ab[r + c - 1])
            {
                --c;
            }
            else if (r && data[r - 1][c] && a[r - 1] == ab[r + c - 1])
            {
                --r;
                chosen_positions[r + c] = true;
            }
            else
            {
                return;
            }
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            //true for a char from the first string "a",
            //false - second "b".
            template<typename t //= std::string
            >
                bool is_interleaving(const t& a, const t& b, const t& ab,
                    std::vector<bool>& chosen_positions)
            {
                const auto s1 = a.size(),
                    s2 = b.size(),
                    s3 = ab.size();
                if (s1 + s2 != s3)
                {
                    return false;
                }

                if (0 == s3)
                {
                    chosen_positions.clear();
                    return true;
                }
                if (0 == s1)
                {
                    chosen_positions.assign(s3, false);
                    return b == ab;
                }
                if (0 == s2)
                {
                    chosen_positions.assign(s3, true);
                    return a == ab;
                }

                if ((a[0] != ab[0] && b[0] != ab[0])
                    || (a[s1 - 1] != ab[s3 - 1] && b[s2 - 1] != ab[s3 - 1]))
                {//quick test.
                    return false;
                }

                std::vector<std::vector<bool>> data(s1 + 1, std::vector<bool>(s2 + 1));
                calc_matrix(a, b, ab, data);
                if (!data[s1][s2])
                {
                    return false;
                }

                backtrack(a, b, ab, data, chosen_positions);
                return true;
            }
        }
    }
}