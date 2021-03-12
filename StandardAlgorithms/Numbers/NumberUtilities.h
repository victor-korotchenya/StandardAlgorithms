#pragma once
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <vector>
#include "Arithmetic.h"
#include "../Utilities/ExceptionUtilities.h"

template <class T, class T2>
void product_modulo(const T a, T& result, const T2 modulus)
{
    static_assert(sizeof(T2) <= sizeof(T));
    result = (a * result) % modulus;
}

// Add/sum modulo.
template <typename T>
void sum_modulo(const T a, T& result, const T modulus)
{
    result = (a + result) % modulus;
}

// Take the square root of a long long integer.
template <typename int_t>
int_t get_sqrt(const int_t n)
{
    static_assert(std::is_integral_v<int_t>);
    assert(n >= 0);
    if constexpr (sizeof(int_t) <= 4)
    {
        const auto s = sqrt(n);
        const auto r = static_cast<int_t>(s);
        return r;
    }
    else
    {
        auto r = static_cast<int_t>(sqrtl(static_cast<long double>(n)));
        while (r * r > n)
            --r;
        while ((r + 1) * (r + 1) > 0 && (r + 1) * (r + 1) <= n)
            ++r;
        return r;
    }
}
// Slow
template <typename int_t>
int_t get_sqrt_slow(const int_t n)
{
    static_assert(sizeof(int_t) >= 8);
    assert(n >= 0);
    int_t left = 1, right = static_cast<int_t>(sqrtl(static_cast<long double>(std::numeric_limits<int_t>::max())));
    while (left + 1 < right)
    {
        const int_t m = (left + right) >> 1;
        if (m * m <= n)
            left = m;
        else
            right = m;
    }
    return right * right <= n ? right : left;
}

// Only for small powers!
template<class int_t, class int_t2>
int_t2 power_inf_with_overflow_check(const int_t2 a, int_t p, const int_t2 maxv = 1000ll * 1000ll * 1000ll * 1000ll * 1000ll * 1000ll)
{
    static_assert(sizeof(int_t) <= sizeof(int_t2), "types");
    assert(a > 0 && p >= 0 && p <= 60 && maxv < std::numeric_limits<int_t2>::max());
#ifndef __int128
    // todo: p1. Why is a simple cycle faster than exponentiation?
    int_t2 r = 1;
    const auto lim = maxv / a;
    while (p--)
    {
        if (r > lim)
            return maxv + 1;
        r *= a;
    }
    return r;
#else
    __int128 r = 1;
    while (p--)
        r *= a;
    return r > maxv ? static_cast<int_t2>(maxv + 1) : static_cast<int_t2>(r);
#endif
}

// Slow
// todo: test it.
// In case of overflow, return n_inf
template<class int_t, class int_t2>
int_t2 power_inf_with_overflow_check_slow(int_t2 a, int_t p,
    const int_t2 maxv = 2ll * 1000ll * 1000ll * 1000ll * 1000ll * 1000ll * 1000ll)
{
    static_assert(sizeof(int_t) <= sizeof(int_t2));
    assert(a > 0 && p >= 0);
    int_t2 r = 1;
    const auto lim = maxv / a;
    while (p)
    {
        if (p & 1)
        {
            if (r > lim)
                return maxv;
            r *= a;
        }

        p >>= 1;

        if (p && a > lim)
            return maxv;

        a *= a;
    }
    return r;
}

// Slow.
// sqrt
// todo: test it.
template <typename int_t, typename int_t2>
int_t2 pow_root_slow(const int_t2 b, const int_t p)
{
    static_assert(sizeof(int_t) <= sizeof(int_t2));
    assert(b > 0 && p > 0);
    if (p >= 60) return 1;
    if (p == 1) return b;

    auto r = static_cast<int_t2>(powl(static_cast<long double>(b), 1.0 / static_cast<long double>(p)));
    while (power_inf_with_overflow_check_slow(r, p) <= b)
        ++r;

#ifdef _DEBUG
    if (p == 2)
    {
        const auto g = get_sqrt(b);
        assert(g == r - 1);
    }
#endif

    return r - 1;
}

