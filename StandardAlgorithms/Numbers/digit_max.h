#pragma once
#include<cstddef>
#include<cstdint>
#include<limits>

namespace Standard::Algorithms::Numbers
{
    // 9E18 < 2**63 - 1.
    constexpr auto max_number_digits = 18;

    constexpr auto digit_max = 9;
    constexpr auto digit_ten = 10;
    static_assert(digit_max < digit_ten);

    constexpr auto digit_prime_max = 7;

    static_assert(2 <= digit_prime_max && digit_prime_max <= digit_max);
} // namespace Standard::Algorithms::Numbers
