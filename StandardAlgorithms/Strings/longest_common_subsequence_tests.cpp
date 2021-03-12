#include"longest_common_subsequence_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"longest_common_subsequence.h"
#include"longest_common_subsequence_hunt_szymanski.h"
#include"map_letter_to_positions.h"
#include<iostream>
#include<string>
#include<unordered_map>

namespace
{
    constexpr auto shall_print_lcs = false;

    constexpr auto profitable_lcs = Standard::Algorithms::Strings::Inner::profitable_lcs;

    constexpr auto min_size = profitable_lcs << 1U;
    constexpr auto max_size = min_size << 1U;

    static_assert(profitable_lcs < min_size && min_size < max_size);

    template<class string_t1, class char_t1, class subsequence_t1 = string_t1, class string_t2 = string_t1>
    struct lcs_test_case final : Standard::Algorithms::Tests::base_test_case
    {
        using char_t = char_t1;
        using string_t = string_t1;
        using subsequence_t = subsequence_t1;

        constexpr lcs_test_case(std::string &&name, string_t &&first, string_t2 &&second, subsequence_t &&expected,
            bool is_precomputed = true) noexcept
            : base_test_case(std::move(name))
            , First(std::move(first))
            , Second(std::move(second))
            , Expected(std::move(expected))
            , Ist_precomputed(is_precomputed)
        {
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("first", First, str);
            ::Standard::Algorithms::print("second", Second, str);
            ::Standard::Algorithms::print("expected", Expected, str);
        }

        [[nodiscard]] constexpr auto first() const &noexcept -> const string_t &
        {
            return First;
        }

        [[nodiscard]] constexpr auto second() const &noexcept -> const string_t &
        {
            return Second;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const subsequence_t &
        {
            return Expected;
        }

        [[nodiscard]] constexpr auto is_precomputed() const noexcept -> bool
        {
            return Ist_precomputed;
        }

private:
        string_t First;
        string_t Second;
        subsequence_t Expected;
        bool Ist_precomputed;
    };

    template<class test_t, class subsequence_t>
    constexpr void run_precomputed(const test_t &test)
    {
        subsequence_t actual;

        Standard::Algorithms::Strings::longest_common_subsequence(test.first(), test.second(), actual);

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "longest_common_subsequence");
    }

    template<class test_t, class match_t>
    constexpr void check_match(const test_t &test, const match_t &match, std::size_t &matches_sum)
    {
        const auto &source_index = std::get<0>(match);
        ::Standard::Algorithms::ert::greater(test.first().size(), source_index, "match source_index");

        const auto &destination_index = std::get<1>(match);
        ::Standard::Algorithms::ert::greater(test.second().size(), destination_index, "match destination_index");

        const auto &length = std::get<2>(match);
        ::Standard::Algorithms::ert::greater(length, 0U, "match length");

        const auto min_size_12 = std::min(test.first().size(), test.second().size());
        ::Standard::Algorithms::ert::greater_or_equal(min_size_12, length, "min size vs match length");

        ::Standard::Algorithms::ert::greater_or_equal(
            test.first().size(), source_index + length, "match source_index + length");
        ::Standard::Algorithms::ert::greater_or_equal(
            test.second().size(), destination_index + length, "match destination_index + length");

        matches_sum += length;
        ::Standard::Algorithms::ert::greater_or_equal(min_size_12, matches_sum, "min size vs matches_sum");
    }

