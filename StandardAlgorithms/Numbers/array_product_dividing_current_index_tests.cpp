#include"array_product_dividing_current_index_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"array_product_dividing_current_index.h"

namespace
{
    using int_t = std::int64_t;
    using vec_int_t = std::vector<int_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, vec_int_t>;

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();
        ::Standard::Algorithms::ert::greater(input.size(), 1U, "input size");
        ::Standard::Algorithms::ert::are_equal(input.size(), test.output().size(), "output size");
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("With zero",
            // NOLINTNEXTLINE
            vec_int_t({ 0, 1, 2, 4, 5 }),
            // NOLINTNEXTLINE
            vec_int_t({ 40, 0, 0, 0, 0 }));

        test_cases.emplace_back("Simple 1",
            // NOLINTNEXTLINE
            vec_int_t({ -3, 1, 2, 4, 5 }),
            // NOLINTNEXTLINE
            vec_int_t({ 40, -120, -60, -30, -24 }));

        constexpr auto min_size = 2U;
        constexpr auto max_size = 10U;

        const auto size = Standard::Algorithms::Utilities::random_t<std::size_t>()(min_size, max_size);
        vec_int_t data(size);

        {
            // 2**64 ~= 10**19
            // 100**10 ~= 10**20; cannot use 100 - it might overflow.
            // 50**10 < 10**17; gut.
            constexpr auto cutter = 50;
            Standard::Algorithms::Utilities::fill_random<int_t, std::int32_t>(data, size, cutter);
        }

        auto expected = Standard::Algorithms::Numbers::array_product_dividing_current_index(data);

        test_cases.emplace_back("Random", std::move(data), std::move(expected));
    }

    constexpr void run_test_case(const test_case &test)
    {
        {
            const auto actual = Standard::Algorithms::Numbers::array_product_dividing_current_index(test.input());

            ::Standard::Algorithms::ert::are_equal(test.output(), actual, "array_product_dividing_current_index");
        }
        {
            const auto actual = Standard::Algorithms::Numbers::array_product_dividing_current_index_slow(test.input());

            ::Standard::Algorithms::ert::are_equal(test.output(), actual, "array_product_dividing_current_index_slow");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::array_product_dividing_current_index_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
