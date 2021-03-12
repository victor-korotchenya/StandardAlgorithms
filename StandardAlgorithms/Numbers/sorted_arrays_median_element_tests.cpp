#include"sorted_arrays_median_element_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"sorted_arrays_median_element.h"
#include<vector>

namespace
{
    using int_t = std::int32_t;
    using random_t = Standard::Algorithms::Utilities::random_t<int_t>;

    using item_t = std::string;
    using array_t = std::vector<item_t>;
    using input_t = std::pair<array_t, array_t>;

    using range_t = std::pair<std::size_t, std::size_t>;
    using output_t = std::pair<range_t, range_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, input_t, output_t>;

    constexpr auto not_computed_position = -1ZU;

    [[nodiscard]] constexpr auto is_computed(const output_t &output) noexcept -> bool
    {
        auto computed = not_computed_position != output.first.first;
        return computed;
    }

    constexpr void check_array_range(const std::string &name, const array_t &arr, const range_t &range)
    {
        assert(!name.empty());

        Standard::Algorithms::require_less_equal(range.first, range.second, "range. " + name);

        const auto size = arr.size();
        Standard::Algorithms::require_less_equal(range.second, size, "range size. " + name);

        if (range.first == range.second)
        {
            return;
        }

        const auto &first_ment = arr.at(range.first);
        const auto &last_ment = arr.at(range.second - 1ZU);

        ::Standard::Algorithms::ert::are_equal(first_ment, last_ment, "array range elements. " + name);
    }

    constexpr void check_median(const std::string &name, const array_t &arr, const range_t &range, const item_t &median)
    {
        const auto [first, last] = range;
        const auto size = arr.size();
        assert(!name.empty() && first <= last && last <= size);

        if (0ZU < first)
        {
            const auto &item = arr.at(first - 1ZU);
            ::Standard::Algorithms::ert::greater(median, item, "before median. " + name);
        }

        if (last < size)
        {
            const auto &item = arr.at(last);
            ::Standard::Algorithms::ert::greater(item, median, "after median. " + name);
        }

        if (first < last)
        {
            const auto &item = arr.at(first);
            ::Standard::Algorithms::ert::are_equal(median, item, "median. " + name);
        }
    }

    constexpr void check_result(const std::string &name, const input_t &input, const output_t &output)
    {
        assert(!name.empty());
        assert(is_computed(output));

        const auto &[arr_0, arr_1] = input;
        const auto &[range_0, range_1] = output;

        check_array_range("test first part. " + name, arr_0, range_0);
        check_array_range("test second part. " + name, arr_1, range_1);

        if (arr_0.empty() && arr_1.empty())
        {
            return;
        }

        const auto is_median_in_0 = range_0.first < range_0.second;
        const auto is_median_in_1 = range_1.first < range_1.second;

        ::Standard::Algorithms::ert::are_equal(true, is_median_in_0 || is_median_in_1, "a median must exist. " + name);

        const auto &median = is_median_in_0 ? arr_0.at(range_0.first) : arr_1.at(range_1.first);

        check_median(name + ", 0-th array", arr_0, range_0, median);
        check_median(name + ", 1-st array", arr_1, range_1, median);
    }

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();

        if (const auto &arr = input.first; !std::is_sorted(arr.begin(), arr.end())) [[unlikely]]
        {
            throw std::runtime_error("0-th array must be sorted.");
        }

        if (const auto &arr = input.second; !std::is_sorted(arr.begin(), arr.end())) [[unlikely]]
        {
            throw std::runtime_error("1-st array must be sorted.");
        }

        const auto &output = test.output();
        if (!is_computed(output))
        {
            return;
        }

