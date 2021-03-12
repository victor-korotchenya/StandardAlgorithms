#include"count_arrays_different_adjacent_same_1n_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"count_arrays_different_adjacent_same_1n.h"
#include"default_modulus.h"
#include"shift.h"
#include<unordered_set>

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    constexpr int_t mod = Standard::Algorithms::Numbers::default_modulus;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, const std::pair<int_t, int_t> &size1_vals, int_t expected = 0) noexcept
            : base_test_case(std::move(name))
            , Size1(size1_vals.first)
            , Vals(size1_vals.second)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto size1() const noexcept -> int_t
        {
            return Size1;
        }

        [[nodiscard]] constexpr auto vals() const noexcept -> int_t
        {
            return Vals;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> int_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Size1, "size");
            Standard::Algorithms::require_positive(Vals, "values");
            Standard::Algorithms::require_non_negative(Expected, "expected");
            Standard::Algorithms::require_greater(mod, Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("size", str, Size1);
            ::Standard::Algorithms::print_value("values", str, Vals);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        int_t Size1;
        int_t Vals;
        int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        using p_t = std::pair<int_t, int_t>;

        // 121, 131, 212, 232, 313, 323.
        test_cases.emplace_back("base1", p_t{ 1, 1 }, 1);
        test_cases.emplace_back("base2", p_t{ 2, 1 }, 1);
        test_cases.emplace_back("base3", p_t{ 3, 1 }, 0);
        test_cases.emplace_back("base4", p_t{ 2, 2 }, 2);
        // NOLINTNEXTLINE
        test_cases.emplace_back("base6", p_t{ 3, 3 }, 6);
        // NOLINTNEXTLINE
        test_cases.emplace_back("base7", p_t{ 5, 4 }, 84);

        std::unordered_set<int_t> uniq{};

        const auto lam = [] [[nodiscard]] (const int_t &one, const int_t &two) noexcept -> int_t
        {
            constexpr auto shift = 16U;

            auto resu = (Standard::Algorithms::Numbers::to_unsigned(one) << shift) |
                Standard::Algorithms::Numbers::to_unsigned(two);

            return Standard::Algorithms::Numbers::to_signed(resu);
        };

        for (const auto &test_case : test_cases)
        {
            const auto key = lam(test_case.size1(), test_case.vals());
            const auto ins = uniq.insert(key);

            ::Standard::Algorithms::ert::are_equal(
                true, ins.second, "Preliminary tests must consist of the unique {size, values} pairs.");
        }

        constexpr auto min_size = 1;
        constexpr auto max_size = 6;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1; // Be careful not to use too large value.

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size1 = rnd();
            const auto vals = rnd();
            const auto key = lam(size1, vals);

            if (!uniq.insert(key).second)
            {
                continue;
            }

            test_cases.emplace_back("Random" + std::to_string(att), p_t{ size1, vals });
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::count_arrays_different_adjacent_same_1n<long_int_t, int_t, mod>(
            test.size1(), test.vals());

        if (test.expected() != 0)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "count_arrays_different_adjacent_same_1n");
        }

        const auto slow = Standard::Algorithms::Numbers::count_arrays_different_adjacent_same_1n_slow<int_t, mod>(
            test.size1(), test.vals());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "count_arrays_different_adjacent_same_1n_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::count_arrays_different_adjacent_same_1n_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