    template<class match_t>
    constexpr void check_matches_prev_cur(const match_t &prev, const match_t &cur)
    {
        assert(&prev != &cur);

        const auto &source_index_pre = std::get<0>(prev);
        const auto &destination_index_pre = std::get<1>(prev);
        const auto &length_pre = std::get<2>(prev);

        const auto &source_index = std::get<0>(cur);
        const auto &destination_index = std::get<1>(cur);

        const auto source_pre_pos = source_index_pre + length_pre;
        ::Standard::Algorithms::ert::greater_or_equal(source_index, source_pre_pos, "source previous match");

        const auto destination_pre_pos = destination_index_pre + length_pre;
        ::Standard::Algorithms::ert::greater_or_equal(
            destination_index, destination_pre_pos, "destination previous match");

        const auto merge_valid = source_pre_pos < source_index || destination_pre_pos < destination_index;

        ::Standard::Algorithms::ert::are_equal(
            true, merge_valid, "the previous match must have been merged with the current");
    }

    template<class test_t, class matches_t>
    constexpr void check_matches(const test_t &test, const matches_t &matches)
    {
        if (test.expected().empty())
        {
            constexpr Standard::Algorithms::Strings::match_info_t empty{};

            ::Standard::Algorithms::ert::are_equal(empty.matches, matches, "hunt_szymanski matches");

            return;
        }

        std::size_t matches_sum{};

        for (std::size_t index{}; index < matches.size(); ++index)
        {
            const auto &cur = matches[index];
            check_match(test, cur, matches_sum);

            if (index == 0U)
            {
                continue;
            }

            const auto &prev = matches[index - 1U];
            check_matches_prev_cur(prev, cur);
        }

        ::Standard::Algorithms::ert::are_equal(test.expected().size(), matches_sum, "hunt_szymanski matches_sum");
    }

    // There can be many LCSs.
    constexpr void verify_lcs_result(const std::string &name, const auto &input_string, const auto &lcs)
    {
        assert(!name.empty());

        if (lcs.empty())
        {
            return;
        }

        const auto size = lcs.size();
        std::size_t matched{};

        for (const auto &cha : input_string)
        {
            const auto &mat = lcs[matched];

            if (cha == mat && ++matched == size)
            {
                return;
            }
        }

        ::Standard::Algorithms::ert::are_equal(size, matched, name + " matched");
    }

    constexpr void verify_lcs(
        const std::string &name, const auto &odin, const auto &dwa, const auto &expected, const auto &actual)
    {
        assert(!name.empty());

        ::Standard::Algorithms::ert::are_equal(expected.size(), actual.size(), name + " size");

        verify_lcs_result(name + " actual first", odin, actual);
        verify_lcs_result(name + " actual second", dwa, actual);
    }

    template<class test_t, class subsequence_t>
    constexpr void run_para(const test_t &test)
    {
        {
            subsequence_t actual;

            Standard::Algorithms::Strings::parallel_longest_common_subsequence(test.first(), test.second(), actual);

            verify_lcs("parallel_longest_common_subsequence", test.first(), test.second(), test.expected(), actual);
        }
        {
            subsequence_t actual;

            Standard::Algorithms::Strings::parallel_longest_common_subsequence_ant(test.first(), test.second(), actual);

            verify_lcs("parallel_longest_common_subsequence_ant", test.first(), test.second(), test.expected(), actual);
        }
    }

    template<class test_t, class string_t, class char_t>
    constexpr void run_hs(const test_t &test)
    {
        using map_t = std::unordered_map<char_t, std::vector<std::size_t>>;
        map_t first_key_pos;
        Standard::Algorithms::Strings::map_letter_to_positions(test.first(), first_key_pos);

        std::vector<std::size_t> commons;
        Standard::Algorithms::Strings::match_info_t match_info;

        constexpr auto shall_fill_matches = true;

        const auto lcs =
            Standard::Algorithms::Strings::longest_common_subsequence_hunt_szymanski::lcs<shall_fill_matches, string_t>(
                test.first(), first_key_pos, test.second(), commons, match_info);

        ::Standard::Algorithms::ert::are_equal(test.expected().size(), lcs, "hunt_szymanski lcs");

        check_matches(test, match_info.matches);
    }

