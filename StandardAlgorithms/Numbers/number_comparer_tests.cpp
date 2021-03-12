#include"number_comparer_tests.h"
#include"../Utilities/test_utilities.h"
#include"arithmetic.h"
#include"number_comparer.h"

namespace
{
    // todo(p4): using floating_t = Standard::Algorithms::floating_point_type;
    using floating_t = double;
    using comparer_t = Standard::Algorithms::Numbers::number_comparer<floating_t>;

    constexpr auto infinity = std::numeric_limits<floating_t>::infinity();
    constexpr floating_t epsilon{ 1.0e-5 };

    const auto *const versus = " vs ";

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        test_case(std::string &&name,
            // Two similar parameters are convenient here.
            // NOLINTNEXTLINE
            floating_t one_, floating_t two_, bool expected_ = {}) noexcept
            : base_test_case(std::move(name))
            , One(one_)
            , Two(two_)
            , Expected(expected_)
        {
        }

        test_case(floating_t one, floating_t two, bool expected = {}) noexcept(false)
            : base_test_case(std::to_string(one) + versus + std::to_string(two))
            , One(one)
            , Two(two)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto one() const &noexcept -> const floating_t &
        {
            return One;
        }

        [[nodiscard]] constexpr auto two() const &noexcept -> const floating_t &
        {
            return Two;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> bool
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            // todo(p3): #include<charconv>, std::to_chars, use a faster alternative than std::ostream?
            str << " (" << One << ", " << Two << ") -> " << Expected;
        }

private:
        floating_t One{};
        floating_t Two{};
        bool Expected{};
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("0 vs -epsilon", 0, -epsilon, true);
        test_cases.emplace_back("0 vs epsilon", 0, epsilon, true);
        test_cases.emplace_back("-epsilon * vs -epsilon * epsilon", -epsilon, -epsilon * epsilon, true);
        test_cases.emplace_back("epsilon * vs -epsilon * epsilon", epsilon, -epsilon * epsilon);
        test_cases.emplace_back("epsilon * vs epsilon * epsilon", epsilon, epsilon * epsilon, true);
        test_cases.emplace_back("0 vs -epsilon / 2", 0, -epsilon / 2, true);
        test_cases.emplace_back("0 vs epsilon / 2", 0, epsilon / 2, true);

        {
            constexpr floating_t magic{ 1.1 };
            test_cases.emplace_back("0 vs -1.1 * epsilon", 0, -magic * epsilon);
            test_cases.emplace_back("0 vs 1.1 * epsilon", 0, magic * epsilon);
        }

        test_cases.emplace_back("0 vs -2 * epsilon", 0, -2 * epsilon);
        test_cases.emplace_back("0 vs 2 * epsilon", 0, 2 * epsilon);
        test_cases.emplace_back("epsilon vs -epsilon", epsilon, -epsilon);
        test_cases.emplace_back("epsilon vs 2 * epsilon", epsilon, 2 * epsilon, true);
        test_cases.emplace_back("epsilon vs 3 * epsilon", epsilon, 3 * epsilon);
        test_cases.emplace_back("1 vs 1 + epsilon / 2", 1, 1 + epsilon / 2, true);
        test_cases.emplace_back("1 vs 1 + epsilon", 1, 1 + epsilon);
        test_cases.emplace_back("1 vs 1 - epsilon", 1, 1 - epsilon, true);
        test_cases.emplace_back("1 vs epsilon", 1, epsilon);
        test_cases.emplace_back(-2, -2, true);
        test_cases.emplace_back(-1, -1, true);
        test_cases.emplace_back(-1, 0);
        test_cases.emplace_back(0, 0, true);
        test_cases.emplace_back(0, 1);
        test_cases.emplace_back(0, infinity);
        test_cases.emplace_back(-epsilon, infinity);
        test_cases.emplace_back(epsilon, infinity);
        test_cases.emplace_back(1, 1, true);
        test_cases.emplace_back(1, 2);
        test_cases.emplace_back(2, 2, true);

        {
            constexpr auto num = 500;
            test_cases.emplace_back("500 vs 500 - epsilon", num, num - epsilon, true);
            test_cases.emplace_back("500 vs 500 + epsilon", num, num + epsilon, true);
            test_cases.emplace_back("500 vs 500 * epsilon", num, num * epsilon);
            test_cases.emplace_back("500 vs -infinity", num, -infinity);
            test_cases.emplace_back("500 vs infinity", num, infinity);
        }
        {
            constexpr floating_t one{ 0.12345678912345678 };
            constexpr floating_t two{ 0.1234567891234568 };
            static_assert(one != two);
            static_assert(one - two != floating_t{});

            test_cases.emplace_back("2 close numbers to be equal", one, two, true);
        }
        {
            constexpr auto initial_number = 34'567'89012LL;
            auto num = initial_number;

            do
            {
                test_cases.emplace_back(-num, -num - 1);
                test_cases.emplace_back(num, -num - 1);
                test_cases.emplace_back(num, -num);
                test_cases.emplace_back(num, num - 1);
                test_cases.emplace_back(num, num, true);
                test_cases.emplace_back(num, num + 1);

                constexpr auto denom = 10;
                num /= denom;
            } while (0 < num);
        }

        test_cases.emplace_back("infinity == infinity", infinity, infinity, true);
        test_cases.emplace_back("-infinity == -infinity", -infinity, -infinity, true);
    }

    constexpr void run_test_case(const test_case &test)
    {
        constexpr comparer_t comparer(epsilon, &Standard::Algorithms::Numbers::absolute_value<floating_t>);

        const auto direct = comparer.are_equal(test.one(), test.two());
        ::Standard::Algorithms::ert::are_equal(test.expected(), direct, "are equal");

        const auto swapped = comparer.are_equal(test.two(), test.one());
        ::Standard::Algorithms::ert::are_equal(direct, swapped, "swapped");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::number_comparer_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