//Return the total number of numbers from 1 to number inclusively,
// having the reminder == "reminder".
//
// "reminder" must belong to [0, divisor).
// "number" and "divisor" must be positive.
template <typename int_t, int_t divisor>
int_t numbers_having_reminder(const int_t reminder, const int_t number)
{
    static_assert(0 < divisor, "The divisor must be strictly positive.");
#ifdef _DEBUG
    if (reminder < 0)
    {
        std::string message = "The reminder (" + std::to_string(reminder)
            + ") must be non-negative";
        throw std::runtime_error(message.c_str());
    }
    if (divisor <= reminder)
    {
        std::string message = "The reminder (" + std::to_string(reminder)
            + ") must be smaller than divisor(" + std::to_string(divisor) + ")";
        throw std::runtime_error(message.c_str());
    }
    if (number <= 0)
    {
        std::string message = "The number (" + std::to_string(number)
            + ") must be strictly positive";
        throw std::runtime_error(message.c_str());
    }
#endif
    auto ret = (number - reminder + divisor) / divisor - (0 == reminder);
    return ret;
}

//It is assumed that beginInclusive <= endInclusive,
// and the "divisor" is not zero.
template<typename int_t>
int_t divisors_in_range(const int_t& beginInclusive, const int_t& endInclusive,
    const int_t& divisor)
{
    assert(divisor > 0);
    const auto result = endInclusive / divisor - (beginInclusive - 1) / divisor;
    return result;
}

// Moebius function is defined for an integer n >0:
// -1 when n is a product of an even number of distinct primes e.g. 30=2*3*5, 7=7**1.
// +1 when n is a product of an odd number of distinct primes e.g. 1=2**0, 15=3*5.
// 0 when n is not square-free e.g. n=12 is divisible by 2**2=4, 243=3**5 divisible by 9.
template<typename int_t>
std::vector<signed char> mobius(const int_t size)
{
    assert(size > 0);
    std::vector<signed char> mob(size + 1ll);
    mob[1] = 1;

    for (int_t i = 1; i <= size; ++i)
        if (mob[i])
            for (auto j = i << 1; j <= size; j += i)
                mob[j] -= mob[i];
    return mob;
}

//The "modulus" must be not zero.
// modular_exponentiation, pow, power.
template<typename int_t, typename int_t2, typename int_t3>
int_t modular_power(int_t base, int_t2 exponent, const int_t3 modulus)
{
    static_assert(sizeof(int_t3) <= sizeof(int_t));
    assert(0 < modulus);

    base = base % modulus;
    if (0 == base)
        return 0;

    int_t result = 1;
    while (exponent)
    {
        if (exponent & 1)
        {//Particular bit is set in the "exponent".
            product_modulo<int_t>(base, result, modulus);
        }

        product_modulo<int_t>(base, base, modulus);
        exponent >>= 1;
    }
    return result;
}

//Sum of powers from 0 to "power_exclusive - 1".
//
//E.g. for modulus==2,
//given power_exclusive, result is:
// 0, 0
// 1, 1
// 2, 3
// 3, 7
// 4, 15
// 10, 1023
template <typename T, T modulus>
T powers_sum(T base, T power_exclusive)
{
    static_assert(0 < modulus, "The modulus must be positive.");

    if (0 == base)
    {
        return 0;
    }

    T result = 0;
    T A = 1;
    T digit = 1;
    while (power_exclusive)
    {
        if (power_exclusive & digit)
        {
            power_exclusive &= ~digit;
            sum_modulo<T>(
                (A * modular_power(base, power_exclusive, modulus)) % modulus,
                result, modulus);
        }

        sum_modulo<T>(
            ((A * modular_power(base, digit, modulus)) % modulus),
            A, modulus);
        digit <<= 1;
    }

    return result;
}