    template<class test_t, class string_t = typename test_t::string_t,
        class subsequence_t = typename test_t::subsequence_t, class char_t = typename test_t::char_t>
    constexpr void run_test_case(const test_t &test)
    {
        if (test.is_precomputed())
        {
            run_precomputed<test_t, subsequence_t>(test);
        }

        run_para<test_t, subsequence_t>(test);
        run_hs<test_t, string_t, char_t>(test);
    }

    using lcs_test_case_string = lcs_test_case<std::string, char, std::vector<char>>;

    void add_random_string(std::vector<lcs_test_case_string> &tests)
    {
        using random_t = Standard::Algorithms::Utilities::random_t<std::int32_t>;
        random_t rnd{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto str_min_size = 0;
            constexpr auto str_max_size = 50;

            static_assert(str_min_size < str_max_size);

            constexpr auto min_let = '0';
            constexpr auto max_let = '2';

            static_assert(min_let < max_let);

            auto rnds = Standard::Algorithms::Utilities::random_string<random_t, min_let, max_let>(
                rnd, str_min_size, str_max_size);

            auto rnds2 = Standard::Algorithms::Utilities::random_string<random_t, min_let, max_let>(
                rnd, str_min_size, str_max_size);

            typename lcs_test_case_string::subsequence_t actual;
            Standard::Algorithms::Strings::longest_common_subsequence(rnds, rnds2, actual);

            constexpr auto precomputed = false;
            tests.emplace_back(
                "random_lcs" + std::to_string(att), std::move(rnds), std::move(rnds2), std::move(actual), precomputed);
        }
    }

    void generate_test_cases_string(std::vector<lcs_test_case_string> &tests)
    {
        using subsequence_t = lcs_test_case_string::subsequence_t;

        tests.emplace_back("Many different", "c_e+xyz", "dcbae pqr", subsequence_t({ 'c', 'e' }));
        tests.emplace_back(
            "Russia", "Borussia", "Belorussia", subsequence_t({ 'B', 'o', 'r', 'u', 's', 's', 'i', 'a' }));
        tests.emplace_back("Sample1", "001245670", "12845671", subsequence_t({ '1', '2', '4', '5', '6', '7' }));
        tests.emplace_back("Sample2", "CAb", "bCAd", subsequence_t({ 'C', 'A' }));
        tests.emplace_back("Sample2.5", "CdAb", "bCAd", subsequence_t({ 'C', 'A' }));
        tests.emplace_back("Sample3", "tarbkcnfhiccb", "ezacfnib", subsequence_t({ 'a', 'c', 'f', 'i', 'b' }));
        tests.emplace_back("Trivial", "A", "A", subsequence_t({ 'A' }));
        tests.emplace_back("Trivial2", "AB", "AB", subsequence_t({ 'A', 'B' }));

        tests.emplace_back(
            "Trivial hot 7", "BAAABBABBBCCACCC", "ABCABACBCBBB", subsequence_t({ 'A', 'B', 'A', 'B', 'A', 'C', 'C' }));
        // "BAA AB B AB BBCC A C C C"
        // "         AB C AB            A C B C BBB"
        // "         01  C  23             4 5  B 6 BBB"
        //
        // And there is another one "AAABBBB" of length 7:
        // "B A       A    A     B    B A BB BCCACCC"
        // "   A BC A B A C B C B     BB"
        // "   0  BC 1  B 2 C 3  C 4      56"
        tests.emplace_back("Trivial hot 7 another", "BAAABBABBBCCACCC", "ABCABACBCBBB",
            subsequence_t({ 'A', 'A', 'A', 'B', 'B', 'B', 'B' }), false);

        add_random_string(tests);
    }

    using lcs_test_case_integer = lcs_test_case<std::vector<std::int32_t>, std::int32_t>;

