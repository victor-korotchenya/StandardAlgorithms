#pragma once
#include<algorithm>
#include<bit>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<stdexcept>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // The FlawFinder finds a would be issue when the function is named "is_permutation" - rename it as a workaround.
    // (buffer) is_permutation: Function does not check the second iterator for over-read conditions (CWE-126).
    // This function is often discouraged by most C++ coding standards in favor of its safer alternatives provided since
    // C++14. Consider using a form of this function that checks the second iterator before potentially overflowing it.
    // Time O(n).
    template<class collection_t>
    [[nodiscard]] constexpr auto is_permutation1(const collection_t &permutation, std::vector<bool> &taken,
        const bool is_zero_start = false) noexcept(false) -> bool
    {
        if (permutation.empty())
        {
            return true;
        }

        taken.assign(permutation.size() + (is_zero_start ? 0U : 1U), false);
        taken.at(0) = !is_zero_start;

        for (const auto &item : permutation)
        {
            const auto index = static_cast<std::size_t>(item);

            if (!(index < permutation.size()) || taken[index])
            {
                return false;
            }

            taken[index] = true;
        }

        return true;
    }

    // Slightly less efficient version.
    template<class collection_t>
    [[nodiscard]] constexpr auto is_permutation_2(
        const collection_t &permutation, const bool is_zero_start = false) noexcept(false) -> bool
    {
        std::vector<bool> taken;

        return is_permutation1<collection_t>(permutation, taken, is_zero_start);
    }

    // Slow time O(n*log(n)).
    template<class collection_t, std::integral item_t>
    [[nodiscard]] constexpr auto is_permutation_slow(const collection_t &permutation, std::vector<item_t> &temp,
        const bool is_zero_start = false) noexcept(false) -> bool
    {
        temp.clear();
        temp.reserve(permutation.size());

        // Note. span has no member named cbegin
        std::copy(permutation.begin(), permutation.end(), std::back_inserter(temp));
        std::sort(temp.begin(), temp.end());

        auto expected = static_cast<item_t>(is_zero_start ? 0 : 1);

        for (const auto &item : temp)
        {
            if (item != expected)
            {
                return false;
            }

            ++expected;
        }

        return true;
    }

    // Slow time O(n*log(n)).
    template<class collection_t, std::integral item_t = typename collection_t::value_type>
    [[nodiscard]] constexpr auto is_permutation_slow2(
        const collection_t &permutation, const bool is_zero_start = false) noexcept(false) -> bool
    {
        std::vector<item_t> temp;

        return is_permutation_slow<collection_t, item_t>(permutation, temp, is_zero_start);
    }

    template<class collection_t, std::integral item_t = typename collection_t::value_type>
    constexpr void rev_permutation(
        const collection_t &perm, collection_t &rev, const bool is_zero_start = false) noexcept(false)
    {
        const auto size = perm.size();
        rev.resize(size + (is_zero_start ? 0U : 1U));

        if (!is_zero_start)
        {
            rev.at(0) = item_t{};
        }

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &item = perm[index];

            assert(item_t{} <= item);

            assert(is_zero_start ? (item < static_cast<item_t>(size)) : !(static_cast<item_t>(size) < item));

            rev.at(item) = static_cast<item_t>(index);
        }
    }

    // todo(p3): faster in O(n), no sortir.
    // Slow time O(n*log(n)).
    template<class collection_t, class collection_t2>
    [[nodiscard]] constexpr auto is_permutation_of_slow(const collection_t &one, const collection_t2 &two) noexcept(
        false) -> bool
    {
        if (one.size() != two.size())
        {
            return false;
        }

        const auto copy_sort = [] [[nodiscard]] (const auto &orig)
        {
            auto cop = orig;
            std::sort(cop.begin(), cop.end());

            return cop;
        };

        const auto ico = copy_sort(one);
        const auto ocop = copy_sort(two);
        auto are_eq = ico == ocop;
        return are_eq;
    }

    // (-1)**(length - 1). Even length, return -1; odd -> 1.
    [[nodiscard]] inline constexpr auto cycle_sign(const std::size_t cycle_length) noexcept -> std::int32_t
    {
        auto was_even = 0U == (cycle_length & 1U);
        return was_even ? -1 : 1;
    }

    // 0-based permutation e.g.
    // 012345
    // 254301
    // is split into 3 cycles: (024)(15)(3).
    template<std::integral int_t>
    constexpr void permutation_split_into_cycles(
        const auto &permutation, std::vector<bool> &buffer, std::vector<std::vector<int_t>> &cycles)
    {
        cycles.clear();

        const auto size = static_cast<std::size_t>(permutation.size());
        buffer.assign(size, false);

        for (std::size_t index{}; index < size; ++index)
        {
            if (buffer[index])
            {
                continue;
            }

            buffer[index] = true;

            auto &cycle = cycles.emplace_back();
            cycle.push_back(index);

            for (auto pre = index;;)
            {
                const auto &curr = permutation.at(pre);
                if (buffer.at(curr))
                {
                    break;
                }

                buffer.at(curr) = true;
                cycle.push_back(curr);

                pre = curr;
            }
        }
    }

    [[nodiscard]] constexpr auto permutation_sign(const auto &permutation) -> std::int32_t
    {
        std::vector<std::vector<std::size_t>> cycles;
        std::vector<bool> buffer;
        permutation_split_into_cycles(permutation, buffer, cycles);

        std::int32_t sign1 = 1;

        for (const auto &cycle : cycles)
        {
            const auto temp = cycle_sign(cycle.size());
            sign1 *= temp;
        }

        return sign1;
    }
} // namespace Standard::Algorithms::Numbers
