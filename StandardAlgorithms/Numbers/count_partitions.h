#pragma once
#include"../Utilities/require_utilities.h"
#include"hash_utilities.h"
#include<array>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::signed_integral long_t, std::signed_integral int_t, class set_t>
    requires(sizeof(int_t) <= sizeof(long_t))
    constexpr void count_partition_rec_slow(
        set_t &uniq, const int_t &left, std::vector<int_t> &buffer, const int_t &pos)
    {
        assert(int_t{} < left && int_t{} < pos);

        if (pos == 1)
        {
            buffer[1] = left;
            uniq.insert(buffer);
            buffer[1] = 0;

            return;
        }

        long_t sum{};

        for (int_t count{};; ++count, sum += pos)
        {
            assert(count <= left);

            if (sum < left)
            {
                buffer[pos] = count;

                count_partition_rec_slow<long_t, int_t, set_t>(
                    uniq, static_cast<int_t>(left - sum), buffer, static_cast<int_t>(pos - 1));

                buffer[pos] = 0;

                continue;
            }

            if (sum == left)
            {
                buffer[pos] = count;
                uniq.insert(buffer);
                buffer[pos] = 0;
            }

            return;
        }
    }

    template<std::signed_integral long_t, std::signed_integral int_t, int_t mod>
    requires(sizeof(int_t) <= sizeof(long_t) && 2 <= mod)
    [[nodiscard]] constexpr auto count_partition_impl_slow(const int_t &size) -> int_t
    {
        require_non_negative(size, "size");

        if (size <= 1)
        {
            return 1;
        }

        std::unordered_set<std::vector<int_t>, Standard::Algorithms::Numbers::vector_hash> uniq{};
        std::vector<int_t> buffer(size + 1LL);

        count_partition_rec_slow<long_t, int_t>(uniq, size, buffer, size);

        auto res = static_cast<int_t>(static_cast<long_t>(uniq.size()) % mod);

        assert(!uniq.empty() && int_t{} <= res && res < mod);

        return res;
    }

    template<std::signed_integral long_t, std::signed_integral int_t, int_t mod>
    requires(sizeof(int_t) <= sizeof(long_t) && 2 <= mod)
    [[nodiscard]] constexpr auto pentagonal_partition_calc(
        const int_t &top, long_t pentagonal_number, const std::vector<int_t> &result, const long_t &subs) -> long_t
    {
        assert(2 <= top && top < static_cast<int_t>(result.size()));

        constexpr std::array<long_t, 2> signs{ -static_cast<long_t>(1), 1 };

        long_t sum{};
        int_t cnt = 1;

        do
        {
            const auto &prev = result.at(top - pentagonal_number);

            // Assume no overflow.
            sum += prev * signs[cnt & 1];

            const auto add = static_cast<long_t>(3) * ++cnt - subs;

            assert(0 < cnt && 0 < pentagonal_number && 0 < add && pentagonal_number < pentagonal_number + add);

            pentagonal_number += add;
        } while (pentagonal_number <= top);

        sum %= mod;

        if (sum < 0)
        {
            sum += mod;
        }

        assert(long_t{} <= sum && sum < mod);

        return sum;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Count all distinct ignoring order partitions of all integers 0 < i <= n_max.
    // 4 has ans[4] = 5 partitions: 4, 3+1, 2+2, 2+1+1, 1+1+1+1.
    // Time O(n*n), space O(n).
    template<std::signed_integral long_t, std::signed_integral int_t, int_t mod>
    requires(sizeof(int_t) <= sizeof(long_t) && 2 <= mod)
    [[nodiscard]] constexpr auto count_partitions(const int_t &n_max) -> std::vector<int_t>
    {
        require_positive(n_max, "n_max");

        std::vector<int_t> result(n_max + 1LL);
        result[1] = result[0] = 1;

        for (int_t number = 2; number <= n_max; ++number)
        {
            // 1,5,12,22,35,51,70,92,117
            const auto positive = Inner::pentagonal_partition_calc<long_t, int_t, mod>(number, 1, result, 2);

            // 2,7,15,26,40,57,77,100
            const auto negative = Inner::pentagonal_partition_calc<long_t, int_t, mod>(number, 2, result, 1);

            auto val = positive + negative;

            if (!(val < mod))
            {
                val -= mod;
            }

            assert(int_t{} <= val && val < mod);

            result[number] = static_cast<int_t>(val);
        }

        return result;
    }

    // Slow time/space O(n*n).
    template<std::signed_integral long_t, std::signed_integral int_t, int_t mod>
    requires(sizeof(int_t) <= sizeof(long_t) && 2 <= mod)
    [[nodiscard]] constexpr auto count_partitions_still_slow(const int_t &n_max) -> std::vector<int_t>
    {
        require_positive(n_max, "n_max");

        std::vector<int_t> result(n_max + 1LL);
        result[1] = result[0] = 1;

        // number, decrement_max
        std::vector<std::vector<int_t>> buffer(n_max + 1LL);
        buffer[0].push_back(1);

        for (int_t number = 1; number <= n_max; ++number)
        {
            auto &cur = buffer[number];
            cur.resize(number + 1LL);
            cur[1] = 1;

            for (int_t dec = 2; dec <= number; ++dec)
            {
                const auto row = static_cast<int_t>(number - dec);
                const auto col = std::min(row, dec);

                const auto &prev1 = buffer[row][col];
                const auto &prev2 = cur[dec - 1];

                auto val = static_cast<long_t>(prev1) + prev2;

                if (!(val < mod))
                {
                    val -= mod;
                }

                assert(int_t{} <= val && val < mod);

                cur[dec] = static_cast<int_t>(val);
            }

            result[number] = cur[number];
        }

        return result;
    }

    // Slow for small n.
    template<std::signed_integral long_t, std::signed_integral int_t, int_t mod>
    requires(sizeof(int_t) <= sizeof(long_t) && 2 <= mod)
    [[nodiscard]] constexpr auto count_partitions_slow(const int_t &n_max) -> std::vector<int_t>
    {
        require_positive(n_max, "n_max");

        {
            constexpr auto greatest = 11;
            require_greater(greatest, n_max, "n_max");
        }

        std::vector<int_t> result(n_max + 1LL);

        for (int_t number{}; number <= n_max; ++number)
        {
            result[number] = Inner::count_partition_impl_slow<long_t, int_t, mod>(number);
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
