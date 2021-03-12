#pragma once
#include <cassert>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class int_t, int_t mod>
    int_t count_arrays_different_adjacent_same_1n_slow_rec(
        std::vector<int_t>& s, const int_t m, const int_t pos)
    {
        static_assert(mod >= 2);
        if (pos < 0)
        {
            if (s.front() != s.back())
                return 0;

            const auto size = static_cast<int>(s.size());
            if (size > 2)
                for (auto i = 0; i < size - 1; ++i)
                {
                    const auto& prev = s[i], & cur = s[i + 1];
                    if (prev == cur)
                        return 0;
                }

            return 1;
        }

        int_t count = 0;
        auto& c = s[pos];
        for (c = 0; c < m; ++c)
        {
            const auto ad = count_arrays_different_adjacent_same_1n_slow_rec<int_t, mod>(s, m, pos - 1);
            count += ad;
            if (count >= mod)
                count -= mod;
            assert(count >= 0 && count < mod);
        }

        return count;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Count arrays of n>0 numbers [1..m] having the same first/last items,
            // and different adjacent ones when n>2.
            template<class int_t = int, int_t mod = 1000 * 1000 * 1000 + 7>
            int_t count_arrays_different_adjacent_same_1n(const int_t n, const int_t m)
            {
                static_assert(mod >= 2);
                RequirePositive(n, "n");
                RequirePositive(m, "m");

                if (n <= 2)
                    return m;

                if (m == 1)
                    return 0;

                std::vector<int_t> end_with_one(n), end_with_one_not(n);
                // 1 234 1
                end_with_one[0] = 1;
                end_with_one[1] = 0;

                end_with_one_not[0] = 0;
                end_with_one_not[1] = m - 1;

                for (int_t i = 2; i < n; ++i)
                {
                    end_with_one[i] = end_with_one_not[i - 1];

                    // arr[i] != 1
                    const auto ad = end_with_one[i - 1] * static_cast<int64_t>(m - 1) +
                        end_with_one_not[i - 1] * static_cast<int64_t>(m - 2);

                    end_with_one_not[i] = static_cast<int_t>(ad % mod);
                }

                const auto count = end_with_one[n - 1] * static_cast<int64_t>(m) % mod;
                assert(count >= 0 && count < mod);
                return static_cast<int>(count);
            }

            template<class int_t = int, int_t mod = 1000 * 1000 * 1000 + 7>
            int_t count_arrays_different_adjacent_same_1n_slow(const int_t n, const int_t m)
            {
                static_assert(mod >= 2);
                RequirePositive(n, "n");
                RequirePositive(m, "m");

                std::vector<int_t> s(n);
                const auto count = count_arrays_different_adjacent_same_1n_slow_rec<int_t, mod>(s, m, n - 1);
                assert(count >= 0 && count < mod);
                return count;
            }
        }
    }
}