#include"knuth_morris_pratt_tests.h"
#include"../Numbers/hash_vector.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"aho_corasick.h"
#include"kmp_todo.h"
#include"knuth_morris_pratt.h"
#include"rabin_karp.h"
#include"ukkonen_suffix_tree_utilities.h"
#include<iterator>
#include<tuple>

namespace // todo(p3): review.
{
    using char_t = char;
    using length_t = std::uint32_t;
    using matches_t = std::vector<std::int32_t>;
    using random_t = Standard::Algorithms::Utilities::random_t<std::int32_t>;

    constexpr auto bit_match_pattern_max_size = Standard::Algorithms::Strings::bit_match_pattern_max_size;

    template<class key_t, class value_t>
    constexpr void pairs_leave_value_and_sort(
        const std::vector<std::pair<key_t, value_t>> &source, std::vector<value_t> &actual)
    {
        actual.clear();
        Standard::Algorithms::Strings::Inner::enque_second(source, actual);
        std::sort(actual.begin(), actual.end());
    }

    struct test_case final : ::Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::string &&text, std::string &&sub, length_t expected) noexcept
            : base_test_case(std::move(name))
            , Text(std::move(text))
            , Sub(std::move(sub))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto text() const &noexcept -> const std::string &
        {
            return Text;
        }

        [[nodiscard]] constexpr auto sub() const &noexcept -> const std::string &
        {
            return Sub;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> length_t
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            str << " Text " << Text << ", Sub " << Sub << ", Expected " << Expected;
        }

