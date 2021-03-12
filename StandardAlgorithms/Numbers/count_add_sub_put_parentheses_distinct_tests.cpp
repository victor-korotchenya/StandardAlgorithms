#include"count_add_sub_put_parentheses_distinct_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"count_add_sub_put_parentheses_distinct.h"

namespace
{
    using int_t = std::int32_t;
    constexpr auto not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&arr, std::vector<bool> &&negatives,
            std::int32_t expected = not_computed) noexcept
            : base_test_case(std::move(name))
            , Arr(std::move(arr))
            , Negatives(std::move(negatives))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto arr() const &noexcept -> const std::vector<int_t> &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto negatives() const &noexcept -> const std::vector<bool> &
        {
            return Negatives;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> std::int32_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(Arr.size(), "arr.size");

            ::Standard::Algorithms::ert::are_equal(Negatives.size(), size, "negatives size");
            ::Standard::Algorithms::ert::are_equal(false, Negatives[0], "negatives[0]");
            Standard::Algorithms::require_all_non_negative(Arr, "arr");

            if (Expected == not_computed)
            {
                return;
            }

            Standard::Algorithms::require_positive(Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("arr", Arr, str);
            ::Standard::Algorithms::print("negatives", Negatives, str);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<int_t> Arr;
        std::vector<bool> Negatives;
        std::int32_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        // NOLINTNEXTLINE
        constexpr auto t = true;

        // NOLINTNEXTLINE
        constexpr auto f = false;

        // NOLINTNEXTLINE
        test_cases.push_back({ "b1", { 10 },
            {
                f,
            },
            1 });
        // NOLINTNEXTLINE
        test_cases.push_back({ "b2", { 10, 20 }, { f, t }, 1 });
        // NOLINTNEXTLINE
        test_cases.push_back({ "b3", { 1, 2, 4 }, { f, t, t }, 2 }); // 1-2-4=-5, 1-(2-4)=3.

        // NOLINTNEXTLINE
        test_cases.push_back({ "b4", { 1, 2, 4, 8 }, { f, t, t, t }, 4 });
        // NOLINTNEXTLINE
        test_cases.push_back({ "b41", { 0, 1, 2, 4 }, { f, t, t, t }, 4 });
        // NOLINTNEXTLINE
        test_cases.push_back({ "b5", { 1, 2, 4, 8, 16 }, { f, t, t, t, t }, 8 });
        // NOLINTNEXTLINE
        test_cases.push_back({ "b51", { 1, 2, 4, 2, 16 }, { f, t, t, t, t }, 8 });

        constexpr auto value_min = 0;
        constexpr auto value_max = 15;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(value_min, value_max);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 10;

            const auto size1 = rnd(min_size, max_size);

            std::vector<int_t> arr(size1);
            std::vector<bool> signes(size1);

            for (std::int32_t index{}; index < size1; ++index)
            {
                arr[index] = rnd();

                signes[index] = 0 < index && static_cast<bool>(rnd);
            }

            test_cases.emplace_back("Random" + std::to_string(att), std::move(arr), std::move(signes));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast =
            Standard::Algorithms::Numbers::count_add_sub_put_parentheses_distinct<int_t>(test.arr(), test.negatives());

        if (test.expected() != not_computed)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "count_add_sub_put_parentheses_distinct");
        }

        // todo(p3): //const auto slow =
        // Standard::Algorithms::Numbers::count_add_sub_put_parentheses_distinct_slow<int_t>(test.arr(),
        // test.negatives());

        // ::Standard::Algorithms::ert::are_equal(fast, slow, "count_add_sub_put_parentheses_distinct_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::count_add_sub_put_parentheses_distinct_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