template<typename int_t>
int_t gcd(int_t a, int_t b)
{
    while (b)
    {
        const auto t = b;
        b = a % b;
        a = t;
    }
    return a;
}

template<typename int_t>
int_t gcd_unsigned(int_t a, int_t b)
{
    static_assert(std::is_unsigned_v<int_t>);
    if (0 == a)
        return b;
    if (0 == b || a == b)
        return a;

    auto twos = 0;
    while (0 == ((a | b) & 1))
    {//Both even.
        a >>= 1;
        b >>= 1;
        ++twos;
    }

    while ((a & 1) == 0)
        a >>= 1;
    // a is odd.
    do
    {
        while ((b & 1) == 0)
            b >>= 1;

        if (a > b)
            std::swap(a, b);

        b -= a;
    } while (b);

    const int_t result = a << twos;
    return result;
}

// Checked in 2019
//
//Return gcd(a, b), finding x and y that satisfy (a*x + b*y) = gcd(a, b).
// b = (b/a) * a + (b % a)
//     (b % a) = b - (b/a) * a
// g = (b % a) * x2 + a * y2 = (b - (b/a) * a) * x2 + a * y2 =
//   = a * (y2 - (b/a) * x2) + b * x2
template<typename int_t>
int_t gcd_extended_simple(int_t a, int_t b, int_t& x, int_t& y)
{
    if (0 == a)
    {
        x = 0;
        y = 1;
        return b;
    }

    const auto quotient = b / a, remainder = b % a,
        g = gcd_extended_simple(remainder, a, y, x);

    x -= quotient * y;
    return g;
}

//Return gcd(a, b), finding x and y that satisfy (a*x + b*y) = gcd(a, b).
template<typename int_t>
int_t gcd_extended(int_t a, int_t b, int_t& x, int_t& y)
{
    if (0 == a)
    {
        x = 0;
        y = 1;
        return b;
    }

    x = 1;
    y = 0;
    if (0 == b)
        return a;

    auto twos = 0;
    while (0 == ((a | b) & 1))
    {//Both even.
        a >>= 1;
        b >>= 1;
        ++twos;
    }

    int_t remainder_x = 0, remainder_y = 1;
    do
    {
        const auto old_b = b, quotient = a / b;
        b = a % b;
        a = old_b;

        const auto old_remainder_x = remainder_x;
        remainder_x = x - (quotient * remainder_x);
        x = old_remainder_x;

        const auto old_remainder_y = remainder_y;
        remainder_y = y - (quotient * remainder_y);
        y = old_remainder_y;
    } while (b);

    const int_t result = a << twos;
    return result;
}

template <typename int_t>
void make_positive(int_t& a, const int_t& Modulus)
{
    a = int_t(((a % Modulus) + Modulus) % Modulus);
}

// r * a = 1 mod m, where r is called modular inverse of a modulo m.
//
// m = (m/a) * a + (m % a)
// (m % a) = m - (m/a) * a =
//         = m - (m/a) * a + a*m - a*m =
//         = a * (m - (m/a)) + m - a*m
// Take mod m.
//
// [a * (m - (m/a))] % m = (m % a) % m
// inv(a) = [(m - (m/a)) * inv(m % a)] % m
template <typename int_t, typename int_t2 = int_t>
void modular_inverses_first_n(const int_t n, const int_t Modulus_prime, std::vector<int_t>& result)
{
    static_assert(sizeof(int_t2) >= sizeof(int_t));
    if (n <= 0) throw std::runtime_error("n must be positive.");

    result.resize(n + 1);
    result[0] = result[n] = 0;
    result[1] = 1;
    for (auto i = 2; i < n; ++i)
    {
        // inv(a) = [(m - (m/a)) * inv(m % a)] % m
        const int_t2 mma = Modulus_prime - (Modulus_prime / i);
        const auto remainder = Modulus_prime % i;
        const auto prod = mma * result[remainder];
        result[i] = static_cast<int_t>(prod % Modulus_prime);
    }
}

