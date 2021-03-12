#pragma once
#include <unordered_map>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class int_t, class maps_t>
    int_t count_lin_eq_slow_rec(const std::vector<int_t>& ar,
        // sum at position -> count
        maps_t& ms,
        const int_t mod,
        const int_t sum, const int pos)
    {
        assert(sum >= 0 && pos >= 0 && pos < ar.size() - 1);
        const auto& val = ar[pos];
        assert(val > 0);
        if (!pos)
            return 0 == (sum % val);

        auto& m = ms[pos];
        if (const auto it = m.find(sum); it != m.end())
            return it->second;

        int_t count{};
        for (auto left = sum;;)
        {
            count += count_lin_eq_slow_rec<int_t, maps_t>(ar, ms, mod, left, pos - 1);
            if (count >= mod)
                count -= mod;

            if (left < val)
                break;

            left -= val;
        }

        assert(count >= 0 && count < mod);
        m[sum] = count;
        return count;
    }
}

namespace Privet::Algorithms::Numbers
{
    // Count solutions of the equation Sum(a[i] * x[i], i=0..n-2) = a[n-1],
    // where n>=2, all coefficients (a[i] , i=0..n-2) are integers > 0, a[n-1] >= 0.
    // E.g. 2*x0 + 4*x1 + 6*x1 = 0 has 1 solution {0, 0, 0}.
    // 2*x0 = 1 has 0 solutions.
    // 2*x0 + 4*x1 + 4*x2 = 10 has 5 solutions.

    // It is slow in the sense that time complexity is O(n * a[n-1]) where
    // a[n-1] might be large.
    // It is very much similar to the pseudo-polynomial Knapsack problem solution.
    template<class int_t>
    int_t count_linear_equation_solutions(const std::vector<int_t>& ar, const int_t mod)
    {
        const auto size = static_cast<int>(ar.size());
        RequireGreater(size, 1, "size");
        RequireGreater(mod, 1, "modulo");
        RequireAllPositive(ar.cbegin(), ar.cend() - 1, "coefficients");

        if (ar.back() == 0)
            return 1;

        RequirePositive(ar.back(), "a[n-1]");

        std::vector<int_t> buf(ar.back() + 1ll);
        buf[0] = 1;

        for (auto i = 0; i < size - 1; ++i)
        {
            for (int_t j = ar[i], d{}; j <= ar.back(); ++j, ++d)
            {
                auto& b = buf[j];
                b += buf[d];
                if (b >= mod)
                    b -= mod;
            }
        }

        const auto& count = buf.back();
        assert(count >= 0 && count < mod);
        return count;
    }

    // Slow.
    template<class int_t>
    int_t count_linear_equation_solutions_slow(const std::vector<int_t>& ar, const int_t mod)
    {
        const auto size = static_cast<int>(ar.size());
        RequireGreater(size, 1, "size");
        RequireGreater(mod, 1, "modulo");
        RequireAllPositive(ar.cbegin(), ar.cend() - 1, "coefficients");

        assert(ar.back() >= 0);

        std::vector<std::unordered_map<int_t, int_t>> ms(size - 1);

        const auto count = count_lin_eq_slow_rec<int_t>(ar, ms, mod, ar.back(), size - 2);
        assert(count >= 0 && count < mod);
        return count;
    }
}