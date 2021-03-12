#pragma once
// "factoring_utilities.h"
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include"gcd.h"
#include"power_root.h"
#include<initializer_list>
#include<optional>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Factorize. It is assumed that (1 <= num).
    // It is good only for a few calls, slow for many.
    // Slow time O(sqrt(n)).
    // See also decompose_eratosthenes_sieve_factoring, prime_number_utility
    template<std::integral long_int_t, std::integral int_t, class divisor_power_t = std::pair<int_t, int_t>>
    requires(same_sign_leq_size<int_t, long_int_t>)
    constexpr void decompose_into_divisors_with_powers(int_t num, std::vector<divisor_power_t> &components) noexcept(
        false)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        assert(zero < num);

        components.clear();

        if (num <= one)
        {
            return;
        }

        if (zero == (num & one))
        {
            components.emplace_back(2, zero);

            do
            {
                ++(components.back().second);
                num >>= 1U;

                assert(zero < num);
            } while (zero == (num & one));
        }

        for (int_t factor = 3; static_cast<long_int_t>(factor) * factor <= num; factor += 2)
        {
            if (zero != (num % factor))
            {
                continue;
            }

            components.emplace_back(factor, zero);

            do
            {
                ++(components.back().second);
                num /= factor;

                assert(zero < num);
            } while (zero == (num % factor));
        }

        if (one < num)
        {
            components.emplace_back(num, one);
        }
    }

    // Slow time O(sqrt(n)).
    template<std::integral int_t, class divisor_power_t = std::pair<int_t, int_t>>
    constexpr void decompose_into_divisors_with_powers_slow(
        int_t num, std::vector<divisor_power_t> &components) noexcept(false)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        assert(!(num < zero));

        const auto upper = int_sqrt(num);

        for (int_t div = 2; div <= upper; ++div)
        {
            if (zero != (num % div))
            {
                continue;
            }

            typename divisor_power_t::second_type power{};

            do
            {
                num /= div, ++power;

                assert(zero < num && zero < power);
            } while (zero == (num % div));

            components.emplace_back(div, power);

            if (one == num)
            {
                return;
            }
        }

        if (one < num)
        {
            components.emplace_back(num, one);
        }
    }

    // Given an integer value,
    // find a divisor and an exp, both greater than 1 and not necessarily a prime, such that (value == divisor**exp).
    // Return 0 base in case of failure.
    // E.g. 15'625 -> (25, 3) because 25**3 == 15'625.
    // Time O(log(n)**2 * log(log(n))).
    template<std::integral int_t>
    [[nodiscard]] constexpr auto find_divisor_exact_power(const int_t &value) -> std::pair<int_t, int_t>
    {
        constexpr int_t two{ 2 };
        constexpr auto max_val = std::numeric_limits<int_t>::max(); // todo(p4): impl. long division.

        static_assert(two < max_val);

        if (value < two * two || !(value < max_val))
        {
            return {};
        }

        for (auto exp1 = two;;)
        {
            const auto root1 = int_root_binary_slow<int_t, max_val>(value, exp1);
            if (root1 < two)
            {
                return std::make_pair(int_t{}, exp1);
            }

            const auto raised = int_power<int_t, int_t>(root1, exp1);
            if (raised == value)
            {
                return std::make_pair(root1, exp1);
            }

            constexpr int_t max_root = 64; // todo(p4): longer, larger integers.
            static_assert(two < max_root);

            assert(exp1 < max_root);

            ++exp1;
        }
    }

    // Find some divisor, also returning # of steps, for a given positive composite number.
    // A prime (non-composite) number must not be an input as it may hang forever!
    // Slow time O(sqrt(n)).
    template<std::integral int_t, int_t max_val = std::numeric_limits<int_t>::max()>
    requires(sizeof(int_t) <= sizeof(std::uint64_t) // todo(p3): Weaken this precondition, impl. long division.
        && int_t{} < max_val)
    [[nodiscard]] constexpr auto polland_rho_find_a_prime_divisor(const int_t &num, const int_t &addon)
        -> std::pair<std::optional<int_t>, std::uint64_t>
    {
        constexpr int_t zero{};
        constexpr int_t one{ 1 };
        constexpr int_t two{ 2 };

        if (num < two || !(num < max_val) || !(zero < addon) || !(addon < num))
        {// Ignore bad input.
            return {};
        }

        int_t xxx{};
        int_t yyy_yyy{};

        for (std::uint64_t steps = 1;;)
        {
            auto next_x = static_cast<int_t>((xxx * xxx + addon) % num); // Overflow risk.

            assert(!(next_x < zero) && next_x < num);

            auto next_yy = static_cast<int_t>((yyy_yyy * yyy_yyy + addon) % num); // Overflow risk.

            assert(!(next_yy < zero) && next_yy < num);

            next_yy = static_cast<int_t>((next_yy * next_yy + addon) % num); // Overflow risk.

            assert(!(next_yy < zero) && next_yy < num);

            // todo(p4): del compare.
            const auto diff = static_cast<int_t>(next_yy < next_x ? next_x - next_yy : next_yy - next_x);

            auto codi = gcd_int<int_t>(diff, num);
            if (one < codi)
            {
                if (codi < num)
                {
                    return std::make_pair(std::move(codi), steps);
                }

                return std::make_pair(std::nullopt, steps);
            }

            ++steps;
            xxx = std::move(next_x);
            yyy_yyy = std::move(next_yy);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                constexpr auto maxins = 1LLU << 20U;
                static_assert(0U < maxins);

                if (maxins < steps) [[unlikely]]
                {// todo(p4): impl. long division, del it.
                    auto err = "DEBUG. Too many steps in polland_rho_find_a_prime_divisor " + std::to_string(num);

                    throw std::runtime_error(err);
                }
            }
        }
    }

    // Return a factor and the # of steps.
    template<std::integral int_t>
    requires(std::convertible_to<int_t, std::uint64_t>)
    [[nodiscard]] constexpr auto run_polland_rho(const int_t &composite) -> std::pair<int_t, std::uint64_t>
    {
        constexpr int_t zero{};
        constexpr int_t two{ 2 };

        if (composite < two)
        {
            return {};
        }

        if (zero == composite % two)
        {
            return std::make_pair(two, 1U);
        }

        std::uint64_t total_steps = 2;

        if (constexpr int_t three{ 3 }; zero == composite % three)
        {
            return std::make_pair(three, total_steps);
        }

        {// Prevent hanging for an exact power.
            const auto [base, exp] = find_divisor_exact_power<int_t>(composite);

            total_steps += static_cast<std::uint64_t>(exp);
            assert(0U < total_steps);

            if (zero < base)
            {
                return std::make_pair(base, total_steps);
            }
        }

        constexpr int_t one{ 1 };

        int_t divisor{};

        for (auto addon = one; addon < composite; ++addon)
        {
            const auto [opt, steps] =
                Standard::Algorithms::Numbers::polland_rho_find_a_prime_divisor<int_t>(composite, addon);

            total_steps += steps;
            assert(0U < steps && !(total_steps < steps));

            if (!opt.has_value())
            {
                continue;
            }

            divisor = opt.value();
            Standard::Algorithms::require_positive(steps, "run_polland_rho steps");
            break;
        }

        {
            const auto *const name = "run_polland_rho divisor";
            Standard::Algorithms::require_greater(divisor, one, name);
            Standard::Algorithms::require_greater(composite, divisor, name);
        }

        const auto quot = static_cast<int_t>(composite / divisor);

        if (const auto remainder = static_cast<int_t>(composite % divisor); zero != remainder) [[unlikely]]
        {
            auto err = "Composite " + std::to_string(composite) + " is not divisible by a factor " +
                std::to_string(divisor) + ", run_polland_rho got a remainder: " + std::to_string(remainder) + ".";

            throw std::runtime_error(err);
        }

        const auto prod = static_cast<int_t>(quot * divisor);
        if (prod != composite) [[unlikely]]
        {
            auto err = "Composite " + std::to_string(composite) + " * factor // factor, where factor is " +
                std::to_string(divisor) + ", run_polland_rho is " + std::to_string(prod) + ".";

            throw std::runtime_error(err);
        }

        return std::make_pair(std::move(divisor), total_steps);
    }

    // It is assumed that (1 <= num).
    // It could be faster to go thru the prepared primes list.
    // Slow time O(sqrt(n)).
    template<std::integral long_int_t, std::integral int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    constexpr void decompose_into_prime_divisors(int_t num, std::vector<int_t> &prime_factors) noexcept(false)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        assert(zero < num);

        prime_factors.clear();

        if (num <= one)
        {
            return;
        }

        if (zero == (num & one))
        {
            prime_factors.push_back(2);

            do
            {
                num >>= 1U;

                assert(zero < num);
            } while (zero == (num & one));
        }

        int_t factor = 3;

        for (; static_cast<long_int_t>(factor) * factor <= num; factor += 2)
        {
            if (zero != (num % factor))
            {
                continue;
            }

            prime_factors.push_back(factor);

            do
            {
                num /= factor;
            } while (zero == (num % factor));
        }

        if (one < num)
        {
            prime_factors.push_back(num);
        }
    }

    // 1 is included into the result.
    // Slow time O(sqrt(n)).
    template<std::integral int_t>
    constexpr void all_divisors(const int_t &num, std::vector<int_t> &factors) noexcept(false)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        assert(zero < num);

        factors.clear();
        factors.push_back(one);

        if (num <= one)
        {
            return;
        }

        const auto root = int_sqrt(num);

        for (int_t div = 2; div < root; ++div)
        {
            if (zero != (num % div))
            {
                continue;
            }

            factors.push_back(static_cast<int_t>(num / div));
            factors.push_back(div);
        }

        if (one < root && zero == (num % root))
        {
            const auto div_2 = static_cast<int_t>(num / root);
            factors.push_back(root);

            if (div_2 != root)
            {
                factors.push_back(div_2);
            }
        }

        std::sort(factors.begin(), factors.end());

        assert(zero < factors.front() && !(factors.back() < factors.front()) && factors.back() < num);

        factors.push_back(num);
    }

    // Too slow time O(x).
    template<std::integral int_t>
    constexpr void all_divisors_slow(const int_t &num, std::unordered_set<int_t> &factors) noexcept(false)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        assert(zero < num);

        factors.clear();

        for (auto div = one; div <= num; ++div)
        {
            if (zero != (num % div))
            {
                continue;
            }

            factors.insert(std::initializer_list<int_t>{ div, static_cast<int_t>(num / div) });
        }
    }
} // namespace Standard::Algorithms::Numbers