// todo: move to prime_utils.h

// Factorize. It is assumed that (1 <= n).
// It is good only for a few calls, slow for many.
// See also decompose_eratosthenes_sieve_factoring
template<class int_t2, class int_t,
    class divisor_power = std::pair<int_t, int_t>>
    void decompose_into_divisors_with_powers(int_t n, std::vector<divisor_power>& components)
{
    static_assert(sizeof(int_t) <= sizeof(int_t2));
    assert(n > 0);

    components.clear();
    if (n <= 1)
        return;

    if (!(n & 1))
    {
        components.emplace_back(2, 0);
        do
        {
            ++(components.back().second);
            n >>= 1;
            assert(n > 0);
        } while (!(n & 1));
    }

    for (int_t c = 3; static_cast<int_t2>(c) * c <= n; c += 2)
    {
        if (n % c)
            continue;

        components.emplace_back(c, 0);

        do
        {
            ++(components.back().second);
            n /= c;
            assert(n > 0);
        } while (0 == n % c);
    }

    if (n > 1)
        components.push_back({ n, 1 });
}

//It is assumed that (1 <= n).
//It could be faster to go thru the prepared primes list.
//Non-primes are not returned.
template<class int_t2, class int_t>
void decompose_into_prime_divisors(int_t n, std::vector<int_t>& prime_factors)
{
    static_assert(sizeof(int_t) <= sizeof(int_t2));
    assert(n > 0);

    prime_factors.clear();
    if (n <= 1)
        return;

    if (!(n & 1))
    {
        prime_factors.push_back(2);
        do n >>= 1;
        while (!(n & 1));
    }

    for (int_t c = 3; static_cast<int_t2>(c) * c <= n; c += 2)
    {
        if (n % c)
            continue;

        prime_factors.push_back(c);
        do n /= c;
        while (0 == n % c);
    }

    if (n > 1)
        prime_factors.push_back(n);
}

template <typename int_t>
void all_divisors(const int_t n, std::vector<int_t>& factors)
{
    assert(n > 0);
    factors.clear();
    factors.push_back(1);
    if (n <= 1)
        return;

    const auto m = static_cast<int_t>(sqrt(n));
    if (1 < m)
    {
        for (int_t i = 2; i < m; ++i)
        {
            if (n % i)
                continue;

            factors.push_back(i);
            factors.push_back(n / i);
        }

        if (0 == n % m)
        {
            factors.push_back(m);
            if (n / m != m)
                factors.push_back(n / m);
        }
    }

    factors.push_back(n);
    std::sort(factors.begin(), factors.end());
}

template <typename int_t>
void all_divisors_slow(const int_t n, std::unordered_set<int_t>& factors)
{
    assert(n > 0);
    factors.clear();

    for (int_t i = 1; i <= n; ++i)
    {
        if (n % i)
            continue;

        factors.insert(i);
        factors.insert(n / i);
    }
}

template<typename int_t,
    typename divisor_power_t = std::pair<int_t, int_t>>
    void decompose_into_divisors_with_powers_slow(int_t n, std::vector<divisor_power_t>& components)
{
    assert(n >= 0);

    const auto upper = get_sqrt(n);
    for (int_t i = 2; i <= upper; ++i)
    {
        if (n % i)
            continue;

        typename divisor_power_t::second_type p = 0;
        do ++p, n /= i;
        while (0 == n % i);

        components.emplace_back(i, p);
        if (1 == n)
            return;
    }

    if (1 < n)
        components.push_back(std::make_pair(n, 1));
}

