#include"occurrences_in_sorted_array_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"../Utilities/require_utilities.h"
#include"occurrences_in_sorted_array.h"

namespace
{
    using int_t = std::int64_t;
    using vec_int_t = std::vector<int_t>;
    using pair_t = Standard::Algorithms::Numbers::occurrences_in_sorted_array::pair_t;

    constexpr auto missing = Standard::Algorithms::Numbers::occurrences_in_sorted_array::not_found_pair();

    constexpr void test_helper(const vec_int_t &data, const int_t &value, const std::string &name,
        const pair_t &expected, const bool has_expected = true)
    {
        const auto fast = Standard::Algorithms::Numbers::occurrences_in_sorted_array::find_occurrences(data, value);

        Standard::Algorithms::require_less_equal(fast.first, fast.second, name + " fast bounds");

        if (has_expected)
        {
            ::Standard::Algorithms::ert::are_equal(expected, fast, name + " fast");
        }

        const auto slow = Standard::Algorithms::Numbers::occurrences_in_sorted_array::find_occurrences_slow(data, value);

        ::Standard::Algorithms::ert::are_equal(fast, slow, name + " slow");
    }

    constexpr void grenze(const vec_int_t &data, const std::string &name)
    {
        Standard::Algorithms::throw_if_empty(name, data);

        {
            const auto &front = data.at(0);
            const auto before = static_cast<int_t>(front - 1);
            Standard::Algorithms::require_greater(front, before, "The front cannot be min in " + name);

            test_helper(data, before, "Before " + name, missing);
        }
        {
            const auto &back = data.back();
            const auto after = static_cast<int_t>(back + 1);
            Standard::Algorithms::require_greater(after, back, "The back cannot be max in " + name);

            test_helper(data, after, "After " + name, missing);
        }
    }

    constexpr void standard_test()
    {
        constexpr int_t middle = 89;
        constexpr int_t most_frequent = 98;
        const std::string name = "Standard";

        const vec_int_t data{ 1, 1, 1, 1, 1, // NOLINTNEXTLINE
            3, 4, 37, 37, 37, // 10 items
            middle, most_frequent, most_frequent, most_frequent, most_frequent, most_frequent, most_frequent,
            most_frequent, // NOLINTNEXTLINE
            100, 1'000'000, 1'000'000 };

        Standard::Algorithms::require_sorted(data, name);

        {
            const auto &first = data.at(0);
            constexpr pair_t expected{ 0, 4 };

            test_helper(data, first, name + " first element", expected);
        }
        {
            constexpr pair_t expected{ 10, 10 };

            test_helper(data, middle, name + " middle element", expected);
        }
        {
            constexpr pair_t expected{ 11, 17 };

            test_helper(data, most_frequent, name + " most frequent element", expected);
        }
        {
            const auto &last = data.back();

            const pair_t expected{ data.size() - 2U, data.size() - 1U };

            test_helper(data, last, name + " last element", expected);
        }

        grenze(data, name);
    }

    constexpr void all_same_test()
    {
        constexpr auto size = 8U;
        constexpr int_t number = 500;
        const std::string name = "All same";

        const vec_int_t data(size, number);

        Standard::Algorithms::require_sorted(data, name);

        {
            constexpr pair_t expected{ 0U, size - 1U };

            test_helper(data, number, name, expected);
        }

        grenze(data, name);
    }

    void random_test()
    {
        constexpr auto min_size = 1U;

        // NOLINTNEXTLINE
        constexpr auto max_size = ::Standard::Algorithms::is_debug ? 50U : 1024U;

        Standard::Algorithms::Utilities::random_t<std::size_t> size_random(min_size, max_size);
        const auto size = size_random();
        vec_int_t data(size);

        // Create a lot of conflicts to have many repetitions.
        constexpr int_t cutter = 10;
        const std::string name = "Random";

        Standard::Algorithms::Utilities::fill_random(data, size, cutter);
        std::sort(data.begin(), data.end());
        Standard::Algorithms::require_sorted(data, name);

        {
            const auto index = 1U < size ? size_random(0U, size - 1U) : 0U;
            const auto &element = data.at(index);

            test_helper(data, element, name, pair_t{}, false);
        }

        grenze(data, name);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::occurrences_in_sorted_array_tests()
{
    standard_test();
    all_same_test();
    random_test();
}
