#include "../test_utilities.h"
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "knuth_morris_pratt.h"
#include "kmp_todo.h"
#include "knuth_morris_pratt_tests.h"
#include "aho_corasick.h"
#include "aho_corasick_tests.h"

using namespace std;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

// todo: p1. review.

namespace
{
    using char_t = char;
    using length_t = unsigned;

    struct test_case final : base_test_case
    {
        string Text, Sub;
        length_t Expected;

        test_case(string&& name,
            string&& text,
            string&& sub,
            length_t expected)
            : base_test_case(forward<string>(name)),
            Text(forward<string>(text)),
            Sub(forward<string>(sub)),
            Expected(expected)
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            str << " Text=" << Text
                << ", Sub=" << Sub
                << ", Expected=" << Expected;
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        constexpr auto minus_one = length_t() - static_cast<length_t>(1);

        const auto someString = "abc";
        const auto longerString = "aabbbcccccaaabb";

        test_cases.insert(test_cases.begin(),
            {
                { "some", "abc abcdab abcdabcdabde", "abcdabd", 15 },
                { "some2", "aaabbbbc aaabbbcdaaaabbb aaabbcdaaabbbcdaaabbddddabbbcdddde",
                "aabbbcdaaabbddddabbbc", 33 },
                { "Start longer", longerString, "aab", 0 },
                { "Middle longer", longerString, "abbbcc", 1 },
                { "End longer", longerString, "bccccc", 4 },

                { "Start", someString, "a", 0 },
                { "Middle", someString, "b", 1 },
                { "End", someString, "c", 2 },

                { "Trivial", "a", "a", 0 },
                { "Trivial not", "a", "b", minus_one },
                { "Trivial other case", "a", "A", minus_one },
                { "Too long", "a", "ab", minus_one },
                { "Not contain", "abcdef", "bf", minus_one },
                { "Not existing letter", "abcdef", "ck", minus_one },
                { "Case sensitive", "abcd", "bC", minus_one },
            });
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = KnuthMorrisPrattStr_todo<string, length_t>(
            test_case.Text, test_case.Sub);
        Assert::AreEqual(test_case.Expected, actual, test_case.get_Name());
    }

    struct base_test_case2 final : base_test_case
    {
        base_test_case2(string&& name)
            : base_test_case(forward<string>(name))
        {
        }
    };

    void generate_test_cases2(vector<base_test_case2>& test_cases)
    {
        for (auto i = 0; i < 20; ++i)
            test_cases.emplace_back("case2_" + to_string(i));
    }

    template<class map_t, class string_t>
    void test_match_impl(const map_t& automaton,
        const string_t& pattern, vector<int>& matches)
    {
        const auto automaton_actual = automaton_pattern_matching_build<char_t>(pattern);
        Assert::AreEqual(automaton, automaton_actual, "automaton_pattern_matching_build " + pattern);

        const auto slow = automaton_pattern_matching_build_slow<char_t>(pattern);
        Assert::AreEqual(automaton, slow, "automaton_pattern_matching_build_slow " + pattern);

        const vector<int> expected{ 0, 1 + static_cast<int>(pattern.size()) };
        const auto text = pattern + "#" + pattern;

        automaton_pattern_matching(automaton, text, matches);
        Assert::AreEqual(expected, matches, "automaton_pattern_matching " + pattern);
    }

    void test_match()
    {
        vector<int> matches;
        {
            const vector<unordered_map<char_t, int>> automaton{
            { { 'a', 1 } },//0
            { { 'a', 1 },{ 'b', 2 }, },
            { { 'a', 3 } },//2
            { { 'a', 1 },{ 'b', 4 } },
            { { 'a', 5 } },//4
            { { 'a', 1 },{ 'b', 4 },{ 'c', 6 } },
            { { 'a', 7 } },//6
            { { 'a', 1 },{ 'b', 2 } },
            };
            const string pattern = "ababaca";
            test_match_impl(automaton, pattern, matches);
        }
        {
            const vector<unordered_map<char_t, int>> automaton{
                { { 'a', 1 } },//0
                { { 'a', 1 },{ 'b', 2 }, },
                { { 'a', 3 } },//2
                { { 'a', 1 },{ 'b', 4 } },
                { { 'a', 5 } },//4
                { { 'a', 1 },{ 'b', 4 }, },
            };
            const string pattern = "ababa";
            test_match_impl(automaton, pattern, matches);
        }
        {
            const vector<unordered_map<char_t, int>> automaton{
                { { 'a', 1 } },//0
                { { 'a', 2 } },
                { { 'a', 3 } },//2
                { { 'a', 3 } }
            };
            const string pattern = "aaa";
            test_match_impl(automaton, pattern, matches);
        }
        {
            const vector<unordered_map<char_t, int>> automaton{
                { { 'd', 1 } },//0
                { { 'd', 1 } },
            };
            const string pattern = "d";
            test_match_impl(automaton, pattern, matches);
        }
    }

