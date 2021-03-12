#include"min_swaps_between_permutations_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"min_swaps_between_permutations.h"
#include<unordered_set>

namespace
{
    using string_t = std::string;
    constexpr auto not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, string_t &&source, string_t &&destination, std::int32_t expected = not_computed) noexcept
            : base_test_case(std::move(name))
            , Source(std::move(source))
            , Destination(std::move(destination))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto source() const &noexcept -> const string_t &
        {
            return Source;
        }

        [[nodiscard]] constexpr auto destination() const &noexcept -> const string_t &
        {
            return Destination;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> std::int32_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            {
                constexpr auto max_size = 20U;
                const auto *const name = "source size";

                const auto size = Standard::Algorithms::require_positive(Source.size(), name);
                Standard::Algorithms::require_less_equal(size, max_size, name);
            }

            ::Standard::Algorithms::ert::are_equal(Source.size(), Destination.size(), "destination size");

            Standard::Algorithms::require_less_equal(not_computed, Expected, "expected");

            {
                const auto max_expected = static_cast<std::int64_t>(Source.size() * Source.size());

                Standard::Algorithms::require_less_equal(Expected, max_expected, "expected");
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("source", str, Source);
            ::Standard::Algorithms::print_value("destination", str, Destination);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        string_t Source;
        string_t Destination;
        std::int32_t Expected;
    };

    [[nodiscard]] auto manual_test_representations(const std::vector<test_case> &test_cases)
        -> std::unordered_set<string_t>
    {
        Standard::Algorithms::require_positive(test_cases.size(), "The manual tests must go first.");

        std::unordered_set<string_t> uniq{};

        for (const auto &test : test_cases)
        {
            const auto &source = test.source();
            const auto &destination = test.destination();

            const auto [iter, is_added] = uniq.emplace(source + destination);

            ::Standard::Algorithms::ert::are_equal(true, is_added, "The Manual tests must be unique.");
        }

        return uniq;
    }

    void add_random(std::vector<test_case> &test_cases)
    {
        auto uniq = manual_test_representations(test_cases);

        constexpr auto min_size = 2;
        constexpr auto max_size = 5;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);
        string_t sum{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size = rnd();

            string_t source(size, 0);
            string_t destination(size, 0);

            for (std::int32_t index{}; index < size; ++index)
            {
                source[index] = destination[index] = static_cast<char>(index + '1');
            }

            Standard::Algorithms::Utilities::shuffle_data(source);
            Standard::Algorithms::Utilities::shuffle_data(destination);

            sum.resize(size * 2LL);
            std::copy(source.cbegin(), source.cend(), sum.begin());
            std::copy(destination.cbegin(), destination.cend(), sum.begin() + size);

            if (uniq.insert(sum).second)
            {
                test_cases.emplace_back("Random" + std::to_string(att), std::move(source), std::move(destination));
            }
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("base 46", "4321", "1234", // NOLINTNEXTLINE
            6);
        test_cases.emplace_back("base 45", "4321", "1243", // NOLINTNEXTLINE
            5);

        test_cases.emplace_back("base 2a", "2341", "2431", 1);
        test_cases.emplace_back("base 2", "231", "312", 2);
        test_cases.emplace_back("same 1", "1", "1", 0);
        test_cases.emplace_back("same 2", "21", "21", 0);
        test_cases.emplace_back("same 3", "312", "312", 0);
        test_cases.emplace_back("base 1", "132", "312", 1);
        test_cases.emplace_back("base11", "321", "312", 1);

        add_random(test_cases);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast =
            Standard::Algorithms::Numbers::min_swaps_between_permutations<string_t>(test.source(), test.destination());

        if (test.expected() != not_computed)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "min_swaps_between_permutations");
        }
        {
            const auto still = Standard::Algorithms::Numbers::min_swaps_between_permutations_slow_still<string_t>(
                test.source(), test.destination());

            ::Standard::Algorithms::ert::are_equal(fast, still.first, "min_swaps_between_permutations_slow_still");
        }
        {
            const auto slow = Standard::Algorithms::Numbers::min_swaps_between_permutations_slow<string_t>(
                test.source(), test.destination());

            ::Standard::Algorithms::ert::are_equal(fast, slow.first, "min_swaps_between_permutations_slow");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::min_swaps_between_permutations_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
