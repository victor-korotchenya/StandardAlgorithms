#include"string_pattern_adjacent_swap_count_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"default_modulus.h"
#include"string_pattern_adjacent_swap_count.h"

namespace
{
    using int_t = std::int32_t;

    constexpr int_t mod = Standard::Algorithms::Numbers::default_modulus;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, int_t expected) noexcept
            : base_test_case(std::move(name))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const int_t &
        {
            return Expected;
        }

        [[nodiscard]] constexpr auto is_computed() const noexcept -> bool
        {
            return int_t{} <= Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            if (!is_computed())
            {
                return;
            }

            Standard::Algorithms::require_greater(mod, Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        int_t Expected;
    };

    void generate_random(std::vector<test_case> &test_cases)
    {
        const auto test_max = std::max_element(test_cases.cbegin(), test_cases.cend(),
            [] [[nodiscard]] (const test_case &test_0, const test_case &test_1) -> bool
            {
                return test_0.name().size() < test_1.name().size();
            });

        const auto n_max = test_cases.empty() ? 0 : static_cast<std::int32_t>(test_max->name().size());

        constexpr auto zero = 0;
        constexpr auto charms_size = 3;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(zero, charms_size - 1);

        const auto size = rnd(n_max + 1, n_max + 3);
        std::string str(size, 0);

        const std::array<char, charms_size> charms{ '0', '1', '?' };

        for (std::int32_t index{}; index < size; ++index)
        {
            const auto pos = rnd();
            str[index] = charms.at(pos);
        }

        test_cases.emplace_back(std::move(str), -1);
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("0", 0); // NOLINTNEXTLINE
        test_cases.emplace_back("1", 0); // NOLINTNEXTLINE
        test_cases.emplace_back("1100?", 10); // NOLINTNEXTLINE
        test_cases.emplace_back("?", 0); // NOLINTNEXTLINE
        test_cases.emplace_back("?00?", 5); // NOLINTNEXTLINE
        test_cases.emplace_back("?01?", 5); // NOLINTNEXTLINE
        test_cases.emplace_back("?0?", 3); // NOLINTNEXTLINE
        test_cases.emplace_back("?11?", 5); // NOLINTNEXTLINE
        test_cases.emplace_back("??", 1); // NOLINTNEXTLINE
        test_cases.emplace_back("???", 6); // NOLINTNEXTLINE

        generate_random(test_cases);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &pattern = test.name();
        const auto fast = Standard::Algorithms::Numbers::string_pattern_adjacent_swap_count<int_t, mod>(pattern);

        if (test.is_computed())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "string_pattern_adjacent_swap_count");
        }

        const auto slow = Standard::Algorithms::Numbers::string_pattern_adjacent_swap_count_slow<int_t, mod>(pattern);

        ::Standard::Algorithms::ert::are_equal(fast, slow, "string_pattern_adjacent_swap_count_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::string_pattern_adjacent_swap_count_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
