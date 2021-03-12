#include"subset_sum_tests.h"
#include"../Utilities/check_chosen_sum.h"
#include"../Utilities/test_utilities.h"
#include"subset_sum.h"

namespace
{
    using int_t = std::uint32_t;
    using long_int_t = std::uint64_t;
    using vec_indexes_t = std::vector<std::size_t>;

    using floating_t = Standard::Algorithms::floating_point_type;
    constexpr floating_t epsilon = 1.0 / 2.0;
    static_assert(Standard::Algorithms::Numbers::is_valid_ptas_epsilon(epsilon));

    // NOLINTNEXTLINE
    static_assert(Standard::Algorithms::Numbers::is_valid_ptas_epsilon(1.0E-10));

    // NOLINTNEXTLINE
    static_assert(!Standard::Algorithms::Numbers::is_valid_ptas_epsilon(1.0E-20));

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&numbers, long_int_t sum_limit,
            vec_indexes_t &&expected_indexes = {}, long_int_t expected_sum = {}) noexcept
            : base_test_case(std::move(name))
            , Numbers(std::move(numbers))
            , Sum_limit(sum_limit)
            , Expected_indexes(std::move(expected_indexes))
            , Expected_sum(expected_sum)
        {
        }

#if defined(__GNUG__) && !defined(__clang__) // todo(p3): del this garbage after G++ is fixed.
        // Avoid an error: 'virtual constexpr {anonymous}::test_case::~test_case()' used before its definition.
        // The "= default" will not compile because of the bug. // NOLINTNEXTLINE
        constexpr ~test_case() noexcept override {}
#endif // todo(p3): End of: del this garbage after G++ is fixed.

        [[nodiscard]] constexpr auto numbers() const &noexcept -> const std::vector<int_t> &
        {
            return Numbers;
        }

        [[nodiscard]] constexpr auto sum_limit() const noexcept -> long_int_t
        {
            return Sum_limit;
        }

        [[nodiscard]] constexpr auto expected_indexes() const &noexcept -> const vec_indexes_t &
        {
            return Expected_indexes;
        }

        [[nodiscard]] constexpr auto expected_sum() const noexcept -> long_int_t
        {
            return Expected_sum;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::throw_if_empty("numbers", Numbers);
            Standard::Algorithms::require_all_positive(Numbers.cbegin(), Numbers.cend(), "numbers");

            Standard::Algorithms::require_positive(Sum_limit, "sum limit");
            Standard::Algorithms::require_less_equal(Expected_sum, Sum_limit, "expected sum vs upper limit");

            Standard::Algorithms::Utilities::check_chosen_sum(
                "validate expected sum", Numbers, Expected_sum, Expected_indexes);
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("numbers", Numbers, str);
            ::Standard::Algorithms::print("sum limit", Sum_limit, str);

            ::Standard::Algorithms::print("expected indexes", Expected_indexes, str);
            ::Standard::Algorithms::print("expected sum", Expected_sum, str);
        }

