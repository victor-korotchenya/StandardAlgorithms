#pragma once
#include"../Utilities/project_constants.h"
#include"factoring_utilities.h"
#include"number_utilities.h" // mobius
#include<cstdlib> // std::exit
#include<iostream>

namespace Standard::Algorithms::Numbers
{
    constexpr auto narendra = false;
} // namespace Standard::Algorithms::Numbers

namespace Standard::Algorithms::Numbers::Inner
{
    // 00 -> false
    // 01 -> true
    // 10 is not minimum rotation, but has no period -> true.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto is_aperiodic(
        const std::vector<bool> &str, const std::vector<int_t> &factors) noexcept(!narendra) -> bool
    {
        const auto size = static_cast<int_t>(str.size());

        assert(int_t{} < size && !factors.empty() && factors.back() == size);

        for (const auto &factor : factors)
        {
            if (size == factor)
            {
                continue;
            }

            assert(int_t{} < factor && factor < size && int_t{} == size % factor);

            constexpr int_t one = 1;

            const auto total_chunks = static_cast<int_t>(size / factor);

            assert(one < total_chunks && total_chunks * factor == size);

            auto has_period = true;

            for (int_t chunk = one, chunk_factor = factor; has_period && chunk < total_chunks;
                 ++chunk, chunk_factor += factor)
            {
                assert(int_t{} < chunk_factor && chunk_factor < size);

                for (int_t pos{}; has_period && pos < factor; ++pos)
                {
                    const auto pos_2 = chunk_factor + pos;
                    assert(chunk_factor <= pos_2 && pos_2 < size);

                    has_period = str[pos] == str.at(pos_2);
                }
            }

            if (has_period)
            {
                return false;
            }
        }

        if constexpr (narendra)
        {
            if (!str.empty() && !str[0] && str.back()) // Cut off some non-LSWs.
            {
                auto output = std::accumulate(str.cbegin(), str.cend(), std::string{},
                    [] [[nodiscard]] (std::string & tema, const auto &bit) -> std::string &
                    {
                        tema += bit ? '1' : '0';
                        return tema;
                    });

                output += "\n";

                std::cout << output;
            }
        }

        return true;
    }

    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto enumerate_boolean_strings(const std::vector<int_t> &factors, std::vector<bool> &str,
        // Better no reference here in a recursive function.
        // todo(p4): no recursion, for i in [0 .. 2**n).
        const int_t ind) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        if (ind < zero)
        {
            // There will be a minimum in an aperiodic sequence among its num rotations.
            auto isa = is_aperiodic<int_t>(str, factors);
            return isa ? one : zero;
        }

        assert(ind < static_cast<int_t>(str.size()));

        str[ind] = true;

        auto result = enumerate_boolean_strings<int_t>(factors, str, static_cast<int_t>(ind - one));

        str[ind] = false;

        result += enumerate_boolean_strings<int_t>(factors, str, static_cast<int_t>(ind - one));

        assert(zero <= result);

        return result;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // How many binary sequences (or LS words) of length n>0, ignoring cyclic permutations, have period exactly n?
    // Lyndon-Shirshov word is a binary sequence that is the lexicographically smallest one among its rotations.
    // E.g. "011" can be rotated to create {"101", "110"} and is distinct and the smallest among others - it is LSW.
    // "00", rotated once, is "00" and is not smaller than its rotation (it is equal and thus periodic) - not LSW.
    // "10", rotated once, is "01" and is greater than its rotation, not smaller - it is not LSW.
    // 00101 shifted: 01010, 10100, 01001, 10010 - it is LSW. But 01010 isn't because its rotation 00101 is smaller.
    // 01011 shifted: 10110, 01101, 11010, 10101 - it is LSW.
    //
    // 2 LSWs of size 1: 0, 1.
    // 1 of size 2: 01. Note that 00, 10, 11 are not LSW.
    // 2 of Size 3: 001, 011. Note that 000, 010, 100, 101, 110, 111 are not LSW.
    // 3 of Size 4: 0001, 0011, 0111.
    // 6 of Size 5: 00001, 00011, 00101, 00111, 01011, 01111.
    // https://matheducators.stackexchange.com/questions/2714/understandable-interesting-uses-of-m%C3%B6bius-inversion
    //
    // Let ans(n) be the answer for length n.
    // A period of n can only be a divisor of n. E.g. 5 cannot be a period of 6.
    // 001001, n=6, has 2 rotations: 010010, 100100. 001 is LSW. So, 001001 adds 3 strings, id est the period 3; 3 | 6.
    //
    // There are 2**n binary words of length n.
    // Also, these words can be classified in another way:
    // - for each divisor of n, sum the product of the divisor times ans(divisor). Here the divisor is the period.
    // 2**n ==== sum(div * ans(div), div divides n).
    // Using Moebius inversion,
    // ans(n) = 1/n * sum(u(div) * 2**(n/div), div divides n)
    // where u(x) is Mobius function, returning -1, 0, or 1.
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto bin_sequence_period(const int_t &num) -> int_t
    {
        {
            const auto *const name = "int_t";
            require_positive<int_t>(num, name);
        }

        std::vector<int_t> factors;
        all_divisors<int_t>(num, factors);

        const auto mobs = mobius(num);
        int_t result{};

        for (const auto &factor : factors)
        {
            assert(int_t{} < factor && static_cast<std::size_t>(factor) < mobs.size());

            constexpr int_t one = 1;

            auto add = mobs[factor] * (one << (num / factor));
            result += static_cast<int_t>(add);
        }

        assert(int_t{} < result && int_t{} == result % num);

        result /= num;

        return result;
    }

    // Slow time O(2**num * num).
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto bin_sequence_period_slow(int_t num) -> int_t
    {
        if constexpr (narendra)
        {
            // NOLINTNEXTLINE
            num = 5;
        }

        {
            const auto *const name = "int_t";
            require_positive<int_t>(num, name);

            constexpr auto max_num = Standard::Algorithms::Utilities::graph_max_nodes<int_t>;
            constexpr int_t small = 20; // 2**small steps.
            constexpr auto mini = std::min(small, max_num);

            require_less_equal(num, mini, name);
        }

        std::vector<int_t> factors;
        all_divisors<int_t>(num, factors);
        assert(!factors.empty());

        std::vector<bool> str(num); // todo(p4): use std::uint32_t instead.

        auto result = Inner::enumerate_boolean_strings<int_t>(factors, str, static_cast<int_t>(num - 1));

        assert(int_t{} < result && int_t{} == result % num);

        // 001, 010, 100 are counted "num" times, including non-smallest; must divide.
        result /= num;

        if constexpr (narendra)
        {
            // NOLINTNEXTLINE
            std::exit(0);
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
