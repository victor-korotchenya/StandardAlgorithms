#include"array_items_counting_over_times_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"array_item_count_over_half_majority_vote.h"
#include"array_items_counting_over_times.h"

namespace
{
    using int_t = std::int32_t;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&data, const std::size_t minimum_count = {},
            std::vector<std::size_t> &&expected = {}) noexcept
            : base_test_case(std::move(name))
            , Data(std::move(data))
            , Minimum_count(minimum_count)
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<int_t> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto minimum_count() const noexcept -> std::size_t
        {
            return Minimum_count;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::vector<std::size_t> &
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Data", Data, str);
            ::Standard::Algorithms::print("Expected", Expected, str);

            str << " Minimum count " << Minimum_count;
        }

private:
        std::vector<int_t> Data;
        std::size_t Minimum_count;
        std::vector<std::size_t> Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "Major",
            // NOLINTNEXTLINE
            { 1, 1, 7, 2, 7, 7, 7 }, 4, { 2 } });

        test_cases.push_back({ "Major7",
            // NOLINTNEXTLINE
            { 1, 7, 7, 2, 7, 7, 7 }, 4, { 1 } });

        test_cases.push_back({ "Same",
            // NOLINTNEXTLINE
            { 7, 7, 7, 7 }, 3, { 0 } });

        test_cases.push_back({ "Simple",
            // NOLINTNEXTLINE
            { 8, 3, 8, 2 }, 2, { 0 } });

        test_cases.push_back({ "Simple2", { 1, 2, 3, 4, 3, 2 }, 2, { 1, 2 } });

        test_cases.push_back({ "None", { 1, 2, 3, 4, 2, 3 }, 3, {} });

        test_cases.push_back({ "One", { 1, 3, 3, 1, 2, 2, 3 }, 3, { 1 } });

        test_cases.push_back({ "All items", { 2, 3, 3, 2, 2, 3 }, 3, { 0, 1 } });

        test_cases.push_back({ "Everything for 1",
            // NOLINTNEXTLINE
            { 1, 2, 3, 1, 7 }, 1, { 0, 1, 2, 4 } });

        test_cases.push_back({ "Nine", { 1, 2, 3, 3, 2, 1, 2, 3, 3 }, 4, { 2 } });

        {
            constexpr auto min_value = 1U;

            constexpr auto max_size = ::Standard::Algorithms::is_debug ? 20U : 99U;

            Standard::Algorithms::Utilities::random_t<std::size_t> size_random(min_value << 1U, max_size);
            const auto size = size_random();

            std::vector<int_t> data;
            Standard::Algorithms::Utilities::fill_random(data, size);

            const auto minimum_count = size_random(min_value, min_value + (size >> 1U));

            auto expected =
                Standard::Algorithms::Numbers::array_items_counting_over_times<int_t>::major_elements_indexes_much_memory(
                    data, minimum_count);

            std::sort(expected.begin(), expected.end());

            test_cases.emplace_back("Random", std::move(data), minimum_count, std::move(expected));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        using alg = std::vector<std::size_t> (*)(const std::vector<int_t> &, const std::size_t);

        const std::vector<std::pair<std::string, alg>> sub_tests = {
            { "Slow",
                &Standard::Algorithms::Numbers::array_items_counting_over_times<
                    int_t>::major_elements_indexes_much_memory },
            { "Fast",
                &Standard::Algorithms::Numbers::array_items_counting_over_times<int_t>::major_elements_indexes_fast }
        };

        for (const auto &sub : sub_tests)
        {
            auto actual = sub.second(test.data(), test.minimum_count());

            // To compare results easily.
            std::sort(actual.begin(), actual.end());

            ::Standard::Algorithms::ert::are_equal(test.expected(), actual, sub.first);

            if (test.minimum_count() <= test.data().size() >> 1U)
            {
                continue;
            }

            const auto actual_it = Standard::Algorithms::Numbers::array_item_count_over_half_majority_vote(
                test.data().cbegin(), test.data().cend());

            std::vector<int_t> expected2;

            if (!test.expected().empty())
            {
                expected2.push_back(test.data().at(test.expected().at(0)));
            }

            std::vector<int_t> actual2;

            if (actual_it != test.data().cend())
            {
                actual2.push_back(*actual_it);
            }

            ::Standard::Algorithms::ert::are_equal(
                expected2, actual2, "array_item_count_over_half_majority_vote " + sub.first);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::array_items_counting_over_times_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
