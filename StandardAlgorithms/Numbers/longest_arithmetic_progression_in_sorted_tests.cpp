#include"longest_arithmetic_progression_in_sorted_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"longest_arithmetic_progression_in_sorted.h"
#include<iostream>

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    constexpr auto print_subseq = false;

    constexpr void demand_subsequence(
        const std::vector<int_t> &arr, const std::string &name, const std::vector<int_t> &subseq)
    {
        assert(!name.empty());

        if (subseq.empty())
        {
            return;
        }

        ::Standard::Algorithms::ert::greater(arr.size(), 0U, "array size " + name);

        for (std::size_t index{}, ind_2{};;)
        {
            if (arr[index] == subseq[ind_2] && ++ind_2 == subseq.size())
            {
                return;
            }

            if (++index == arr.size()) [[unlikely]]
            {
                auto err = "expected " + std::to_string(subseq[ind_2]) + " must have a match in arr. " + name;

                throw std::runtime_error(err);
            }
        }
    }

    constexpr void demand_arithmetic_progression(const std::vector<int_t> &arr, const std::string &name)
    {
        assert(!name.empty());

        auto size = arr.size();

        if (size <= 2U)
        {
            return;
        }

        const auto expected_dif = static_cast<long_int_t>(static_cast<long_int_t>(arr[size - 1U]) - arr[size - 2U]);
        size -= 2U;

        do
        {
            const auto &prev = arr[size - 1U];
            const auto &cur = arr[size];
            const auto actual_dif = static_cast<long_int_t>(static_cast<long_int_t>(cur) - prev);

            ::Standard::Algorithms::ert::are_equal(
                expected_dif, actual_dif, name + ", difference at " + std::to_string(size));
        } while (0U < --size);
    }

    constexpr void compare_increasing_subsequences(const std::vector<int_t> &arr, const std::string &name,
        const std::vector<int_t> &expected, const std::vector<int_t> &actual)
    {
        demand_subsequence(arr, name + " expected", expected);

        if (expected == actual)
        {
            return;
        }

        if (expected.size() != actual.size())
        {
            ::Standard::Algorithms::ert::are_equal(expected, actual, name);
        }

        demand_subsequence(arr, name + " actual", actual);
        demand_arithmetic_progression(actual, name);

        if (actual.size() <= 2U)
        {
            return;
        }

        const auto expected_diff = static_cast<long_int_t>(actual[0]) - actual[1];
        const auto size = static_cast<std::int32_t>(actual.size());

        for (auto index = 1; index < size - 1; ++index)
        {
            const auto actual_diff = static_cast<long_int_t>(actual[index]) - actual[index + 1];

            ::Standard::Algorithms::ert::are_equal(expected_diff, actual_diff, std::to_string(index) + " " + name);
        }
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&arr, std::vector<int_t> &&expected = {}) noexcept
            : base_test_case(std::move(name))
            , Arr(std::move(arr))
            , Expected(std::move(expected))
        {
            std::sort(Arr.begin(), Arr.end());

            std::sort(Expected.begin(), Expected.end());
        }

        [[nodiscard]] constexpr auto arr() const &noexcept -> const std::vector<int_t> &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::vector<int_t> &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            ::Standard::Algorithms::ert::greater_or_equal(Arr.size(), Expected.size(), "array vs expected sizes");

            demand_subsequence(Arr, "Array validate", Expected);
            demand_arithmetic_progression(Expected, "Expected validate");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print("array", Arr, str);
            ::Standard::Algorithms::print("expected", Expected, str);
        }