    constexpr void generate_test_cases_integer(std::vector<lcs_test_case_integer> &tests)
    {
        using subsequence_t = lcs_test_case_integer::subsequence_t;

        tests.emplace_back("Repetitions in second",
            // Ignore clang-tidy: test data.
            // NOLINTNEXTLINE
            subsequence_t({ 10, 23, 59, -97, 672, -3245450, 34976 }),
            // NOLINTNEXTLINE
            subsequence_t({ -34976, 23, 23, 23, 23, 234867, 23, 672, 23, 672, 672, 345, 97, 97, 724 }),
            // NOLINTNEXTLINE
            subsequence_t({ 23, 672 }));
    }

    template<std::integral int_t>
    constexpr void integer_lcs_speed_test(auto &temp, auto &rnd, auto &temp_2, auto &subsequence, auto &subsequence_2)
    {
        assert(&subsequence != &subsequence_2);

        constexpr auto min_value = std::numeric_limits<int_t>::min();
        constexpr auto max_value = std::numeric_limits<int_t>::max();
        static_assert(min_value < max_value);

        temp = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

        temp_2 = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

        subsequence.clear();
        subsequence_2.clear();

        const Standard::Algorithms::elapsed_time_ns tim;
        Standard::Algorithms::Strings::longest_common_subsequence(temp, temp_2, subsequence);
        [[maybe_unused]] const auto elapsed = tim.elapsed();

        const Standard::Algorithms::elapsed_time_ns par_tim;
        Standard::Algorithms::Strings::parallel_longest_common_subsequence(temp, temp_2, subsequence_2);
        [[maybe_unused]] const auto par_elapsed = par_tim.elapsed();

        ::Standard::Algorithms::ert::are_equal(subsequence.size(), subsequence_2.size(), "parallel LCS size");

        subsequence_2.assign(subsequence.size(), 0);

        const Standard::Algorithms::elapsed_time_ns ant_par_tim;
        Standard::Algorithms::Strings::parallel_longest_common_subsequence_ant(temp, temp_2, subsequence_2);
        [[maybe_unused]] const auto ant_par_elapsed = ant_par_tim.elapsed();

        ::Standard::Algorithms::ert::are_equal(
            subsequence.size(), subsequence_2.size(), "parallel anti-diagonal LCS size");

        if (!shall_print_lcs)
        {
            return;
        }

        const auto ratio = ::Standard::Algorithms::ratio_compute(par_elapsed, elapsed);
        const auto ant_ratio = ::Standard::Algorithms::ratio_compute(ant_par_elapsed, elapsed);

        const auto rep = " lcs(" + std::to_string(temp.size()) + ", " + std::to_string(temp_2.size()) + ") seq " +
            std::to_string(elapsed) + ", par " + std::to_string(par_elapsed) + ", anti " +
            std::to_string(ant_par_elapsed) + ", seq/par " + std::to_string(ratio) + ", seq/anti " +
            std::to_string(ant_ratio) + "\n";

        std::cout << rep;
    }

    void integer_lcs_speed_tests()
    {
        using int_t = std::int32_t;

        Standard::Algorithms::Utilities::random_t<int_t> rnd{};
        std::vector<int_t> temp(max_size);
        std::vector<int_t> temp_2(max_size);
        std::vector<int_t> subsequence(max_size);
        std::vector<int_t> subsequence_2(max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            try
            {
                integer_lcs_speed_test<int_t>(temp, rnd, temp_2, subsequence, subsequence_2);
            }
            catch (const std::exception &exc)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                ::Standard::Algorithms::print("first", temp, str);
                ::Standard::Algorithms::print("second", temp_2, str);
                str << "\nError: " << exc.what();

                throw std::runtime_error(str.str());
            }
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::longest_common_subsequence_tests()
{
    Standard::Algorithms::Tests::test_utilities<lcs_test_case_string>::run_tests(
        run_test_case, generate_test_cases_string);

    Standard::Algorithms::Tests::test_utilities<lcs_test_case_integer>::run_tests(
        run_test_case, generate_test_cases_integer);

    integer_lcs_speed_tests();
}
