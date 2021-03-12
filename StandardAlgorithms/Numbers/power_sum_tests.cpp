#include<functional>
#include "power_sum_tests.h"
#include "Arithmetic.h"
#include "fraction.h"
#include "power_sum.h"
#include "../test_utilities.h"
#include "../Utilities/Random.h"

using namespace std;

void Privet::Algorithms::Numbers::Tests::power_sum_tests()
{
    using number_t = int;
    using number_t2 = int64_t;
    constexpr auto mod = 97;

    using num_op_t = function<number_t2(const number_t2, const number_t2)>;
    const num_op_t add = [](const number_t2 a, const number_t2 b)->auto{
        return AddSigned(a, b); };
    const num_op_t prod = [](const number_t2 a, const number_t2 b)->auto{
        return MultiplySigned(a, b); };
    const num_op_t div = [](const number_t2 a, const number_t2 b)->auto{
        if (!b)
            throw runtime_error("Cannot div by 0.");
        const auto r = a / b;
        if (r * b == a)
            return r;
        const auto err = "div overflow: " + to_string(a) + " / " + to_string(b);
        throw runtime_error(err);
    };

    const num_op_t add_mod = [=](const number_t2 a, const number_t2 b)->auto{
        return (a + b) % mod; };
    const num_op_t prod_mod = [=](const number_t2 a, const number_t2 b)->auto{
        return a * b % mod; };
    const num_op_t div_mod = [=](const number_t2 a, const number_t2 b)->auto{
        if (!b)
            throw runtime_error("Cannot div mod by 0.");
        const auto inv = modular_power(b, mod - 2, mod);
        const auto r = a * inv % mod;
        assert(a == r * b % mod);
        return r;
    };

    using fraction_t = fraction<number_t2>;
    using fraction_t_mod = fraction_mod<number_t2, mod, number_t2>;

    IntRandom r;
    vector<fraction_t> temp;
    vector<fraction_t_mod> temp_mod;

    constexpr auto attemps = 1;
    const vector<string> names = { "sum", "sum modulo" };
    for (auto attempt = 0; attempt < attemps; ++attempt)
    {
        const auto power = r(1, 10);
        const auto n = r(0, 10);
        auto ind = 0;
        try
        {
            number_t2 expected{};
            const auto is_small = power <= 10 && n <= 10;
            if (is_small)
            {
                const auto coefs = faulhaber_coef_range<number_t2, number_t, num_op_t, num_op_t, fraction_t>(prod, div, temp, power);
                expected = power_sum_slow<number_t2, number_t, num_op_t, num_op_t>(add, prod, power, n);
                const auto actual = faulhaber_power_sum<number_t2, number_t, fraction_t>(coefs, power, n);
                Assert::AreEqual(expected, actual, names[ind]);
            }
            {
                ++ind;
                const auto coefs_mod = faulhaber_coef_range<number_t2, number_t, num_op_t, num_op_t, fraction_t_mod>(prod_mod, div_mod, temp_mod, power);
                const auto expected2 = power_sum_slow<number_t2, number_t, num_op_t, num_op_t>(add_mod, prod_mod, power, n);
                if (is_small)
                    Assert::AreEqual(expected % mod, expected2, names[ind] + string("_slow"));

                const auto actual = faulhaber_power_sum<number_t2, number_t, fraction_t_mod>(coefs_mod, power, n);
                Assert::AreEqual(expected2, actual, names[ind]);
            }
        }
        catch (const exception& e)
        {
            const auto err = "power=" + to_string(power)
                + ", n=" + to_string(n)
                + ", name=" + names[ind]
                + ", message=" + e.what();
            throw runtime_error(err);
        }
    }
}