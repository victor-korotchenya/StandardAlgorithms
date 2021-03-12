#pragma once
#include"../Utilities/is_debug.h"
#include"../Utilities/same_sign_leq_size.h"
#include<cassert>
#include<compare>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<ostream>
#include<type_traits>

namespace Standard::Algorithms::Numbers
{
    template<std::integral int_t, int_t mod, std::integral long_int_t>
    constexpr bool are_number_mod_ints = same_sign_leq_size<int_t, long_int_t> && 2 <= mod;

    template<std::integral int_t1, int_t1 mod, std::integral long_int_t1>
    requires(are_number_mod_ints<int_t1, mod, long_int_t1>)
    struct number_mod final
    {
        using int_t = int_t1;
        using long_int_t = long_int_t1;

        static constexpr auto is_signed1 = std::is_signed_v<long_int_t>;

        template<std::integral long_int_t2 = long_int_t>
        requires(std::convertible_to<long_int_t2, long_int_t>)
        // Bitte kein explicit hier.
        // NOLINTNEXTLINE // cppcheck-suppress [noExplicitConstructor]
        explicit(false) constexpr number_mod(const long_int_t2 &num_ = {})
            : Value(static_cast<int_t>(num_ % mod))
        {
            if constexpr (is_signed1)
            {
                if (Value < int_t{})
                {
                    Value += mod;
                }
            }

            self_check();
        }

        [[nodiscard]] constexpr auto value() const &noexcept -> const int_t &
        {
            self_check();

            return Value;
        }

        [[nodiscard]] constexpr auto operator() () const &noexcept -> const int_t &
        {
            return value();
        }

        [[nodiscard]] static constexpr auto modulo() noexcept
        {
            return mod;
        }

        [[nodiscard]] inline constexpr auto operator<=> (const number_mod &) const noexcept = default;

        inline friend auto operator<< (std::ostream &str, const number_mod &num) -> std::ostream &
        {
            num.self_check();

            str << num.Value;
            return str;
        }

        constexpr auto operator+= (const number_mod &num) & -> number_mod &
        {
            self_check();
            num.self_check();

            Value += num.Value;

            if (!(Value < mod))
            {
                Value -= mod;
            }

            self_check();

            return *this;
        }

        constexpr auto operator*= (const number_mod &num) & -> number_mod &
        {
            self_check();
            num.self_check();

            Value = static_cast<int_t>((static_cast<long_int_t>(Value) * static_cast<long_int_t>(num.Value)) % mod);

            self_check();

            return *this;
        }

private:
        constexpr void self_check() const noexcept
        {
            assert(!(Value < int_t{}) && Value < mod);
        }

        int_t Value;
    };

    template<class num_mod_t, std::integral int_t = typename num_mod_t::int_t, int_t mod = num_mod_t::modulo(),
        std::integral long_int_t = typename num_mod_t::long_int_t>
    requires(are_number_mod_ints<int_t, mod, long_int_t>)
    [[nodiscard]] constexpr auto operator+ (const num_mod_t &one, const num_mod_t &two) -> num_mod_t
    {
        auto result = one;
        result += two;
        return result;
    }

    template<class num_mod_t, std::integral int_t = typename num_mod_t::int_t, int_t mod = num_mod_t::modulo(),
        std::integral long_int_t = typename num_mod_t::long_int_t>
    requires(are_number_mod_ints<int_t, mod, long_int_t>)
    [[nodiscard]] constexpr auto operator* (const num_mod_t &one, const num_mod_t &two) -> num_mod_t
    {
        auto result = one;
        result *= two;
        return result;
    }

    // Fast modular exponentiation.
    template<class num_mod_t, std::integral exponent_t, std::integral int_t = typename num_mod_t::int_t,
        int_t mod = num_mod_t::modulo(), std::integral long_int_t = typename num_mod_t::long_int_t>
    requires(are_number_mod_ints<int_t, mod, long_int_t>)
    [[nodiscard]] constexpr auto modular_power(num_mod_t base, exponent_t exponent) -> num_mod_t
    {
        constexpr exponent_t zero_exp{};

        assert(!(exponent < zero_exp));

        if (int_t{} == base.value())
        {
            return {};
        }

        num_mod_t result{ 1 };

        while (zero_exp < exponent)
        {
            if (constexpr exponent_t one = 1; zero_exp != (exponent & one))
            {
                result *= base;
            }

            base *= base;
            exponent >>= 1U;
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
