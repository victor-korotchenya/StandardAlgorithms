#pragma once
#include<algorithm>
#include<bit>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<stdexcept>
#include<string>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<class item_t, std::unsigned_integral mask_t>
    requires(std::endian::native == std::endian::little)
    [[nodiscard]] constexpr auto longest_alt_sub_slow(const std::vector<item_t> &items, mask_t mask) noexcept -> mask_t
    {
        assert(mask_t{} < mask);

        constexpr mask_t one = 1U;

        std::int32_t prev_index{};
        {
            const auto prev_mask = mask & -mask;
            const auto left_mask = mask ^ prev_mask;

            assert(0U < prev_mask && prev_mask <= mask && left_mask < mask);

            prev_index = static_cast<std::int32_t>(std::countr_zero(prev_mask));
            assert(prev_mask == (one << prev_index));

            mask = left_mask;
        }

        if (0U == mask)
        {// Was 1 item in the initial mask.
            return {};
        }

        auto cand = one;
        auto dir = 0;

        do
        {
            const auto prev_mask = mask & -mask;
            const auto cur_index = static_cast<std::int32_t>(std::countr_zero(prev_mask));

            assert(0U < prev_mask && prev_mask <= mask && prev_mask == (one << cur_index));

            assert(prev_index < cur_index);

            const auto &old = items.at(prev_index);
            const auto &curr = items.at(cur_index);

            if (old < curr)
            {
                if (0 < dir)
                {
                    break;
                }

                dir = 1;
            }
            else if (curr < old)
            {
                if (dir < 0)
                {
                    break;
                }

                dir = -1;
            }
            else
            {
                assert(curr == old);
                break;
            }

            assert(cand < items.size());
            ++cand;

            mask ^= prev_mask;
            prev_index = cur_index;
        } while (0U < mask);

        assert(0U < cand);

        return cand;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given an array, find the longest alternative subsequence where:
    // - either the items first increase, then decrease, again increase, and so on e.g 5,7,2,4;
    // - or decrease, increase, decrease, increase, decrease, .. like 9,3,4,3,50.
    // No two adjacent integers can be the same.
    // Time O(n), space O(1).
    template<class iter_t>
    [[nodiscard]] constexpr auto longest_alternative_subsequence(iter_t start, const iter_t stop) noexcept
        -> std::size_t
    {
        if (start == stop)
        {
            return {};
        }

        std::size_t max_lenght = 1;
        auto sign1 = 0;

        for (;;)
        {
            const auto prev = start;

            if (++start == stop)
            {// todo(p4): restore the selected indexes.
                return max_lenght;
            }

            if (*prev == *start)
            {
                continue;
            }

            const auto sign2 = *prev < *start ? 1 : -1;
            if (sign1 == sign2)
            {
                continue;
            }

            sign1 = sign2;
            ++max_lenght;
        }
    }

    // Time O(n * 2**n).
    template<class item_t, std::unsigned_integral mask_t = std::uint32_t>
    [[nodiscard]] constexpr auto longest_alternative_subsequence_slow(const std::vector<item_t> &items) -> std::size_t
    {
        const auto size = items.size();

        if (0U == size)
        {
            return {};
        }

        if (constexpr auto max_size = 20U; max_size < size) [[unlikely]]
        {
            auto err = "Too large size " + std::to_string(size);

            throw std::runtime_error(err);
        }

        constexpr mask_t one = 1U;

        auto max_lenght = one;
        auto mask = static_cast<mask_t>(one << size);

        while (0U < --mask)
        {
            const auto cand = Inner::longest_alt_sub_slow<item_t, mask_t>(items, mask);

            max_lenght = std::max(max_lenght, cand);
        }

        return max_lenght;
    }
} // namespace Standard::Algorithms::Numbers
