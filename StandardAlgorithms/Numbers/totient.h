#pragma once
#include"../Utilities/iota_vector.h"
#include"../Utilities/require_utilities.h"
#include"factoring_utilities.h"

namespace Standard::Algorithms::Numbers
{
    // How many integers [1..n] are relatively prime to n.
    // E.g. n=12, there are 4 integers {1,5,7,11}, exclude {2,3,4,6,8,9,10,12}.
    //  n=1, there is 1 integer {1}.
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto euler_phi_totient(int_t value, std::vector<int_t> &prime_factors) -> int_t
    {
        require_positive(value, "value");

        decompose_into_prime_divisors<long_int_t, int_t>(value, prime_factors);

        [[maybe_unused]] constexpr int_t zero{};

        for (const auto &factor : prime_factors)
        {
            value -= value / factor;
            assert(zero < value);
        }

        return value;
    }

    // Euler's totient function for all integers in [1..n_max].
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto euler_phi_totient_all(const int_t &n_max) noexcept(false) -> std::vector<int_t>
    {
        require_positive(n_max, "n_max");

        auto tots = Standard::Algorithms::Utilities::iota_vector<int_t>(n_max + static_cast<long_int_t>(1));

        for (int_t prime = 2; prime <= n_max; ++prime)
        {
            if (tots[prime] != prime)
            {
                continue;
            }

            // A prime number.
            auto mult = static_cast<long_int_t>(prime);

            do
            {
                tots[mult] -= tots[mult] / prime;
            } while ((mult += prime) <= n_max);
        }

        return tots;
    }
} // namespace Standard::Algorithms::Numbers
