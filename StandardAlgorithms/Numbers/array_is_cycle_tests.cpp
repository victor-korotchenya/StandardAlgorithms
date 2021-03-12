#include"array_is_cycle_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"array_is_cycle.h"

namespace
{
    using int_t = std::int32_t;
    using data_t = std::vector<int_t>;

    constexpr auto not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, data_t &&arr, std::int32_t expected) noexcept
            : base_test_case(std::move(name))
            , Arr(std::move(arr))
            , Expected(static_cast<char>(expected))
        {
        }

        [[nodiscard]] constexpr auto arr() const &noexcept -> const data_t &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> char
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = static_cast<int_t>(Arr.size());
            Standard::Algorithms::require_positive(size, "arr size");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("arr", Arr, str);
            ::Standard::Algorithms::print_value("expected", str, static_cast<std::int32_t>(Expected));
        }

private:
        data_t Arr;
        char Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("1", data_t({ 1 }), 1);
        test_cases.emplace_back("2", data_t({ 2 }), 1);
        test_cases.emplace_back("2 ones", data_t({ 1, 3 }), 1);

        test_cases.emplace_back("3 ones",
            // NOLINTNEXTLINE
            data_t({ 7, 4, 4 }), 1);

        test_cases.emplace_back("4 ones",
            // NOLINTNEXTLINE
            data_t({ 1, 5, 1, 1 }), 1);

        test_cases.emplace_back("5 ones",
            // NOLINTNEXTLINE
            data_t({ 1, 101, -10'024, 6, -4 }), 1);

        test_cases.emplace_back("2 0s", data_t({ 0, 0 }), 0);
        test_cases.emplace_back("2 2-cycles", data_t({ 1, -1, 1, -1 }), 0);

        constexpr auto min_size = 1;
        constexpr auto max_size = 10;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto value_max = 1'000;

            const auto size = rnd();
            data_t arr;
            Standard::Algorithms::Utilities::fill_random(arr, size, value_max);

            test_cases.emplace_back("Random" + std::to_string(att), std::move(arr), not_computed);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::array_is_cycle<int_t>(test.arr());

        if (test.expected() != not_computed)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected() != 0, fast, "array_is_cycle");
        }

        const auto slow = Standard::Algorithms::Numbers::array_is_cycle_slow<int_t>(test.arr());
        ::Standard::Algorithms::ert::are_equal(fast, slow, "array_is_cycle_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::array_is_cycle_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