    void run_test_case2(const base_test_case2&)
    {
        constexpr auto min_size = 1;
        constexpr auto min_char = 'A';
#ifdef _DEBUG
        constexpr auto max_size = 30;
        constexpr auto max_char = 'C';
        constexpr auto text_length_shift = 5;
#else
        constexpr auto max_size = 64;
        constexpr auto max_char = 'Z';
        constexpr auto text_length_shift = 8;
#endif
        vector<int> actual, expected, suff, good_chars;
        vector<uint64_t> bits;
        vector<bool> bools;
        map<char, int> last_positions_rev;
        vector<pair<const string*, int>> aho_actual;
        vector<pair<int, int>> bom_cells;
        IntRandom r;

        auto pattern = random_string<IntRandom, min_char, max_char>(r, min_size, max_size);

        int ty = 0;
        if (++ty == 1)
            pattern = "ABCDABD";
        else if (++ty == 2)
            pattern = "abcdbbd";
        //const string pattern = "ABC";
        const auto min_text_size = static_cast<int>(pattern.size());

        const auto max_text_size = static_cast<int>(pattern.size()) << text_length_shift;
        const auto text = random_string<IntRandom, min_char, max_char>(r, min_text_size, max_text_size)
            //const auto text = "CAACABABABBBAABBAAACCBCCCAABBABCBAACBCBCCCACBCAABACACABBAAA"
            //const auto text = "BABCAA"
            + pattern;
        assert(0 < min_text_size && pattern.size() <= text.size());

        // auto pattern = string("ABABABAB"), text= string("ACABCBBACBC");

        actual.clear();
        expected.clear();
        bits.clear();

        const auto table = knuth_morris_pratt_prep(pattern);
        knuth_morris_pratt(pattern, table, text, actual);

        const auto n = static_cast<int>(text.size() - pattern.size());
        for (auto i = 0; i <= n; ++i)
        {
            if (text.substr(i, pattern.size()) == pattern)
                expected.push_back(i);
        }

        const auto name = "pattern '" + pattern + "', text '" + text + '\'';
        Assert::AreEqual(actual, expected, name);

        //  "ABCDABD"
        //  01234567 - indexes
        //  00000120 - table_clr
        //
        //  "AAAAAAA"
        //  01234567 - indexes
        //  00123456 - table_clr
        //
        //  "abcdbbd"
        //  01234567 - indexes
        //  00000000 - table_clr
        const auto table_clr = knuth_morris_pratt_prep(pattern);
        vector<int> actual_clr;
        knuth_morris_pratt(pattern, table_clr, text, actual_clr);
        Assert::AreEqual(actual_clr, expected, "clr " + name);

        shift_or<string>(pattern, text, bits, actual);
        Assert::AreEqual(actual, expected, "shift_or " + name);

        shift_and(pattern, text, bits, actual);
        Assert::AreEqual(actual, expected, "shift_and " + name);

        boyer_moore_horspool(pattern, text, last_positions_rev, actual);
        Assert::AreEqual(actual, expected, "boyer_moore_horspool " + name);

        boyer_moore(pattern, text, last_positions_rev, suff, good_chars, actual);
        Assert::AreEqual(actual, expected, "boyer_moore " + name);

        backward_nondeterministic_dawg_matching(pattern, text, bits, actual);
        Assert::AreEqual(actual, expected, "bndm " + name);

        const aho_corasick<string> aho({ pattern });
        aho.find(text, aho_actual);
        pairs_leave_value_and_sort(aho_actual, actual);
        Assert::AreEqual(actual, expected, "aho_corasick " + name);

        string zz = "ABABA";
        backward_oracle_matching(zz, text, suff, good_chars, bools, bom_cells, actual);

        backward_oracle_matching(pattern, text, suff, good_chars, bools, bom_cells, actual);
        Assert::AreEqual(actual, expected, "bom " + name);
    }
}

void Privet::Algorithms::Strings::Tests::KnuthMorrisPrattStrTests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}

void Privet::Algorithms::Strings::Tests::knuth_morris_pratt_tests()
{
    test_match();

    test_utilities<base_test_case2>::run_tests<0>(run_test_case2, generate_test_cases2);

    vector<int> actual, expected;
    vector<uint64_t> bits;

    {
        const auto pattern2 = "aaB";
        const string text2 = "zaaguaBaaghaaBjh";
        // todo: end. auto h=text2.find("gh");

        actual.clear();
        expected.clear();
        bits.clear();

        shift_or<string>(pattern2, text2, bits, actual);
        expected.clear();
        expected.push_back(11);
        Assert::AreEqual(actual, expected, "shift_or simple test");
    }
}