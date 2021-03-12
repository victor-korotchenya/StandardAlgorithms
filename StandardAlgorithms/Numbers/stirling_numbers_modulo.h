#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Stirling numbers of the 1st kind s(n, k) count
    // permutations of length n having k cycles.
    //
    // 11 permutations of n=4, having k=2 cycles:
    // 8 orbits of sizes 1 and 3:
    //   {1, 234}, {1,243},
    //   {2, 134}, {2,143},
    //   {3, 124}, {3,142},
    //   {4, 123}, {4,132};
    // 3 orbits of size 2:
    //   {12,34}, {13,24}, {14,23}.
    //
    // [4, 2] = s(4, 2) = 11:
    // rising_factorial(x, 4) = x(x + 1)(x + 2)(x + 3) =
    // = (x*x + x)(x*x + 5*x + 6) = (x**4 + 5*x*x*x + 6*x*x) + (x*x*x + 5*x*x + 6*x) =
    // = 0 + 6*x + 11*x**2 + 6*x**3 + 1*x**4.
    // The x**2 coefficient is 11.
    template<std::int32_t modulo>
    requires(1 < modulo)
    [[nodiscard]] constexpr auto stirling_numbers_1_permut_cycle_cnt_modulo(const std::int32_t size)
        -> std::vector<std::vector<std::int32_t>>
    {
        using int_t = std::int32_t;
        using long_int_t = std::int64_t;

        constexpr int_t zero{};
        constexpr int_t one = 1;

        require_between(one, size, modulo - one, "size");

        std::vector<std::vector<int_t>> stirs(size + one);
        stirs[zero] = { one, zero };

        for (auto len = one; len <= size; ++len)
        {
            const auto &prevs = stirs[len - one];
            auto &currs = stirs[len];
            currs.resize(len + 2LL);

            const auto coef = static_cast<long_int_t>(len - one);

            for (auto kkk = one; kkk <= len; ++kkk)
            {
                const auto pronto = coef * prevs[kkk] + prevs[kkk - one];

                currs[kkk] = static_cast<int_t>(pronto % modulo);
            }
        }

        return stirs;
    }

    // Stirling numbers of the 2nd kind S(n, k) are
    // ways to partition a set of n labeled objects
    // into k non-empty unlabeled subsets.
    //
    // Q1. Count ways to split 10 different men into 3 non-empty indistinguishable groups?
    // Answer: S(10, 3) = 9330.
    //
    // Q2. Count ways to assign 10 integers [1..10] to 5 distinct women,
    // each gets at least 1 number, numbers not shared?
    // Answer: S(10, 5) * 5!= 42,525 * 120 = 5,103,000.
    template<std::int32_t modulo>
    requires(1 < modulo)
    [[nodiscard]] constexpr auto stirling_numbers_2_partition_modulo(const std::int32_t size)
        -> std::vector<std::vector<std::int32_t>>
    {
        using int_t = std::int32_t;
        using long_int_t = std::int64_t;

        constexpr int_t zero{};
        constexpr int_t one = 1;

        require_between(one, size, modulo - one, "size");

        std::vector<std::vector<int_t>> parts(size + one);
        parts[zero] = { one, zero };

        for (auto len = one; len <= size; ++len)
        {
            const auto &prevs = parts[len - one];
            auto &currs = parts[len];
            currs.resize(len + 2LL);

            for (long_int_t kkk = one; kkk <= len; ++kkk)
            {
                const auto brutta = kkk * prevs[kkk] + prevs[kkk - one];

                currs[kkk] = static_cast<int_t>(brutta % modulo);
            }
        }

        return parts;
    }
} // namespace Standard::Algorithms::Numbers
