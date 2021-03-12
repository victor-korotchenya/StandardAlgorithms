#include"modulo_linear_equation_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"modulo_linear_equation.h"
#include<optional>
#include<tuple>

namespace
{
    using int_t = std::int64_t;
    using mod_a_b_t = std::tuple<int_t, int_t, int_t>;
    using vec_int_t = std::vector<int_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, mod_a_b_t, std::optional<vec_int_t>>;

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();

        const auto &mod = std::get<0>(input);
        Standard::Algorithms::require_positive(mod, "mod");

        const auto &opt_output = test.output();
        if (!opt_output.has_value())
        {
            return;
        }

        const auto &output = opt_output.value();
        if (output.empty())
        {
            return;
        }

        Standard::Algorithms::require_less_equal(output.size(), static_cast<std::uint64_t>(mod), "output size");

        Standard::Algorithms::require_unique(output, "output");
        Standard::Algorithms::require_sorted(output, "output");

        const auto &mini = output[0];
        Standard::Algorithms::require_non_negative(mini, "output min element");

        const auto &maxi = output.back();
        Standard::Algorithms::require_greater(mod, maxi, "output max element");
    }

    void generate_test_cases(std::vector<test_case> &tests)
    {
        tests.emplace_back("test 00 mod 1", // NOLINTNEXTLINE
            mod_a_b_t{ 1, 0, 0 }, vec_int_t{ 0 });

        // mod 2
        tests.emplace_back("test 00 mod 2", // NOLINTNEXTLINE
            mod_a_b_t{ 2, 0, 0 }, vec_int_t{ 0, 1 });

        tests.emplace_back("test 01 mod 2", // NOLINTNEXTLINE
            mod_a_b_t{ 2, 0, 1 }, vec_int_t{});

        tests.emplace_back("test 10 mod 2", // NOLINTNEXTLINE
            mod_a_b_t{ 2, 1, 0 }, vec_int_t{ 0 });

        tests.emplace_back("test 11 mod 2", // NOLINTNEXTLINE
            mod_a_b_t{ 2, 1, 1 }, vec_int_t{ 1 });

        tests.emplace_back("test 60", // NOLINTNEXTLINE
            mod_a_b_t{ 60, 10, 30 }, // NOLINTNEXTLINE
            // Solve. Gcd(60, 10) = 10; 10 divides 30, so delta = 30/10 = 3 produces 10 solutions:
            // 3*10 == 30 modulo 60
            // 3*10 + 1*60 == 30 modulo 60
            // 3*10 + 2*60 == 30 modulo 60
            // ..
            // 3*10 + 9*60 == 30 modulo 60 // NOLINTNEXTLINE
            vec_int_t{ 3, 9, 15, 21, 27, 33, 39, 45, 51, 57 });


        constexpr int_t zero{};
        constexpr int_t one{ 1 };
        constexpr int_t mod_min_value = one;
        constexpr int_t max_value{ 100 };
        static_assert(zero < mod_min_value && mod_min_value < max_value);

        Standard::Algorithms::Utilities::random_t<int_t> rnd(mod_min_value, max_value);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto mod = rnd();
            assert(zero < mod);

            const auto upper_value = static_cast<int_t>(mod - one);
            const auto aaa = rnd(zero, upper_value);
            assert(zero <= aaa && aaa < mod);

            const auto bbb = rnd(zero, upper_value);
            assert(zero <= bbb && bbb < mod);

            tests.emplace_back("random" + std::to_string(att), mod_a_b_t{ mod, aaa, bbb }, std::nullopt);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto [mod, aaa, bbb] = test.input();

        std::vector<int_t> fast;
        Standard::Algorithms::Numbers::modulo_linear_equation<int_t>(mod, aaa, fast, bbb);

        if (const auto &opt_output = test.output(); opt_output.has_value())
        {
            const auto &output = opt_output.value();

            ::Standard::Algorithms::ert::are_equal(output, fast, "modulo_linear_equation");
        }

        std::vector<int_t> slow;
        Standard::Algorithms::Numbers::modulo_linear_equation_slow<int_t>(mod, aaa, slow, bbb);

        ::Standard::Algorithms::ert::are_equal(fast, slow, "modulo_linear_equation_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::modulo_linear_equation_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
