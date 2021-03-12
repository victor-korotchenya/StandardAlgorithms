#pragma once
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include"bernoulli.h"
#include<array>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Calculate C(n, m) which is n choose m.
    // Dangerous warning: might produce wrongful result, espeCIAlly for large inputs.
    // E.g. C(11, 5) = 462. C(5, 11) is 0.
    template<class multiply_t, class divide_t, std::integral int_t>
    [[nodiscard]] constexpr auto binomial_choose_naive(
        const int_t &nnn, multiply_t multiply, divide_t divide, const int_t &mmm) -> int_t
    {
        constexpr int_t zero{};

        assert(!(nnn < zero) && !(mmm < zero));

        if (mmm < zero || nnn < mmm)
        {// todo(p5): fix for <0 ints.
            return zero;
        }

        if (nnn == zero || mmm == zero || nnn == mmm)
        {
            return 1;
        }

        // Let nnn = 11, mmm = 5;
        // then dif = 6, mini = 5, maxi = 6.
        //
        // C(11, 5) = 11!/6!/5! = 7*8*9*10*11 / (5!)
        const auto dif = static_cast<int_t>(nnn - mmm);
        const auto &mini = std::min(mmm, dif);
        const auto &maxi = std::max(mmm, dif);

        int_t divisor = 2;
        auto left = static_cast<int_t>(maxi + 1);

        auto result = left;
        assert(zero < result);

        while (++left <= nnn)
        {
            // Try to avoid too big numbers.
            while (divisor <= mini && (zero == (result % divisor)))
            {
                result = divide(result, divisor++);
                assert(zero < divisor && zero < result);
            }

            result = multiply(result, left);
            assert(zero < divisor && zero < result);
        }

        while (divisor <= mini)
        {
            result = divide(result, divisor++);
            assert(zero < divisor && zero < result);
        }

        assert(zero < result);
        return result;
    }

    // Faulhaber sum(n, power) = 1**power + 2**power + .. + n**power.
    template<std::integral long_int_t, std::integral int_t, class fraction_t>
    requires(same_sign_leq_size<int_t, long_int_t> && sizeof(int_t) <= sizeof(std::uint64_t))
    [[nodiscard]] constexpr auto faulhaber_power_sum(const long_int_t &nnn,
        const std::vector<std::vector<std::pair<fraction_t, int_t>>> &coefficients, const int_t &power) noexcept(false)
        -> long_int_t
    {
        if constexpr (std::is_signed_v<int_t>)
        {
            require_non_negative(nnn, "nnn");
            require_non_negative(power, "power");
        }

        require_greater(coefficients.size(), static_cast<std::uint64_t>(power), "power");

        const auto &coefs = coefficients.at(power);
        const fraction_t source_fraction{ nnn };

        fraction_t result{};
        fraction_t prod{ 1 };
        long_int_t temp_pow{};

        for (const auto &[frac, num] : coefs)
        {
            while (temp_pow < num)
            {
                ++temp_pow;
                prod = prod * source_fraction;
            }

            result = frac * prod + result;
        }

        auto value = result.value();
        return value;
    }

    // For usual (not modular) fraction, the power cannot be > 30 as an exception is thrown.
    template<std::integral long_int_t, std::integral int_t, class multiply_t, class divide_t, class fraction_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto faulhaber_coef(multiply_t multiply, divide_t divide, std::vector<fraction_t> &temp,
        const int_t &power) -> std::vector<std::pair<fraction_t, int_t>>
    {
        if constexpr (std::is_signed_v<int_t>)
        {
            require_non_negative(power, "power");
        }

        std::vector<std::pair<fraction_t, int_t>> coefficients;

        const auto power_plus_one = static_cast<int_t>(power + 1);
        require_greater(power_plus_one, power, "power+1");

        constexpr std::array<fraction_t, 2> signs{ fraction_t{ 1 }, -fraction_t{ 1 } };
        const fraction_t frac{ 1, power_plus_one };

        for (int_t index{}, sign{}; index < power_plus_one; ++index, sign ^= 1)
        {
            const auto berlin = bernoulli<fraction_t, int_t>(temp, index);

            const auto chon =
                binomial_choose_naive<multiply_t, divide_t, long_int_t>(power_plus_one, multiply, divide, index);

            const auto prod = frac * berlin * signs[sign] * fraction_t{ chon };

            if (constexpr fraction_t zero{ 0 }; prod == zero)
            {
                continue;
            }

            auto power_2 = power_plus_one - index;
            coefficients.emplace_back(prod, power_2);
        }

        std::reverse(coefficients.begin(), coefficients.end());

        return coefficients;
    }

    template<std::integral long_int_t, std::integral int_t, class multiply_t, class divide_t, class fraction_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto faulhaber_coef_range(multiply_t multiply, divide_t divide,
        std::vector<fraction_t> &temp, const int_t &max_power) -> std::vector<std::vector<std::pair<fraction_t, int_t>>>
    {
        if constexpr (std::is_signed_v<int_t>)
        {
            require_non_negative(max_power, "max power");
        }

        const auto max_power_plus_one = static_cast<int_t>(max_power + 1);
        require_greater(max_power_plus_one, max_power, "max power+1");

        std::vector<std::vector<std::pair<fraction_t, int_t>>> coefficients(max_power_plus_one);

        for (int_t power{}; power < max_power_plus_one; ++power)
        {
            coefficients[power] =
                faulhaber_coef<long_int_t, int_t, multiply_t, divide_t, fraction_t>(multiply, divide, temp, power);
        }

        return coefficients;
    }

    template<std::integral long_int_t, std::integral int_t, class multiply_t, class add_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto power_sum_slow(
        const long_int_t &nnn, multiply_t multiply, add_t add, const int_t &power) -> long_int_t
    {
        throw_if_null_skip_non_pointer("multiply", multiply);
        throw_if_null_skip_non_pointer("add", add);

        if constexpr (std::is_signed_v<int_t>)
        {
            require_non_negative(nnn, "nnn");
            require_non_negative(power, "power");
        }

        long_int_t result{};

        for (long_int_t value = 1; value <= nnn; ++value)
        {
            long_int_t prod = 1;

            {
                int_t temp_pow{};

                while (temp_pow++ < power)
                {
                    prod = multiply(prod, value);

                    assert(long_int_t{} < prod);
                }
            }

            result = add(result, prod);
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