private:
        std::string Text;
        std::string Sub;
        length_t Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr length_t minus_one = length_t{} - static_cast<length_t>(1);

        const auto *const some_string = "abc";
        const auto *const longer_string = "aabbbcccccaaabb";

        constexpr auto exp15 = 15;
        constexpr auto exp33 = 33;
        test_cases.insert(test_cases.begin(),
            {
                { "some", "abc abcdab abcdabcdabde", "abcdabd", exp15 },
                { "some2", "aaabbbbc aaabbbcdaaaabbb aaabbcdaaabbbcdaaabbddddabbbcdddde", "aabbbcdaaabbddddabbbc",
                    exp33 },
                { "Start longer", longer_string, "aab", 0 },
                { "Middle longer", longer_string, "abbbcc", 1 },
                { "End longer", longer_string, "bccccc", 4 },

                { "Start", some_string, "a", 0 },
                { "Middle", some_string, "b", 1 },
                { "End", some_string, "c", 2 },

                { "Trivial", "a", "a", 0 },
                { "Trivial not", "a", "b", minus_one },
                { "Trivial other case", "a", "A", minus_one },
                { "Too long", "a", "ab", minus_one },
                { "Not contain", "abcdef", "bf", minus_one },
                { "Not existing letter", "abcdef", "ck", minus_one },
                { "Case sensitive", "abcd", "bC", minus_one },
            });
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual =
            ::Standard::Algorithms::Strings::knuth_morris_pratt_str_todo<std::string, std::string, length_t>(
                test.text(), test.sub());

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "knuth_morris_pratt_str_todo");
    }

    template<class map_t, class string_t>
    constexpr void test_match_impl(const map_t &automaton, const string_t &pattern, matches_t &matches)
    {
        const auto automaton_actual = ::Standard::Algorithms::Strings::automaton_pattern_matching_build<char_t>(pattern);

        ::Standard::Algorithms::ert::are_equal(
            automaton, automaton_actual, "automaton_pattern_matching_build " + pattern);

        const auto slow = ::Standard::Algorithms::Strings::automaton_pattern_matching_build_slow<char_t>(pattern);

        ::Standard::Algorithms::ert::are_equal(automaton, slow, "automaton_pattern_matching_build_slow " + pattern);

        const matches_t expected{ 0, 1 + static_cast<std::int32_t>(pattern.size()) };
        const auto text = std::string().append(pattern).append("#").append(pattern);

        ::Standard::Algorithms::Strings::automaton_pattern_matching(automaton, text, matches);
        ::Standard::Algorithms::ert::are_equal(expected, matches, "automaton_pattern_matching " + pattern);
    }

    void test_match()
    {
        matches_t matches{};
        {
            const std::vector<std::unordered_map<char_t, std::int32_t>> automaton{
                { { 'a', 1 } }, // 0
                {
                    { 'a', 1 },
                    { 'b', 2 },
                },
                { { 'a', 3 } }, // 2
                { { 'a', 1 }, { 'b', 4 } },
                { { 'a', 5 } }, // 4
                { { 'a', 1 }, { 'b', 4 }, { 'c', 6 } },
                { { 'a', 7 } }, // 6
                { { 'a', 1 }, { 'b', 2 } },
            };
            const std::string pattern = "ababaca";
            test_match_impl(automaton, pattern, matches);
        }
        {
            const std::vector<std::unordered_map<char_t, std::int32_t>> automaton{
                { { 'a', 1 } }, // 0
                {
                    { 'a', 1 },
                    { 'b', 2 },
                },
                { { 'a', 3 } }, // 2
                { { 'a', 1 }, { 'b', 4 } },
                { { 'a', 5 } }, // 4
                {
                    { 'a', 1 },
                    { 'b', 4 },
                },
            };
            const std::string pattern = "ababa";
            test_match_impl(automaton, pattern, matches);
        }
        {
            const std::vector<std::unordered_map<char_t, std::int32_t>> automaton{ { { 'a', 1 } }, // 0
                { { 'a', 2 } }, { { 'a', 3 } }, // 2
                { { 'a', 3 } } };
            const std::string pattern = "aaa";
            test_match_impl(automaton, pattern, matches);
        }
        {
            const std::vector<std::unordered_map<char_t, std::int32_t>> automaton{
                { { 'd', 1 } }, // 0
                { { 'd', 1 } },
            };
            const std::string pattern = "d";
            test_match_impl(automaton, pattern, matches);
        }
    }

    [[nodiscard]] constexpr auto better_tests() -> bool
    {
        using pattern_table_table2_t = std::tuple<std::string, matches_t, matches_t>;

        const std::vector<pattern_table_table2_t> tests{// NOLINTNEXTLINE
            { "ababab", { 0, 0, 1, 2, 3, 4 }, { 0, 0, 0, 0, 0, 4 } }, // NOLINTNEXTLINE
            { "ababaa", { 0, 0, 1, 2, 3, 1 }, { 0, 0, 0, 0, 3, 1 } }, // NOLINTNEXTLINE
            { "aaaa", { 0, 1, 2, 3 }, { 0, 0, 0, 3 } }, // NOLINTNEXTLINE
            { "aaa", { 0, 1, 2 }, { 0, 0, 2 } }, // NOLINTNEXTLINE
            { "aa", { 0, 1 }, { 0, 1 } }, // NOLINTNEXTLINE
            { "a", { 0 }, { 0 } }
        };

        for (const auto &[pattern, expected_table, expected_table_2] : tests)
        {
            auto table = Standard::Algorithms::Strings::knuth_morris_pratt_prep(pattern);

            ::Standard::Algorithms::ert::are_equal(expected_table, table, "knuth_morris_pratt_prep " + pattern);

            Standard::Algorithms::Strings::improve_kmp_table(pattern, table);
            ::Standard::Algorithms::ert::are_equal(expected_table_2, table, "improve_kmp_table " + pattern);

            {
                constexpr auto some_value = 5;

                table.assign(pattern.size() + 1ZU, some_value);
            }

            Standard::Algorithms::Strings::better_knuth_morris_pratt_prep(pattern, table);

            ::Standard::Algorithms::ert::are_equal(expected_table_2, table, "better_knuth_morris_pratt_prep " + pattern);
        }

        return true;
    }

    void string_search_tests(random_t &rnd, matches_t &actual, const std::int32_t att)
    {
        constexpr auto min_size = 1U;
        constexpr auto text_length_shift = 3U;

        constexpr auto min_char = 'A';
        constexpr auto max_char = ::Standard::Algorithms::is_debug ? 'C' : 'Z';

        const auto pattern = Standard::Algorithms::Utilities::random_string<random_t, min_char, max_char>(
            rnd, min_size, bit_match_pattern_max_size);

        const auto min_text_size = static_cast<std::int32_t>(pattern.size());
        const auto max_text_size = static_cast<std::int32_t>(pattern.size() << text_length_shift);

        const auto text = Standard::Algorithms::Utilities::random_string<random_t, min_char, max_char>(
                              rnd, min_text_size, max_text_size) +
            pattern + " PEACE " + std::to_string(att);
        assert(0 < min_text_size && pattern.size() <= text.size());

        matches_t expected{};
        {
            const std::string_view view{ text };

            for (std::int32_t index{}, cnt = static_cast<std::int32_t>(text.size() - pattern.size()); index <= cnt;
                 ++index)
            {
                if (view.substr(index, pattern.size()) == pattern)
                {
                    expected.push_back(index);
                }
            }
        }

        const auto name = std::string("pattern '").append(pattern).append("', text '").append(text).append("'");
        {
            auto table = ::Standard::Algorithms::Strings::knuth_morris_pratt_prep(pattern);

            ::Standard::Algorithms::Strings::knuth_morris_pratt(pattern, table, text, actual);

            ::Standard::Algorithms::ert::are_equal(expected, actual, "knuth_morris_pratt " + name);

            ::Standard::Algorithms::Strings::improve_kmp_table(pattern, table);

            ::Standard::Algorithms::Strings::knuth_morris_pratt(pattern, table, text, actual);

            ::Standard::Algorithms::ert::are_equal(expected, actual, "better knuth_morris_pratt " + name);

            {
                constexpr auto some_value = 4;

                table.assign(pattern.size() + 1ZU, some_value);
            }

            Standard::Algorithms::Strings::better_knuth_morris_pratt_prep(pattern, table);

            ::Standard::Algorithms::Strings::knuth_morris_pratt(pattern, table, text, actual);

            ::Standard::Algorithms::ert::are_equal(expected, actual, "better kmp " + name);
        }
        {
            using hash_t = std::int64_t;

            constexpr hash_t bas = 7'001;
            constexpr hash_t mod = 1'000'000'007;

            using iter_t = decltype(pattern.cbegin());

            ::Standard::Algorithms::Strings::rabin_karp(
                pattern, bas, text, mod, ::Standard::Algorithms::Numbers::rolling_hash<hash_t, iter_t>, actual);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "rabin_karp " + name);
        }

        {
            std::vector<std::uint64_t> bits;

            ::Standard::Algorithms::Strings::shift_or<std::string>(pattern, bits, text, actual);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "shift_or " + name);

            ::Standard::Algorithms::Strings::shift_and(pattern, bits, text, actual);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "shift_and " + name);

            ::Standard::Algorithms::Strings::backward_nondeterministic_dawg_matching(pattern, bits, text, actual);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "bndm " + name);
        }

        std::map<char, std::int32_t> last_positions_rev;

        ::Standard::Algorithms::Strings::boyer_moore_horspool(pattern, text, last_positions_rev, actual);
        ::Standard::Algorithms::ert::are_equal(expected, actual, "boyer_moore_horspool " + name);

        {
            matches_t good_chars;
            matches_t suff;

            ::Standard::Algorithms::Strings::boyer_moore(pattern, suff, last_positions_rev, good_chars, text, actual);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "boyer_moore " + name);

            std::vector<bool> bools;
            std::vector<std::pair<std::int32_t, std::int32_t>> bom_cells;

            ::Standard::Algorithms::Strings::backward_oracle_matching(
                pattern, suff, text, bools, good_chars, bom_cells, actual);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "bom " + name);
        }
        {// It is checked in another test.
            const ::Standard::Algorithms::Strings::aho_corasick<std::string> aho({ pattern });

            std::vector<std::pair<const std::string *, std::int32_t>> aho_actual;
            aho.find(text, aho_actual);

            pairs_leave_value_and_sort(aho_actual, actual);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "aho_corasick " + name);
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::knuth_morris_pratt_str_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}

void Standard::Algorithms::Strings::Tests::knuth_morris_pratt_tests()
{
    static_assert(better_tests());

    test_match();

    random_t rnd{};
    matches_t actual{};

    constexpr auto max_attempts = 1;

    for (std::int32_t att{}; att < max_attempts; ++att)
    {
        string_search_tests(rnd, actual, att);
    }

    {
        const auto *const pattern2 = "aaB";
        const std::string text2 = "zaaguaBaaghaaBjh";

        std::vector<std::uint64_t> bits;
        ::Standard::Algorithms::Strings::shift_or<std::string>(pattern2, bits, text2, actual);

        constexpr auto expect_position = 11;

        ::Standard::Algorithms::ert::are_equal(matches_t{ expect_position }, actual, "shift_or simple test");

        // todo(p3): finish.
    }
}
