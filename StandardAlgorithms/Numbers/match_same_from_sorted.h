#pragma once
#include<algorithm>
#include<cassert>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // todo(p3): test?
    // eins, zweis, dreis are sorted.
    template<class a_t, class b_t, class c_t>
    constexpr void match_same_from_sorted(const std::vector<a_t> &eins, const std::vector<a_t> &zweis,
        const std::vector<a_t> &dreis, std::vector<a_t> &same_values)
    {
        assert(std::is_sorted(eins.cbegin(), eins.cend()));
        assert(std::is_sorted(zweis.cbegin(), zweis.cend()));
        assert(std::is_sorted(dreis.cbegin(), dreis.cend()));

        same_values.clear();

        auto beg_ein = eins.cbegin();
        auto beg_zwe = zweis.cbegin();
        auto beg_dre = dreis.cbegin();

        while (beg_ein != eins.cend() && beg_zwe != zweis.cend() && beg_dre != dreis.cend())
        {
            const auto &max_val = std::max({ *beg_ein, *beg_zwe, *beg_dre });

            if (*beg_ein < max_val)
            {
                ++beg_ein;
            }
            else if (*beg_zwe < max_val)
            {
                ++beg_zwe;
            }
            else if (*beg_dre < max_val)
            {
                ++beg_dre;
            }
            else
            {
                const auto &item = *beg_ein;

                assert(item == *beg_zwe && item == *beg_dre && *beg_zwe == *beg_dre);

                same_values.push_back(item);

                ++beg_ein, ++beg_zwe, ++beg_dre;
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
