#pragma once
#include<cassert>
#include<concepts>
#include<cstdint>
#include<utility> // std::swap

namespace Standard::Algorithms::Numbers
{
    // See also Euclid's std::gcd defined in the <numeric> header.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto gcd_int(int_t aaa, int_t bbb) noexcept -> int_t
    {
        constexpr int_t zero{};

        while (zero != bbb)
        {
            auto tampa = bbb;
            bbb = aaa % bbb;
            aaa = tampa;
        }

        return aaa;
    }

    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto gcd_unsigned(int_t aaa, int_t bbb) noexcept -> int_t
    {
        if (0U == aaa)
        {
            return bbb;
        }

        if (0U == bbb || aaa == bbb)
        {
            return aaa;
        }

        std::uint32_t shifts{};

        while (0U == ((aaa | bbb) & 1U))
        {// Both even.
            aaa >>= 1U;
            bbb >>= 1U;
            ++shifts;

            assert(0U < aaa && 0U < bbb && 0U < shifts);
        }

        while ((aaa & 1U) == 0U)
        {
            aaa >>= 1U;
        }

        // aaa is odd.
        do
        {
            while ((bbb & 1U) == 0U)
            {
                bbb >>= 1U;
            }

            if (bbb < aaa)
            {
                std::swap(aaa, bbb);
            }

            bbb -= aaa;
        } while (0U < bbb);

        aaa <<= shifts;
        return aaa;
    }

    // Checked in 2019.
    //
    // Return gcd(a, b), finding x and y that satisfy (a*x + b*y) = gcd(a, b).
    // b = (b/a) * a + (b % a)
    //     (b % a) = b - (b/a) * a
    // g = (b % a) * x2 + a * y2 = (b - (b/a) * a) * x2 + a * y2 =
    //   = a * (y2 - (b/a) * x2) + b * x2
    template<std::integral int_t>
    // todo(p5): change the signature?
    [[nodiscard]] constexpr auto gcd_extended_simple(int_t aaa, int_t bbb, int_t &xxxxx, int_t &yyyy) noexcept -> int_t
    {
        constexpr int_t zero{};

        if (zero == aaa)
        {
            xxxxx = zero;
            yyyy = 1;

            return bbb;
        }

        const auto quotient = static_cast<int_t>(bbb / aaa);
        const auto remainder = static_cast<int_t>(bbb % aaa);
        auto gcd1 = gcd_extended_simple<int_t>(remainder, aaa, yyyy, xxxxx);

        xxxxx -= static_cast<int_t>(quotient * yyyy);

        return gcd1;
    }

    // Return gcd(a, b), finding x and y that satisfy (a*x + b*y) = gcd(a, b).
    template<std::integral int_t>
    // todo(p5): change the signature?
    // NOLINTNEXTLINE
    [[nodiscard]] constexpr auto gcd_extended(int_t aaa, int_t bbb, int_t &xxxxx, int_t &yyyy) noexcept -> int_t
    {
        constexpr int_t zero{};

        if (zero == aaa)
        {
            xxxxx = zero;
            yyyy = 1;

            return bbb;
        }

        xxxxx = 1;
        yyyy = zero;

        if (zero == bbb)
        {
            return aaa;
        }

        std::uint32_t shifts{};

        while (zero == ((aaa | bbb) & 1))
        {// Both even.
            aaa >>= 1U;
            bbb >>= 1U;
            ++shifts;

            assert(zero != aaa && zero != bbb && 0U < shifts);
        }

        int_t remainder_x{};
        int_t remainder_y = 1;

        do
        {
            const auto old_b = bbb;
            const auto quotient = static_cast<int_t>(aaa / bbb);
            bbb = static_cast<int_t>(aaa % bbb);
            aaa = old_b;

            const auto old_remainder_x = remainder_x;
            remainder_x = static_cast<int_t>(xxxxx - (quotient * remainder_x));
            xxxxx = old_remainder_x;

            auto old_remainder_y = remainder_y;
            remainder_y = static_cast<int_t>(yyyy - (quotient * remainder_y));
            yyyy = old_remainder_y;
        } while (zero != bbb);

        aaa <<= shifts;

        return aaa;
    }
} // namespace Standard::Algorithms::Numbers
