#include"longest_alternative_subsequence_tests.h"
#include"../Utilities/test_utilities.h"
#include"longest_alternative_subsequence.h"
#include<list>

namespace
{
    using int_t = std::int32_t;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&arr, std::size_t expected) noexcept
            : base_test_case(std::move(name))
            , Arr(std::move(arr))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto arr() const &noexcept -> const std::vector<int_t> &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::size_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            ::Standard::Algorithms::ert::greater_or_equal(Arr.size(), Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("array", Arr, str);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<int_t> Arr;
        std::size_t Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "4", // NOLINTNEXTLINE
            { -100, -1, -1, 5, 1, 9, 10, 200 }, 4 });

        test_cases.push_back({ "0", {}, 0 });
        test_cases.push_back({ "1", { 1 }, 1 });

        test_cases.push_back({ "same", // NOLINTNEXTLINE
            { 2, 2, 2, 2, 2, 2 }, 1 });

        test_cases.push_back({ "same2", // NOLINTNEXTLINE
            { 2, 2, 2, 2, 2, 2, 2 }, 1 });

        test_cases.push_back({ "2", { 1, 2 }, 2 });
        test_cases.push_back({ "-2", { 2, 1 }, 2 });

        test_cases.push_back({ "3", // NOLINTNEXTLINE
            { 4, 5, 4, 4 }, 3 });

        test_cases.push_back({ "-3", // NOLINTNEXTLINE
            { 4, 3, 3, 4 }, 3 });

        constexpr auto neg_inf = std::numeric_limits<int_t>::min();
        constexpr auto inf = std::numeric_limits<int_t>::max();

        test_cases.push_back({ "inf", { neg_inf, inf, neg_inf, neg_inf, inf, neg_inf, neg_inf }, // NOLINTNEXTLINE
            5 });

        test_cases.push_back(
            { "neg_inf", { inf, inf, neg_inf, neg_inf, inf, neg_inf, neg_inf, inf, neg_inf }, // NOLINTNEXTLINE
                6 });
    }

    void run_test_case(const test_case &test)
    {
        const auto fast =
            Standard::Algorithms::Numbers::longest_alternative_subsequence(test.arr().cbegin(), test.arr().cend());

        ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "longest_alternative_subsequence");

        {
            const auto slow = Standard::Algorithms::Numbers::longest_alternative_subsequence_slow(test.arr());

            ::Standard::Algorithms::ert::are_equal(test.expected(), slow, "longest_alternative_subsequence_slow");
        }

        const std::list<int_t> litmus(test.arr().cbegin(), test.arr().cend());

        const auto actual_li =
            Standard::Algorithms::Numbers::longest_alternative_subsequence(litmus.cbegin(), litmus.cend());

        ::Standard::Algorithms::ert::are_equal(fast, actual_li, "longest_alternative_subsequence on list");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::longest_alternative_subsequence_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
