#pragma once
#include <algorithm>
#include <cassert>

namespace Privet
{
    namespace Algorithms
    {
        //Calculates C(n, m). E.g. C(11, 5) is 462.
        //Returns 0 on error.
        template <class multiply_t, class divide_t, class number_t>
        number_t binomial_choose_naive(const multiply_t& multiply, const divide_t& divide, const number_t n, const number_t m)
        {
            assert(multiply);
            assert(divide);
            assert(n >= 0 && m >= 0);
            if (n < m) //It is an error!
                return 0;

            if (!n || !m || n == m)
                return 1;

            // n = 11, m = 5
            //  then a = 5, b = 6.
            //
            // C(11, 5) = 11!/6!/5! = 7*8*9*10*11 / (5!)
            const number_t a = std::min(m, n - m);
            const number_t b = std::max(m, n - m);

            number_t left = b + 1, divisor = 2, result = left;
            while (++left <= n)
            {
                //Try to avoid too big numbers.
                while (divisor <= a && (0 == (result % divisor)))
                    result = divide(result, divisor++);

                result = multiply(result, left);
            }

            while (divisor <= a)
                result = divide(result, divisor++);

            return result;
        }

        template <class multiply_t, class number_t>
        number_t factorial(const multiply_t& multiply, const number_t n)
        {
            assert(n >= 0);

            number_t result = 1;
            for (number_t j = 2; j <= n; ++j)
                result = multiply(result, j);

            return result;
        }
    }
}