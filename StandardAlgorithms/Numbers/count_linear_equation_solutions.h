#pragma once
#include"../Utilities/require_utilities.h"
#include<unordered_map>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::integral int_t, class maps_t>
    [[nodiscard]] constexpr auto count_lin_eq_slow_rec(const std::vector<int_t> &arr, const int_t &mod,
        // sum at position -> count
        maps_t &map8, const int_t &sum, const std::int32_t pos) -> int_t
    {
        assert(!(sum < int_t{}) && !(pos < 0) && static_cast<std::uint64_t>(pos) + 1U < arr.size());

        const auto &val = require_positive(arr.at(pos), "val");

        if (pos == 0)
        {
            return int_t{} == (sum % val) ? 1 : 0;
        }

        auto &keys = map8[pos];

        if (const auto iter = keys.find(sum); iter != keys.end())
        {
            return iter->second;
        }

        int_t count{};

        for (auto left = sum;;)
        {
            count += count_lin_eq_slow_rec<int_t, maps_t>(arr, mod, map8, left, pos - 1);

            if (!(count < mod))
            {
                count -= mod;
            }

            if (left < val)
            {
                break;
            }

            left -= val;
        }

        assert(!(count < 0) && count < mod);

        keys[sum] = count;

        return count;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Count solutions of the equation Sum(a[index] * x[index], index=0..n-2) = a[n-1],
    // where n>=2, all coefficients (a[index] , index=0..n-2) are integers > 0, a[n-1] >= 0.
    // E.g. 2*x0 + 4*x1 + 6*x1 = 0 has 1 solution {0, 0, 0}.
    // 2*x0 = 1 has 0 solutions.
    // 2*x0 + 4*x1 + 4*x2 = 10 has 5 solutions.

    // It is slow in the sense that time complexity is O(n * a[n-1]) where
    // a[n-1] might be large.
    // It is very much similar to the pseudo-polynomial Knapsack problem solution.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto count_linear_equation_solutions(const std::vector<int_t> &arr, const int_t &mod)
        -> int_t
    {
        const auto size = require_greater(static_cast<std::int32_t>(arr.size()), 1, "size");

        constexpr auto greater1 = std::numeric_limits<std::int32_t>::max() - 2;
        require_greater(greater1, size, "size");

        require_greater(mod, 1, "modulo");

        require_all_positive(arr.cbegin(), arr.cend() - 1, "coefficients");

        if (arr.back() == 0)
        {
            return 1;
        }

        require_positive(arr.back(), "a[n-1]");

        std::vector<int_t> buffer(arr.back() + 1LL);

        require_positive(buffer.size(), "buffer size");

        buffer.at(0) = 1;

        for (std::int32_t index{}; index < size - 1; ++index)
        {
            for (int_t ind_2 = arr[index], step{}; ind_2 <= arr.back(); ++ind_2, ++step)
            {
                auto &buf = buffer.at(ind_2);
                buf += buffer.at(step);

                if (!(buf < mod))
                {
                    buf -= mod;
                }
            }
        }

        const auto &count = buffer.back();

        assert(!(count < 0) && count < mod);

        return count;
    }

    // Slow.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto count_linear_equation_solutions_slow(const std::vector<int_t> &arr, const int_t &mod)
        -> int_t
    {
        const auto size = require_greater(static_cast<std::int32_t>(arr.size()), 1, "size");

        require_greater(mod, 1, "modulo");
        require_all_positive(arr.cbegin(), arr.cend() - 1, "coefficients");

        assert(!(arr.back() < 0));

        std::vector<std::unordered_map<int_t, int_t>> map8(size - 1);

        const auto count = Inner::count_lin_eq_slow_rec<int_t>(arr, mod, map8, arr.back(), size - 2);

        assert(!(count < 0) && count < mod);

        return count;
    }
} // namespace Standard::Algorithms::Numbers
