#include"text_justification_tests.h"
#include"../Utilities/test_utilities.h"
#include"text_justification.h"

namespace
{
    using pair_t = std::pair<std::size_t, std::size_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<std::size_t> &&word_lengths, std::size_t line_length,
            std::vector<pair_t> &&expected_chosen_indexes, std::size_t expected_result) noexcept
            : base_test_case(std::move(name))
            , Word_lengths(std::move(word_lengths))
            , Line_length(line_length)
            , Expected_result(expected_result)
            , Expected_chosen_indexes(std::move(expected_chosen_indexes))
        {
        }

        [[nodiscard]] constexpr auto word_lengths() const &noexcept -> const std::vector<std::size_t> &
        {
            return Word_lengths;
        }

        [[nodiscard]] constexpr auto line_length() const noexcept -> std::size_t
        {
            return Line_length;
        }

        [[nodiscard]] constexpr auto expected_result() const noexcept -> std::size_t
        {
            return Expected_result;
        }

        [[nodiscard]] constexpr auto expected_chosen_indexes() const &noexcept -> const std::vector<pair_t> &
        {
            return Expected_chosen_indexes;
        }

        void print(std::ostream &str) const override
        {
            str << ", line length " << Line_length << ", expected result " << Expected_result;
            ::Standard::Algorithms::print(", word lengths ", Word_lengths, str);
            ::Standard::Algorithms::print(", expected chosen indexes", Expected_chosen_indexes, str);
        }

        constexpr void validate() const override
        {
            base_test_case::validate();
            ::Standard::Algorithms::ert::greater(Word_lengths.size(), 0U, "There must be at least one word.");
            ::Standard::Algorithms::ert::greater(
                Expected_chosen_indexes.size(), 0U, "There must be at least one chosen index.");

            ::Standard::Algorithms::ert::greater_or_equal(
                Word_lengths.size(), Expected_chosen_indexes.size(), "Expected_chosen_indexes.size.");

            ::Standard::Algorithms::ert::are_equal(Expected_chosen_indexes[0].first, 0U, "The first index must be 0.");

            ::Standard::Algorithms::ert::are_equal(Word_lengths.size() - 1U, Expected_chosen_indexes.back().second,
                "The last chosen index must be equal to (size-1).");

            for (std::size_t index = 1; index < Expected_chosen_indexes.size(); ++index)
            {
                ::Standard::Algorithms::ert::greater(Expected_chosen_indexes[index].first,
                    Expected_chosen_indexes[index - 1ZU].second,
                    "chosen indexes at " + ::Standard::Algorithms::Utilities::zu_string(index));
            }
        }

private:
        std::vector<std::size_t> Word_lengths;
        std::size_t Line_length;
        std::size_t Expected_result;
        std::vector<pair_t> Expected_chosen_indexes;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        // NOLINTNEXTLINE
        test_cases.push_back({ "two lines",
            {
                // NOLINTNEXTLINE
                9, 1, 2, 1, 1 // 8
            },
            // NOLINTNEXTLINE
            10, { pair_t(0, 0), pair_t(1, 4) }, 1 + 4 });

        test_cases.push_back({ "one line",
            {
                1, 2, 1, 1 // 8
            },
            // NOLINTNEXTLINE
            10, { pair_t(0, 3) }, 4 });

        // NOLINTNEXTLINE
        test_cases.push_back({ "simple", { 6, 3, 5, 2, 4 },
            // NOLINTNEXTLINE
            10, { pair_t(0, 0), pair_t(1, 2), pair_t(3, 4) }, 4 * 4 + 1 + 9 });

        test_cases.push_back({ "three lines",
            {
                1, 2, 3, // 8
                         // NOLINTNEXTLINE
                5, 3, // 9
                1, 1, 2, 1 // 8
            },
            // NOLINTNEXTLINE
            10, { pair_t(0, 2), pair_t(3, 4), pair_t(5, 8) }, 4 + 1 + 4 });
    }

    constexpr void run_test_case(const test_case &test)
    {
        {
            // No constant.
            // NOLINTNEXTLINE
            std::vector<pair_t> chosen_indexes;

            const auto actual = Standard::Algorithms::Strings::text_justification_line_word_breaking_wrapping<pair_t>(
                test.word_lengths(), test.line_length(), chosen_indexes);

            ::Standard::Algorithms::ert::are_equal(test.expected_result(), actual, "result");
            ::Standard::Algorithms::ert::are_equal(test.expected_chosen_indexes(), chosen_indexes, "chosen indexes");
        }
        {
            // No constant
            // NOLINTNEXTLINE
            std::vector<pair_t> chosen_indexes;

            const auto slow = Standard::Algorithms::Strings::text_justification_line_word_breaking_wrapping_slow<pair_t>(
                test.word_lengths(), test.line_length(), chosen_indexes);

            ::Standard::Algorithms::ert::are_equal(test.expected_result(), slow, "result slow");
            ::Standard::Algorithms::ert::are_equal(test.expected_chosen_indexes(), chosen_indexes, "chosen indexes slow");
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::text_justification_tests()
{
    // todo(p3): static_assert
    (
        ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases));
}
