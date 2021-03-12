#pragma once
#include"../Numbers/bit_utilities.h"
#include"../Numbers/shift.h"
#include"../Numbers/xor_1.h"
#include<array>
#include<cassert>
#include<cstddef>
#include<cstdint>
#include<cstring>
#include<limits>

namespace Standard::Algorithms::Trees
{
    // Given bool[n]:
    // - get sum over [b, e).
    // - invert [b, e) i.e. false => true and vice versa.
    // Indexes are 0 based.
    template<std::int32_t logsi>
    requires(0 < logsi)
    struct segm_tree_sum_invert final
    {
        using bit_t = std::uint8_t;
        static_assert(sizeof(bit_t) == 1);

        static constexpr bit_t zero{};
        static constexpr bit_t one{ 1 };

        static constexpr auto max_log = 30;
        static_assert(logsi <= max_log);

        static constexpr auto size = ::Standard::Algorithms::Numbers::shift_left(static_cast<std::int32_t>(1), logsi);
        static constexpr auto size_2 = size * 2;

        static_assert(0 < size && size < size_2 && size_2 < std::numeric_limits<std::int32_t>::max());

        constexpr void clear() noexcept
        {
            std::fill(sums.begin(), sums.end(), 0);
            std::fill(ups.begin(), ups.end(), zero);
        }

        // The "build" must be called afterwards.
        constexpr void set1(const std::int32_t pos,
            const bool flag) // todo(p3): set 32 .. 512 bits at once.
        {
            assert(check_range(pos, size));

            sums.at(pos + size) = flag ? 1 : 0;
        }

        constexpr void build() noexcept
        {
            for (auto ind = size - 1; 0 < ind; --ind)
            {
                const auto pari = ::Standard::Algorithms::Numbers::to_unsigned(ind) << 1U;

                sums[ind] = sums[pari] + sums[pari | 1U];
            }
        }

        [[nodiscard]] constexpr auto count_in_range(std::int32_t left, std::int32_t ri_exclusive) -> std::int32_t
        {
            assert(check_range(left, ri_exclusive));

            left += size;
            ri_exclusive += size;

            puto(left);
            puto(ri_exclusive - 1);

            std::int32_t res{};

            while (left < ri_exclusive)
            {
                if (::Standard::Algorithms::Numbers::has_zero_bit(left))
                {
                    res += sums[left++];
                }

                if (::Standard::Algorithms::Numbers::has_zero_bit(ri_exclusive))
                {
                    res += sums[ri_exclusive - 1];
                }

                left /= 2;
                ri_exclusive /= 2;
            }

            return res;
        }

        constexpr void invert_in_range(std::int32_t left, std::int32_t ri_exclusive)
        {
            assert(check_range(left, ri_exclusive));

            left += size;
            ri_exclusive += size;

            const auto old_left = left;
            const auto old_ri_inc = ri_exclusive - 1;

            while (left < ri_exclusive)
            {
                if (::Standard::Algorithms::Numbers::has_zero_bit(left))
                {
                    ups[left++] ^= one;
                }

                if (::Standard::Algorithms::Numbers::has_zero_bit(ri_exclusive))
                {
                    ups[ri_exclusive - 1] ^= one;
                }

                left /= 2;
                ri_exclusive /= 2;
            }

            puto(old_left);
            puto(old_ri_inc);
        }

private:
        [[nodiscard]] static constexpr auto check_range(
            const std::int32_t left, const std::int32_t ri_exclusive) noexcept -> bool
        {
            return !(left < 0) && left < ri_exclusive && ri_exclusive <= size;
        }

        [[nodiscard]] static constexpr auto calc_divs(const std::int32_t index, std::array<std::int32_t, logsi> &vals)
            -> std::int32_t
        {
            assert(0 < index);

            std::int32_t cnt{};

            {
                auto ind2 = index;

                while (0 < (ind2 /= 2))
                {
                    vals[cnt++] = ind2;
                }
            }

            assert(0 < cnt && cnt <= logsi);
            assert(index / 2 == vals[0]);
            assert(1 == vals[cnt - 1]);

            return cnt;
        }

        [[nodiscard]] constexpr auto top_down(
            const std::int32_t index, const std::array<std::int32_t, logsi> &vals, const std::int32_t cnt) -> bool
        {
            auto any = false;

            for (std::int32_t ind = cnt - 1, tree_size = size; 0 <= ind; --ind, tree_size /= 2)
            {// from 1 to (index / 2)
                const auto &val = vals[ind];
                assert(0 < val && val < size);

                const auto xorred = ::Standard::Algorithms::Numbers::xor_1(val);

                for (const auto &pos : { val, xorred })
                {
                    {
                        auto &parent_pos = ups[pos];
                        if (parent_pos == zero)
                        {
                            continue;
                        }

                        parent_pos = zero;
                    }
                    {
                        auto &sup = sums[pos];
                        assert(0 <= sup && sup <= tree_size);

                        sup = tree_size - sup;
                        assert(0 <= sup && sup <= tree_size);
                    }

                    const auto left = pos << 1U;
                    const auto right = left | 1U;
                    assert(right < size_2);

                    ups[left] ^= one;
                    ups[right] ^= one;
                    any = true;
                }
            }

            {// leaves
                const auto xorred = ::Standard::Algorithms::Numbers::xor_1(index);

                for (const auto &chi : { index, xorred })
                {
                    auto &ups_chi = ups[chi];

                    if (ups_chi == zero)
                    {
                        continue;
                    }

                    ups_chi = zero;
                    sums[chi] ^= 1U;
                    any = true;
                }
            }

            return any;
        }

        constexpr void bottom_up(const std::array<std::int32_t, logsi> &vals, const std::int32_t cnt)
        {
            for (std::int32_t ind{}; ind < cnt; ++ind)
            {
                const auto &parent = vals[ind];
                const auto left = parent << 1U;
                const auto right = left | 1U;

                sums[parent] = sums[left] + sums[right];
            }
        }

        constexpr void puto(const std::int32_t index)
        {
            assert(size <= index && index < size_2);

            std::array<std::int32_t, logsi> vals{};

            const auto cnt = calc_divs(index, vals);

            if (top_down(index, vals, cnt))
            {
                bottom_up(vals, cnt);
            }
        }

        std::array<std::int32_t, size_2> sums{};
        std::array<bit_t, size_2> ups{}; // todo(p3): use 1 bit instead of 8.
    };
} // namespace Standard::Algorithms::Trees
