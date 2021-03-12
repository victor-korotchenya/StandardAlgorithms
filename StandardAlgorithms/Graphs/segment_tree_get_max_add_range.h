#pragma once
#include"../Utilities/require_utilities.h"
#include<array>
#include<numeric> // std::midpoint
#include<vector>

namespace Standard::Algorithms::Trees
{
    template<class value_t, std::uint32_t size_log>
    struct segment_tree_get_max_add_range final
    {
        static constexpr auto min_log = 2U;
        static constexpr auto max_log = 29U;
        static_assert(min_log <= size_log && size_log <= max_log);

        static constexpr auto capacity = 1U << size_log;
        static constexpr auto capacity_2 = capacity << 1U;

        static_assert(0U < capacity && 0U < capacity_2 && capacity_2 == capacity * 2LLU);

        constexpr explicit segment_tree_get_max_add_range(const std::uint32_t size1)
            : Actual_size(require_less_equal(require_positive(size1, "size"), capacity, "size"))
        {
        }

        [[deprecated("Use capacity instead.")]] [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return capacity;
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::array<value_t, capacity_2> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto data() &noexcept -> std::array<value_t, capacity_2> &
        {
            return Data;
        }

        constexpr void clear()
        {
            Debt = {};
            Data = {};
        }

        constexpr void add(const value_t &val,
            // todo(p3): const std::pair<std::size_t, std::size_t> &range)
            const std::size_t left, const std::size_t right_incl)
        {
            assert(left <= right_incl && right_incl < Actual_size);

            add_impl({ val, { left, right_incl } }, Actual_size);
        }

        [[nodiscard]] constexpr auto max(const std::size_t left, const std::size_t right_incl) -> value_t
        {
            assert(left <= right_incl && right_incl < Actual_size);

            auto res = max_impl({ left, right_incl }, Actual_size);
            return res;
        }

private:
        using range_t = std::pair<std::size_t, std::size_t>;

        using value_range_incl_t = std::pair<value_t, range_t>;

        constexpr void add1(const std::size_t index, const value_t &val)
        {
            Data[index] += val;

            if (index < capacity)
            {
                Debt[index] += val;
            }
        }

        constexpr void push_down1(const std::size_t parent)
        {
            auto &debt1 = Debt[parent];

            add1(parent << 1U, debt1);
            add1((parent << 1U) | 1U, debt1);

            debt1 = {};
        }

        constexpr void add_impl(const value_range_incl_t &val_range, const std::size_t right,
            const std::size_t left = 0, const std::size_t parent = 1)
        {
            // [[assume(!(right < left))]];
            assert(!(right < left));

            const auto &val = val_range.first;
            const auto &qle = val_range.second.first;
            const auto &qri = val_range.second.second;

            if (qle <= left && right <= qri)
            {
                add1(parent, val);
                return;
            }

            push_down1(parent);

            const auto mid = std::midpoint(left, right);

            if (qle <= mid)
            {
                add_impl(val_range, mid, left, parent << 1U);
            }

            if (mid < qri)
            {
                add_impl(val_range, right, mid + 1U, (parent << 1U) | 1U);
            }

            Data[parent] = std::max(Data[parent << 1U], Data[(parent << 1U) | 1U]);
        }

        [[nodiscard]] constexpr auto max_impl(const range_t &range, const std::size_t right, const std::size_t left = 0,
            const std::size_t parent = 1) -> value_t
        {
            const auto &qle = range.first;
            const auto &qri = range.second;

            // [[assume(!(right < left))]];
            assert(qle <= qri && qri < Actual_size && left <= right && right <= Actual_size);

            if (qle <= left && right <= qri)
            {
                return Data[parent];
            }

            push_down1(parent);

            const auto mid = std::midpoint(left, right);

            auto left_max = qle <= mid ? max_impl(range, mid, left, parent << 1U) : std::numeric_limits<value_t>::min();

            auto right_max =
                mid < qri ? max_impl(range, right, mid + 1U, (parent << 1U) | 1U) : std::numeric_limits<value_t>::min();

            auto maxv = std::max(left_max, right_max);
            return maxv;
        }

        std::uint32_t Actual_size{};
        std::array<value_t, capacity> Debt{};
        std::array<value_t, capacity_2> Data{};
    };
} // namespace Standard::Algorithms::Trees
