#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Report n-th order differences considering arr[-1] = 0.
    //   arr: 1 3 6 10 15 21 28
    // dif_0: 1 2 3  4  5  6  7
    // dif_1:   1 1  1  1  1  1 // stop when the difference becomes 0.
    template<class reporter_t, std::integral int_t>
    constexpr void report_diffs(reporter_t reporter, std::vector<int_t> &diffs, int_t curr)
    {
        require_positive(diffs.size(), "diffs size");

        for (std::size_t index{};;)
        {
            constexpr int_t zero{};

            auto &dif = diffs.at(index);
            const auto sub = curr - dif;

            if (zero == sub)
            {
                diffs.resize(index + 1U);
                return;
            }

            reporter(sub);
            dif = curr;
            curr = sub;

            if (++index == diffs.size())
            {
                diffs.push_back(sub);
                return;
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
