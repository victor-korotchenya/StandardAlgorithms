#pragma once
#include <algorithm>
#include <array>
#include <unordered_set>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "hash_utilities.h"

namespace
{
    template<class long_t, class int_t, class set_t>
    void count_partition_rec_slow(set_t& uniq,
        std::vector<int_t>& buf,
        const int_t left,
        const int_t pos)
    {
        assert(left > 0 && pos > 0);
        if (pos == 1)
        {
            buf[1] = left;
            uniq.insert(buf);
            buf[1] = 0;
            return;
        }

        long_t sum{};
        for (int_t count = 0; ; ++count, sum += pos)
        {
            assert(count <= left);
            if (sum < left)
            {
                buf[pos] = count;

                count_partition_rec_slow<long_t, int_t, set_t>(uniq, buf,
                    static_cast<int_t>(left - sum), static_cast<int_t>(pos - 1));

                buf[pos] = 0;
                continue;
            }

            if (sum == left)
            {
                buf[pos] = count;
                uniq.insert(buf);
                buf[pos] = 0;
            }

            return;
        }
    }

    // Slow for small n
    template<class long_t, class int_t, int_t mod = 1000 * 1000 * 1000 + 7>
    int_t count_partition_slow(const int_t n)
    {
        static_assert(sizeof(long_t) >= sizeof(int_t) && mod >= 2);
        RequireNonNegative(n, "n");
        if (n <= 1)
            return 1;

        std::unordered_set<std::vector<int_t>, std::vector_hash> uniq;
        std::vector<int_t> buf(n + 1);

        count_partition_rec_slow<long_t, int_t>(uniq, buf, n, n);

        const auto r = static_cast<int_t>(uniq.size() % mod);
        assert(uniq.size() && r >= 0 && r < mod);
        return r;
    }

    template<class long_t, class int_t, int_t mod>
    long_t pentagonal_partition_calc(const std::vector<int_t>& result,
        const int_t n, const long_t init, const long_t subs)
    {
        static_assert(sizeof(long_t) >= sizeof(int_t) && mod >= 2);
        assert(n >= 2 && n < static_cast<int_t>(result.size()));

        constexpr std::array<long_t, 2> signs{ -long_t(1), 1 };

        long_t pentagonal_number = init, sum{};
        int_t k = 1;
        do
        {
            const auto& prev = result[n - pentagonal_number];
            sum += prev * signs[k & 1];

            const auto ad = long_t(3) * ++k - subs;
            assert(k > 0 && pentagonal_number > 0 && ad > 0 && pentagonal_number + ad > pentagonal_number);

            pentagonal_number += ad;
        } while (pentagonal_number <= n);

        sum %= mod;
        if (sum < 0)
            sum += mod;
        assert(sum >= 0 && sum < mod);
        return sum;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Count all distinct ignoring order partitions of all ints 0 < n <= n_max.
            // 4 has partitions: 4, 3+1, 2+2, 2+1+1, 1+1+1+1.
            // ans[4] = 5
            // Time O(n*n), space O(n).
            template<class long_t, class int_t, int_t mod = 1000 * 1000 * 1000 + 7>
            std::vector<int_t> count_partitions(const int_t n_max)
            {
                static_assert(sizeof(long_t) >= sizeof(int_t) && mod >= 2);
                RequirePositive(n_max, "n_max");

                std::vector<int_t> result(n_max + 1);
                result[1] = result[0] = 1;

                for (int_t n = 2; n <= n_max; ++n)
                {
                    // 1,5,12,22,35,51,70,92,117
                    const auto positive = pentagonal_partition_calc<long_t, int_t, mod>(result,
                        n, 1, 2);

                    // 2,7,15,26,40,57,77,100
                    const auto negative = pentagonal_partition_calc<long_t, int_t, mod>(result,
                        n, 2, 1);
                    auto val = positive + negative;
                    if (val >= mod)
                        val -= mod;

                    assert(val >= 0 && val < mod);
                    result[n] = static_cast<int_t>(val);
                }

                return result;
            }

            // Slow time/space O(n*n).
            template<class long_t, class int_t, int_t mod = 1000 * 1000 * 1000 + 7>
            std::vector<int_t> count_partitions_still_slow(const int_t n_max)
            {
                static_assert(sizeof(long_t) >= sizeof(int_t) && mod >= 2);
                RequirePositive(n_max, "n_max");

                std::vector<int_t> result(n_max + 1ll);
                result[1] = result[0] = 1;

                // number, decrement_max
                std::vector<std::vector<int_t>> buf(n_max + 1);
                buf[0].push_back(1);

                for (int_t n = 1; n <= n_max; ++n)
                {
                    auto& cur = buf[n];
                    cur.resize(n + 1);
                    cur[1] = 1;

                    for (int_t d = 2; d <= n; ++d)
                    {
                        const auto row = static_cast<int_t>(n - d),
                            col = std::min(row, d);
                        const auto& prev1 = buf[row][col],
                            & prev2 = cur[d - 1];

                        auto val = static_cast<long_t>(prev1) + prev2;
                        if (val >= mod)
                            val -= mod;

                        assert(val >= 0 && val < mod);
                        cur[d] = static_cast<int_t>(val);
                    }

                    result[n] = cur[n];
                }

                return result;
            }

            // Slow for small n_max
            template<class long_t, class int_t, int_t mod = 1000 * 1000 * 1000 + 7>
            std::vector<int_t> count_partitions_slow(const int_t n_max)
            {
                static_assert(sizeof(long_t) >= sizeof(int_t) && mod >= 2);
                RequirePositive(n_max, "n_max");

                std::vector<int_t> result(n_max + 1ll);

                for (int_t n = 0; n <= n_max; ++n)
                    result[n] = count_partition_slow<long_t, int_t, mod>(n);

                return result;
            }
        }
    }
}