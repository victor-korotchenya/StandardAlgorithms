#include"min_time_array_abs_dif_sum_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"min_time_array_abs_dif_sum.h"
#include<numeric>

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    constexpr long_int_t not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&arr, long_int_t expected = not_computed) noexcept
            : base_test_case(std::move(name))
            , Arr(std::move(arr))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto arr() const &noexcept -> const std::vector<int_t> &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> long_int_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(static_cast<long_int_t>(Arr.size()), "array size");

            Standard::Algorithms::require_all_positive(Arr, "array");

            if (Expected <= long_int_t{})
            {
                return;
            }

            const auto sum = Standard::Algorithms::require_positive(
                std::accumulate(Arr.cbegin(), Arr.cend(), long_int_t{}), "numbers sum");

            const auto time_sum =
                Standard::Algorithms::require_positive(size * (size + 1LL) / 2, "1 .. size time total sum");
            const auto upper_limit = Standard::Algorithms::require_positive(sum + time_sum, "sum upper limit");

            Standard::Algorithms::require_less_equal(Expected, upper_limit, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("arr", Arr, str);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<int_t> Arr;
        long_int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "Basil", // NOLINTNEXTLINE
            { 2, 3, 3, 5, 5, 6, 6 }, 4 });
        // { 2, 3, 1, 5, 4, 6, 7 } - time
        // { 0, 0, 2, 0, 1, 0, 1 } - |difference|, summing to 4.
        //
        // { 1, 2, 3, 4, 5, 6, 7 } - another valid time assignment
        // { 1, 1, 0, 1, 0, 0, 1 } - |difference|, summing to 4.
        //
        //
        // { 2, 3, 3, 5, 5, 6, 6 } - the source array, again.
        // { 2, 3,*0, 5, 0, 6, 0 } - partially filled time
        // { 0, 0,*0, 0, 0, 0, 0 } - and initially zero differences.
        //
        // It is locally good to assign time[2] = 4 to get |difference| = 1
        // instead of setting time[2] = 1 to get |difference| = 2. Now, Let's observe a big mistake:
        // { 2, 3,*4, 5, 0, 6, 0 } - partially filled time.
        // { 0, 0,*1, 0, 0, 0, 0 } - temp |difference|.
        //  Then, set time[4] = 7.
        // { 2, 3, 4, 5,*7, 6, 0 } - partially filled time.
        // { 0, 0, 1, 0,*2, 0, 0 } - temp |difference|.
        //  Lastly, set time[6] = 8.
        // { 2, 3, 4, 5, 7, 6,*8 } - time.
        // { 0, 0, 1, 0, 2, 0,*2 } - |difference|, summing to 5, not optimal.
        //
        //
        // index source[index]
        //     0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 -  time
        // 0 2   1 0 1 2 3 (last + 1), min is 0
        // 1 3     2 0 1 2 3 ..
        // 2 3       2 1 2 3
        // 3 5         3 1 2 3
        // 4 5           3 2 3 4
        // 5 6             3 3 4 5
        // 6 6               4 5 6 7 - min is 4
        //
        // todo(p4): For arr[index], there are (index+1) unused rows - skip them.

        test_cases.push_back({ "Vasiliy", // NOLINTNEXTLINE
            { 2, 3, 3, 5 }, 1 });
        // { 2, 3, 4, 5 } - time
        // { 0, 0, 1, 0 } - |difference|, summing to 1.
        // Note, that choosing time[2] = 1 is bad here. Proof:
        // { 2, 3, 1, 5 } - time
        // { 0, 0, 2, 0 } - |difference|, summing to 2, not the very best result.

        test_cases.push_back({ "1", { 1 }, 0 });
        test_cases.push_back({ "11", { 1, 1 }, 1 });
        test_cases.push_back({ "111", { 1, 1, 1 }, 3 });
        test_cases.push_back({ "12", { 1, 2 }, 0 });
        test_cases.push_back({ "22", { 2, 2 }, 1 });
        test_cases.push_back({ "222", { 2, 2, 2 }, 2 });
        test_cases.push_back({ "2222", { 2, 2, 2, 2 }, 4 });

        test_cases.push_back({ "35", // NOLINTNEXTLINE
            { 5, 3 }, 0 });

        test_cases.push_back({ "55", // NOLINTNEXTLINE
            { 5, 5 }, 1 });

        test_cases.push_back({ "1114", // NOLINTNEXTLINE
            { 1, 1, 1, 4 }, 3 });
        // { 1, 2, 3, 4 }
        // { 0, 1, 2, 0 }

        test_cases.push_back({ "many 1; 3", // NOLINTNEXTLINE
            { 1, 1, 1, 1, 1, 3 }, 13 });
        //  { 1, 2, 4, 5, 6, 3 }
        //  { 0, 1, 3, 4, 5, 0 }

        test_cases.push_back({ "many 1; 5", // NOLINTNEXTLINE
            { 1, 1, 1, 1, 1, 5 }, 11 });
        //{ 1, 2, 3, 4, 6, 5 }
        //{ 0, 1, 2, 3, 5, 0 }

        test_cases.push_back({ "many 1", // NOLINTNEXTLINE
            { 1, 1, 1, 1, 1 }, 10 });
        //{ 1, 2, 3, 4, 5 }
        //{ 0, 1, 2, 3, 4 }

        test_cases.push_back({ "many 2", // NOLINTNEXTLINE
            { 2, 2, 2, 2, 2, 2 }, 11 });
        //  { 1, 2, 3, 4, 5, 6 }
        //  { 1, 0, 1, 2, 3, 4 }

        test_cases.push_back({ "1 then 3", // NOLINTNEXTLINE
            { 1, 1, 1, 3, 3, 3, 3, 3 }, 18 });
        //  { 1, 2, 4, 3, 5, 6, 7, 8 }
        //  { 0, 1, 3, 0, 2, 3, 4, 5 } 4 + 14

        test_cases.push_back({ "1, 1 and 3", // NOLINTNEXTLINE
            { 1, 1, 1, 1, 3, 3, 3, 3, 3 }, 26 });
        //  { 1, 2, 4, 5, 3, 6, 7, 8, 9 }
        //  { 0, 1, 3, 4, 0, 3, 4, 5, 6 } 8 + 18

        test_cases.push_back({ "20", // NOLINTNEXTLINE
            { 20 }, 0 });

        // no item <=0 for now.
        // test_cases.push_back({ "-1", { -1 }, 2 });
        // test_cases.push_back({ "-1, -2", // NOLINTNEXTLINE
        //{ -1, -2 }, 6 });

        test_cases.push_back({ "We can wait for a long, long time when the numbers are distinct.", // NOLINTNEXTLINE
            { 3, 2, 1, 7 }, 0 });

        test_cases.push_back({ "base1", // NOLINTNEXTLINE
            { 1, 2, 5, 7, 7 }, 1 });
        // { 1, 2, 5, 6, 7 }
        // { 0, 0, 0, 1, 0 }

        test_cases.push_back({ "base2", // NOLINTNEXTLINE
            { 1, 2, 5, 7, 7, 7 }, 2 });
        // { 1, 2, 5, 6, 7, 8 }
        // { 0, 0, 0, 1, 0, 1 }

        test_cases.push_back({ "base3", // NOLINTNEXTLINE
            { 1, 2, 6, 7, 7, 7 }, 3 });
        // { 1, 2, 6, 5, 7, 8 }
        // { 0, 0, 0, 2, 0, 1 }

        test_cases.push_back({ "base4", // NOLINTNEXTLINE
            { 2, 2, 2, 2, 100, 100, 100, 100 }, 8 });
        //  { 1, 2, 3, 4, _98, _99, 100, 101 }
        //  { 1, 0, 1, 2, 002, 001, 000, 001 }

        test_cases.push_back({ "Unique base20", // NOLINTNEXTLINE
            { 1, 2, 3, 9, 7 }, 0 });

        test_cases.push_back({ "base30", // NOLINTNEXTLINE
            { 2, 2, 4, 4, 4, 5 }, 4 });
        // { 1, 2, 3, 4, 6, 5 }
        // { 1, 0, 1, 0, 2, 0 }
        // or
        // { 1, 2, 3, 4, 5, 6}
        // { 1, 0, 1, 0, 1, 1 }

        constexpr auto min_value = 1;
        constexpr auto max_value = 50;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_value, max_value);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 8;

            auto arr = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

            test_cases.emplace_back("Random" + std::to_string(att), std::move(arr));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::min_time_array_abs_dif_sum_still<long_int_t>(test.arr());

        if (not_computed < test.expected())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "min_time_array_abs_dif_sum_still");
        }
        {
            const auto slow = Standard::Algorithms::Numbers::min_time_array_abs_dif_sum_slow<long_int_t>(test.arr());

            ::Standard::Algorithms::ert::are_equal(fast, slow, "min_time_array_abs_dif_sum_slow");
        }
        {
            // const auto soso = Standard::Algorithms::Numbers::min_time_array_abs_dif_sum<long_int_t>(test.arr());

            // ::Standard::Algorithms::ert::are_equal(fast, soso, "min_time_array_abs_dif_sum");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::min_time_array_abs_dif_sum_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
