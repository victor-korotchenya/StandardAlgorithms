#pragma once
#include <array>
#include <cassert>
#include "number_mod.h"
#include "matrix.h"

namespace Privet::Algorithms::Numbers
{
    // A rover on the Moon can move forward 1, 2, or 3 meters.
    // Count in how many ways n>= meter path can be made?
    // f(1) = 1
    // f(2) = |{1+1, 2}| = 2
    // f(3) = |{1+1+1, 1+2, 2+1, 3}| = 4
    //
    // Linear Algebra is serving the common good - time O(log(n)).
    template <class int_t, int_t modulo>
    int_t fib3(int_t n)
    {
        static_assert(modulo >= 2);
        assert(n >= 0);

        if (n <= 1)
            return 1;

        // todo: p2. Can gener. func be faster?
        using num_mod_t = number_mod<int_t, modulo, int_t>;
        using matrix_t = matrix<num_mod_t, 3, 3>;

        const num_mod_t coefs[3][3] = { {1,1,1},
           {1,0,0},
           {0,1,0},
        };
        matrix_t source(coefs), buf;
        matrix_power(source, n, buf);

        const auto& r = buf[0][0].val();
        assert(r >= 0 && r < modulo);
        return r;
    }

    // Slow.
    template <class int_t, int_t modulo>
    int_t fib3_slow(int_t n)
    {
        static_assert(modulo >= 2);
        assert(n >= 0);

        if (n <= 1)
            return 1;
        //if (n <= 3)
        //    return (n == 3 ? 4 : 2) % modulo;

        std::array<int_t, 4> a{
            // -1, 0, 1
            0, 1, 1
        };
        auto pos = 3;
        --n;
        for (;;)
        {
            auto& r = a[pos];
            r = a[(pos + 1) & 3] + a[(pos + 2) & 3];
            if (r >= modulo)
                r -= modulo;

            r += a[(pos + 3) & 3];
            if (r >= modulo)
                r -= modulo;

            assert(r >= 0 && r < modulo);
            if (!--n)
                return r;

            pos = (pos + 1) & 3;
        }
    }
}