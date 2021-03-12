#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <stdexcept>
#include <vector>
#include<bits/stdc++.h>//__lg

namespace Privet::Algorithms::Trees
{
    // Given an array A, find the min value index in A[start..stop).
    template <class value_t, int log_n_max, int n_max = 1 << log_n_max>
    struct sparse_table_min final
    {
        static_assert(log_n_max >= 2 && log_n_max <= 29 && n_max <= (1 << log_n_max) && n_max >= 2);

        std::array<value_t, n_max> data;

        // Fill in the 'data' array before.
        // Time O(n*log(n)).
        void init(const int n)
        {
            assert(n > 0 && n <= n_max);

            this->n = n;
            std::iota(indexes[0].begin(), indexes[0].begin() + n, 0);

            for (auto lg = 1; (1 << lg) <= n; ++lg)
            {
                const auto length = 1 << (lg - 1),
                    up = n - (1 << lg);
                const auto& prev = indexes[lg - 1ll];
                auto& cur = indexes[lg];

                for (auto i = 0; i <= up; ++i)
                {
                    const auto& p = prev[i],
                        p2 = prev[static_cast<size_t>(i) + length];
                    cur[i] = data[p2] < data[p] ? p2 : p;
                }
            }
        }

        // Return index, not value.
        // Time O(1).
        const value_t& range_minimum_query_index(const int from, const int to) const
        {
            assert(from >= 0 && from < to&& to <= n && n > 0);

            const int fl = __lg(static_cast<int64_t>(to) - from);
            const auto& inds = indexes[fl];
            const auto from2 = to - (1 << fl);
            const auto& ip = inds[from],
                & ip2 = inds[from2];

            return data[ip2] < data[ip] ? ip2 : ip;
        }

    private:
        std::array<std::array<value_t, n_max>, log_n_max> indexes;
        int n = {};
    };
}