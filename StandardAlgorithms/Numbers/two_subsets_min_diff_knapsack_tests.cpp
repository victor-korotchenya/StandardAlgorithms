#include"two_subsets_min_diff_knapsack_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"two_subsets_min_diff_knapsack.h"

namespace
{
    using int_t = std::int16_t;
    using long_int_t = std::int32_t;

    [[nodiscard]] constexpr auto sum_by_signs(const std::vector<int_t> &source, const std::vector<bool> &negative_signs)
        -> long_int_t
    {
        ::Standard::Algorithms::ert::are_equal(source.size(), negative_signs.size(), "negative signs size");

        long_int_t sum{};

        for (std::size_t index{}; index < source.size(); ++index)
        {
            sum += (negative_signs[index] ? -1 : 1) * source[index];
        }

        sum = static_cast<long_int_t>(std::abs(sum));

        Standard::Algorithms::require_less_equal(0, sum, "computed sum by signs");

        return sum;
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&source, long_int_t expected_sum = {},
            std::vector<bool> &&negative_signs = {}) noexcept
            : base_test_case(std::move(name))
            , Source(std::move(source))
            , Expected_sum(expected_sum)
            , Negative_signs(std::move(negative_signs))
        {
        }

        [[nodiscard]] constexpr auto source() const &noexcept -> const std::vector<int_t> &
        {
            return Source;
        }

        [[nodiscard]] constexpr auto expected_sum() const noexcept -> long_int_t
        {
            return Expected_sum;
        }

        [[nodiscard]] constexpr auto is_computed() const noexcept -> bool
        {
            return !Negative_signs.empty();
        }

        [[nodiscard]] constexpr auto negative_signs() const &noexcept -> const std::vector<bool> &
        {
            return Negative_signs;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Source.size(), "source size.");
            Standard::Algorithms::require_all_non_negative(Source, "source");

            if (!is_computed())
            {
                return;
            }

            ::Standard::Algorithms::ert::are_equal(Source.size(), Negative_signs.size(), "negative signs size.");

            Standard::Algorithms::require_less_equal(0, Expected_sum, "expected sum");

            const auto sum = sum_by_signs(Source, Negative_signs);

            ::Standard::Algorithms::ert::are_equal(Expected_sum, sum, "expected sum");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("source", Source, str);
            ::Standard::Algorithms::print_value("expected sum", str, Expected_sum);
            ::Standard::Algorithms::print("negative signs", Negative_signs, str);
        }

private:
        std::vector<int_t> Source;
        long_int_t Expected_sum;
        std::vector<bool> Negative_signs;
    };

    void generate_test_case_random(std::vector<test_case> &test_cases)
    {
        constexpr int_t min_size = 1;
        constexpr int_t max_size = ::Standard::Algorithms::is_debug ? 5 : 8;

        static_assert(min_size <= max_size);

        constexpr auto cutter = static_cast<int_t>(max_size * 2);
        static_assert(max_size < cutter);

        Standard::Algorithms::Utilities::random_t<int_t> rnd(min_size, max_size);
        std::vector<int_t> values(rnd.operator() ());

        Standard::Algorithms::Utilities::fill_random(values, values.size(), cutter);

        for (auto &value : values)
        {
            value = static_cast<int_t>(std::abs(value));
        }

        test_cases.emplace_back("Random", std::move(values));
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        generate_test_case_random(test_cases);

        test_cases.emplace_back("simple",
            std::vector<int_t>{// NOLINTNEXTLINE
                10, 40, 7, 20 },
            // NOLINTNEXTLINE
            3, std::vector<bool>{ false, true, false, false });

        {
            constexpr int_t some = 5;

            test_cases.emplace_back("trivial", std::vector<int_t>{ some }, some, std::vector<bool>{ false });
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<bool> negative_signs;

        const auto fast = Standard::Algorithms::Numbers::two_subsets_min_diff_knapsack(test.source(), negative_signs);

        {
            const auto actual_sum = sum_by_signs(test.source(), negative_signs);
            ::Standard::Algorithms::ert::are_equal(fast, actual_sum, "two_subsets_min_diff_knapsack sum");
        }

        if (test.is_computed())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected_sum(), fast, "two_subsets_min_diff_knapsack");
        }

        {
            negative_signs.clear();

            const auto slow =
                Standard::Algorithms::Numbers::two_subsets_min_diff_knapsack_slow<int_t>(test.source(), negative_signs);

            ::Standard::Algorithms::ert::are_equal(fast, slow, "two_subsets_min_diff_knapsack_slow");

            const auto actual_sum = sum_by_signs(test.source(), negative_signs);
            ::Standard::Algorithms::ert::are_equal(slow, actual_sum, "two_subsets_min_diff_knapsack_slow sum");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::two_subsets_min_diff_knapsack_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
