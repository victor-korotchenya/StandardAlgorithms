#pragma once
#include"../Numbers/to_unsigned.h"
#include"../Utilities/is_debug.h"
#include"binary_tree_utilities.h"
#include<compare>
#include<cstddef>
#include<cstdint>

namespace Standard::Algorithms::Heaps
{
    //              1 root
    //        2                  3
    //   4      5          6        7
    // 8 9 10 11   12 13 14 15
    struct one_based_index_t final
    {
        inline constexpr explicit one_based_index_t(std::size_t zero_based = 0U)
            : Zero_based(zero_based)
        {
        }

        [[nodiscard]] inline constexpr auto zero_based() const noexcept -> std::size_t
        {
            return Zero_based;
        }

        [[nodiscard]] inline constexpr auto one_based() const noexcept -> std::size_t
        {
            return Zero_based + 1ZU;
        }

        inline constexpr auto operator++ () &noexcept -> one_based_index_t &
        {
            ++Zero_based;
            return *this;
        }

        [[nodiscard]] inline constexpr auto operator<=> (const one_based_index_t &) const noexcept = default;

private:
        std::size_t Zero_based;
    };

    [[nodiscard]] inline constexpr auto operator+ (
        const one_based_index_t &first, const one_based_index_t &second) noexcept -> one_based_index_t
    {
        return one_based_index_t(first.zero_based() + second.zero_based());
    }

    // Given 6 or 7, return 3.
    [[nodiscard]] inline constexpr auto parent(const one_based_index_t &child) noexcept -> one_based_index_t
    {
        const auto index = child.one_based();
        assert(0U < index);

        auto result = index >> 1U;

        return one_based_index_t{ 0U < result ? result - 1U : 0U };
    }

    // Given 6, return 12.
    [[nodiscard]] inline constexpr auto left_child(const one_based_index_t &par) noexcept -> one_based_index_t
    {
        const auto index = par.one_based();
        assert(0U < index);

        auto result = index << 1U;

        return one_based_index_t{ result - 1U };
    }

    // The 1-based input must be 2 or greater.
    // Or, equivalently, the 0-based input must be between 1 and ((max of std::size_t) - 1U).
    // todo(p4): Check for max.
    // Given 14, return 10; from 10 follows 14.
    // In binary:
    // 1110 <-> 1010
    // 101 <-> 111
    // 100 <-> 110
    // 10 <-> 11
    [[nodiscard]] inline constexpr auto symmetric_index(const one_based_index_t &value) noexcept(false)
        -> one_based_index_t
    {
        const auto index = require_positive(value.zero_based(), "symmetric index");
        assert(index != std::numeric_limits<std::size_t>::max());

        const auto highest_bit = Standard::Algorithms::Trees::binary_tree_utilities<>::level(index);

        assert(0 < highest_bit);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            require_positive(highest_bit, "highest bit");
        }

        const auto second_highest = ::Standard::Algorithms::Numbers::to_unsigned(highest_bit - 1);

        constexpr std::size_t one = 1;

        auto result = (index + one) ^ (one << second_highest);
        assert(one < result);

        return one_based_index_t{ result - one };
    }

    // Whether the highest two bits are 11 in binary.
    // Return false for 2, true for 3.
    //   1
    // [10]; [11]
    // [100..101];  [110..111]
    // [1000..1011];  [1100..1111]
    // [10000..10111];  [11000..11111]
    //[[nodiscard]] auto is_right(one_based_index_t value) noexcept -> bool;
    //
    // It can be implemented as: (symmetric_index(value) < value).
    //
    //
    //[[nodiscard]] constexpr auto is_right(const one_based_index_t &value) noexcept -> bool
    //{
    //    const auto index = value.one_based();
    //    assert(0U < index);
    //
    //    constexpr auto max_bit = static_cast<std::int32_t>(sizeof(std::size_t) * 8);
    //
    //    const auto shift = max_bit - std::bit_width(index);
    //    const auto lead_ones = std::countl_one(index << shift);
    //    return 2 <= lead_ones;
    //}
} // namespace Standard::Algorithms::Heaps
