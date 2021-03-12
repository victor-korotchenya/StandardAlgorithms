#include <unordered_set>
#include "ukkonen_suffix_tree_utilities.h"
#include "ukkonen_suffix_tree_tests.h"
#include "../Utilities/Random.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

namespace Privet::Algorithms::Strings
{
    template<class char_t, class int_t>
    std::ostream& operator<<(std::ostream& str, const full_suffix_node_t<char_t, int_t>& node)
    {
        str << "Id=" << node.d
            << ", begin=" << node.b
            << ", end=" << node.e
            << ", suffix_index=" << node.suffix_index
            << ", suffix_link=" << node.suffix_link << ", ";

        Print("children", node.ch.begin(), node.ch.end(), str);

        return str;
    }
}

using namespace std;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = int;
    using string_t = string;
    using char_t = string_t::value_type;
    using node_t = full_suffix_node_t<char_t, int_t>;

    constexpr auto n_max = 10;

    bool operator==(const node_t& a, const node_t& b)
    {
        const auto eq = a.d == b.d && a.b == b.b && a.e == b.e &&
            a.suffix_link == b.suffix_link && a.suffix_index == b.suffix_index &&
            a.ch == b.ch;
        return eq;
    }

    struct test_case final : base_test_case
    {
        vector<string_t> patterns;

        test_case(string&& name, vector<string_t>&& patterns)
            : base_test_case(forward<string>(name)),
            patterns(forward<vector<string_t>>(patterns))
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("patterns", patterns, str);
        }
    };

    string_t rs(IntRandom& r, int min_size = 0)
    {
        return random_string(r, min_size);
    }

    void add_random(vector<test_case>& tests)
    {
        unordered_set<string_t> uniq;
        IntRandom r;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            auto text = random_string_unique(r, uniq, 1, n_max);
            if (!uniq.insert(text).second)
                continue;

            const auto m = r(3, 5);
            vector<string_t> patterns(m);
            for (auto i = 0; i < m; ++i)
                patterns.emplace_back(rs(r));

            patterns.emplace_back(rs(r) + text + rs(r) + text + rs(r));

            tests.emplace_back(move(text), move(patterns));
        }
    }

    void generate_test_cases(vector<test_case>& tests)
    {
        {
            const auto text = "THIS IS A TEST TEXT$";
            // "THIS IS A "  T  E  S  T  ' '  T  E  X  T  $
            //  0123456789  10 11 12 13   14 15 16 17 18 19
            // Space at: 4,7,9,14

            tests.emplace_back(text, vector<string_t> {
                "A",
                    " ",
                    "  ",
                    " I",
                    " IS A ",
                    "IS IS A T",
                    "IS IS A Te",
                    "IS A",
                    "ISB",
                    "S IS A T",
                    "TEST",
                    "TEST1",
                    "TES",
                    "TESA",
                    "THIS IS GOOD",
                    "THIS IS A A A A TEST TEXT$",
            });
        }

        add_random(tests);
    }

    template<class int_t, class node_t, class string_t, class string_t2>
    void find_string_test(const vector<node_t>& nodes,
        const string_t& pattern, const string_t2& text,
        vector<int_t>& buf, vector<int_t>& buf2)
    {
        const auto actual = find_string<int_t>(nodes, pattern, text);

        const auto f = text.find(pattern);
        const auto expected = static_cast<int_t>(f);
        Assert::AreEqual(expected, actual, "find_string");

        find_all_strings(nodes, pattern, text, buf);
        if (expected < 0)
            Assert::Empty(buf, "find_all_strings");
        else
        {
            buf2.clear();
            size_t pos = 0 - size_t(1);
            for (;;)
            {
                pos = text.find(pattern, pos + 1);
                if (pos >= text.size())
                    break;

                buf2.push_back(static_cast<int_t>(pos));
            }

            Assert::AreEqual(buf, buf2, "find_all_strings");
        }
    }

    void run_test_case(const test_case& test)
    {
        const auto& text = test.get_Name();
        const ukkonen_suffix_tree<string, int_t> full_tree(text);
        const auto& nodes = full_tree.nodes;
        vector<int_t> buf, buf2;

        for (const auto& pattern : test.patterns)
            find_string_test<int_t>(nodes, pattern, text, buf, buf2);
    }

    void verify_built_tree()
    {
        const string_t text = "abcabx"; // fixed
        const ukkonen_suffix_tree<string_t, int_t, char_t> full_tree(text);

        // 012345
        // abcabx == 1-> 5 [0,1 1,-1] -> 2 [2,5 1,0] suffix_link = 1, suffix_index = 0
        //  bcabx == 1-> 7 [1,1 1,-1] -> 3 [2,5 1,1]
        //   cabx == 1-> 4 [2,5 1,2]
        //    abx == 1-> 5 [0,1 7,-1] -> 6 [5,5 1,3] ; 5.suffix_link = 7
        //     bx == 1-> 7 [1,1 1,-1] -> 8 [5,5 1,4]
        //      x == 1-> 9 [5,5 1,5]

        vector<node_t> expected(10);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1, {
            {'a', int_t(5)},
            {'b', int_t(7)},
            {'c', int_t(4)},
            {'x', int_t(9)},
        } };
        expected[2] = { 2, 2, 5, 1, 0 }; // suffix_link = 1, not 3!
        expected[3] = { 3, 2, 5, 1, 1 };
        expected[4] = { 4, 2, 5, 1, 2 };
        expected[5] = { 5, 0, 1, 7, -1, // suffix_link = 7, not 1!
            {
            { 'c', int_t(2) },
            { 'x', int_t(6) },
        } };
        expected[6] = { 6, 5, 5, 1, 3 };
        expected[7] = { 7, 1, 1, 1, -1, {
            { 'c', int_t(3) },
            { 'x', int_t(8) },
        } };
        expected[8] = { 8, 5, 5, 1, 4 };
        expected[9] = { 9, 5, 5, 1, 5 };

        Assert::AreEqual(expected, full_tree.nodes, text + " nodes");
    }

    void verify_built_tree1()
    {
        const string_t text = "a";
        const ukkonen_suffix_tree<string_t, int_t> full_tree(text);

        vector<decltype(full_tree)::node_alias_t> expected(3);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1, {
            {'a', int_t(2)},
        } };
        expected[2] = { 2, 0, 0, 1, 0 };

        Assert::AreEqual(expected, full_tree.nodes, text + " nodes");
    }

    void verify_built_tree2()
    {
        const string_t text = "ab";
        const ukkonen_suffix_tree<string_t, int_t> full_tree(text);

        vector<decltype(full_tree)::node_alias_t> expected(4);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1, {
            {'a', int_t(2)},
            {'b', int_t(3)},
        } };
        expected[2] = { 2, 0, 1, 1, 0 };
        expected[3] = { 3, 1, 1, 1, 1 };

        Assert::AreEqual(expected, full_tree.nodes, text + " nodes");
    }

    void verify_built_tree3()
    {
        const string_t text = "aab";
        const ukkonen_suffix_tree<string_t, int_t> full_tree(text);

        vector<decltype(full_tree)::node_alias_t> expected(6);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1, {
            {'a', int_t(3)},
            {'b', int_t(5)},
        } };
        expected[2] = { 2, 1, 2, 1, 0 };
        expected[3] = { 3, 0, 0, 1, -1, {
            { 'a', int_t(2) },
            { 'b', int_t(4) },
        } };
        expected[4] = { 4, 2, 2, 1, 1 };
        expected[5] = { 5, 2, 2, 1, 2 };

        Assert::AreEqual(expected, full_tree.nodes, text + " nodes");
    }

    void verify_built_tree40()
    {
        constexpr auto separator = '~', separator2 = '%';
        const string_t text1 = "ab", text2 = "ca",
            text = text1 + separator + text2 + separator2;
        const ukkonen_suffix_tree<string_t, int_t, char_t, separator> full_tree(text);

        vector<decltype(full_tree)::node_alias_t> expected(9);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1, {
            {separator2, int_t(8)},
            {'a', int_t(6)},
            {'b', int_t(3)},
            {'c', int_t(5)},
            {separator, int_t(4)},
        } };
        expected[2] = { 2, 1, 5, 1, 0 };
        expected[3] = { 3, 1, 5, 1, 1 };
        expected[4] = { 4, 2, 5, 1, 2 };
        expected[5] = { 5, 3, 5, 1, 3 };
        expected[6] = { 6, 0, 0, 1, -1,{
            { separator2, int_t(7) },
            { 'b', int_t(2) },
        } };
        expected[7] = { 7, 5, 5, 1, 4 };
        expected[8] = { 8, 5, 5, 1, 5 };

        Assert::AreEqual(expected, full_tree.nodes, text + " nodes");
    }

    void verify_built_tree41()
    {
        constexpr auto separator = '~', separator2 = '%';
        const string_t text1 = "ab", text2 = "ca",
            text = text1 + separator + text2 + separator2;
        const ukkonen_suffix_tree<string_t, int_t, char_t, separator> full_tree(text, true);

        vector<decltype(full_tree)::node_alias_t> expected(9);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1, {
            {separator2, int_t(8)},
            {'a', int_t(6)},
            {'b', int_t(3)},
            {'c', int_t(5)},
            {separator, int_t(4)},
        } };
        //  ab~ca%
        //  012345
        //
        //        1    -> 4,8
        //  /     |   \
        // b3    a6   c5
        // b~  /  a   ca%
        //   %7   |
        //  a%    b2
        //       ab~
        expected[2] = { 2, 1, 2, 1, 0 }; // { 2, 1, 5, 1, 0 };
        expected[3] = { 3, 1, 2, 1, 1 }; // { 3, 1, 5, 1, 1 };
        expected[4] = { 4, 2, 2, 1, 2 }; // { 4, 2, 5, 1, 2 };
        expected[5] = { 5, 3, 5, 1, 3 };
        expected[6] = { 6, 0, 0, 1, -1,{
            { separator2, int_t(7) },
            { 'b', int_t(2) },
        } };
        expected[7] = { 7, 5, 5, 1, 4 };
        expected[8] = { 8, 5, 5, 1, 5 };

        Assert::AreEqual(expected, full_tree.nodes, text + " nodes");
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            namespace Tests
            {
                void ukkonen_suffix_tree_tests()
                {
                    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);

                    verify_built_tree();
                    verify_built_tree1();
                    verify_built_tree2();
                    verify_built_tree3();
                    verify_built_tree40();
                    verify_built_tree41();
                }
            }
        }
    }
}