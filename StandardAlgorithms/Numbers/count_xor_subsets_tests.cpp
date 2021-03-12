#include"count_xor_subsets_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"count_xor_subsets.h"
#include"default_modulus.h"

namespace
{
    using int_t = std::uint32_t;

    constexpr int_t mod = Standard::Algorithms::Numbers::default_modulus;
    constexpr auto not_computed = mod;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, int_t xxx, std::vector<int_t> &&arr, int_t expected = not_computed) noexcept
            : base_test_case(std::move(name))
            , Arr(std::move(arr))
            , Xxx(xxx)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto arr() const &noexcept -> const std::vector<int_t> &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto xxx() const noexcept -> int_t
        {
            return Xxx;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> int_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            ::Standard::Algorithms::ert::greater_or_equal(mod, Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("arr", Arr, str);
            ::Standard::Algorithms::print_value("x", str, Xxx);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<int_t> Arr;
        int_t Xxx;
        int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "base1",
            // 0100 ^ 1101 ^ 1110 = 0111
            // NOLINTNEXTLINE
            7,
            // NOLINTNEXTLINE
            { 0, 1, 2, 3, 4, 0xD, 0xE },
            // with or without 0 -> times two
            // 124, 34, 4DE, 123 ^ 4DE
            // NOLINTNEXTLINE
            4 * 2 });

        test_cases.push_back({ "empty 0", 0, {}, 1 }); // The empty subset counts.
        test_cases.push_back({ "empty 1", 1, {}, 0 });
        test_cases.push_back({ "empty 2", 2, {}, 0 });

        test_cases.push_back({ "one same",
            // NOLINTNEXTLINE
            7, { 7 }, 1 });

        test_cases.push_back({ "one 0",
            // NOLINTNEXTLINE
            0, { 7 }, 1 });

        test_cases.push_back({ "one different",
            // NOLINTNEXTLINE
            4, { 7 }, 0 });

        test_cases.push_back({ "zero 1", 1, { 0 }, 0 });
        test_cases.push_back({ "zero 0", 0, { 0 }, 2 });

        test_cases.push_back({ "two 0 0", 0, { 0, 0 }, 4 });
        test_cases.push_back({ "two 1", 1, { 0, 0 }, 0 });
        test_cases.push_back({ "two same 0", 0, { 2, 2 }, 2 });
        test_cases.push_back({ "two same", 2, { 2, 2 }, 2 });

        test_cases.push_back({ "three same",
            // NOLINTNEXTLINE
            8, { 8, 8, 8 }, 4 });

        test_cases.push_back({ "three same 0",
            // NOLINTNEXTLINE
            0, { 0, 0, 0 }, 8 });

        test_cases.push_back({ "three same 0 1", 1, { 0, 0, 0 }, 0 });
        test_cases.push_back({ "three",
            // NOLINTNEXTLINE
            7, { 1, 2, 4 }, 1 });

        test_cases.push_back({ "three 2 same 0", 0, { 1, 2, 2 }, 2 });
        test_cases.push_back({ "three 2 same 1", 1, { 1, 2, 2 }, 2 });
        test_cases.push_back({ "three 2 same 2", 2, { 1, 2, 2 }, 2 });
        test_cases.push_back({ "three 2 same 3", 3, { 1, 2, 2 }, 2 });
        test_cases.push_back({ "three 2 same 4",
            // NOLINTNEXTLINE
            4, { 1, 2, 2 }, 0 });

        constexpr int_t value_max = 15;

        Standard::Algorithms::Utilities::random_t<std::uint32_t> rnd(0U, value_max);

        auto gena = [&rnd]
        {
            return static_cast<int_t>(rnd());
        };

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr int_t max_size = 5;

            const auto size1 = rnd(0U, max_size);

            std::vector<int_t> arr(size1);
            std::generate(arr.begin(), arr.end(), gena);

            auto xxx = static_cast<int_t>(rnd());

            test_cases.emplace_back("Random" + std::to_string(att), xxx, std::move(arr));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::count_xor_subsets<int_t, mod>(test.arr(), test.xxx());

        if (test.expected() < mod)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "count_xor_subsets");
        }
        {
            const auto slow =
                Standard::Algorithms::Numbers::count_xor_subsets_slow_still<int_t, mod>(test.arr(), test.xxx());

            ::Standard::Algorithms::ert::are_equal(fast, slow, "count_xor_subsets_slow_still");
        }
        {
            const auto slow = Standard::Algorithms::Numbers::count_xor_subsets_slow<int_t, mod>(test.arr(), test.xxx());

            ::Standard::Algorithms::ert::are_equal(fast, slow, "count_xor_subsets_slow");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::count_xor_subsets_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
