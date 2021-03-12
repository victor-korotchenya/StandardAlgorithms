#pragma once
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include<cassert>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::signed_integral int_t, int_t mod>
    requires(2 <= mod)
    [[nodiscard]] constexpr auto cadas_slow_rec(const int_t &vals, std::vector<int_t> &buffer, const int_t &pos)
        -> int_t
    {
        if (pos < 0)
        {
            if (buffer.at(0) != buffer.back())
            {
                return 0;
            }

            const auto size = static_cast<int_t>(buffer.size());
            if (size <= 2)
            {
                return 1;
            }

            for (std::int32_t index{}; index < size - 1; ++index)
            {
                const auto &prev = buffer[index];
                const auto &cur = buffer[index + 1];

                if (prev == cur)
                {
                    return 0;
                }
            }

            return 1;
        }

        int_t count{};
        auto &buf = buffer[pos];

        for (buf = 0; buf < vals; ++buf)
        {
            const auto add = cadas_slow_rec<int_t, mod>(vals, buffer, static_cast<int_t>(pos - 1));

            count += add;

            if (!(count < mod))
            {
                count -= mod;
            }

            assert(!(count < 0) && count < mod);
        }

        return count;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Count arrays of size1>0 numbers [1..vals] having the same first/last items,
    // and different adjacent ones when size1>2.
    template<std::integral long_int_t, std::integral int_t, int_t mod>
    requires(same_sign_leq_size<int_t, long_int_t> && 2 <= mod)
    [[nodiscard]] constexpr auto count_arrays_different_adjacent_same_1n(const int_t &size1, const int_t &vals) -> int_t
    {
        require_positive(size1, "size1");
        require_positive(vals, "vals");

        if (size1 <= 2)
        {
            return vals;
        }

        if (vals == 1)
        {
            return 0;
        }

        std::vector<int_t> end_with_one(size1);
        std::vector<int_t> end_with_one_not(size1);
        // 1 234 1
        end_with_one[0] = 1;
        end_with_one[1] = 0;
        end_with_one_not[0] = 0;

        const auto val_1 = static_cast<long_int_t>((vals - 1) % mod);
        const auto val_2 = static_cast<long_int_t>((vals - 2) % mod);

        end_with_one_not[1] = static_cast<int_t>(val_1);

        for (int_t index = 2; index < size1; ++index)
        {
            const auto &prev_not = end_with_one_not[index - 1];
            end_with_one[index] = prev_not;

            // arr[index] != 1
            const auto add = end_with_one[index - 1] * val_1 +
                // Assume no overflow here.
                prev_not * val_2;

            end_with_one_not[index] = static_cast<int_t>(add % mod);
        }

        auto count = static_cast<int_t>(end_with_one[size1 - 1] * static_cast<long_int_t>(vals) % mod);

        assert(!(count < 0) && count < mod);

        return count;
    }

    template<std::signed_integral int_t, int_t mod>
    requires(2 <= mod)
    [[nodiscard]] constexpr auto count_arrays_different_adjacent_same_1n_slow(const int_t &size1, const int_t &vals)
        -> int_t
    {
        require_positive(size1, "size1");
        require_positive(vals, "vals");

        std::vector<int_t> buffer(size1);

        auto count = Inner::cadas_slow_rec<int_t, mod>(vals, buffer, static_cast<int_t>(size1 - 1));

        assert(!(count < 0) && count < mod);

        return count;
    }
} // namespace Standard::Algorithms::Numbers
