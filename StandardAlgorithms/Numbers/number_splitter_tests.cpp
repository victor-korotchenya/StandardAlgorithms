#include"number_splitter_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"number_splitter.h"
#include"permutation.h"

namespace
{
    using int_t = std::int16_t;
    using vec_int_t = std::vector<int_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, vec_int_t>;

    constexpr int_t zero{};

    constexpr void check_split(const vec_int_t &output)
    {
        const auto size = output.size();
        std::size_t index{};

        while (index < size && output[index] < zero)
        {
            ++index;
        }

        while (index < size && output[index] == zero)
        {
            ++index;
        }

        while (index < size && zero < output[index])
        {
            ++index;
        }

        ::Standard::Algorithms::ert::are_equal(size, index, "output must be a valid split around zero.");
    }

    constexpr void simple_validate(const test_case &test)
    {
        const auto &output = test.output();
        if (output.empty())
        {
            return;
        }

        check_split(output);

        {
            const auto isp = Standard::Algorithms::Numbers::is_permutation_of_slow(test.input(), output);
            ::Standard::Algorithms::ert::are_equal(true, isp, "out must be a permutation of in");
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("Simple1", // NOLINTNEXTLINE
            vec_int_t{ 9, -1, 5, -5, 3, 4, -7, -8 }, // NOLINTNEXTLINE
            vec_int_t{ -1, -5, -7, -8, 9, 5, 3, 4 });

        test_cases.emplace_back("Simple2 with 0-s", // NOLINTNEXTLINE
            vec_int_t{ 9, -1, 0, 5, -5, -7, 0, 3, 4, -8, 0 }, // NOLINTNEXTLINE
            vec_int_t{ -1, -5, -7, -8, 0, 0, 0, 9, 5, 3, 4 });

        test_cases.emplace_back("All negative", // NOLINTNEXTLINE
            vec_int_t{ -5, -3, -8 }, // NOLINTNEXTLINE
            vec_int_t{ -5, -3, -8 });

        test_cases.emplace_back("All zeros", // NOLINTNEXTLINE
            vec_int_t{ 0, 0, 0, 0, 0, 0 }, // NOLINTNEXTLINE
            vec_int_t{ 0, 0, 0, 0, 0, 0 });

        test_cases.emplace_back("All positive", // NOLINTNEXTLINE
            vec_int_t{ 7, 5, 3, 8 }, // NOLINTNEXTLINE
            vec_int_t{ 7, 5, 3, 8 });

        constexpr auto min_size = 0;
        constexpr auto max_size = ::Standard::Algorithms::is_debug ? 10 : 50;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);

        constexpr auto max_attemps = 1;

        for (std::int32_t attemp{}; attemp < max_attemps; ++attemp)
        {
            const auto size = rnd();
            vec_int_t input(size);
            Standard::Algorithms::Utilities::fill_random(input, size);

            test_cases.emplace_back("Random" + std::to_string(attemp), std::move(input));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        auto slow = test.input();

        Standard::Algorithms::Numbers::number_splitter_slow<int_t>(slow);

        if (!test.output().empty())
        {
            ::Standard::Algorithms::ert::are_equal(test.output(), slow, "number splitter slow");
        }

        // todo(p2): finish the test.
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::number_splitter_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
