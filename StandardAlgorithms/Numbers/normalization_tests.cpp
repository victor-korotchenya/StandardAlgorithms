#include"normalization_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"normalization.h"
#include"require_same_ranks.h" // build_ranks

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;
    using floating_t = Standard::Algorithms::floating_point_type;

    // The final score norms must have 0 mean, 1 st.dev.
    template<std::floating_point flo_t>
    constexpr void verify_score_norms(const std::string &name, const std::vector<flo_t> &score_norms)
    {
        Standard::Algorithms::throw_if_empty("name", name);

        const auto size = Standard::Algorithms::require_positive(score_norms.size(), name + " score_norms size");

        const auto [sum1, sq_sum] = Standard::Algorithms::Numbers::sum_and_squared_sum<flo_t>(score_norms);

        ::Standard::Algorithms::ert::are_equal_with_epsilon(0.0, sum1, "score_norms final mean");

        ::Standard::Algorithms::ert::are_equal_with_epsilon(static_cast<flo_t>(size), sq_sum, "score_norms final st.dev");
    }

    struct norm_test_t final
    {
        std::string name{};
        std::vector<int_t> input{};
        std::vector<std::size_t> expected_ranks{};
        std::vector<floating_t> min_max_norms{};
        std::pair<long_int_t, long_int_t> sum_sqsum{};
        std::vector<floating_t> score_norms{};
    };

    constexpr void run_norm_tests(const norm_test_t &test)
    {
        Standard::Algorithms::throw_if_empty("name", test.name);

        const auto size = Standard::Algorithms::require_positive(test.input.size(), test.name + " input size");

        ::Standard::Algorithms::ert::are_equal(size, test.expected_ranks.size(), test.name + " expected_ranks size");
        ::Standard::Algorithms::ert::are_equal(size, test.min_max_norms.size(), test.name + " min_max_norms size");
        ::Standard::Algorithms::ert::are_equal(size, test.score_norms.size(), test.name + " score_norms size");

        verify_score_norms(test.name, test.score_norms);

        std::vector<floating_t> temp;
        {
            Standard::Algorithms::Numbers::min_max_normalization(test.input, temp);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(
                test.min_max_norms, temp, test.name + " min_max_normalization");

            const auto ranks = Standard::Algorithms::Numbers::build_ranks(test.min_max_norms);
            ::Standard::Algorithms::ert::are_equal(
                test.expected_ranks, ranks, test.name + " min_max_normalization ranks");
        }
        {
            const auto actual = Standard::Algorithms::Numbers::sum_and_squared_sum<long_int_t>(test.input);

            ::Standard::Algorithms::ert::are_equal(test.sum_sqsum, actual, test.name + " sum_and_squared_sum");
        }
        {
            Standard::Algorithms::Numbers::score_standardize(test.input, temp);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(test.score_norms, temp, test.name + " score_standardize");

            const auto ranks = Standard::Algorithms::Numbers::build_ranks(test.score_norms);
            ::Standard::Algorithms::ert::are_equal(test.expected_ranks, ranks, test.name + " score_standardize ranks");
        }
    }

    [[nodiscard]] constexpr auto pre_normalization_tests() -> bool
    {
        // NOLINTNEXTLINE
        constexpr auto sum1 = 1LL + 2 + 4 + 5 + 6 + 3; // NOLINTNEXTLINE
        constexpr auto sq_sum = 1LL + 4 + 16 + 25 + 36 + 9;

        static_assert( // NOLINTNEXTLINE
            21 == sum1 && 91 == sq_sum);

        // No const as GCC throws.
        // NOLINTNEXTLINE
        norm_test_t test{ .name = "pre", // NOLINTNEXTLINE
            .input = std::vector<int_t>{ 1, 2, 4, 5, 6, 3 }, // NOLINTNEXTLINE
            .expected_ranks = std::vector<std::size_t>{ 0, 1, 3, 4, 5, 2 }, // NOLINTNEXTLINE
            .min_max_norms = std::vector<floating_t>{ 0, 0.2, 0.6, 0.8, 1, 0.4 }, // NOLINTNEXTLINE
            .sum_sqsum = std::make_pair(sum1, sq_sum),
            .score_norms = std::vector<floating_t>{// NOLINTNEXTLINE
                -1.463850109422799805614, -0.878310065653679861164, // NOLINTNEXTLINE
                0.292770021884559972225, 0.878310065653679861164, // NOLINTNEXTLINE
                1.463850109422799805614, -0.292770021884559972225 } };

        run_norm_tests(test);

        return true;
    }

    [[nodiscard]] constexpr auto pretty_normalization_tests() -> bool
    {
        // Mean = E(x) = 15/5 = 3.
        constexpr auto sum1 = 15;

        // E(x*x) = 125/5 = 25.
        // Variance = E(x*x) - E(x)**2 = 25 - 9 = 16.
        // Standard deviation = Sqrt(Variance) = 4. // NOLINTNEXTLINE
        constexpr auto sq_sum = 4 * 1 + 121;

        constexpr floating_t sma = -0.5; // (1 - 3)/4
        constexpr floating_t larger = 2; // (11 - 3)/4

        // No const as GCC throws.
        // NOLINTNEXTLINE
        norm_test_t test{ .name = "pretty", // NOLINTNEXTLINE
            .input = std::vector<int_t>{ 1, 1, 1, 11, 1 },
            .expected_ranks = std::vector<std::size_t>{ 0, 0, 0, 1, 0 },
            .min_max_norms = std::vector<floating_t>{ 0, 0, 0, 1, 0 },
            .sum_sqsum = std::make_pair(sum1, sq_sum),
            .score_norms = std::vector<floating_t>{ sma, sma, sma, larger, sma } };

        run_norm_tests(test);

        return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::normalization_tests()
{
    static_assert(pre_normalization_tests());
    static_assert(pretty_normalization_tests());
}
