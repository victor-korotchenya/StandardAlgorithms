#include"max_index_difference_two_items_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"max_index_difference_two_items.h"

namespace
{
    using long_int_t = std::int32_t;
    using vec_long_int_t = std::vector<long_int_t>;
    using int_t = std::int16_t;
    using output_t = std::pair<int_t, int_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, vec_long_int_t, output_t>;

    constexpr int_t not_computed = -1;
    constexpr int_t zero_size{};

    constexpr void simple_validate(const test_case &test)
    {
        const auto &[min_index, max_index] = test.output();

        if (const auto isnt_computed = max_index < zero_size; isnt_computed)
        {
            return;
        }

        ::Standard::Algorithms::ert::greater_or_equal(min_index, zero_size, "min index");
        ::Standard::Algorithms::ert::greater_or_equal(max_index, min_index, "min max indexes");

        const auto &input = test.input();
        ::Standard::Algorithms::ert::greater(input.size(), static_cast<std::size_t>(max_index), "max index");
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("Simple1", // NOLINTNEXTLINE
            vec_long_int_t{ 31'236, -1'548, 920, -1'685, 1'376, 2'148 }, // NOLINTNEXTLINE
            output_t{ 1, 5 });

        test_cases.emplace_back("Simple2", // NOLINTNEXTLINE
            vec_long_int_t{ 31'236, -1'548, 920, -1'685, 1'376, 2'148, 31'236 + 3 }, // NOLINTNEXTLINE
            output_t{ 0, 6 });

        test_cases.emplace_back("Decreasing", // NOLINTNEXTLINE
            vec_long_int_t{ 12'345, 1'234, 123, 12, 1 }, // NOLINTNEXTLINE
            output_t{ 0, 0 });

        test_cases.emplace_back("Non-increasing", // NOLINTNEXTLINE
            vec_long_int_t{ 1'234, 1'234, 12, 12, 1 }, // NOLINTNEXTLINE
            output_t{ 0, 0 });

        test_cases.emplace_back("ZigZag", // NOLINTNEXTLINE
            vec_long_int_t{ 10'000, 9'999, 10'000, 12, 9'999 }, // NOLINTNEXTLINE
            output_t{ 1, 2 });

        test_cases.emplace_back("Many decreasing", // NOLINTNEXTLINE
            vec_long_int_t{ 12, 7, 10, 5, 8, 3, 11, 1 }, // NOLINTNEXTLINE
            output_t{ 1, 6 });

        constexpr auto max_attempts = 1;

        for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
        {
            constexpr int_t min_size = 5;
            constexpr long_int_t cutter = std::numeric_limits<long_int_t>::max() >> 2;

            const auto size = std::max(min_size, static_cast<int_t>(min_size + attempt));

            vec_long_int_t data;
            Standard::Algorithms::Utilities::fill_random(data, size, cutter);

            test_cases.emplace_back(
                "Random" + std::to_string(attempt), std::move(data), output_t{ not_computed, not_computed });
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        using service_t = Standard::Algorithms::Numbers::max_index_difference_two_items<long_int_t, int_t, output_t>;
        using function_t = output_t (service_t::*)(const vec_long_int_t &) const;
        using name_function_t = std::pair<const char *, function_t>;

        const std::array sub_cases{ name_function_t("Slow", &service_t::slow),
            name_function_t("Fast", &service_t::fast) };

        const service_t service{};
        auto expected = test.output();

        for (const auto &sub : sub_cases)
        {
            try
            {
                const auto actual = (service.*(sub.second))(test.input());
                {
                    const auto &[min_index, max_index] = actual;

                    ::Standard::Algorithms::ert::greater_or_equal(min_index, zero_size, "actual min index");

                    ::Standard::Algorithms::ert::greater_or_equal(max_index, min_index, "actual min max index");

                    ::Standard::Algorithms::ert::greater(
                        test.input().size(), static_cast<std::size_t>(max_index), "actual max index");
                }

                if (expected.second < zero_size)
                {
                    expected = actual;
                    continue;
                }

                ::Standard::Algorithms::ert::are_equal(expected, actual, "result");
            }
            catch (const std::exception &exc)
            {
                const auto &name = sub.first;

                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << name << " might have an error: " << exc.what();

                Standard::Algorithms::throw_exception(str);
            }
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::max_index_difference_two_items_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
