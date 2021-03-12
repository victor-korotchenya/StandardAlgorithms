#include"sorted_array_two_element_sum_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"sorted_array_two_element_sum.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;
    using pair_t = Standard::Algorithms::Numbers::sorted_array_two_element_sum::pair_t;

    using algorithm_t = pair_t (*)(const std::vector<long_int_t> &, const long_int_t &); // Algorithm type.

    constexpr long_int_t cutter = 100;

    [[nodiscard]] auto build_data(Standard::Algorithms::Utilities::random_t<std::size_t> &size_random)
        -> std::vector<long_int_t>
    {
        constexpr auto min_size = 2U;
        constexpr auto max_size = 50U;

        const auto size = size_random(min_size, max_size);
        std::vector<long_int_t> data(size);

        Standard::Algorithms::Utilities::fill_random<long_int_t, int_t>(data, size, cutter);

        std::sort(data.begin(), data.end());

        ++data.back();
        Standard::Algorithms::require_greater(
            data.back(), data.at(data.size() - 2U), "Ensure that twice the maximum is not achievable.");

        return data;
    }

    [[nodiscard]] auto good_bad_sums(const std::vector<long_int_t> &data,
        Standard::Algorithms::Utilities::random_t<std::size_t> &size_random) -> std::pair<long_int_t, long_int_t>
    {
        const auto size = data.size();
        const auto middle = Standard::Algorithms::require_positive(size >> 1U, "middle");

        const auto low_index = size_random(0U, middle - 1U);
        const auto high_index = size_random(middle, size - 1U);

        const auto good_sum =
            static_cast<long_int_t>(static_cast<long_int_t>(data.at(low_index)) + data.at(high_index));

        auto too_large_sum = data.back() * 2LL;

        return { good_sum, too_large_sum };
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::sorted_array_two_element_sum_tests()
{
    Standard::Algorithms::Utilities::random_t<std::size_t> size_random;

    const auto sorted_data = build_data(size_random);
    const auto size = sorted_data.size();
    const auto [expectedSum, tooLargeSum] = good_bad_sums(sorted_data, size_random);

    const std::vector<std::pair<std::string, algorithm_t>> test_cases{
        std::make_pair("Slow", Standard::Algorithms::Numbers::sorted_array_two_element_sum::slow<long_int_t>),
        std::make_pair("Fast",
            [] [[nodiscard]] (const std::vector<long_int_t> &data2, const long_int_t &sum2) -> pair_t
            {
                auto result2 = Standard::Algorithms::Numbers::sorted_array_two_element_sum::fast(data2, sum2);

                return result2;
            })
    };

    for (const auto &[name, algor] : test_cases)
    {
        assert(!name.empty());
        {
            const auto actual = algor(sorted_data, expectedSum);

            ::Standard::Algorithms::ert::greater(size, actual.first, name + " first index");

            ::Standard::Algorithms::ert::greater(size, actual.second, name + " second index");

            ::Standard::Algorithms::ert::not_equal(actual.first, actual.second, name + " Indexes");

            const auto actual_sum = static_cast<long_int_t>(
                static_cast<long_int_t>(sorted_data.at(actual.first)) + sorted_data.at(actual.second));

            ::Standard::Algorithms::ert::are_equal(expectedSum, actual_sum, name + " Sum");
        }
        {
            const auto actual = algor(sorted_data, tooLargeSum);

            ::Standard::Algorithms::ert::are_equal(
                Standard::Algorithms::Numbers::sorted_array_two_element_sum::not_found_pair(), actual,
                name + " TooLargeSum");
        }
    }
}
