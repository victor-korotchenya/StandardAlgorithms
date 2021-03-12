#include"count_linear_equation_solutions_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"count_linear_equation_solutions.h"
#include"default_modulus.h"

namespace
{
    using int_t = std::int64_t;
    using vec_int_t = std::vector<int_t>;
    using output_t = std::int32_t;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, vec_int_t, output_t>;

    constexpr auto not_computed = -1;
    constexpr auto mod = Standard::Algorithms::Numbers::default_modulus;

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();
        ::Standard::Algorithms::ert::greater_or_equal(input.size(), 2U, "data size");

        const auto &output = test.output();
        ::Standard::Algorithms::ert::greater_or_equal(output, not_computed, "output");
    }

    void generate_test_cases(std::vector<test_case> &tests)
    {
        tests.emplace_back("3x + 4y + 12z + 5a = 20",
            // 2: 12+5+3, 12+4*2
            // 4: 5*4, 5*2+4+3*2, 5+4*3+3, 5+3*5
            // 2: 4*5, 4*2+3*4
            // NOLINTNEXTLINE
            vec_int_t{ 3, 4, 12, 5, 20 }, 8);

        tests.emplace_back("base3",
            // 6+4, 6+2+2, 4+4+2, 4+2+2+2, 2*5
            // NOLINTNEXTLINE
            vec_int_t{ 2, 4, 6, 10 }, 5);

        tests.emplace_back("all 0s",
            // NOLINTNEXTLINE
            vec_int_t{ 2, 4, 6, 0 }, 1);

        tests.emplace_back("no solution3",
            // NOLINTNEXTLINE
            vec_int_t{ 2, 4, 6, 1 }, 0);

        tests.emplace_back("no solution2",
            // NOLINTNEXTLINE
            vec_int_t{ 2, 4, 1 }, 0);

        tests.emplace_back("no solution1", vec_int_t{ 2, 1 }, 0);

        constexpr auto min_value = 1;
        constexpr auto max_value = 10;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_value, max_value);

        constexpr auto max_att = 1;

        for (std::int32_t att{}; att < max_att; ++att)
        {
            constexpr auto min_size = 2;
            constexpr auto max_size = 10;

            auto data = Standard::Algorithms::Utilities::random_vector<decltype(rnd), int_t>(
                rnd, min_size, max_size, min_value, max_value);

            Standard::Algorithms::throw_if_empty("data", data);

            constexpr auto max_add = 5;
            data.back() += rnd(0, max_add);

            tests.emplace_back("Random" + std::to_string(att), std::move(data), not_computed);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::count_linear_equation_solutions<int_t>(test.input(), mod);

        if (0 <= test.output())
        {
            ::Standard::Algorithms::ert::are_equal(test.output(), fast, "count_linear_equation_solutions");
        }

        const auto slow = Standard::Algorithms::Numbers::count_linear_equation_solutions_slow<int_t>(test.input(), mod);

        ::Standard::Algorithms::ert::are_equal(fast, slow, "count_linear_equation_solutions_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::count_linear_equation_solutions_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
