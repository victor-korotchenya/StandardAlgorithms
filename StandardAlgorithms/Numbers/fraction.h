#pragma once
#include"number_utilities.h" // modular_power
#include<numeric> // std::gcd
#include<string>

namespace Standard::Algorithms::Numbers
{
    // A fraction like -6/5 with overflow checks.
    template<std::signed_integral int_t1>
    struct fraction final
    {
        using int_t = int_t1;

        static constexpr int_t zero{};
        static constexpr int_t one = 1;
        static constexpr int_t minus_1 = zero - one;

        static_assert(minus_1 < zero && zero < one);

        constexpr explicit fraction(
            // Using a pair - not convenient use.
            // NOLINTNEXTLINE
            const int_t &num = zero, const int_t &den = one)
            : Numerator(num)
            , Denominator(den)
        {
            if (Numerator == zero)
            {// Let 0/0 be 0/1, hockey.
                Denominator = one;

                return;
            }

            if (Denominator == zero) [[unlikely]]
            {
                throw std::runtime_error("The fraction denominator cannot be zero.");
            }

            if (const auto gcd1 = std::gcd(Numerator, Denominator); one < gcd1 || gcd1 < minus_1)
            {
                assert(zero == (Numerator % gcd1) && zero == (Denominator % gcd1));

                Numerator /= gcd1;
                Denominator /= gcd1;
            }

            if (!(Denominator < zero))
            {
                return;
            }

            // The case, when both are minimum, has been executed above.
            check_inf_denominator(Denominator);
            check_inf_numerator(Numerator);

            Denominator = -Denominator;
            Numerator = -Numerator;
        }

        [[nodiscard]] constexpr auto numerator() const &noexcept -> const int_t &
        {
            return Numerator;
        }

        [[nodiscard]] constexpr auto denominator() const &noexcept -> const int_t &
        {
            return Denominator;
        }

        [[nodiscard]] constexpr auto value() const &noexcept(false) -> const int_t &
        {
            if (Numerator == zero || Denominator == one) [[likely]]
            {
                return Numerator;
            }

            auto err = "Cannot get the value because denominator " + std::to_string(Denominator);

            throw std::runtime_error(err);
        }

        [[nodiscard]] constexpr auto operator+ (const fraction &fra) const -> fraction
        {
            auto first = multiply(Numerator, fra.Denominator);
            auto second = multiply(Denominator, fra.Numerator);
            auto nume = add(first, second);

            auto denom = multiply(Denominator, fra.Denominator);

            return fraction(nume, denom);
        }

        [[nodiscard]] constexpr auto operator- () const -> fraction
        {
            check_inf_numerator(Numerator);

            return fraction(-Numerator, Denominator);
        }

        [[nodiscard]] constexpr auto operator- (const fraction &fra) const -> fraction
        {
            return (*this) + (-fra);
        }

        [[nodiscard]] constexpr auto operator* (const fraction &fra) const -> fraction
        {
            auto first = multiply(Numerator, fra.Numerator);
            auto second = multiply(Denominator, fra.Denominator);

            return fraction(first, second);
        }

        [[nodiscard]] constexpr auto operator== (const fraction &france) const noexcept -> bool
        {
            return Numerator == france.Numerator && Denominator == france.Denominator;
        }

        [[nodiscard]] constexpr auto operator<(const fraction &fra) const -> bool
        {
            if (Denominator == fra.Denominator)
            {
                return Numerator < fra.Numerator;
            }

            const auto first = multiply(Numerator, fra.Denominator);
            const auto second = multiply(fra.Numerator, Denominator);

            return first < second;
        }

private:
        template<std::signed_integral num_t2 = int_t>
        [[nodiscard]] static constexpr auto add(const int_t &first, const num_t2 &second) -> int_t
        {
            return add_signed<int_t>(first, second);
        }

        template<std::signed_integral num_t2 = int_t>
        [[nodiscard]] static constexpr auto multiply(const int_t &first, const num_t2 &second) -> int_t
        {
            return multiply_signed<int_t>(first, second);
        }

        static constexpr void check_inf(const int_t &val, const char *const error)
        {
            assert(error != nullptr);

            if (minus_inf == val) [[unlikely]]
            {
                throw std::runtime_error(error);
            }
        }

        static constexpr void check_inf_denominator(const int_t &val)
        {
            check_inf(val, "The denominator is the minimum and will overflow.");
        }

        static constexpr void check_inf_numerator(const int_t &val)
        {
            check_inf(val, "The numerator is the minimum and will overflow.");
        }

        static constexpr auto minus_inf = std::numeric_limits<int_t>::min();

        static_assert(minus_inf < int_t{});

        int_t Numerator;
        int_t Denominator;
    };

    template<std::integral int_t, int_t mod, std::integral long_int_t>
    requires(same_sign_leq_size<int_t, long_int_t> && 2 <= mod)
    struct fraction_mod final
    {
        static constexpr int_t zero{};

        constexpr explicit fraction_mod(const long_int_t &val = zero)
            : Value(val % mod)
        {
            if constexpr (std::is_signed_v<int_t>)
            {
                if (Value < zero)
                {
                    Value += mod;
                }
            }

            assert(zero <= Value && Value < mod);
        }

        constexpr fraction_mod(const long_int_t &num, const long_int_t &den)
            : fraction_mod(num % mod * modular_power<long_int_t>(den % mod, mod - 2, mod) % mod)
        {
        }

        [[nodiscard]] constexpr auto value() const &noexcept -> const int_t &
        {
            assert(zero <= Value && Value < mod);

            return Value;
        }

        [[nodiscard]] constexpr auto operator+ (const fraction_mod &fra) const -> fraction_mod
        {
            auto val = Value + fra.Value;

            return fraction_mod(val < mod ? val : val - mod);
        }

        [[nodiscard]] constexpr auto operator* (const fraction_mod &fra) const -> fraction_mod
        {
            return fraction_mod(static_cast<long_int_t>(Value) * fra.Value % mod);
        }

        [[nodiscard]] constexpr auto operator- () const -> fraction_mod
        {
            return fraction_mod(zero < Value ? mod - Value : int_t{});
        }

        [[nodiscard]] constexpr auto operator- (const fraction_mod &fra) const -> fraction_mod
        {
            auto val = mod + Value - fra.Value;

            return fraction_mod(val < mod ? val : val - mod);
        }

        [[nodiscard]] constexpr auto operator== (const fraction_mod &fra) const noexcept -> bool
        {
            return Value == fra.Value;
        }

private:
        int_t Value;
    };
} // namespace Standard::Algorithms::Numbers
