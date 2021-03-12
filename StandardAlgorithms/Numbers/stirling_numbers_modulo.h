#pragma once

#include <cassert>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Stirling numbers of 1st kind s(n, k) count
            // permutations of length n having k cycles.
            //
            // To find [4, 2] = s(4, 2) = 11:
            // rising_factorial(x, 4) = x(x + 1)(x + 2)(x + 3) = x**4 + 6*x**3 + 11*x**2 + 6*x.
            //
            // Permutations of n=4, having k=2 cycles:
            // 2 orbits each of size 2:
            //   {12,34}, {13, 24}, {14,23};
            // 1 orbit of 3, and 1 orbit of 1:
            //   {1, 234}, {1,243},
            //   {2, 134}, {2,143},
            //   {3, 124}, {3,142},
            //   {4, 123}, {4,132}.
            template<int mod>
            std::vector<std::vector<int>> stirling_numbers_1_permut_cycle_cnt_modulo(const int n)
            {
                static_assert(mod >= 2);
                RequirePositive(n, "n");
                assert(n < mod);

                std::vector<std::vector<int>> st(n + 1);
                st[0] = { 1, 0 };

                for (auto i = 1; i <= n; i++)
                {
                    const auto& prev = st[i - 1];
                    auto& cur = st[i];
                    cur.resize(i + 2);

                    for (auto k = 1; k <= i; k++)
                    {
                        const auto raw = static_cast<int64_t>(i - 1) * prev[k] + prev[k - 1];
                        cur[k] = static_cast<int>(raw % mod);
                    }
                }

                return st;
            }

            // Stirling numbers of 2nd kind S(n, k) are
            // ways to partition a set of n labelled objects
            // into k non-empty unlabelled subsets.
            //
            // Q1. Count ways to split 10 men into 3 non-empty groups?
            // Answer1: S(10, 3) = 9330.
            //
            // Q2. Count ways to assign 10 integers [1..10] to 5 men, each employee gets at least 1 number, numbers not shared?
            // Answer2: S(10, 5) * 5!= 42525 * 120 = 5,103,000.
            template<int mod>
            std::vector<std::vector<int>> stirling_numbers_2_partition_modulo(const int n)
            {
                static_assert(mod >= 2);
                RequirePositive(n, "n");
                assert(n < mod);

                std::vector<std::vector<int>> st(n + 1);
                st[0] = { 1, 0 };

                for (auto i = 1; i <= n; i++)
                {
                    const auto& prev = st[i - 1];
                    auto& cur = st[i];
                    cur.resize(i + 2);

                    for (auto k = 1; k <= i; k++)
                    {
                        const auto raw = static_cast<int64_t>(k) * prev[k] + prev[k - 1];
                        assert(raw < mod);
                        cur[k] = static_cast<int>(raw % mod);
                    }
                }

                return st;
            }
        }
    }
}