#pragma once
#include <cassert>
#include <type_traits>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template <class int_t>
    int_t min_sum_array_to1_slow_rec(const std::vector<int_t>& ar,
        const int_t m)
    {
        static_assert(std::is_signed_v<int_t>);

        const auto size = static_cast<int_t>(ar.size());
        assert(size >= m && m >= 2);

        std::vector<int_t> buf(size - m + 1);
        auto result = std::numeric_limits<int_t>::max();

        for (int_t start = 0; start <= size - m; ++start)
        {
            for (int_t j = 0; j <= start; ++j)
                buf[j] = ar[j];

            for (int_t j = 1; j < m; ++j)
                buf[start] += ar[start + j];

            auto s = buf[start];
            if (size > m)
            {
                for (int_t j = start + 1; j <= size - m; ++j)
                    buf[j] = ar[j + m - 1];
                const auto c = min_sum_array_to1_slow_rec<int_t>(buf, m);
                s += c;
            }

            if (result > s)
                result = s;
        }

        assert(result < std::numeric_limits<int_t>::max());
        return result;
    }

    template <class long_int_t, class int_t>
    void min_sum_ar_process_interval_they(const std::vector<long_int_t>& sums, std::vector<std::vector<long_int_t>>& buf,
        const int_t m, int_t start, const int_t stop_incl,
        const bool skip_merge, const bool is_log)
    {
        auto best = std::numeric_limits<long_int_t>::max();

        for (auto mid = start; mid < stop_incl; mid += m -
            1) // +1 is wrong e.g. [2,3] + [4,6] when m=4.
        {
            const auto& first = buf[start][mid], & second = buf[mid + 1][stop_incl],
                cand = first + second;

            if (is_log)
                puts((" [" + std::to_string(start) + ", " + std::to_string(mid)
                    + "] [" + std::to_string(mid + 1) + ", " + std::to_string(stop_incl)
                    + "] = " + std::to_string(first) + " + " + std::to_string(second)
                    + " = " + std::to_string(cand)
                    ).c_str());

            if (best > cand)
                best = cand;
        }

        buf[start][stop_incl] = best;

        if (skip_merge)
            return;

        const auto ad = sums[stop_incl + 1] - sums[start];
        auto& value = buf[start][stop_incl];

        if (is_log)
            puts(("  buf[" + std::to_string(start) + "][" + std::to_string(stop_incl)
                + "] = " + std::to_string(value) + " += " + std::to_string(ad)
                + " = " + std::to_string(value + ad)
                ).c_str());

        value += ad;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Given n>0 ints, do until 1 number remains: choose m>1 consecutive items and replace them by their sum.
            // Return min sum, or -1 if impossible.
            // Sample a = {3, 5, 1, 2, 6}, m = 3, answer = 25.
            // - Merge first  3: 3+5+1 + {9, 2, 6} = 9 + 9+2+6 = 26.
            // - Merge middle 3: 5+1+2 + {3, 8, 6} = 8 + 3+8+6 = 25 min.
            // - Merge last   3: 1+2+6 + {3, 5, 9} = 9 + 3+5+9 = 26.
            // Warn. No overflow checks.
            // Time O(n*n*m), space O(n*n).

            // todo: p1. Review.
            template<class long_int_t = int64_t, class int_t>
            long_int_t min_sum_array_to1_replacing_consecutive_by_sum_they(const std::vector<int_t>& ar, const int_t m)
            {
                static_assert(std::is_signed_v<long_int_t> && std::is_signed_v<int_t> && sizeof(long_int_t) >= sizeof(int_t));

                const auto size = static_cast<int_t>(ar.size());
                RequirePositive(size, "size");
                RequirePositive(m, "m");
                RequirePositive(m - 1, "m-1");

                if (size == 1)
                    return ar[0];

                {
                    const auto rem = (size - 1) % (m - 1);
                    if (rem)
                        return -1;

                    assert(m <= size);
                }

                std::vector<long_int_t> sums(size + 1);
                for (int_t i = 0; i < size; ++i)
                    sums[i + 1] = sums[i] + ar[i];

                std::vector<std::vector<long_int_t>> buf(size,
                    std::vector<long_int_t>(size));

                auto is_log = false;
                if (is_log)
                    puts(("_comp " + std::to_string(size) + ", m= " + std::to_string(m)
                        ).c_str());

                for (int_t interval_length = m; interval_length <= size; ++interval_length)
                {
                    const bool skip_merge = (interval_length - 1) % (m - 1);

                    for (int_t start = 0; start <= size - interval_length; ++start)
                    {
                        const int_t stop_incl = start + interval_length - 1;
                        min_sum_ar_process_interval_they<long_int_t, int_t>(sums, buf, m, start, stop_incl, skip_merge, is_log);
                    }
                }

                const auto& result = buf.front().back();
                assert(result < std::numeric_limits<long_int_t>::max());
                return result;
            }

            constexpr auto factorial_max_slow = 12;

            // Slow.
            // Time O(n!).
            template<class int_t>
            int_t min_sum_array_to1_replacing_consecutive_by_sum_slow(const std::vector<int_t>& ar, const int_t m)
            {
                static_assert(std::is_signed_v<int_t>);

                const auto size = static_cast<int_t>(ar.size());
                assert(size <= factorial_max_slow);
                RequirePositive(size, "size");
                RequirePositive(m, "m");
                RequirePositive(m - 1, "m-1");

                if (size == 1)
                    return ar[0];

                {
                    const auto rem = (size - 1) % (m - 1);
                    if (rem)
                        return -1;
                }

                const auto result = min_sum_array_to1_slow_rec<int_t>(ar, m);
                return result;
            }
        }
    }
}