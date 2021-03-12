#pragma once
#include"matrix.h"
#include"number_mod.h"

namespace Standard::Algorithms::Numbers
{
    // A rover on the Moon can move forward 1, 2, or 3 meters.
    // Count in how many ways (meter path <= size) can be made?
    // fib3(0) = fib3(1) = 1
    // fib3(2) = |{1+1, 2}| = 2
    // fib3(3) = |{1+1+1, 1+2, 2+1, 3}| = 4
    // fib3(4) = |{1+1+1+1, 1+1+2, 1+2+1, 1+3,
    //  2+1+1, 2+2, 3+1}| = 7
    //
    // Linear Algebra is serving the common good - time O(log(size)).
    template<std::integral int_t, int_t modulo>
    requires(int_t{} < modulo)
    [[nodiscard]] constexpr auto fib3(const int_t &size) -> int_t
    {
        assert(int_t{} <= size);

        constexpr int_t one = 1;

        if (size <= one)
        {
            return static_cast<int_t>(one % modulo);
        }

        // todo(p3): Can generating functions be even faster?
        using num_mod_t = number_mod<int_t, modulo, int_t>;

        constexpr num_mod_t nu_0{};
        constexpr num_mod_t nu_1{ 1 };
        constexpr int_t dim = 3;

        using line_t = std::array<num_mod_t, dim>;
        using array_t = std::array<line_t, dim>;

        constexpr array_t source{ { { nu_1, nu_1, nu_1 }, { nu_1, nu_0, nu_0 }, { nu_0, nu_1, nu_0 } } };

        using matrix_t = matrix<num_mod_t, dim, dim>;

        matrix_t source_matr(source);

        matrix_t buf{};
        matrix_power(source_matr, size, buf);

        auto &res = buf[0, 0].value();

        assert(int_t{} <= res && res < modulo);

        return std::move(res);
    }

    // Slow.
    template<std::integral int_t, int_t modulo>
    requires(int_t{} < modulo)
    [[nodiscard]] constexpr auto fib3_slow(int_t size) -> int_t
    {
        assert(int_t{} <= size);

        constexpr int_t one = 1;

        if (size <= one)
        {
            return static_cast<int_t>(one % modulo);
        }

        constexpr int_t dim = 4;

        // Start from adding 0 + 1 to get 1.
        // Then 1 + 1 to get 2.
        std::array<int_t, dim> arr{ 0, 0, 1, 1 };

        --size;

        for (int_t pos{};;)
        {
            constexpr int_t mask = 3;

            auto &res = arr[pos];

            res = static_cast<int_t>(arr[(pos + one) & mask] + arr[(pos + 2) & mask]);

            if (!(res < modulo))
            {
                res -= modulo;
            }

            res += arr[(pos + 3) & mask];

            if (!(res < modulo))
            {
                res -= modulo;
            }

            assert(int_t{} <= res && res < modulo);

            if (int_t{} == --size)
            {
                return res;
            }

            pos = (pos + one) & mask;
        }
    }
} // namespace Standard::Algorithms::Numbers
