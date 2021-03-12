#include"power_sum_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"arithmetic.h"
#include"fraction.h"
#include"power_sum.h"
#include<functional>

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    constexpr int_t mod = 97;
    static_assert(int_t{} < mod);

    using fraction_t = Standard::Algorithms::Numbers::fraction<long_int_t>;
    using fraction_t_mod = Standard::Algorithms::Numbers::fraction_mod<long_int_t, mod, long_int_t>;
} // namespace

void Standard::Algorithms::Numbers::Tests::power_sum_tests()
{
    const auto add = [] [[nodiscard]] (const long_int_t &one, const long_int_t &two) -> long_int_t
    {
        return add_signed(one, two);
    };

    const auto prod = [] [[nodiscard]] (const long_int_t &one, const long_int_t &two) -> long_int_t
    {
        return multiply_signed(one, two);
    };

    const auto div = [] [[nodiscard]] (const long_int_t &one, const long_int_t &two) -> long_int_t
    {
        if (two == long_int_t{}) [[unlikely]]
        {
            throw std::runtime_error("Cannot divide by 0.");
        }

        auto quot = one / two;
        if (quot * two == one) [[likely]]
        {
            return quot;
        }

        auto err = "Divide overflow: " + std::to_string(one) + " / " + std::to_string(two);

        throw std::runtime_error(err);
    };

    const auto add_mod = [] [[nodiscard]] (const long_int_t &one, const long_int_t &two) -> long_int_t
    {
        return (one + two) % mod;
    };

    const auto prod_mod = [] [[nodiscard]] (const long_int_t &one, const long_int_t &two) -> long_int_t
    {
        return one * two % mod;
    };

    const auto div_mod = [] [[nodiscard]] (const long_int_t &one, const long_int_t &two) -> long_int_t
    {
        const auto bas = two % mod;
        if (bas == long_int_t{}) [[unlikely]]
        {
            throw std::runtime_error("Cannot divide mod by 0.");
        }

        const auto inv = modular_power(bas, mod - 2, mod);
        auto res = one * inv % mod;

        assert(one == res * two % mod);

        return res;
    };

    constexpr int_t min_value = 0;
    constexpr int_t max_value = 10;

    Standard::Algorithms::Utilities::random_t<int_t> rnd(min_value, max_value);
    std::vector<fraction_t> temp;
    std::vector<fraction_t_mod> temp_mod;

    constexpr auto max_attemps = 1;

    for (std::int32_t attempt{}; attempt < max_attemps; ++attempt)
    {
        constexpr int_t min_power = 1;
        constexpr int_t max_power = 10;

        const auto power = rnd(min_power, max_power);

        static_assert(min_value <= max_value && max_value < mod);

        const auto nnn = static_cast<long_int_t>(rnd());
        int_t step{};

        try
        {
            const auto expected = power_sum_slow(nnn, prod, add, power);
            {
                const auto coefs = faulhaber_coef_range<long_int_t>(prod, div, temp, power);

                const auto actual = faulhaber_power_sum(nnn, coefs, power);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "faulhaber_power_sum");
            }
            ++step;
            {
                const auto coefs_mod = faulhaber_coef_range<long_int_t>(prod_mod, div_mod, temp_mod, power);

                const auto expected_2 = expected % mod;
                const auto actual = power_sum_slow(nnn, prod_mod, add_mod, power);

                ::Standard::Algorithms::ert::are_equal(expected_2, actual, " power_sum_slow modulo");

                const auto actual_2 = faulhaber_power_sum(nnn, coefs_mod, power);
                ::Standard::Algorithms::ert::are_equal(expected_2, actual_2, "faulhaber_power_sum modulo");
            }
        }
        catch (const std::exception &exc)
        {
            const auto err = "power " + std::to_string(power) + ", value " + std::to_string(nnn) + ", step " +
                std::to_string(step) + ", error: " + exc.what();

            throw std::runtime_error(err);
        }
    }
}
