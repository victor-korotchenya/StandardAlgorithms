#include"minimum_moves_to_sort_tests.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"minimum_moves_to_sort.h"

namespace
{
    // A function pointer (instead of std::less) is being tested.
    template<class t>
    [[nodiscard]] constexpr auto my_less(const t &one, const t &two) noexcept -> bool
    {
        return one < two;
    }

    template<class t>
    [[nodiscard]] constexpr auto our_less_equal(const t &one, const t &two) -> bool
    {
        return one <= two;
    }

    using int_t = std::int32_t;
    using vec_int_t = std::vector<int_t>;
    using output_t = std::size_t;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, vec_int_t, output_t>;

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();
        const auto &output = test.output();

        if (const auto is_sorted = std::is_sorted(input.cbegin(), input.cend()); is_sorted)
        {
            ::Standard::Algorithms::ert::are_equal(0U, output, "output");
        }
        else
        {
            ::Standard::Algorithms::ert::greater(output, 0U, "output");
        }
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("No move", vec_int_t{ 1, 2, 3, 4 }, 0);

        test_cases.emplace_back("One move", vec_int_t{ 4, 1, 2, 3 }, 1);

        test_cases.emplace_back("One move with repetitions", // NOLINTNEXTLINE
            vec_int_t{ 4, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 4 }, 1);

        test_cases.emplace_back("max moves", vec_int_t{ 4, 3, 2, 1 }, 3);

        test_cases.emplace_back("Two moves", // NOLINTNEXTLINE
            vec_int_t{ 6, 3, 1, 2, 4, 5 }, 2);
    }

    constexpr void run_test_case(const test_case &test)
    {
        using alg_t = bool (*)(const int_t &, const int_t &);
        using name_alg_t = std::pair<std::string, alg_t>;

        const auto &input = test.input();
        const auto &output = test.output();

        constexpr bool throw_on_error = false;
        const auto has_unique_values = Standard::Algorithms::require_unique(input, "input", throw_on_error);

        const auto sub_tests = [] [[nodiscard]] (bool is_strict)
        {
            std::vector<name_alg_t> sub_tests_temp{ { "our less equal", &our_less_equal<int_t> } };

            if (is_strict)
            {
                sub_tests_temp.emplace_back("my less", &my_less<int_t>);
            }

            return sub_tests_temp;
        }(has_unique_values);

        for (const auto &sub_test : sub_tests)
        {
            const auto actual =
                Standard::Algorithms::Numbers::minimum_moves_to_sort<int_t, alg_t>(input, sub_test.second);

            ::Standard::Algorithms::ert::are_equal(output, actual, sub_test.first);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::minimum_moves_to_sort_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
