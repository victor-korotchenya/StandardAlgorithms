#include"knapsack_dynamo_tests.h"
#include"../Utilities/ert.h"
#include"knapsack_dynamo.h"

namespace
{
    using int_t = std::int32_t;
    using weight_t = std::uint64_t;
    using knapsack_item_t = Standard::Algorithms::Numbers::knapsack_item_t<int_t, weight_t>;

    struct test_case_knapsack final
    {
        std::vector<knapsack_item_t> items{};
        weight_t maximum_weight{};
        int_t expected_value{};
        std::vector<std::size_t> expected_indexes{};
    };

    [[nodiscard]] constexpr auto generate_test_cases() -> std::vector<test_case_knapsack>
    {
        std::vector<test_case_knapsack> test_cases;

        test_cases.push_back({// NOLINTNEXTLINE
            std::vector<knapsack_item_t>{ { 15, 2 }, { 10, 2 }, { 9, 5 }, { 100, 30 } }, // NOLINTNEXTLINE
            8, 25, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 1 } });

        test_cases.push_back({// NOLINTNEXTLINE
            std::vector<knapsack_item_t>{ { 15, 2 }, { 10, 2 }, { 9, 4 }, { 100, 30 } }, // NOLINTNEXTLINE
            8, 34, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 1, 2 } });

        test_cases.push_back({// NOLINTNEXTLINE
            std::vector<knapsack_item_t>{ { 10, 2 }, { 15, 2 }, { 1'000, 35 }, { 100, 5 } }, // NOLINTNEXTLINE
            8, 115, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 1, 3 } });

        test_cases.push_back({// NOLINTNEXTLINE
            std::vector<knapsack_item_t>{ { 1, 9 }, { 1, 10 } }, // NOLINTNEXTLINE
            8, 0, // NOLINTNEXTLINE
            std::vector<std::size_t>{} });

        test_cases.push_back({// NOLINTNEXTLINE
            std::vector<knapsack_item_t>{ { 2, 8 } }, // NOLINTNEXTLINE
            8, 2, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0 } });

        test_cases.push_back({// NOLINTNEXTLINE
            std::vector<knapsack_item_t>{ { 2, 8 }, { 107, 204 } }, // NOLINTNEXTLINE
            8 + 204, 2 + 107, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 1 } });

        test_cases.push_back({// NOLINTNEXTLINE
            std::vector<knapsack_item_t>{ { 2, 8 }, { 107, 204 } }, // NOLINTNEXTLINE
            8 + 204 + 1'000, 2 + 107, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 1 } });

        test_cases.push_back({// NOLINTNEXTLINE
            std::vector<knapsack_item_t>{ { 10'000, 64 }, { 15, 1 }, { 10, 5 }, { 9, 3 }, // NOLINTNEXTLINE
                { 1'000, 17 }, { 5, 4 }, { 100, 59 } }, // NOLINTNEXTLINE
            8, 29, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 1, 3, 5 } });

        return test_cases;
    }

    constexpr void run_test_case(const test_case_knapsack &test, std::vector<std::size_t> &chosen_indexes)
    {
        {
            chosen_indexes.clear();

            const auto actual = Standard::Algorithms::Numbers::knapsack_dynamo<int_t, weight_t>(
                test.items, test.maximum_weight, chosen_indexes);

            ::Standard::Algorithms::ert::are_equal(test.expected_value, actual, "knapsack_dynamo value");

            ::Standard::Algorithms::ert::are_equal(
                test.expected_indexes, chosen_indexes, "knapsack_dynamo chosenIndexes");
        }
        {
            chosen_indexes.clear();

            const auto actual = Standard::Algorithms::Numbers::knapsack_dynamo_slow_still<int_t, weight_t>(
                test.items, test.maximum_weight, chosen_indexes);

            ::Standard::Algorithms::ert::are_equal(test.expected_value, actual, "knapsack_dynamo_slow_still int_t");

            ::Standard::Algorithms::ert::are_equal(
                test.expected_indexes, chosen_indexes, "knapsack_dynamo_slow_still chosenIndexes");
        }
        {
            chosen_indexes.clear();

            const auto actual = Standard::Algorithms::Numbers::knapsack_dynamo_slow<int_t, weight_t>(
                test.items, test.maximum_weight, chosen_indexes);

            ::Standard::Algorithms::ert::are_equal(test.expected_value, actual, "knapsack_dynamo_slow int_t");

            ::Standard::Algorithms::ert::are_equal(
                test.expected_indexes, chosen_indexes, "knapsack_dynamo_slow chosenIndexes");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::knapsack_dynamo_tests()
{
    const auto tests = generate_test_cases();

    std::vector<std::size_t> chosen_indexes;

    for (std::size_t index{}; index < tests.size(); ++index)
    {
        try
        {
            run_test_case(tests[index], chosen_indexes);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Index " << index;
            str << ", error: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }
}
