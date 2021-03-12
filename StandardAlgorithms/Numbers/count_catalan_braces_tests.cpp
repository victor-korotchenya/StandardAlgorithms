#include"count_catalan_braces_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"count_catalan_braces.h"

namespace
{
    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::int32_t expected) noexcept
            : base_test_case(std::move(name))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> std::int32_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            for (const auto &nam = this->name(); const auto &cha : nam)
            {
                const auto gut = cha == Standard::Algorithms::Numbers::default_question_c ||
                    cha == Standard::Algorithms::Numbers::default_open_c ||
                    cha == Standard::Algorithms::Numbers::default_close_c;

                ::Standard::Algorithms::ert::are_equal(true, gut, "String chars");
            }

            ::Standard::Algorithms::ert::greater_or_equal(Expected, 0, "expected");

            ::Standard::Algorithms::ert::greater(Standard::Algorithms::Numbers::default_modulus, Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::int32_t Expected;
    };

    void gen_random(std::vector<test_case> &tests, Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd,
        const std::int32_t size)
    {
        Standard::Algorithms::require_positive(size, "size");

        ::Standard::Algorithms::ert::are_equal(
            false, ::Standard::Algorithms::Numbers::has_zero_bit(size), "size must be even");

        std::string str(size, 0);

        for (std::int32_t index{}; index < size; ++index)
        {
            const auto temp = rnd.operator() ();

            if (Standard::Algorithms::Numbers::has_zero_bit(temp))
            {
                str[index] = Standard::Algorithms::Numbers::default_question_c;
                continue;
            }

            str[index] = ::Standard::Algorithms::Numbers::has_zero_bit(temp / 2)
                ? Standard::Algorithms::Numbers::default_open_c
                : Standard::Algorithms::Numbers::default_close_c;
        }

        auto actual = Standard::Algorithms::Numbers::count_catalan_braces(str);

        tests.emplace_back(std::move(str), actual);
    }

    void generate_test_cases(std::vector<test_case> &tests)
    {
        {
            Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

            // NOLINTNEXTLINE
            gen_random(tests, rnd, 10);

            // NOLINTNEXTLINE
            gen_random(tests, rnd, 8);
        }

        // The sizes below must be less than 8.

        // NOLINTNEXTLINE
        tests.emplace_back("??????", 5);

        tests.emplace_back("[?[??]", 3);
        //[][??]
        //[][[]] +1
        //[][][] +1

        //[[[??]
        //[[[]]] + 1

        tests.emplace_back("?[[]]?", 1);

        // size 4
        tests.emplace_back("????", 2);
        tests.emplace_back("[??]", 2);
        tests.emplace_back("???]", 2);
        tests.emplace_back("[]]]", 0);
        tests.emplace_back("[[[]", 0);

        // 3**2 strings of size=2.
        tests.emplace_back("??", 1);
        tests.emplace_back("?]", 1);
        tests.emplace_back("?[", 0);

        tests.emplace_back("[?", 1);
        tests.emplace_back("[[", 0);
        tests.emplace_back("[]", 1);

        tests.emplace_back("]?", 0);
        tests.emplace_back("]]", 0);
        tests.emplace_back("][", 0);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::count_catalan_braces(test.name());

        ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "count_catalan_braces");

        const auto actual_slow = Standard::Algorithms::Numbers::count_catalan_braces_slow(test.name());

        ::Standard::Algorithms::ert::are_equal(fast, actual_slow, "count_catalan_braces_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::count_catalan_braces_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
