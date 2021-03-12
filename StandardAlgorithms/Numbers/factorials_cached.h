#pragma once
#include"default_modulus.h"
#include"number_utilities.h" // modular_power
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Compute many C(n, m) quickly when 0 <= n, m <= size.
    template<std::unsigned_integral int_t = std::uint32_t, std::unsigned_integral long_int_t = std::uint64_t,
        int_t modulus = Standard::Algorithms::Numbers::default_modulus>
    requires(sizeof(int_t) <= sizeof(long_int_t) && int_t{} < modulus)
    struct factorials_cached final
    {
        constexpr explicit factorials_cached(const int_t &size)
            : Factorials(verify_size(size) + 1U)
            , Factorial_inverses(Factorials.size())
        {
            assert(0U < size && size < std::numeric_limits<int_t>::max());

            Factorials.at(0) = 1U;

            for (long_int_t fact = 1, index = 1; index <= size; ++index)
            {
                fact = (fact * index) % modulus;

                Factorials[index] = static_cast<int_t>(fact);
            }

            {
                const auto last_factorial = static_cast<long_int_t>(Factorials[size]);

                // Fermat's theorem: 1 (mod) === power(val, mod-1).
                // Divide by val to get val's modulo inverse: power(a, -1) === power(a, mod-2).

                Factorial_inverses[size] = static_cast<int_t>(modular_power(last_factorial, modulus - 2U, modulus));
            }

            for (long_int_t index = size; 0U < index; --index)
            {
                Factorial_inverses[index - 1U] = static_cast<int_t>(Factorial_inverses[index] * index % modulus);
            }
        }

        // (n choose m) modulo mod.
        [[nodiscard]] constexpr auto choose_modulo(const int_t &nnn, const int_t &mmmm) const -> int_t
        {
            check(nnn);
            check(mmmm);

            if (nnn < mmmm)
            {
                return {};
            }

            auto res = static_cast<int_t>(static_cast<long_int_t>(Factorials[nnn]) * Factorial_inverses[mmmm] %
                modulus * Factorial_inverses[nnn - mmmm] % modulus);

            assert(int_t{} < res);

            return res;
        }

        [[nodiscard]] constexpr auto factorial_modulo(const int_t &value) const & -> const int_t &
        {
            check(value);

            return Factorials[value];
        }

        [[nodiscard]] constexpr auto inverse_factorial_modulo(const int_t &value) const & -> const int_t &
        {
            check(value);

            return Factorial_inverses[value];
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Factorials.size() - 1U;
        }

private:
        [[nodiscard]] static constexpr auto verify_size(const int_t &size) -> const int_t &
        {
            require_positive(size, "size");

            {
                const auto size_plus_one = static_cast<int_t>(size + 1U);

                require_positive(size_plus_one, "size + 1 overflows");
            }

            return size;
        }

        inline constexpr void check(const int_t &value) const
        {
            require_greater(Factorials.size(), value, "value");
        }

        std::vector<int_t> Factorials;
        std::vector<int_t> Factorial_inverses;
    };
} // namespace Standard::Algorithms::Numbers
