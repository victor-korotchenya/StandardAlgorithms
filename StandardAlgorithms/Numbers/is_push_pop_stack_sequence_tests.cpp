#include"is_push_pop_stack_sequence_tests.h"
#include"../Utilities/test_utilities.h"
#include"is_push_pop_stack_sequence.h"

namespace
{
    using int_t = std::int16_t;
    using vec_int_t = std::vector<int_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, vec_int_t &&array1, vec_int_t &&array2, const bool expected = false) noexcept
            : base_test_case(std::move(name))
            , Array1(std::move(array1))
            , Array2(std::move(array2))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto array1() const &noexcept -> const vec_int_t &
        {
            return Array1;
        }

        [[nodiscard]] constexpr auto array2() const &noexcept -> const vec_int_t &
        {
            return Array2;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> bool
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print("Array 1", Array1, str);
            ::Standard::Algorithms::print("Array 2", Array2, str);
            ::Standard::Algorithms::print_value("Expected", str, Expected);
        }

private:
        vec_int_t Array1;
        vec_int_t Array2;
        bool Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "Equal", { 1, 2, 3 }, { 1, 2, 3 }, true });
        test_cases.push_back({ "Reversed", { 1, 2, 3 }, { 3, 2, 1 }, true });
        test_cases.push_back({ "213", { 1, 2, 3 }, { 2, 1, 3 }, true });

        test_cases.push_back({ "different last item", { 1, 2, 3 }, { 2, 1, 2 } });
        test_cases.push_back({ "smaller size", { 1, 2, 3 }, { 2, 1 } });
        test_cases.push_back({ "greater size", { 1, 2, 3 }, { 2, 1, 3, 4 } });
        test_cases.push_back({ "all same items", { 1, 2, 3 }, { 2, 2, 2 } });
        test_cases.push_back({ "empty 2", { 1, 2, 3 }, {} });
        test_cases.push_back({ "impossible 312", { 1, 2, 3 }, { 3, 1, 2 } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual =
            Standard::Algorithms::Numbers::is_push_pop_stack_sequence<int_t>(test.array1(), test.array2());

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "is_push_pop_stack_sequence");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::is_push_pop_stack_sequence_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
