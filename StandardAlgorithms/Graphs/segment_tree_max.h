#pragma once
#include"../Utilities/require_utilities.h"
#include<array>
#include<vector>

namespace Standard::Algorithms::Trees
{
    // Segment tree with lazy propagation.
    //
    // max on range, add to range.
    template<class value_t, std::uint32_t size_log>
    struct segment_tree_max final
    {
        static constexpr auto min_log = 2U;
        static constexpr auto max_log = 29U;
        static_assert(min_log <= size_log && size_log <= max_log);

        static constexpr auto capacity = 1U << size_log;
        static constexpr auto capacity_2 = capacity << 1U;

        static_assert(0U < capacity && 0U < capacity_2 && capacity_2 == capacity * 2LLU);

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

        // todo(p3): Warning: not tested!
        constexpr void init(const std::vector<value_t> &source, bool shall_clear = true)
        {
            const auto *const name = "source size";

            const auto source_size = require_positive(source.size(), name);
            require_less_equal(source_size, capacity, name);

            if (shall_clear)
            {
                clear();
            }

            std::copy(source.cbegin(), source.cend(), Data.begin() + capacity);

            auto parent = source_size + capacity;

            while (0U < --parent)
            {
                Data[parent] = std::max(Data[parent << 1U], Data[(parent << 1U) | 1U]);
            }
        }

        constexpr void add(const value_t &val,
            // todo(p3): const std::pair<std::size_t, std::size_t> &range)
            std::size_t left, std::size_t rex)
        {
            assert(left < rex && rex <= capacity);

            left += capacity;
            rex += capacity;

            const auto lin = left;
            const auto rin = rex - 1U;

            while (left < rex)
            {
                if ((left & 1U) != 0U)
                {
                    add1(left++, val);
                }

                if ((rex & 1U) != 0U)
                {
                    add1(rex - 1U, val);
                }

                left >>= 1U;
                rex >>= 1U;
            }

            push_up(lin);

            if (lin != rin)
            {
                push_up(rin);
            }
        }

        [[nodiscard]] constexpr auto max(std::size_t left, std::size_t rex) -> value_t
        {
            assert(left < rex && rex <= capacity);

            left += capacity;
            rex += capacity;

            push_down(left);

            if (const auto rin = rex - 1U; left != rin)
            {
                push_down(rin);
            }

            auto res = std::numeric_limits<value_t>::min();

            while (left < rex)
            {
                if ((left & 1U) != 0U)
                {
                    res = std::max(res, Data[left++]);
                }

                if ((rex & 1U) != 0U)
                {
                    res = std::max(Data[rex - 1U], res);
                }

                left >>= 1U;
                rex >>= 1U;
            }

            return res;
        }

private:
        constexpr void add1(const std::size_t index, const value_t &value)
        {
            Data[index] += value;

            if (index < capacity)
            {
                Debt[index] += value;
            }
        }

        constexpr void push_down(const std::size_t left)
        {
            assert(capacity <= left && left < capacity_2);

            auto height = size_log;

            do
            {
                assert(0U < height);

                const auto parent = left >> height;

                if (auto &debt1 = Debt[parent]; debt1 != value_t{})
                {
                    add1(parent << 1U, debt1);
                    add1((parent << 1U) | 1U, debt1);
                    debt1 = {};
                }
            } while (--height != 0U);
        }

        constexpr void push_up(std::size_t parent)
        {
            while (1U < parent)
            {
                parent >>= 1U;

                const auto &left1 = Data[parent << 1U];
                const auto &right1 = Data[(parent << 1U) | 1U];

                const auto new_value = Debt[parent] + std::max(left1, right1);

                Data[parent] = static_cast<value_t>(new_value);
            }
        }

        std::array<value_t, capacity> Debt{};
        std::array<value_t, capacity_2> Data{};
    };
} // namespace Standard::Algorithms::Trees