//How many integers [1..n] are relatively prime to n.
//E.g. n=12, there are 4 integers {1,5,7,11}, exclude {2,3,4,6,8,9,10,12}.
// n=1, there is 1 integer {1}.
template<class int_t2, class int_t>
int_t Euler_phi_totient(int_t a, std::vector<int_t>& prime_factors)
{
    static_assert(sizeof(int_t) <= sizeof(int_t2));
    assert(a > 0);

    decompose_into_prime_divisors<int_t2, int_t>(a, prime_factors);

    for (const auto& factor : prime_factors)
    {
        a -= a / factor;
        assert(a > 0);
    }

    return a;
}

// Euler's totient function for all integers in [1..n_max].
template<class int_t2, class int_t>
std::vector<int_t> euler_phi_totient_all(const int_t n_max)
{
    static_assert(sizeof(int_t) <= sizeof(int_t2));
    RequirePositive(n_max, "n_max");

    std::vector<int_t> tots(n_max + int_t2(1));
    std::iota(tots.begin(), tots.end(), int_t());

    for (int_t prime = 2; prime <= n_max; ++prime)
    {
        if (tots[prime] != prime)
            continue;

        // A prime number.
        auto i = static_cast<int_t2>(prime);
        do
            tots[i] -= tots[i] / prime;
        while ((i += prime) <= n_max);
    }

    return tots;
}

//When modulus is a prime number.
template<typename int_t>
int_t modulo_inverse_simple(int_t a, int_t modulus)
{
    //Fermat's theorem: 1 (mod) === power(a, mod-1).
    //Divide by a: power(a, -1) === power(a, mod-2).
    auto result = modular_power(a, modulus - 2, modulus);
    return result;
}
//TODO: p1. check it!
template <typename int_t>
int_t modulo_inverse(int_t a, const int_t& Modulus)
{
    assert(0 < Modulus);// "Modulus must be positive."

    make_positive<int_t>(a, Modulus);

    int_t b = Modulus, result = 1, remainder = 0;
    while (1 < a)
    {
        const auto old_b = b, quotient = int_t(a / b);
        b = int_t(a % b);
        a = old_b;

        const auto old_remainder = remainder;
        remainder = int_t(result - (quotient * remainder));
        result = old_remainder;
    }

    make_positive<int_t>(result, Modulus);
    return result;
}

template <class int_t2, class int_t, int_t Modulus>
int_t modulo_inverse_slow_but_checked(int_t a, std::vector<int_t>& prime_factors)
{
    static_assert(0 < Modulus, "Modulus must be positive.");
    make_positive<int_t>(a, Modulus);

    const auto phi = Euler_phi_totient<int_t2, int_t>(Modulus, prime_factors);

    const auto result = modular_power(a, phi - 1, Modulus);
    return result;
}

//To subtract spaces, 0x30 should be replicated eight times.
template<unsigned long long subtrahend = 0x3030303030303030ull>
void multi_byte_subtract(unsigned long long& minuend)
{
    static_assert(0 == (subtrahend & 0x8080808080808080ull),
        "The subtrahend must have no high bit set.");

    const auto avoidOverflow = (minuend | 0x8080808080808080ull)
        - subtrahend;

    const auto highBits = minuend | 0x7F7F7F7F7F7F7F7Full;
    minuend = ~(avoidOverflow ^ highBits);
}

//To find first byte not greater than 9,
// (0x7F - 9) - 0x76 should be replicated eight times.
template<unsigned long long mask = 0x7676767676767676ull>
unsigned find_first_byte(const unsigned long long& value)
{
    static_assert(0 == (mask & 0x8080808080808080ull),
        "The mask must have no high bit set.");

    const auto eighties = ~(((value & 0x7F7F7F7F7F7F7F7Full) + mask)
        | value
        | 0x7F7F7F7F7F7F7F7Full);

    const auto result = 0 == eighties ? 8 : (MostSignificantBit64(eighties) >> 3);
    return result;
}