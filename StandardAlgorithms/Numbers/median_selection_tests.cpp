#include"median_selection_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"dutch_flag_split.h"
#include"median_selection.h"
#include<tuple>

namespace
{
    constexpr std::int32_t delta = 170;

    template<class value_t>
    [[nodiscard]] constexpr auto less_equal_greater(const value_t &value, std::vector<value_t> &arr)
        -> std::tuple<std::size_t, std::size_t, std::size_t>
    {
        auto begin = arr.begin();
        auto end = arr.end();

        const auto [less_end, equ_end] =
            ::Standard::Algorithms::Numbers::dutch_flag_split_into_less_equal_greater_3way_partition(begin, value, end);

        const auto less1 = static_cast<std::size_t>(less_end - begin);
        const auto equal1 = static_cast<std::size_t>(equ_end - less_end);
        const auto greater1 = static_cast<std::size_t>(end - equ_end);

        assert(std::max({ less1, equal1, greater1 }) <= arr.size());
        assert(less1 + equal1 + greater1 == arr.size());

        return { less1, equal1, greater1 };
    }

    using select_order_statistic_function_t = std::int32_t &(*)(std::span<std::int32_t> spa,
        const std::int32_t statistic_order);

    constexpr void test_helper(std::span<std::int32_t> spa, const std::int32_t function_index,
        select_order_statistic_function_t function1, const std::string &error, const std::int32_t order)
    {
        assert(!error.empty());

        Standard::Algorithms::Utilities::shuffle_data(spa);

        const auto &actual_value = function1(spa, order);

        const std::int32_t expected_value = delta + order + 1;

        ::Standard::Algorithms::ert::are_equal(
            expected_value, actual_value, error + ::Standard::Algorithms::Utilities::zu_string(function_index));
    }

    constexpr void fill_array(std::vector<std::int32_t> &data_vector, const std::int32_t size)
    {
        Standard::Algorithms::require_positive(size, "size in FillArray");

        data_vector.resize(size);

        for (std::int32_t index{}; index < size; ++index)
        {
            data_vector[index] = delta + size - index;
        }
    }

    constexpr void test_by_data_size(std::vector<std::int32_t> &data_vector, const std::int32_t data_size,
        Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd, select_order_statistic_function_t function1,
        const std::int32_t function_index)
    {
        fill_array(data_vector, data_size);

        const std::span<std::int32_t> spa(data_vector);
        {
            constexpr std::int32_t order_min{};

            test_helper(spa, function_index, function1, "value min", order_min);
        }

        {
            const auto order_middle = data_size / 2;

            test_helper(spa, function_index, function1, "value middle", order_middle);
        }

        {
            const auto order_max = data_size - 1;

            test_helper(spa, function_index, function1, "value max", order_max);
        }

        if (data_size < 2)
        {
            return;
        }

        auto random_order = static_cast<std::int32_t>(rnd() % data_size);

        if (random_order < 0)
        {
            random_order += data_size;
        }

        test_helper(spa, function_index, function1, "value random", random_order);
    }

    [[nodiscard]] constexpr auto quick_sort_selection_order_statistic(
        std::span<std::int32_t> spa, const std::int32_t statistic_order) -> std::int32_t &
    {
        Standard::Algorithms::require_less_equal(0, statistic_order, "statistic order");

        Standard::Algorithms::require_greater(
            spa.size(), static_cast<std::size_t>(statistic_order), "size vs statistic order");

        std::sort(spa.begin(), spa.end());

        auto &result = spa[statistic_order];
        return result;
    }

    [[nodiscard]] constexpr auto select_order_statistic_test(
        std::span<std::int32_t> spa, const std::int32_t statistic_order) -> std::int32_t &
    {
        Standard::Algorithms::require_less_equal(0, statistic_order, "statistic order");

        Standard::Algorithms::require_greater(
            spa.size(), static_cast<std::size_t>(statistic_order), "size vs statistic order");

        auto &result =
            ::Standard::Algorithms::Numbers::median_selection<std::int32_t>::select_order_statistic(spa, statistic_order);

        return result;
    }

    void tests_performance()
    {
        std::vector<std::int32_t> data_vector;

        constexpr auto data_size = 20;
        constexpr auto median_statistic_order = data_size / 2;

        fill_array(data_vector, data_size);

        constexpr std::int32_t max_attempts = 1;

        for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
        {
            Standard::Algorithms::Utilities::shuffle_data(data_vector);

            auto data_vector1 = data_vector;
            auto data_vector2 = data_vector;

            const Standard::Algorithms::elapsed_time_ns tim1;

            std::nth_element(data_vector1.begin(), data_vector1.begin() + median_statistic_order, data_vector1.end());

            const auto &result1 = data_vector1[median_statistic_order];
            const auto &elapsed1 = tim1.elapsed();


            const Standard::Algorithms::elapsed_time_ns tim2;

            const std::span<std::int32_t> spa(data_vector2);

            const auto &result2 = ::Standard::Algorithms::Numbers::median_selection<std::int32_t>::select_order_statistic(
                spa, median_statistic_order);

            const auto &elapsed2 = tim2.elapsed();

            ::Standard::Algorithms::ert::are_equal(result1, result2, "nth_element vs select_order_statistic");

            if (elapsed1 <= 0 || elapsed2 <= 0 || attempt == 0)
            {
                continue;
            }

            // todo(p4): compare times.
        }
    }

