#include"del_min_side_items_2min_gt_max_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"del_min_side_items_2min_gt_max.h"

namespace
{
    using int_t = std::int16_t;
    using long_int_t = std::int64_t;
    using pair_t = std::pair<std::size_t, std::size_t>;

    constexpr auto not_computed = std::numeric_limits<std::size_t>::max();

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&arr, std::size_t left = not_computed,
            std::size_t right = not_computed) noexcept
            : base_test_case(std::move(name))
            , Arr(std::move(arr))
            , Expected({ left, right })
        {
        }

        [[nodiscard]] constexpr auto is_computed() const noexcept -> bool
        {
            return not_computed != Expected.first;
        }

        [[nodiscard]] constexpr auto arr() const &noexcept -> const std::vector<int_t> &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const pair_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            if (!is_computed())
            {
                return;
            }

            const auto size = Arr.size();

            ::Standard::Algorithms::ert::greater_or_equal(size, Expected.first, "expected first");

            ::Standard::Algorithms::ert::greater_or_equal(size, Expected.second, "expected second");

            ::Standard::Algorithms::ert::greater_or_equal(size, Expected.first + Expected.second, "expected sum");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("arr", Arr, str);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<int_t> Arr;
        pair_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr auto min_size = 1;
        constexpr auto max_size = 10;

        Standard::Algorithms::Utilities::random_t<std::int16_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size = rnd();
            std::vector<int_t> arr(size);

            constexpr int_t cutter = 100;

            Standard::Algorithms::Utilities::fill_random(arr, size, cutter);

            // Make positive.
            const auto mini = *std::min_element(arr.cbegin(), arr.cend());

            for (auto &datum : arr)
            {
                datum = static_cast<int_t>(datum + 1 - mini);

                assert(int_t{} < datum);
            }

            test_cases.emplace_back("Random" + std::to_string(att), std::move(arr));
        }

        test_cases.push_back({ "some",
            // NOLINTNEXTLINE
            { 10, 1, 12, 100 }, 0, 3 });

        test_cases.push_back({ "all",
            // NOLINTNEXTLINE
            { -10, -1, -12, -100, 0 }, 5, 0 });

        test_cases.push_back({ "none", { 1, 1, 1, 1 }, 0, 0 });

        test_cases.push_back({ "base1",
            // NOLINTNEXTLINE
            { 1, 10, 1, 1, 1, 5, 7, 80 }, 2, 3 });
    }

    constexpr void check_cut(const std::string &name, const std::vector<int_t> &arr, const pair_t &result)
    {
        const auto size = arr.size();

        ::Standard::Algorithms::ert::greater_or_equal(size, result.first, name + " first");

        ::Standard::Algorithms::ert::greater_or_equal(size, result.second, name + " second");

        if (size == 0U)
        {
            return;
        }

        if (const auto is_dell_all = size == result.first + result.second; is_dell_all)
        {
            const auto &max1 = *std::max_element(arr.cbegin(), arr.cend());

            ::Standard::Algorithms::ert::greater_or_equal(int_t{}, max1, name + " max element");

            return;
        }

        ::Standard::Algorithms::ert::greater(size, result.first + result.second, name + " total removed");

        auto mini = arr.at(result.first);
        auto maxi = mini;

        for (auto index = result.first + 1U; index < size - result.second; ++index)
        {
            const auto &datum = arr[index];

            if (datum < mini)
            {
                mini = datum;
            }
            else if (maxi < datum)
            {
                maxi = datum;
            }
        }

        Standard::Algorithms::require_positive(mini, name + " mini");

        ::Standard::Algorithms::ert::greater_or_equal(maxi, mini, name + " maxi");

        ::Standard::Algorithms::ert::greater(mini * static_cast<long_int_t>(2), maxi, name + " maxi < 2*mini");
    }

    constexpr void compare_lengths(
        const std::string &name, const pair_t &expected, const std::vector<int_t> &arr, const pair_t &actual)
    {
        check_cut(name + " expected", arr, expected);

        if (expected == actual)
        {
            return;
        }

        check_cut(name + " actual", arr, actual);

        if (expected.second + expected.first != actual.second + actual.first)
        {
            ::Standard::Algorithms::ert::are_equal(expected, actual, name);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::del_min_side_items_2min_gt_max<long_int_t>(test.arr());

        if (test.is_computed())
        {
            compare_lengths("del_min_side_items_2min_gt_max", test.expected(), test.arr(), fast);
        }

        const auto slow = Standard::Algorithms::Numbers::del_min_side_items_2min_gt_max_slow<long_int_t>(test.arr());

        compare_lengths("del_min_side_items_2min_gt_max_slow", fast, test.arr(), slow);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::del_min_side_items_2min_gt_max_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
