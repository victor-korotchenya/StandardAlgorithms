#pragma once

#include <cassert>
#include <utility>
#include <vector>
#include <random>
#include "dutch_flag_split.h"

namespace
{
    template <class value_t>
    void random_to_rightmost(std::random_device& rd,
        std::vector<value_t>& ar,
        const size_t left, const size_t right)
    {
        assert(left <= right && right < ar.size());
        const auto num = static_cast<size_t>(rd());
        const auto d = right - left + 1u;
        assert(d > 0u);

        const auto index = num % d + left;
        std::swap(ar[index], ar[right]);
    }

    // Return the inclusive range of values equal to a random item in [left, right].
    template <class value_t>
    std::pair<size_t, size_t> eq_incl(std::random_device& rd,
        std::vector<value_t>& ar,
        const size_t left, const size_t right)
    {
        assert(left <= right && right < ar.size());

        random_to_rightmost<value_t>(rd, ar, left, right);
        const auto& pivot = ar[right];

        const auto p = Privet::Algorithms::Numbers::dutch_flag_split_into3_less_equal_greater_3way_partition(
            ar.begin() + left, ar.begin() + right, pivot);

        std::swap(*p.second, ar[right]);
        const auto r = std::make_pair<size_t, size_t>(p.first - ar.begin(), p.second - ar.begin());
        assert(left <= r.first && r.first <= r.second && r.second <= right);
        return r;
    }
}

namespace Privet::Algorithms::Numbers
{
    // Values are not unique.
    template <typename value_t>
    size_t quick_select_kth_smallest(std::vector<value_t>& ar,
        // Starts from 0.
        size_t rank)
    {
        assert(ar.size() && rank < ar.size());

        std::random_device rd;
        size_t left = 0, right = ar.size() - 1u;
        for (;;)
        {
            assert(ar.size() && rank <= right);

            const auto par = eq_incl<value_t>(rd, ar, left, right);
#ifdef _DEBUG
            for (auto k = left; k < par.first; ++k)
                assert(ar[k] < ar[par.first]);

            for (auto k = par.first; k <= par.second; ++k)
                assert(ar[par.first] == ar[k]);

            for (auto k = par.second + 1; k <= right; ++k)
                assert(ar[par.first] < ar[k]);
#endif
            if (par.first - left > rank)
                right = par.first - 1u;
            else if (par.second - left < rank)
            {
                const auto del = par.second + 1u - left;
                assert(del <= rank);
                rank -= del,
                    left = par.second + 1u;
            }
            else
                return par.first;
        }
    }
}