        check_result("validate output", input, output);
    }

    constexpr auto min_size = 0;
    constexpr auto max_size = 10;

    [[nodiscard]] auto build_array(random_t &rnd) -> array_t
    {
        const auto size = rnd();

        array_t arr(size);
        std::generate(arr.begin(), arr.end(),
            [&rnd]
            {
                return Standard::Algorithms::Utilities::random_string(rnd, min_size, max_size);
            });

        std::sort(arr.begin(), arr.end());
        return arr;
    }

    void add_random(std::vector<test_case> &test_cases)
    {
        random_t rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            auto arr_0 = build_array(rnd);
            auto arr_1 = build_array(rnd);

            test_cases.emplace_back("random" + std::to_string(att), std::make_pair(std::move(arr_0), std::move(arr_1)),
                output_t{ range_t{ not_computed_position, not_computed_position },
                    range_t{ not_computed_position, not_computed_position } });
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        const item_t desyat = "10 e";
        const item_t twenty = "20 p";
        const item_t dreissig = "30 r";
        const item_t sishi = "40 c";
        const item_t amore = "50 t";

        test_cases.emplace_back("both empty stripes", input_t{}, output_t{});

        test_cases.emplace_back("empty + 1", input_t{ {}, { desyat } }, output_t{ {}, { 0, 1 } });

        test_cases.emplace_back("empty + 11", input_t{ {}, { desyat, desyat } }, output_t{ {}, { 0, 2 } });

        test_cases.emplace_back("empty + 12", input_t{ {}, { desyat, twenty } }, output_t{ {}, { 1, 2 } });

        test_cases.emplace_back("1 + 11", input_t{ { desyat }, { desyat, desyat } }, output_t{ { 0, 1 }, { 0, 2 } });

        test_cases.emplace_back("1 + 12", input_t{ { desyat }, { desyat, twenty } }, output_t{ { 0, 1 }, { 0, 1 } });

        test_cases.emplace_back(
            "1 + 123", input_t{ { desyat }, { desyat, twenty, dreissig } }, output_t{ { 1, 1 }, { 1, 2 } });

        test_cases.emplace_back(
            "1 + 1234", input_t{ { desyat }, { desyat, twenty, dreissig, sishi } }, output_t{ { 1, 1 }, { 1, 2 } });

        test_cases.emplace_back("1 + 12345", input_t{ { desyat }, { desyat, twenty, dreissig, sishi, amore } },
            output_t{ { 1, 1 }, { 2, 3 } });

        test_cases.emplace_back("1 + 22", input_t{ { desyat }, { twenty, twenty } }, output_t{ { 1, 1 }, { 0, 2 } });

        test_cases.emplace_back(
            "11 + 22", input_t{ { desyat, desyat }, { twenty, twenty } }, output_t{ { 2, 2 }, { 0, 2 } });

        test_cases.emplace_back(
            "11 + 23", input_t{ { desyat, desyat }, { twenty, dreissig } }, output_t{ { 2, 2 }, { 0, 1 } });

        test_cases.emplace_back(
            "12 + 13", input_t{ { desyat, twenty }, { desyat, dreissig } }, output_t{ { 1, 2 }, { 1, 1 } });

        test_cases.emplace_back("2 + 11", input_t{ { twenty }, { desyat, desyat } }, output_t{ { 0, 0 }, { 0, 2 } });

        test_cases.emplace_back("2 + 12", input_t{ { twenty }, { desyat, twenty } }, output_t{ { 0, 1 }, { 1, 2 } });

        test_cases.emplace_back("2 + 22", input_t{ { twenty }, { twenty, twenty } }, output_t{ { 0, 1 }, { 0, 2 } });

        test_cases.emplace_back(
            "3 + 124", input_t{ { dreissig }, { desyat, twenty, sishi } }, output_t{ { 0, 1 }, { 2, 2 } });

        add_random(test_cases);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &input = test.input();
        const auto &[arr_0, arr_1] = input;
        const auto slow = Standard::Algorithms::Numbers::sorted_arrays_median_element_slow<item_t>(arr_0, arr_1);

        if (const auto &output = test.output(); is_computed(output))
        {
            ::Standard::Algorithms::ert::are_equal(output, slow, "sorted_arrays_median_element_slow");
        }
        else
        {
            check_result("test slow output", input, slow);
        }

        const auto expected_rev = std::make_pair(slow.second, slow.first);
        {
            const auto actual = Standard::Algorithms::Numbers::sorted_arrays_median_element_slow<item_t>(arr_1, arr_0);

            ::Standard::Algorithms::ert::are_equal(expected_rev, actual, "sorted_arrays_median_element_slow rev");
        }
        {
            const auto actual = Standard::Algorithms::Numbers::sorted_arrays_median_element(arr_0, arr_1);

            ::Standard::Algorithms::ert::are_equal(slow, actual, "sorted_arrays_median_element");
        }
        {
            const auto actual = Standard::Algorithms::Numbers::sorted_arrays_median_element(arr_1, arr_0);

            ::Standard::Algorithms::ert::are_equal(expected_rev, actual, "sorted_arrays_median_element rev");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::sorted_arrays_median_element_tests()
{
    // todo(p4): GCC oka; compile error in Clang: "error: invalid operands to binary expression".
    // const std::string item_0 = "0"; const std::string item_1 = "1";
    // const auto comp = item_0 <=> item_1;

    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
