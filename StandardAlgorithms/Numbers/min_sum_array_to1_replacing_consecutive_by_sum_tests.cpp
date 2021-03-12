#include"min_sum_array_to1_replacing_consecutive_by_sum_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"min_sum_array_to1_replacing_consecutive_by_sum.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    constexpr long_int_t not_computed = -2;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, long_int_t takes, std::vector<int_t> &&arr, long_int_t expected = not_computed) noexcept
            : base_test_case(std::move(name))
            , Takes(takes)
            , Arr(std::move(arr))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto takes() const noexcept -> long_int_t
        {
            return Takes;
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

            Standard::Algorithms::require_positive(Arr.size(), "arr size");

            ::Standard::Algorithms::ert::greater_or_equal(Takes, 2, "takes");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("takes", str, Takes);
            ::Standard::Algorithms::print("arr", Arr, str);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        long_int_t Takes;
        std::vector<int_t> Arr;
        long_int_t Expected;
    };

    void add_random(std::vector<test_case> &test_cases)
    {
        constexpr auto max_value = 100;
        static_assert(0 < max_value);

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(-max_value, max_value);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 8;

            const auto size = rnd(min_size, max_size);
            std::vector<int_t> arr(size);

            const auto takes = rnd(min_size + 1, size + 1);

            for (std::int32_t index{}; index < size; ++index)
            {
                arr[index] = rnd();
            }

            test_cases.emplace_back("Random" + std::to_string(att), takes, std::move(arr));
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "item sum 17 vase", // NOLINTNEXTLINE
            3, { 3, 5, 1, 2, 6 }, 25 });
        // Source { 3, 5, 1, 2, 6 }, "takes" is 3, min total sum 25.
        // The first step has 3 possibilities; then only 1 way remains:
        // { 3+5+1, 2, 6 } = { 9, 2, 6 } = { 9+2+6 } = { 17 }; sum = (3+5+1) + (9+2+6) = 9 + 17 = 26.
        // { 3, 5+1+2, 6 } = { 3, 8, 6 } = { 3+8+6 } = { 17 }; sum = (5+1+2) + (3+8+6) = 8 + 17 = 25 (min).
        // { 3, 5, 1+2+6 } = { 3, 5, 9 } = { 3+5+9 } = { 17 }; sum = (1+2+6) + (3+5+9) = 9 + 17 = 26.
        // 17 is the total sum of the items, and not the correct answer which is 25.
        //
        //
        // Log:
        // interval length 3, is divisible 1
        //  best [0][2] = 0 + 9 = 9
        //  best [1][3] = 0 + 8 = 8
        //  best [2][4] = 0 + 9 = 9
        // interval length 4, is divisible 0
        //   partial [0, 0] [1, 3] = 0 + 8 = 8; better 1
        //   partial [0, 2] [3, 3] = 9 + 0 = 9; better 0
        //
        //   partial [1, 1] [2, 4] = 0 + 9 = 9; better 1
        //   partial [1, 3] [4, 4] = 8 + 0 = 8; better 1
        //
        // interval length 5, is divisible 1
        //   partial [0, 0] [1, 4] = 0 + 8 = 8; better 1. Merge (5+1+2)
        //   partial [0, 2] [3, 4] = 9 + 0 = 9; better 0. Merge (3+5+1)
        //  best [0][4] = 8 + 17 = 25

        test_cases.push_back({ "base7", // NOLINTNEXTLINE
            4, { 5, 3, 1, 2, 6, 8, 4 }, 40 });

        test_cases.push_back({ "base4", // NOLINTNEXTLINE
            4, { 5, 3, 1, 2 }, 11 });

        test_cases.push_back({ "base10", // NOLINTNEXTLINE
            4, { 5, 3, 1, 2, 6, 8, 4, 2, 5, 1 }, 60 });

        test_cases.push_back({ "base1", // NOLINTNEXTLINE
            4, { 5 }, 5 });


        test_cases.push_back({ "all < 0", // NOLINTNEXTLINE
            2, { -1, -12, -17 }, -59 });

        test_cases.push_back({ "0 < all", // NOLINTNEXTLINE
            2, { 1, 12, 17 }, 43 });

        test_cases.push_back({ "Impossible", // NOLINTNEXTLINE
            7, { 1, 2 }, -1 });

        test_cases.push_back({ "0", 2, { 0 }, 0 });
        test_cases.push_back({ "1", 3, { 1 }, 1 });

        test_cases.push_back({ "-2", // NOLINTNEXTLINE
            4, { -2 }, -2 });

        add_random(test_cases);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast =
            Standard::Algorithms::Numbers::min_sum_array_to1_replacing_consecutive_by_sum<long_int_t, int_t>(
                test.arr(), test.takes());

        if (not_computed < test.expected())
        {
            ::Standard::Algorithms::ert::are_equal(
                test.expected(), fast, "min_sum_array_to1_replacing_consecutive_by_sum");
        }

        if (static_cast<std::size_t>(Standard::Algorithms::Utilities::factorial_max_slow) < test.arr().size())
        {
            return;
        }

        const auto slow =
            Standard::Algorithms::Numbers::min_sum_array_to1_replacing_consecutive_by_sum_slow<long_int_t, int_t>(
                test.arr(), test.takes());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "min_sum_array_to1_replacing_consecutive_by_sum_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::min_sum_array_to1_replacing_consecutive_by_sum_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
