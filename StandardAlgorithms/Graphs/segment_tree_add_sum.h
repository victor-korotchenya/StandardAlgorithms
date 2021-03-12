#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Trees
{
    // Segment tree with lazy propagation.
    //
    // sum on range, add to range.
    template<class value_t = std::int64_t>
    struct segment_tree_add_sum final
    {
        static constexpr auto default_size_log = 18U;
        static constexpr auto max_log = 30U;

        constexpr explicit segment_tree_add_sum(std::uint32_t size_log = default_size_log)
            : Size_log(check_size_log(size_log))
            , Debt(1LLU << size_log)
            , Data(2LLU << size_log)
        {
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            assert(0U < Size_log && 0U < Debt.size() && Debt.size() * 2LLU == Data.size());

            return Debt.size();
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<value_t> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto data() &noexcept -> std::vector<value_t> &
        {
            return Data;
        }

        constexpr void clear()
        {
            Debt.assign(Debt.size(), {});
            Data.assign(Data.size(), {});
        }

        // void init(const std::vector<value_t>&vals)
        constexpr void init()
        {
            // assert(vals.size() == size());
            // std::copy(vals.cbegin(), vals.cend(), Data.begin() + size());

            auto parent = size();
            assert(0U < parent);

            while (--parent != 0U)
            {
                Data[parent] = Data[parent << 1U] + Data[(parent << 1U) | 1U];
            }
        }

        constexpr void add(const value_t &val,
            // todo(p3): const std::pair<std::size_t, std::size_t> &range)
            std::size_t left, std::size_t rex)
        {
            assert(left < rex && rex <= size());

            left += size();
            rex += size();

            const auto lin = left;
            const auto rin = rex - 1U;

            push_down(left);

            if (left != rin)
            {
                push_down(rin);
            }

            value_t powered = 1;

            while (left < rex)
            {
                assert(value_t{} != powered); // no overflow.

                if ((left & 1U) != 0U)
                {
                    add_mult(val, left++, powered);
                }

                if ((rex & 1U) != 0U)
                {
                    add_mult(val, rex - 1U, powered);
                }

                left >>= 1U;
                rex >>= 1U;
                powered <<= 1U;
            }

            push_up(lin);

            if (lin != rin)
            {
                push_up(rin);
            }
        }

        [[nodiscard]] constexpr auto sum(std::size_t left, std::size_t rex) -> value_t
        {
            assert(left < rex && rex <= size());

            left += size();
            rex += size();

            push_down(left);

            if (const auto rin = rex - 1U; left != rin)
            {
                push_down(rin);
            }

            value_t res{};

            while (left < rex)
            {
                if ((left & 1U) != 0U)
                {
                    res += Data[left++];
                }

                if ((rex & 1U) != 0U)
                {
                    res += Data[rex - 1U];
                }

                left >>= 1U;
                rex >>= 1U;
            }

            return res;
        }

private:
        static constexpr auto check_size_log(const std::uint32_t size_log) -> std::uint32_t
        {
            const auto *const name = "size log";

            {
                constexpr auto edge = 1U;
                require_greater(size_log, edge, name);
            }

            require_greater(max_log + 1U, size_log, name);

            return size_log;
        }

        constexpr void add_mult(const value_t &debt1, const std::size_t index, const value_t &powered)
        {
            Data[index] += static_cast<value_t>(debt1 * powered);

            if (index < size())
            {
                Debt[index] += debt1;
            }
        }

        constexpr void push_down(std::size_t left)
        {
            assert(size() <= left && left < size() * 2LLU);
            assert(0U < Size_log && Size_log <= max_log);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                require_positive(Size_log, "Size log");
                require_greater(max_log + 1U, Size_log, "Size log");
            }

            constexpr value_t one = 1;

            auto powered = static_cast<value_t>(one << (Size_log - 1U));
            auto height = Size_log;

            do
            {
                assert(0U < height);
                assert(value_t{} != powered); // no overflow.

                const auto parent = left >> height;

                if (auto &debt1 = Debt[parent]; debt1 != value_t{})
                {
                    add_mult(debt1, parent << 1U, powered);
                    add_mult(debt1, (parent << 1U) | 1U, powered);
                    debt1 = {};
                }

                powered >>= 1U;
            } while (--height != 0U);
        }

        constexpr void push_up(std::size_t parent)
        {
            value_t powered = 2;

            for (;;)
            {
                parent >>= 1U;

                if (parent == 0U)
                {
                    break;
                }

                assert(value_t{} != powered); // no overflow.

                const auto new_value = Debt[parent] * powered + Data[parent << 1U] + Data[(parent << 1U) | 1U];

                Data[parent] = static_cast<value_t>(new_value);

                powered <<= 1U;
            }
        }

        std::uint32_t Size_log;
        std::vector<value_t> Debt;
        std::vector<value_t> Data;
    };
} // namespace Standard::Algorithms::Trees
