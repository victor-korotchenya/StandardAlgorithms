#pragma once
#include<cassert>
#include<vector>

namespace Privet::Algorithms::Numbers
{
    template <class fraction_t, class number_t>
    fraction_t bernoulli(std::vector<fraction_t>& temp, const number_t n)
    {
        assert(n >= 0);
        temp.clear();
        temp.reserve(n + 1);

        for (number_t m{}; m <= n; ++m)
        {
            temp.emplace_back(1, m + 1);
            for (number_t i = m; i > 0; --i)
                temp[i - 1] = (temp[i - 1] - temp[i]) * fraction_t(i);
        }

        const auto r = n == 1 ? -temp[0] : temp[0];
        return r;
    }
}