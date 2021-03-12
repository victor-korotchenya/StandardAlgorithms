#include"sum_min_two_consecutive_numbers_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/require_utilities.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"sum_min_two_consecutive_numbers.h"

namespace
{
    using int_t = std::uint64_t;
    using test_alg = Standard::Algorithms::Numbers::sum_min_two_consecutive_numbers<int_t>;

    using pair_t = test_alg::pair_t;
    using vec_pair_t = test_alg::vec_pair_t;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, int_t, vec_pair_t>;

    constexpr void simple_validate(const test_case &test)
    {
        const auto &output = test.output();
        if (output.empty())
        {
            return;
        }

        {
            constexpr auto require_unique = true;
            Standard::Algorithms::require_sorted(output, "output", require_unique);
        }

        const auto &input = test.input();

        for (const auto &[low, high] : output)
        {
            ::Standard::Algorithms::ert::greater(high, low, "High vs low");

            const auto actual = Standard::Algorithms::Numbers::sum_to_n(low, high);
            ::Standard::Algorithms::ert::are_equal(input, actual, "range actual sum");
        }
    }

    [[nodiscard]] constexpr auto get_next_number(const std::vector<test_case> &test_cases) -> int_t
    {
        ::Standard::Algorithms::ert::not_empty(test_cases, "GetNextNumber");

        const auto max_test = std::max_element(test_cases.cbegin(), test_cases.cend(),
            [] [[nodiscard]] (const test_case &first, const test_case &second)
            {
                return first.input() < second.input();
            });

        auto result = static_cast<int_t>(max_test->input() + 1);
        return result;
    }

    void add_test_cases_regular(std::vector<test_case> &test_cases)
    {
        if constexpr (!::Standard::Algorithms::is_debug)
        {
            {
                constexpr int_t numb = 3;

                test_cases.emplace_back(std::to_string(numb), numb,
                    vec_pair_t{// NOLINTNEXTLINE
                        pair_t(1, 2) });
            }
            {
                constexpr int_t numb = 5;

                test_cases.emplace_back(std::to_string(numb), numb,
                    vec_pair_t{// NOLINTNEXTLINE
                        pair_t(2, 3) });
            }
            {
                constexpr int_t numb = 6;

                test_cases.emplace_back(std::to_string(numb), numb,
                    vec_pair_t{// NOLINTNEXTLINE
                        pair_t(1, 3) });
            }
            {
                constexpr int_t numb = 7;

                test_cases.emplace_back(std::to_string(numb), numb,
                    vec_pair_t{// NOLINTNEXTLINE
                        pair_t(3, 4) });
            }
            {
                constexpr int_t numb = 9;

                test_cases.emplace_back(std::to_string(numb), numb,
                    vec_pair_t{// NOLINTNEXTLINE
                        pair_t(2, 4), pair_t(4, 5) });
            }
            {
                constexpr int_t numb = 10;

                test_cases.emplace_back(std::to_string(numb), numb,
                    vec_pair_t{// NOLINTNEXTLINE
                        pair_t(1, 4) });
            }
            {
                constexpr int_t numb = 15;

                test_cases.emplace_back(std::to_string(numb), numb,
                    vec_pair_t{// NOLINTNEXTLINE
                        pair_t(1, 5), pair_t(4, 6), pair_t(7, 8) });
            }
        }

        {
            constexpr int_t numb = 105;

            test_cases.emplace_back(std::to_string(numb), numb,
                vec_pair_t{// NOLINTNEXTLINE
                    pair_t(1, 14), pair_t(6, 15), pair_t(12, 18), pair_t(15, 20), // NOLINTNEXTLINE
                    pair_t(19, 23), pair_t(34, 36), pair_t(52, 53) });
        }

        if constexpr (!::Standard::Algorithms::is_debug)
        {
            {
                constexpr int_t numb = 127;

                test_cases.emplace_back(std::to_string(numb), numb,
                    vec_pair_t{// NOLINTNEXTLINE
                        pair_t(63, 64) });
            }
            {
                constexpr int_t numb = 129;

                test_cases.emplace_back(std::to_string(numb), numb,
                    vec_pair_t{// NOLINTNEXTLINE
                        pair_t(19, 24), pair_t(42, 44), pair_t(64, 65) });
            }
            {
                constexpr int_t numb = 4095;

                test_cases.emplace_back(std::to_string(numb), numb,
                    vec_pair_t{// NOLINTNEXTLINE
                        pair_t(1, 90), pair_t(14, 91), pair_t(24, 93), pair_t(31, 95), // NOLINTNEXTLINE
                        pair_t(34, 96), pair_t(69, 113), pair_t(77, 118), pair_t(86, 124), // NOLINTNEXTLINE
                        pair_t(100, 134), pair_t(122, 151), pair_t(145, 170), pair_t(185, 205), // NOLINTNEXTLINE
                        pair_t(219, 236), pair_t(266, 280), pair_t(286, 299), pair_t(309, 321), // NOLINTNEXTLINE
                        pair_t(405, 414), pair_t(451, 459), pair_t(582, 588), pair_t(680, 685), // NOLINTNEXTLINE
                        pair_t(817, 821), pair_t(1364, 1366), pair_t(2047, 2048) });
            }
        }
    }

    constexpr void add_test_cases_power_of_two(std::vector<test_case> &test_cases)
    {
        constexpr int_t one = 1;
        constexpr int_t min_value = one << 31U;
        constexpr int_t max_value = one << 36U;

        static_assert(min_value < max_value);

        for (auto index = min_value; index < max_value; index <<= 1U)
        {// Empty expected.
            test_cases.emplace_back(std::to_string(index), index);
        }
    }

    void add_test_case_random(std::vector<test_case> &test_cases)
    {
        constexpr int_t some_number = 100;

        const auto min_value = get_next_number(test_cases);
        const auto max_value = min_value + some_number;

        Standard::Algorithms::Utilities::random_t<int_t> rnd(min_value, max_value);

        for (;;)
        {
            auto input = rnd();

            if (Standard::Algorithms::Numbers::is_power_of_two(input))
            {
                continue;
            }

            auto output = test_alg::slow(input);

            test_cases.emplace_back("Random " + std::to_string(input), input, std::move(output));

            return;
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        add_test_cases_regular(test_cases);
        add_test_case_random(test_cases);

        // Large numbers must be after the random tests.
        add_test_cases_power_of_two(test_cases);
    }

    constexpr void run_test_case(const test_case &test)
    {
        using alg_t  = vec_pair_t (*)(const int_t &);
        using name_alg_t  = std::pair<const char *const, alg_t>;

        const std::vector<name_alg_t> sub_tests{ std::make_pair("Slow", test_alg::slow),
            std::make_pair("over_optimized", test_alg::over_optimized) };

        for (const auto &[name, alg] : sub_tests)
        {
            const auto actual = alg(test.input());

            ::Standard::Algorithms::ert::are_equal(test.output(), actual, name);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::sum_min_two_consecutive_numbers_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
