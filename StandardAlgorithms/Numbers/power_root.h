#pragma once
// "power_root.h"
#include"../Utilities/is_debug.h"
#include"../Utilities/is_integral_pure.h"
#include"logarithm.h"
#include<algorithm>
#include<cassert>
#include<cmath> // std::sqrt
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<limits>
#include<numeric> // std::midpoint

namespace Standard::Algorithms::Numbers
{
    // See also "logarithm.h".

    // Given integers (2, 3), return pow(2, 3) = 8.
    // The exponent must be non-negative.
    template<class value_t, std::integral int_t>
    requires(std::is_arithmetic_v<value_t>)
    [[nodiscard]] constexpr auto int_power(value_t base, int_t exponent) noexcept -> value_t
    {
        constexpr int_t int_zero{};

        assert(!(exponent < int_zero));

        if (value_t{} == base)
        {
            return {};
        }

        constexpr int_t int_one{ 1 };

        value_t result{ 1 };

        while (int_zero < exponent)
        {
            if (int_zero != (exponent & int_one))
            {// A particular bit is set in the "exponent".
                result *= base;
            }

            base *= base;
            exponent >>= 1U;
        }

        return result;
    }

    template<std::integral int_t, int_t max_val = std::numeric_limits<int_t>::max()>
    requires(int_t{} < max_val)
    [[nodiscard]] constexpr auto power_with_overflow_check(int_t base, int_t exponent) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one{ 1 };

        assert(zero < base && !(exponent < zero));

        if (!(zero < exponent))
        {
            return one;
        }

        auto result = one;
        auto lim = static_cast<int_t>(max_val / base);

