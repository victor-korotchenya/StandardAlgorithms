#include"array_max_xor_two_elements_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"array_max_xor_two_elements.h"

namespace
{
    using int_t = std::conditional_t<::Standard::Algorithms::is_debug, std::uint16_t, std::uint64_t>;
    using vec_int_t = std::vector<int_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, vec_int_t, int_t>;

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases = {// NOLINTNEXTLINE
            test_case{ "Trivial close", vec_int_t{ 5, 6 }, static_cast<int_t>(3) },
            // NOLINTNEXTLINE
            { "Trivial close symmetric", vec_int_t{ 6, 5 }, 3 },
            // NOLINTNEXTLINE
            { "Many", vec_int_t{ 2, 8, 13, 7, 10, 12 }, 15 }, { "Smallest", vec_int_t{ 0, 1 }, 1 },
            { "Smallest symmetric", vec_int_t{ 1, 0 }, 1 },
            { "Smallest with repetitions", vec_int_t{ 0, 1, 0, 0, 1, 1, 1, 1 }, 1 },
            // NOLINTNEXTLINE
            { "Trivial", vec_int_t{ 5, 2 }, 7 },
            // NOLINTNEXTLINE
            { "Trivial symmetric", vec_int_t{ 2, 5 }, 7 },
            // NOLINTNEXTLINE
            { "Trivial large", vec_int_t{ 127, 16 }, 111 },
            // NOLINTNEXTLINE
            { "Trivial large symmetric", vec_int_t{ 16, 127 }, 111 }, { "Equal 0", vec_int_t{ 0, 0 }, 0 },
            { "Equal 1", vec_int_t{ 1, 1 }, 0 },
            // NOLINTNEXTLINE
            { "Equal 5", vec_int_t{ 5, 5 }, 0 }
        };

        const auto max_attempts = ::Standard::Algorithms::is_debug ? 1 : 5;

        for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
        {
            constexpr auto min_value = 2U;

            constexpr auto max_value = ::Standard::Algorithms::is_debug ? 10U : 33U;

            Standard::Algorithms::Utilities::random_t<std::size_t> size_random;
            const auto data_size = size_random(min_value, max_value);

            constexpr auto cutter = ::Standard::Algorithms::is_debug ? 64U : 0U;

            std::vector<int_t> data;
            Standard::Algorithms::Utilities::fill_random<int_t, std::uint32_t>(data, data_size, cutter);

            const auto expected = Standard::Algorithms::Numbers::array_max_xor_two_elements(data);

            test_cases.emplace_back("Random" + std::to_string(attempt), std::move(data), expected);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual_slow = Standard::Algorithms::Numbers::array_max_xor_two_elements_slow(test.input());

        ::Standard::Algorithms::ert::are_equal(test.output(), actual_slow, "array_max_xor_two_elements_slow");

        const auto actual = Standard::Algorithms::Numbers::array_max_xor_two_elements(test.input());

        ::Standard::Algorithms::ert::are_equal(test.output(), actual, "array_max_xor_two_elements");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::array_max_xor_two_elements_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