    constexpr void dutch_flag_split_into3_tests()
    {
        using value_t = std::int16_t;

        constexpr value_t value = 4;

        std::vector<value_t> source{// NOLINTNEXTLINE
            8, 9, 12, 10, 0, value, 1, value, 7, value, 20, 23, 22, 24
        };

        const std::vector<value_t> expected_data{// NOLINTNEXTLINE
            1, 0, value, value, value, // NOLINTNEXTLINE
            10, 12, 7, 9, 8, 20, 23, 22, 24
        };

        const std::tuple<std::size_t, std::size_t, std::size_t> expected_split{// NOLINTNEXTLINE
            // less, equal, greater
            2, 3, 9
        };

        {
            const auto actual = less_equal_greater(value, source);

            ::Standard::Algorithms::ert::are_equal(expected_split, actual,
                "dutch_flag_split_into_less_equal_greater_3way_partition Less, equal AMD greater split.");

            ::Standard::Algorithms::ert::are_equal(
                expected_data, source, "dutch_flag_split_into_less_equal_greater_3way_partition Data.");
        }

        const std::span<value_t> spa(source);

        const auto actual_less_equal = Standard::Algorithms::Numbers::Inner::count_less_equal<value_t>(spa, value);

        const auto expected_less_equal = std::make_pair(std::get<0>(expected_split), std::get<1>(expected_split));

        ::Standard::Algorithms::ert::are_equal(expected_less_equal, actual_less_equal, "count_less_equal");
    }

    template<std::integral value_t, class random_t>
    [[nodiscard]] constexpr auto make_non_existing_value(const std::vector<value_t> &data, random_t &rnd)
    {
        constexpr value_t min_val = 1;
        constexpr auto max_val = std::numeric_limits<value_t>::max();

        static_assert(min_val < max_val);

        {
            constexpr auto third = static_cast<std::size_t>(max_val / 3);

            Standard::Algorithms::require_greater(third, data.size(), "third size");
        }

        constexpr auto max_attempts = ::Standard::Algorithms::is_debug ? 20 : 1'000;

        for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
        {
            auto value = static_cast<value_t>(rnd(min_val, max_val));

            if (const auto iter = std::find(data.begin(), data.cend(), value); iter == data.cend())
            {
                return value;
            }
        }

        auto err = std::string("Failed to generate a non-existing value of type ") + typeid(value_t).name();

        throw std::runtime_error(err);
    }

    void dutch_flag_split_into3_tests_random()
    {// todo(p4): merge-join with the above.
        using value_t = std::uint16_t;

        constexpr auto min_size = 1;
        constexpr auto max_size = 30;

        Standard::Algorithms::Utilities::random_t<std::size_t> rnd(min_size, max_size);
        const auto size = rnd();

        std::vector<value_t> vec(size);
        Standard::Algorithms::Utilities::fill_random<value_t>(vec, size, {}, true);
        const auto copy = vec;

        value_t value{};

        if (const auto is_head_or_tail = static_cast<bool>(rnd()); is_head_or_tail)
        {// Use an existing value.
            value = vec.at(vec.size() >> 1U);
        }
        else
        {
            value = make_non_existing_value<value_t>(vec, rnd);
        }

        const auto actual = less_equal_greater(value, vec);

        const std::span<const value_t> spa(copy);

        const auto [less1, equal1] = Standard::Algorithms::Numbers::Inner::count_less_equal<const value_t>(spa, value);

        const auto greater1 = copy.size() - less1 - equal1;

        const auto expected = std::make_tuple(less1, equal1, greater1);

        if (expected == actual)
        {
            return;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();

        ::Standard::Algorithms::print("dutch_flag_split_into_less_equal_greater_3way_partition random", copy, str);

        ::Standard::Algorithms::print(", expected", expected, str);
        ::Standard::Algorithms::print(", actual", actual, str);

        throw std::runtime_error(str.str());
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::median_selection_tests()
{
    std::array functions{ static_cast<select_order_statistic_function_t>(quick_sort_selection_order_statistic),
        static_cast<select_order_statistic_function_t>(select_order_statistic_test) };

    const auto function_index_max = static_cast<std::int32_t>(functions.size());

    constexpr std::int32_t data_size_max = 10;

    std::vector<std::int32_t> data_vector(data_size_max);
    Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

    for (auto data_size = data_size_max; 0 < data_size; --data_size)
    {
        for (std::int32_t function_index{}; function_index < function_index_max; ++function_index)
        {
            test_by_data_size(data_vector, data_size, rnd, functions.at(function_index), function_index);
        }
    }

    // todo(p4): split into several tests.

    tests_performance();
    dutch_flag_split_into3_tests();
    dutch_flag_split_into3_tests_random();
}