        for (;;)
        {
            if (zero != (exponent & one))
            {// A particular bit is set in the "exponent".
                if (lim < result)
                {
                    return max_val;
                }

                result *= base;
            }

            exponent >>= 1U;

            if (!(zero < exponent))
            {
                return result;
            }

            if (lim < base)
            {
                return max_val;
            }

            lim /= base;
            base *= base;

            if (!(zero < base))
            {
                return max_val;
            }
        }
    }

    inline constexpr std::size_t newton_max_steps = 60;

    // Given (base, exp), return pow(base, 1.0/exp).
    // (1024.0, 10), return 2.0.
    // The inputs must be non-negative.
    template<std::floating_point floating_t, std::integral int_t>
    requires(std::convertible_to<int_t, floating_t>)
    [[nodiscard]] constexpr auto root_newton(
        const floating_t &base, const int_t &exponent, floating_t epsilon = {}, const bool busy = false) -> floating_t
    {
        // f(x) = x**exp - base. f'(x) = exp * x**(exp - 1).
        // x1 = x - f(x)/f'(x) = x - (x**exp - base)/(exp * x**(exp - 1)) =
        // = x - x/exp + base/exp / x**(exp - 1) = (exp - 1)/exp*x + base/exp / x**(exp - 1)
        // x1 = (exp - 1)/exp*x + base/exp / x**(exp - 1)
        // Slow convergence for large exp!
        //
        //
        // Let g(x) = log(x**exp) - log(base) = exp*log(x) - log(base). f'(x) = exp / x.
        // x1 = x - f(x)/f'(x) = x - (exp*log(x) - log(base))/(exp / x) =
        // = x - x*log(x) + log(base)/exp * x = (log(base)/exp + 1 - log(x)) * x
        // x1 = (log(base)/exp + 1 - log(x)) * x
        // Set x0 = 1.0.
        constexpr floating_t zero{};

        if (exponent < int_t{} || base < zero)
        {// todo(p4): (-7) ** (1*3) can be computed for an odd exponent.
            return static_cast<floating_t>(NAN);
        }

        constexpr floating_t one{ 1 };

        if (int_t{ 1 } == exponent || zero == base || one == base
#ifndef __clang__ // todo(p3): review in C++26
            || std::isinf(base) || std::isnan(base)
#endif
        )
        {
            return base;
        }

        if (int_t{} == exponent)
        {
            return one;
        }

        if (base < one)
        {
            if (busy) [[unlikely]]
            {// An error.
                assert(0);
                return static_cast<floating_t>(NAN);
            }

            const auto rev_base = static_cast<floating_t>(one / base);
            const auto raw = root_newton<floating_t, int_t>(rev_base, exponent, epsilon, true);
            auto revv = static_cast<floating_t>(one / raw);
            return revv;
        }

        if (epsilon < zero)
        {
            epsilon = zero;
        }

        [[maybe_unused]] constexpr auto inf = std::numeric_limits<floating_t>::infinity();

        const auto coef = static_cast<floating_t>(std::log(base) / static_cast<floating_t>(exponent) + one);

        std::size_t steps{}; // todo(p3): 12**0.5, 3**0.1 converge slowly.

        for (floating_t left{}, right = one;;)
        {
            assert(zero < right);
            left = right;

            const auto logx = static_cast<floating_t>(std::log(right));
            // x1 = (log(base)/exp + 1 - log(x)) * x
            right = static_cast<floating_t>((coef - logx) * right);

            if (const auto diff = std::abs(left - right); newton_max_steps <= ++steps || !(epsilon < diff))
            {
                assert(right < inf);
                return right;
            }
        }
    }

    // Given integers (base, exp), return an integer pow(base, 1/exp).
    // Given (1024, 10), return 2.
    // (1023, 10) -> 1.
    // The inputs must be non-negative.
    // Time O(64).
    template<std::integral int_t, int_t max_val = std::numeric_limits<int_t>::max()>
    requires(sizeof(int_t) <= sizeof(std::uint64_t) // todo(p3): Weaken this precondition, impl. long division.
        && int_t{} < max_val)
    [[nodiscard]] constexpr auto int_root_binary_slow(const int_t &base, const int_t &exponent) noexcept -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one{ 1 };

        assert(!(base < zero) && !(exponent < zero));

        if (!(zero < base) || one == exponent)
        {
            return base;
        }

        constexpr std::int32_t max_bit_0_based = (sizeof(int_t) << 3U) - (std::is_signed_v<int_t> ? 2 : 1);
        static_assert(0 < max_bit_0_based);

        if (!(zero < exponent) || !(exponent < base) ||
            max_bit_0_based < exponent
            // todo(p4): fix overflow check. pow(2**64 - 1, 1/63) = 2.02212
            || !(base < max_val))
        {
            return one;
        }

        // 1 < exponent < base
        auto logar = log_base_2_down(static_cast<std::uint64_t>(base)) / exponent;
        assert(0 <= logar && logar <= max_bit_0_based);

        for (int_t xxx{};;)
        {
            const auto shifted = static_cast<int_t>(one << logar);
            const auto cand = static_cast<int_t>(xxx + shifted);

            assert(zero < shifted && xxx < cand);

            const auto raised = power_with_overflow_check<int_t, max_val>(cand, exponent);

            if (!(base < raised))
            {
                xxx = cand;
            }

            assert(!(xxx < zero) && xxx < base);
            assert(!(base < xxx * xxx)); // At least square.

            if (logar-- <= 0)
            {
                assert(zero < xxx);
                return xxx;
            }
        }
    }

    // Given 4.0, return sqrt(4.0) = 2.0.
    // The input must be non-negative.
    template<std::floating_point floating_t>
    [[nodiscard]] constexpr auto sqrt_newton(const floating_t &base) noexcept -> floating_t
    {
#ifndef __clang__ // todo(p3): review in C++26
        if (std::isinf(base) || std::isnan(base))
        {// Garbage.
            return base;
        }
#endif

        constexpr floating_t zero{};
        if (base < zero)
        {
            return static_cast<floating_t>(NAN);
        }

        constexpr floating_t two{ 2 };
        [[maybe_unused]] constexpr auto inf = std::numeric_limits<floating_t>::infinity();

        [[maybe_unused]] std::size_t steps{};

        for (floating_t left{}, right = base;;)
        {
            if (left == right)
            {
                assert(!(base < zero) && base < inf);

                return right;
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                constexpr std::size_t max_steps = 100;

                ++steps;
                assert(steps < max_steps);
            }

            assert(zero < right);

            left = right;
            right = static_cast<floating_t>((right + base / right) / two);
        }
    }

    namespace Inner
    {
        template<std::integral int_t>
        constexpr void fix_int_sqrt_result(const int_t &num, int_t &root) noexcept
        {
            constexpr int_t zero{};

            assert(!(num < zero) && !(root < zero));

            if (num < root * root)
            {
                do
                {
                    assert(zero < root);

                    --root;
                } while (num < root * root);
            }
            else
            {
                for (constexpr int_t one = 1;;)
                {
                    const auto extra = root + one;
                    const auto prod = extra * extra;

                    if (!(zero < prod) || num < prod)
                    {
                        break;
                    }

                    ++root;
                }
            }

            assert(zero <= root && root * root <= num);
        }
    } // namespace Inner

    // Sqrt for integers.
    // Given 4, return sqrt(4) = 2.
    template<std::integral int_t>
    requires(sizeof(int_t) <= sizeof(std::uint64_t))
    [[nodiscard]] constexpr auto int_sqrt(const int_t &num) noexcept -> int_t
    {
        if constexpr (std::is_signed_v<int_t>)
        {
            assert(!(num < int_t{}));
        }

        if (std::is_constant_evaluated()) // if consteval
        {// todo(p4): Del when fixed: sqrt is constexpr only in GCC.
            using typ = std::uint64_t;

            const auto n_2 = static_cast<typ>(num);

            auto root = static_cast<typ>(sqrt_newton(static_cast<long double>(num)));

            Inner::fix_int_sqrt_result(n_2, root);

            auto res = static_cast<int_t>(root);
            assert(int_t{} <= res && res * res <= num);

            return res;
        }

        if constexpr (sizeof(int_t) <= sizeof(std::int32_t))
        {
            const auto root = std::sqrt(num);
            auto res = static_cast<int_t>(root);

            assert(int_t{} <= res && res * res <= num);

            return res;
        }
        else
        {
            auto res = static_cast<int_t>(std::sqrt(static_cast<long double>(num)));

            Inner::fix_int_sqrt_result(num, res);

            assert(int_t{} <= res && res * res <= num);

            return res;
        }
    }

    // Slow sqrt for integers.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto int_sqrt_slow(const int_t &num) noexcept -> int_t
    {
        if (!(int_t{} < num))
        {
            assert(!(num < int_t{}));

            return {};
        }

#ifdef __clang__
        const
#else
        constexpr
#endif
            auto initial_right =
                static_cast<int_t>(std::sqrt(static_cast<long double>(std::numeric_limits<int_t>::max())));

        int_t left{ 1 };
        auto right = initial_right;

        while (left + 1 < right)
        {
            assert(left < right);
            // [[assume(left < right)]];

            auto mid = std::midpoint(left, right);

            if (mid * mid <= num)
            {
                left = mid;
            }
            else
            {
                right = mid;
            }
        }

        return right * right <= num ? right : left;
    }

    // One sqrt for both integers and floating numbers.
    template<class int_t>
    requires(std::is_arithmetic_v<int_t>)
    [[nodiscard]] constexpr auto sqrt(const int_t &num) noexcept -> int_t
    {
        if constexpr (is_integral_pure<int_t>)
        {
            return int_sqrt<int_t>(num);
        }

        return sqrt_newton<int_t>(num);
    }
} // namespace Standard::Algorithms::Numbers
