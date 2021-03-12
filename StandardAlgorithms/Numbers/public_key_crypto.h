#pragma once
// "public_key_crypto.h"
#include"../Utilities/require_utilities.h"
#include"gcd.h"
#include"modulo_inverse.h"
#include"number_utilities.h" // modular_power
#include"prime_number_utility.h" // is_prime_simple
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Public key: {primes_product, public_prime}.
    // Private key (secret key): {primes_product, private_inverse}.
    template<std::integral int_t>
    struct rsa_keys final
    {// NOLINTNEXTLINE
        int_t private_prime1{}; // NOLINTNEXTLINE
        int_t private_prime2{}; // NOLINTNEXTLINE
        int_t primes_product{}; // private_prime1 * private_prime2.

        // NOLINTNEXTLINE
        int_t public_prime{}; // Relatively prime to (private_prime1 - 1) * (private_prime2 - 1).

        // NOLINTNEXTLINE
        int_t private_inverse{}; // The inverse of public_prime modulo (private_prime1 - 1) * (private_prime2 - 1).

        [[nodiscard]] constexpr auto operator<=> (const rsa_keys &) const noexcept = default;
    };

    // Of course, the keys should not be printed for everyone's eyes to see.
    template<std::integral int_t>
    auto operator<< (std::ostream &str, const rsa_keys<int_t> &keys) -> std::ostream &
    {
        str << keys.private_prime1 << " * " << keys.private_prime2 << " = " << keys.primes_product << ", "
            << keys.public_prime << " has inverse " << keys.private_inverse;

        return str;
    }

    template<std::integral int_t>
    constexpr void validate_prime(const std::string &name, const int_t &prime)
    {
        assert(!name.empty());

        {
            constexpr int_t two{ 2 };
            require_greater(prime, two, name);
        }
        {
            const auto isprim = is_prime_simple(prime);
            require_equal(true, "is prime " + name, isprim);
        }
    }

    template<std::integral int_t>
    constexpr void validate(const rsa_keys<int_t> &keys)
    {
        validate_prime<int_t>("private prime 1", keys.private_prime1);
        validate_prime<int_t>("private prime 2", keys.private_prime2);
        validate_prime<int_t>("public prime", keys.public_prime);

        if (keys.private_prime1 == keys.private_prime2) [[unlikely]]
        {
            throw std::runtime_error("private primes must be distinct");
        }

        {
            const auto temp_prod = static_cast<int_t>(keys.private_prime1 * keys.private_prime2);

            require_equal(keys.primes_product, "private primes product", temp_prod);

            {
                [[maybe_unused]] constexpr int_t two{ 2 };
                assert(two < keys.private_prime1);
            }

            const auto divi = static_cast<int_t>(temp_prod / keys.private_prime1);

            require_equal(keys.private_prime2, "private primes product / 1st prime", divi);
        }

        constexpr int_t one{ 1 };
        constexpr int_t two{ 2 };
        {
            require_greater(keys.primes_product, two, "primes product");
            require_greater(keys.private_inverse, one, "private inverse");
        }
        {
            const auto phi = static_cast<int_t>(keys.primes_product + one - keys.private_prime1 - keys.private_prime2);
            require_greater(phi, two, "phi");

            const auto produ = static_cast<int_t>(keys.private_inverse * keys.public_prime);
            const auto produ_mod = static_cast<int_t>(produ % phi);

            require_equal(one, "public prime * private inverse % mod", produ_mod);
        }
    }

    namespace Inner
    {
        template<std::integral int_t, std::size_t max_attempts>
        requires(0U < max_attempts)
        [[nodiscard]] constexpr auto gen_public_prime(auto &gena, const int_t &phi, rsa_keys<int_t> &keys) -> bool
        {
            constexpr int_t one{ 1 };
            [[maybe_unused]] constexpr int_t two{ 2 };

            assert(two < keys.private_prime1 && two < keys.private_prime2 && two < phi);
            assert(keys.private_prime1 != keys.private_prime2);
            assert(keys.primes_product == keys.private_prime1 * keys.private_prime2);

            for (std::size_t attempt{}; attempt < max_attempts; ++attempt)
            {
                keys.public_prime = gena();
                assert(two < keys.public_prime);

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(is_prime_simple(keys.public_prime));
                }

                const auto gcd = gcd_int<int_t>(phi, keys.public_prime);
                if (one == gcd)
                {
                    assert(keys.private_prime1 != keys.public_prime && keys.private_prime2 != keys.public_prime);

                    return true;
                }
            }

            return false;
        }
    } // namespace Inner

    constexpr auto default_gen_rsa_keys_max_attempts = 100U;

    // Watch out for an overflow.
    template<std::integral int_t, std::size_t max_attempts = default_gen_rsa_keys_max_attempts>
    requires(0U < max_attempts)
    [[nodiscard]] constexpr auto gen_rsa_keys(auto &gena) -> rsa_keys<int_t>
    {
        constexpr int_t one{ 1 };
        [[maybe_unused]] constexpr int_t two{ 2 };

        rsa_keys<int_t> keys{};

        for (std::size_t attempt{}; attempt < max_attempts; ++attempt)
        {
            keys.private_prime1 = gena();
            keys.private_prime2 = gena();
            assert(two < keys.private_prime1 && two < keys.private_prime2);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(is_prime_simple(keys.private_prime1) && is_prime_simple(keys.private_prime2));
            }

            if (keys.private_prime1 == keys.private_prime2)
            {
                continue;
            }

            keys.primes_product = keys.private_prime1 * keys.private_prime2;

            // (private_prime1 - 1) * (private_prime2 - 1)
            // private_prime1 * private_prime2 + 1 - private_prime1 - private_prime2
            const auto phi = static_cast<int_t>(keys.primes_product + one - keys.private_prime1 - keys.private_prime2);
            assert(one < phi);

            if (!Inner::gen_public_prime<int_t, max_attempts>(gena, phi, keys))
            {
                continue;
            }

            assert(two < keys.public_prime);

            keys.private_inverse = modulo_inverse<int_t>(keys.public_prime, phi);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                validate<int_t>(keys);
            }

            return keys;
        }

        auto err = "All " + std::to_string(max_attempts) + " attempts to generate RSA keys have failed.";

        throw std::out_of_range(err);
    }

    template<std::integral int_t>
    constexpr void encrypt_rsa(const int_t &plain_text, const rsa_keys<int_t> &keys, int_t &cypher_text)
    {
        assert(!(plain_text < int_t{}));

        const auto &exponent = keys.public_prime;
        const auto &modulus = keys.primes_product;
        assert(int_t{} < exponent && int_t{} < modulus);

        require_greater(keys.primes_product, plain_text, "Max key (primes product) vs. plain text");

        if constexpr (::Standard::Algorithms::is_debug)
        {
            validate<int_t>(keys);
        }


        cypher_text = modular_power<int_t, int_t, int_t>(plain_text, exponent, modulus);
    }

    template<std::integral int_t>
    constexpr void decrypt_rsa(const int_t &cypher_text, const rsa_keys<int_t> &keys, int_t &plain_text)
    {
        assert(!(cypher_text < int_t{}));

        const auto &exponent = keys.private_inverse;
        const auto &modulus = keys.primes_product;
        assert(int_t{} < exponent && int_t{} < modulus);

        require_greater(keys.primes_product, cypher_text, "Max key (primes product) vs. plain text");

        if constexpr (::Standard::Algorithms::is_debug)
        {
            validate<int_t>(keys);
        }

        plain_text = modular_power<int_t, int_t, int_t>(cypher_text, exponent, modulus);
    }
} // namespace Standard::Algorithms::Numbers
