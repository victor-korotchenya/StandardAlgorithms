#include"sorted_array_three_element_sum_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"sorted_array_three_element_sum.h"
#include<numeric> // accumulate

namespace
{
    using int_t = std::int64_t;
    using tuple_t = Standard::Algorithms::Numbers::sorted_array_three_element_sum::tuple_t;

    constexpr auto min_size = 3U;

    [[nodiscard]] auto get_random_sum(const std::vector<int_t> &sorted_data) -> int_t
    {
        const auto size = sorted_data.size();
        ::Standard::Algorithms::ert::greater_or_equal(size, min_size, "size");

        const auto third = size / min_size;
        Standard::Algorithms::Utilities::random_t<std::size_t> size_random(0U, third - 1U);

        const auto index1 = size_random();
        const auto index2 = size_random(third, (third << 1U) - 1U);
        const auto index3 = size_random(third << 1U, size - 1U);

        auto result = static_cast<int_t>(sorted_data[index1] + sorted_data[index2] + sorted_data[index3]);

        return result;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::sorted_array_three_element_sum_tests()
{
    constexpr auto cutter = static_cast<int_t>(static_cast<int_t>(min_size) * 16);

    const auto size =
        Standard::Algorithms::Utilities::random_t<std::size_t>(min_size, static_cast<std::size_t>(cutter - 1))();

    std::vector<int_t> sorted_data;

    // std::int32_t to have negative numbers.
    Standard::Algorithms::Utilities::fill_random<int_t, std::int32_t>(sorted_data, size, cutter);
    std::sort(sorted_data.begin(), sorted_data.end());

    const auto expected_sum = get_random_sum(sorted_data);

    using algorithm_t = tuple_t (*)(const std::vector<int_t> &, const int_t &);

    const std::vector<std::pair<std::string, algorithm_t>> test_cases{
        std::make_pair("Slow", Standard::Algorithms::Numbers::sorted_array_three_element_sum::slow<int_t>),
        std::make_pair("Fast", Standard::Algorithms::Numbers::sorted_array_three_element_sum::fast<int_t>),
    };

    for (const auto &test : test_cases)
    {
        const auto actual = test.second(sorted_data, expected_sum);

        const std::vector<std::size_t> actual_indexes{ std::get<0>(actual), std::get<1>(actual), std::get<2>(actual) };

        {
            constexpr auto require_unique = true;

            Standard::Algorithms::require_sorted(actual_indexes, "actualIndexes", require_unique);
        }

        ::Standard::Algorithms::ert::greater(size, actual_indexes.at(0), test.first + " first index");

        ::Standard::Algorithms::ert::greater(size, actual_indexes.back(), test.first + " last index");

        const auto actual_sum = std::accumulate(actual_indexes.cbegin(), actual_indexes.cend(), int_t{},
            [&sorted_data](const int_t &sum1, const std::size_t &index1)
            {
                return static_cast<int_t>(sum1 + sorted_data.at(index1));
            });

        ::Standard::Algorithms::ert::are_equal(expected_sum, actual_sum, test.first + " Sum");
    }
}
