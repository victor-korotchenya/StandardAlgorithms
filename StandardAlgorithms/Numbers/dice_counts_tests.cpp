#include"dice_counts_tests.h"
#include"../Utilities/test_utilities.h"
#include"dice_counts.h"
#include<numeric>

namespace
{
    using int_t = std::uint64_t;
    using vect_t = std::vector<int_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, int_t size, vect_t &&expected = {}) noexcept
            : base_test_case(std::move(name))
            , Size(size)
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto size() const noexcept -> int_t
        {
            return Size;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const vect_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            ::Standard::Algorithms::ert::greater(Size, int_t{}, "size");

            constexpr auto sides = static_cast<int_t>(Standard::Algorithms::Numbers::dice_counts<int_t>::sides);

            {
                const auto expected_size = static_cast<int_t>(Size * sides + 1LLU);

                ::Standard::Algorithms::ert::are_equal(expected_size, Expected.size(), "Expected size");
            }
            // todo(p3): Is Bitonic sorted, symmetric.
            // Standard::Algorithms::require_sorted<SumCount>(Expected, "Expected_" +
            // Name, true);
            {
                constexpr int_t min_value{};

                ::Standard::Algorithms::ert::are_equal(min_value, Expected.at(0), "MinValue");
            }
            {
                constexpr int_t max_value = 1;

                const auto &last = Expected.back();
                ::Standard::Algorithms::ert::are_equal(max_value, last, "MaxValue");
            }
            {
                auto sum_counts_expected = sides;

                for (int_t index = 1; index < Size; ++index)
                {
                    sum_counts_expected = Standard::Algorithms::Numbers::multiply_unsigned(sum_counts_expected, sides);
                }

                const auto sum_counts_actual = sum_counts();
                ::Standard::Algorithms::ert::are_equal(sum_counts_expected, sum_counts_actual, "SumCounts");
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print(", Expected ", Expected, str);
            str << " size " << Size;
        }

        [[nodiscard]] constexpr auto sum_counts() const -> int_t
        {
            auto result = std::accumulate(Expected.cbegin(), Expected.cend(), int_t{},
                [] [[nodiscard]] (const int_t &one, const int_t &two)
                {
                    auto result2 = Standard::Algorithms::Numbers::add_unsigned(one, two);
                    return result2;
                });

            return result;
        }

private:
        int_t Size;
        vect_t Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("one", 1, vect_t({ 0, 1, 1, 1, 1, 1, 1 }));

        // 11 12 13 14 15 16
        // 21 22 23 24 25 26
        // 31 32 33 34 35 36
        // 41 42 43 44 45 46
        // 51 52 53 54 55 56
        // 61 62 63 64 65 66
        // Go by diagonals starting from the left upper corner.
        test_cases.emplace_back("two", 2,
            // NOLINTNEXTLINE
            vect_t({ 0, 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1 }));

        test_cases.emplace_back("three", 3,
            vect_t({ 0, 0, 0,
                // NOLINTNEXTLINE
                1, 3, 6, 10, 15, 21, 25, 27,
                // NOLINTNEXTLINE
                27, 25, 21, 15, 10, 6, 3, 1 }));
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual = Standard::Algorithms::Numbers::dice_counts<int_t>::calc_count_per_each_sum(test.size());

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "dice_counts");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::dice_counts_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
