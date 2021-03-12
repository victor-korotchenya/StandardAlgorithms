#include"weighted_median_selection_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"dutch_flag_split.h"
#include"weighted_median_selection.h"
#include<array>
#include<atomic>
#include<vector>

namespace
{
    using int_t = std::int16_t;
    using long_int_t = std::int32_t;
    static_assert(sizeof(int_t) <= sizeof(long_int_t));

    using random_t = Standard::Algorithms::Utilities::random_t<int_t>;
    using p_t = std::pair<int_t, int_t>;

    constexpr int_t first_positive = 1;
    constexpr int_t max_size = 300;
    constexpr int_t max_value = std::numeric_limits<int_t>::max();

    static_assert(first_positive < max_size && first_positive < max_value);

    constexpr void match_one_of(const char *const name, const auto &expected, const p_t &actual)
    {
        assert(name != nullptr);

        // todo(p2): compare a pair of pairs.
        if (expected.first == actual || expected.second == actual) [[likely]]
        {
            return;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "Error in " << name << ": None of the expected medians (" << expected.first.first << ", "
            << expected.first.second << "), (" << expected.second.first << ", " << expected.second.second
            << ") matches the actual (" << actual.first << ", " << actual.second << ").";

        throw std::runtime_error(str.str());
    }

    void run_test_once(const std::vector<p_t> &source, std::atomic<std::uint32_t> &stage,
        std::vector<p_t> &working_copy, const std::pair<p_t, p_t> &expected = {}, const bool has_expected = false)
    {
        ++stage;
        working_copy = source;

        ++stage;
        const auto fast = // Cannot use a reference because the working copy will soon be overwritten.
            Standard::Algorithms::Numbers::weighted_median_selection<long_int_t>(working_copy);

        ++stage;
        working_copy = source;

        ++stage;
        const auto slow = Standard::Algorithms::Numbers::weighted_median_selection_slow<long_int_t>(working_copy);

        ++stage;
        if (has_expected)
        {
            // todo(p2): compare a pair of pairs.
            ::Standard::Algorithms::ert::are_equal(expected.first, slow.first, "weighted_median_selection_slow lower");

            ::Standard::Algorithms::ert::are_equal(expected.second, slow.second, "weighted_median_selection_slow upper");
        }

        ++stage;
        match_one_of("weighted_median_selection", slow, fast);
    }

    constexpr void simple_tests()
    {
        using answer_t = std::pair<p_t, p_t>;
        using test_case_t = std::pair<std::vector<p_t>, answer_t>;

        constexpr p_t median_0_40{ 0, 40 };
        constexpr p_t median_1_10{ 1, 10 };
        constexpr p_t median_1_30{ 1, 30 };
        constexpr p_t median_1_49{ 1, 49 };
        constexpr p_t median_2_40{ 2, 40 };
        constexpr p_t median_3_3{ 3, 3 };
        constexpr p_t median_3_5{ 3, 5 };
        constexpr p_t median_3_30{ 3, 30 };

        const std::array test_cases{
            test_case_t( // Too large first {3, 30}m, {4, 10}, {5, 10}, {6, 8}, {7, 1}; |0 - 29| <= 30.
                {// NOLINTNEXTLINE
                    { 4, 10 }, { 7, 1 }, { 6, 8 }, { 5, 10 }, median_3_30 },
                { median_3_30, median_3_30 }),
            test_case_t( // Two medians end {0,30}, {1,10}m, {2,40}m; |30 - 40| <= 10; |40 - 0| <= 40.
                { median_1_10, median_2_40, // NOLINTNEXTLINE
                    { 0, 30 } },
                { median_1_10, median_2_40 }),
            test_case_t( // Two medians begin {0,40}m, {1,30}m, {2,10}; |0 - 40| <= 40; |40 - 10| <= 30.
                { median_0_40, // NOLINTNEXTLINE
                    { 2, 10 }, median_1_30 },
                { median_0_40, median_1_30 }),
            test_case_t( // Inside {0,71}, {1,49}m, {2,21}, {3,34}; |71 - 55| <= 49.
                {// NOLINTNEXTLINE
                    { 0, 71 }, { 3, 34 }, median_1_49, { 2, 21 } },
                { median_1_49, median_1_49 }),
            test_case_t( // Not plain median {0,6}, {1,1}, {2,5}, {3,5}m, {4,8}; |12 - 8| <= 5.
                {// NOLINTNEXTLINE
                    { 2, 5 }, { 1, 1 }, { 4, 8 }, { 0, 6 }, median_3_5 },
                { median_3_5, median_3_5 }),
            test_case_t( // Size greater than five
                //   1      5      6      9 m     14     16    // half cost = 8.
                // {0,1}, {1,4}, {2,1}, {3,3}m, {4,5}, {5,2}; |6 - 7| <= 3.
                { median_3_3, // NOLINTNEXTLINE
                    { 2, 1 }, { 4, 5 }, { 5, 2 }, { 1, 4 }, { 0, 1 } },
                { median_3_3, median_3_3 })
        };

        std::vector<p_t> working_copy;
        std::atomic<std::uint32_t> stage{};

        for (const auto &test : test_cases)
        {
            stage = 0U;
            run_test_once(test.first, stage, working_copy, test.second, true);
        }
    }

    void build_random(random_t &rnd, std::vector<p_t> &source)
    {
        source.clear();

        const auto size = rnd(first_positive, max_size);

        for (long_int_t index{}; index < size; ++index)
        {
            auto value = rnd();
            source.emplace_back(index, value);
        }

        Standard::Algorithms::Utilities::shuffle_data(source);
    }

    void random_tests()
    {
        std::vector<p_t> source(max_size);
        std::vector<p_t> working_copy(max_size);
        random_t rnd(first_positive, max_value);
        std::atomic<std::uint32_t> stage{};

        try
        {
            constexpr auto max_attempts = 1;

            for (std::int32_t att{}; att < max_attempts; ++att)
            {
                constexpr auto init_stage = 100U;

                stage = init_stage;
                build_random(rnd, source);
                run_test_once(source, stage, working_copy);
            }
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            ::Standard::Algorithms::print("Random source data", source, str);
            str << ", stage " << stage << ", error: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::weighted_median_selection_tests()
{
    simple_tests();
    random_tests();
}
