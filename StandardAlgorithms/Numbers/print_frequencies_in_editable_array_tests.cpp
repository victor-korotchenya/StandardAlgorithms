#include"print_frequencies_in_editable_array_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"print_frequencies_in_editable_array.h"

namespace
{
    using int_t = std::int16_t;
    using int_list_t = std::vector<int_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, int_list_t>;

    [[nodiscard]] auto frequencies_slow(const int_list_t &source) noexcept(false) -> int_list_t
    {
        const auto size = Standard::Algorithms::require_positive(source.size(), "size");

        int_list_t frequencies(size);

        for (const auto &bum : source)
        {
            constexpr int_t one = 1;

            const auto index = bum - one;
            ++frequencies.at(index);
        }

        return frequencies;
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("Simple1", int_list_t{ 1 }, int_list_t{ 1 });

        test_cases.emplace_back("Simple23", int_list_t{ 2, 3, 2, 3, 2 }, int_list_t{ 0, 3, 2, 0, 0 });

        {
            constexpr int_t length = 9;
            constexpr int_t nyc = 5;

            test_cases.emplace_back("All same", int_list_t(length, nyc), int_list_t{ 0, 0, 0, 0, length, 0, 0, 0, 0 });
        }
        {
            constexpr int_t size = 20;
            constexpr auto min_size = size;
            constexpr auto max_size = size;

            constexpr int_t min_value = 1;
            constexpr auto max_value = size;

            Standard::Algorithms::Utilities::random_t<int_t> rnd(min_value, max_value);

            int_list_t input =
                Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

            test_cases.emplace_back("Random", std::move(input));
        }
    }

    void run_test_case(const test_case &test)
    {
        const auto &input = test.input();
        auto fast = input;

        Standard::Algorithms::Numbers::print_frequencies_in_editable_array::print_frequencies(fast);

        if (!test.output().empty())
        {
            ::Standard::Algorithms::ert::are_equal(test.output(), fast, "print_frequencies");
        }

        const auto slow = frequencies_slow(input);
        ::Standard::Algorithms::ert::are_equal(fast, slow, "FrequenciesSlow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::print_frequencies_in_editable_array_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