private:
        std::vector<int_t> Numbers;
        long_int_t Sum_limit;

        vec_indexes_t Expected_indexes;
        long_int_t Expected_sum;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("amigos", // NOLINTNEXTLINE
            std::vector<int_t>{ 292, 268, 276, 254, 288, 242 }, 1'000, // NOLINTNEXTLINE
            vec_indexes_t{ 0, 2, 4 }, 856);

        test_cases.emplace_back("test1", // NOLINTNEXTLINE
            std::vector<int_t>{ 2, 2, 3 }, 6, // NOLINTNEXTLINE
            vec_indexes_t{ 1, 2 }, 5);

        test_cases.emplace_back("test2", // NOLINTNEXTLINE
            std::vector<int_t>{ 2, 11, 4, 20, 3, 20, 11, 110, 9, 10010 }, 10, // NOLINTNEXTLINE
            vec_indexes_t{ 8 }, 9);

        test_cases.emplace_back("All too large", // NOLINTNEXTLINE
            std::vector<int_t>{ 10, 201, 3008 }, 6);

        test_cases.emplace_back("Exact 1-item match", // NOLINTNEXTLINE
            std::vector<int_t>{ 2, 11, 4, 20, 3, 20, 11, 110, 9, 10, 10010 }, 10, // NOLINTNEXTLINE
            vec_indexes_t{ 9 }, 10);

        test_cases.emplace_back("All numbers", // NOLINTNEXTLINE
            std::vector<int_t>{ 7, 500, 202, 20 }, 729, // NOLINTNEXTLINE
            vec_indexes_t{ 0, 1, 2, 3 }, 729);

        test_cases.emplace_back("All numbers plus", // NOLINTNEXTLINE
            std::vector<int_t>{ 7, 500, 202, 20 }, 730, // NOLINTNEXTLINE
            vec_indexes_t{ 0, 1, 2, 3 }, 729);

        test_cases.emplace_back("Almost all", // NOLINTNEXTLINE
            std::vector<int_t>{ 7, 500, 202, 20 }, 239, // NOLINTNEXTLINE
            vec_indexes_t{ 0, 2, 3 }, 229);

        test_cases.emplace_back("Some numbers", // NOLINTNEXTLINE
            std::vector<int_t>{ 7, 500, 202, 20 }, 228, // NOLINTNEXTLINE
            vec_indexes_t{ 2, 3 }, 222);
    }

    constexpr void verify_approx(const test_case &test, const std::string &name, const long_int_t &approx_sum,
        const floating_t &ratio, vec_indexes_t *const chosen_indexes = nullptr)
    {
        assert(!name.empty());

        Standard::Algorithms::require_greater(ratio, floating_t{ 1 }, "approximation ratio");

        if (chosen_indexes != nullptr)
        {
            Standard::Algorithms::Utilities::check_chosen_sum(name, test.numbers(), approx_sum, *chosen_indexes);
        }

        {
            const auto &sum_limit = test.sum_limit();
            Standard::Algorithms::require_less_equal(approx_sum, sum_limit, name + " approx sum vs limit");
        }

        const auto &exact_sum = test.expected_sum();
        Standard::Algorithms::require_less_equal(approx_sum, exact_sum, name + " approx sum vs exact sum");

        const auto exact_flo = static_cast<floating_t>(exact_sum);
        const auto approx_with_err = static_cast<floating_t>(approx_sum) * ratio;

        Standard::Algorithms::require_less_equal(exact_flo, approx_with_err, name + " sum approximation quality");
    }

    constexpr void run_test_case(const test_case &test)
    {
        vec_indexes_t chosen_indexes{};

        {
            const auto sum = Standard::Algorithms::Numbers::maximum_subset_sum<int_t, long_int_t>(
                test.numbers(), test.sum_limit(), chosen_indexes);

            ::Standard::Algorithms::ert::are_equal(test.expected_sum(), sum, "maximum_subset_sum sum");

            std::sort(chosen_indexes.begin(), chosen_indexes.end());
            ::Standard::Algorithms::ert::are_equal(
                test.expected_indexes(), chosen_indexes, "maximum_subset_sum chosen indexes");
        }

        verify_approx(test, "maximum_subset_sum_approximation_half",
            Standard::Algorithms::Numbers::maximum_subset_sum_approximation_half<int_t, long_int_t>(
                test.numbers(), test.sum_limit(), chosen_indexes),
            static_cast<floating_t>(Standard::Algorithms::Numbers::subset_sum_approx_ratio), &chosen_indexes);

        verify_approx(test, "maximum_subset_sum_fptas_approximation",
            Standard::Algorithms::Numbers::maximum_subset_sum_fptas_approximation<floating_t, int_t, long_int_t>(
                test.numbers(), test.sum_limit(), epsilon),
            static_cast<floating_t>(1.0 + epsilon));
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::subset_sum_tests()
{
    static_assert(Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases));
}
