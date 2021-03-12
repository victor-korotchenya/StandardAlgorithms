#include"min_merges_get_all_binary_1_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"min_merges_get_all_binary_1.h"

namespace
{
    using int_t = std::int32_t;

    constexpr int_t impossible = -1;
    constexpr int_t not_computed{};

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<std::vector<bool>> &&arr, int_t expected = impossible) noexcept
            : base_test_case(std::move(name))
            , Arr(std::move(arr))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto arr() const &noexcept -> const std::vector<std::vector<bool>> &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const int_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Arr.size(), "array size");
            Standard::Algorithms::require_positive(Arr.at(0).size(), "array[0] size");

            Standard::Algorithms::require_less_equal(impossible, Expected, "expected");

            if (Expected <= 0)
            {
                return;
            }

            Standard::Algorithms::require_less_equal(static_cast<std::size_t>(Expected), Arr.size(), "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("array", Arr, str);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<std::vector<bool>> Arr;
        int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        // Less typing.
        // NOLINTNEXTLINE
        constexpr auto f = false;
        // NOLINTNEXTLINE
        constexpr auto t = true;

        test_cases.push_back({ "2 cleans",
            { { t, t, f, t, f, f }, { f, f, t, t, f, f }, { t, t, f, f, f, f }, { t, f, f, t, t, f },
                { f, f, f, f, t, t } },
            3 });

        test_cases.push_back({ "largest excluded",
            { { t, t, t, f, f, f }, // largest 3
                { t, f, f, f, f, t }, { f, t, f, f, t, f }, { f, f, t, t, f, f } },
            3 });

        test_cases.push_back({ "0s row",
            { { t, f, f }, { f, f, f }, // 0
                { f, t, t } },
            2 });

        test_cases.push_back({ "0s row 2",
            { { f, f, f, f, f, f }, // 0
                { t, f, f, f, f, t }, { f, t, f, f, t, f }, { f, f, t, t, f, f } },
            3 });

        test_cases.push_back({ "all 1",
            { { t, f, f, f, f, t }, { f, t, f, f, t, f }, { t, t, t, t, t, t }, // 1
                { f, f, t, t, f, f } },
            1 });

        test_cases.push_back({ "two",
            { { t, t, f, t, f, t }, { f, f, t, f, t, f }, { f, t, f, t, t, f }, { t, f, f, t, t, t },
                { t, f, t, t, t, t } },
            2 });

        test_cases.push_back({ "1", { { t } }, 1 });

        test_cases.push_back({ "-1", { { f } } });

        test_cases.push_back({ "never column 0", { { f, f, f, t }, { f, f, t, f }, { f, t, f, f } } });

        test_cases.push_back(
            { "never columns 02", { { f, f, f, t }, { f, t, f, f }, { f, f, f, t }, { f, t, f, f } } });

        constexpr auto min_size = 1;
        constexpr auto max_size = 6;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto rows = rnd();
            const auto columns = rnd();

            std::vector<std::vector<bool>> arr(rows, std::vector<bool>(columns, false));

            Standard::Algorithms::Utilities::fill_random(rows, arr, columns);

            test_cases.emplace_back("Random" + std::to_string(att), std::move(arr), not_computed);
        }
    }

    [[nodiscard]] constexpr auto run_fast(const test_case &test) -> int_t
    {
        auto to_be_edited = test.arr();

        auto actul = Standard::Algorithms::Numbers::min_merges_get_all_binary_1_favor_columns<int_t>(to_be_edited);

        if (test.expected() != not_computed)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), actul, "min_merges_get_all_binary_1_favor_columns");
        }

        return actul;
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = run_fast(test);

        const auto rows = Standard::Algorithms::Numbers::min_merges_get_all_binary_1_favor_rows<int_t>(test.arr());

        ::Standard::Algorithms::ert::are_equal(fast, rows, "min_merges_get_all_binary_1_favor_rows");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::min_merges_get_all_binary_1_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