private:
        std::vector<int_t> Arr;
        std::vector<int_t> Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "three", // NOLINTNEXTLINE
            { 0, 6, 11, 12, 13 }, { 11, 12, 13 } });

        test_cases.push_back({ "three b", // NOLINTNEXTLINE
            { 0, 6, 11, 12, 13 }, { 0, 6, 12 } });

        {
            constexpr auto max_size = 9;

            for (std::int32_t siz{}; siz <= max_size; ++siz)
            {
                constexpr int_t item = 10;

                test_cases.emplace_back(
                    "mono" + std::to_string(siz), std::vector<int_t>(siz, item), std::vector<int_t>(siz, item));
            }
        }

        test_cases.push_back({ "almost mono2", // NOLINTNEXTLINE
            { 9, 10, 10, 11 }, { 9, 10, 11 } });

        test_cases.push_back({ "almost mono3", // NOLINTNEXTLINE
            { 9, 10, 10, 10, 11 }, { 10, 10, 10 } });

        test_cases.push_back({ "almost mono3b", // NOLINTNEXTLINE
            { 9, 10, 10, 10, 11 }, { 9, 10, 11 } });

        test_cases.push_back({ "almost mono4", // NOLINTNEXTLINE
            { 9, 10, 10, 10, 10, 11 }, { 10, 10, 10, 10 } });

        test_cases.push_back({ "almost mono5", // NOLINTNEXTLINE
            { 9, 10, 10, 10, 10, 10, 11 }, { 10, 10, 10, 10, 10 } });

        test_cases.push_back({ "almost mono6", // NOLINTNEXTLINE
            { 10, 10, 10, 10, 10, 10, 11 }, { 10, 10, 10, 10, 10, 10 } });

        test_cases.push_back({ "almost mono7", // NOLINTNEXTLINE
            { 9, 10, 10, 10, 10, 10, 10, 10 }, { 10, 10, 10, 10, 10, 10, 10 } });

        test_cases.push_back({ "s2", // NOLINTNEXTLINE
            { 3, 4, 5, 7, 9 }, { 3, 5, 7, 9 } });

        test_cases.push_back({ "sa", // NOLINTNEXTLINE
            { 0, 3, 4, 5, 7, 9 }, { 3, 5, 7, 9 } });

        test_cases.push_back({ "s0", // NOLINTNEXTLINE
            { 3, 5, 7, 9 }, { 3, 5, 7, 9 } });

        test_cases.push_back({ "s1", // NOLINTNEXTLINE
            { 0, 3, 5, 7, 9 }, { 3, 5, 7, 9 } });

        test_cases.push_back({ "base1", // NOLINTNEXTLINE
            { 5, 5, 15, 21, 25 }, { 5, 15, 25 } });

        test_cases.push_back({ "base2", // NOLINTNEXTLINE
            { 5, 7, 15, 20, 25 }, { 5, 15, 25 } });

        test_cases.push_back({ "power2", // NOLINTNEXTLINE
            { 1, 2, 4, 8, 16, 32, 64 }, { 1, 2 } });

        test_cases.push_back({ "1", // NOLINTNEXTLINE
            { -64 }, { -64 } });

        test_cases.push_back({ "2", // NOLINTNEXTLINE
            { -64, 57 }, { -64, 57 } });

        test_cases.push_back({ "2a", // NOLINTNEXTLINE
            { -64, 0, 57 }, { -64, 0 } });

        constexpr auto min_size = 1;
        constexpr auto max_size = 10;

        static_assert(0 < min_size && min_size <= max_size);

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr int_t cutter = 1'000'000;

            static_assert(int_t{} < cutter && max_size < std::numeric_limits<int_t>::max() / 2 / cutter);

            const auto size = rnd();
            std::vector<int_t> arr;
            Standard::Algorithms::Utilities::fill_random(arr, size, cutter);

            std::sort(arr.begin(), arr.end());

            test_cases.emplace_back("Random" + std::to_string(att), std::move(arr));
        }
    }

    void run_test_case(const test_case &test)
    {
        ::Standard::Algorithms::ert::are_equal(true, std::is_sorted(test.arr().cbegin(), test.arr().cend()), "is_sorted");

        const Standard::Algorithms::elapsed_time_ns tim0;

        const auto fast =
            Standard::Algorithms::Numbers::longest_arithmetic_progression_in_sorted_slow2<long_int_t>(test.arr());
        // const auto fast =
        // Standard::Algorithms::Numbers::longest_arithmetic_progression_in_sorted<long_int_t>(test.arr());

        [[maybe_unused]] const auto elapsed0 = tim0.elapsed();

        if (!test.expected().empty())
        {
            compare_increasing_subsequences(
                test.arr(), "longest_arithmetic_progression_in_sorted", test.expected(), fast);
        }

        const Standard::Algorithms::elapsed_time_ns tim1;

        const auto other =
            Standard::Algorithms::Numbers::longest_arithmetic_progression_in_sorted_other<long_int_t>(test.arr());

        if constexpr (print_subseq)
        {
            const auto elapsed1 = tim1.elapsed();

            const auto ratio = ::Standard::Algorithms::ratio_compute(elapsed0, elapsed1);

            std::cout << " default " << elapsed0 << ", other " << elapsed1 << ", other/def " << ratio << '\n';
        }

        compare_increasing_subsequences(test.arr(), "longest_arithmetic_progression_in_sorted_other", fast, other);

        //{
        //    const auto slow2 =
        //    Standard::Algorithms::Numbers::longest_arithmetic_progression_in_sorted_slow2<long_int_t>(test.arr());
        //
        //    compare_increasing_subsequences(test.arr(), "longest_arithmetic_progression_in_sorted_slow2", fast,
        //    slow2);
        //}
        {
            const auto slow =
                Standard::Algorithms::Numbers::longest_arithmetic_progression_in_sorted_slow<long_int_t>(test.arr());

            compare_increasing_subsequences(test.arr(), "longest_arithmetic_progression_in_sorted_slow", fast, slow);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::longest_arithmetic_progression_in_sorted_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
