#include"ukkonen_suffix_tree_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"ukkonen_suffix_tree_utilities.h"
#include<unordered_set>

namespace Standard::Algorithms::Strings
{
    template<class char_t, std::signed_integral int_t>
    auto operator<< (std::ostream &str, const full_suffix_node_t<char_t, int_t> &node) -> std::ostream &
    {
        str << "Ide " << node.d << ", begin " << node.b << ", end " << node.e << ", suffix index " << node.suffix_index
            << ", suffix link " << node.suffix_link << ", ";

        ::Standard::Algorithms::print("children", node.ch.cbegin(), node.ch.cend(), str);

        return str;
    }

    template<class char_t, std::signed_integral int_t>
    [[nodiscard]] constexpr auto operator== (
        const full_suffix_node_t<char_t, int_t> &one, const full_suffix_node_t<char_t, int_t> &two) -> bool
    {
        return one.d == two.d && one.b == two.b && one.e == two.e && one.suffix_link == two.suffix_link &&
            one.suffix_index == two.suffix_index && one.ch == two.ch;
    }
} // namespace Standard::Algorithms::Strings

namespace
{
    using int_t = std::int32_t;
    using string_t = std::string;
    using char_t = typename string_t::value_type;
    using node_t = Standard::Algorithms::Strings::full_suffix_node_t<char_t, int_t>;

    constexpr auto n_max = 10;

    // [[nodiscard]] auto operator== (const node_t& a, const node_t& b) -> bool
    //    {
    //        auto equ = a.d == b.d && a.b == b.b && a.e == b.e &&
    //            a.suffix_link == b.suffix_link && a.suffix_index == b.suffix_index &&
    //            a.ch == b.ch;
    //        return equ;
    //    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<string_t> &&patterns) noexcept
            : base_test_case(std::move(name))
            , Patterns(std::move(patterns))
        {
        }

        [[nodiscard]] constexpr auto patterns() const &noexcept -> const std::vector<string_t> &
        {
            return Patterns;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Patterns", Patterns, str);
        }

private:
        std::vector<string_t> Patterns{};
    };

    [[nodiscard]] constexpr auto rds(
        Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd, std::int32_t min_size = 1) -> string_t
    {
        return Standard::Algorithms::Utilities::random_string(rnd, min_size);
    }

    void add_random(std::vector<test_case> &tests)
    {
        std::unordered_set<string_t> uniq{};
        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        auto gena = [&rnd] -> string_t
        {
            return rds(rnd);
        };

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            auto text = Standard::Algorithms::Utilities::random_string_unique(rnd, uniq, 1, n_max);

            if (text.empty())
            {
                continue;
            }

            constexpr auto last_unique_char = '~';

            text += last_unique_char;

            constexpr auto min_size = 3;
            constexpr auto max_size = 5;

            const auto len = rnd(min_size, max_size);

            std::vector<string_t> patterns(len);
            std::generate(patterns.begin(), patterns.end(), gena);

            patterns.emplace_back(rds(rnd).append(text).append(rds(rnd)).append(text).append(rds(rnd)));

            tests.emplace_back(std::move(text), std::move(patterns));
        }
    }

    void generate_test_cases(std::vector<test_case> &tests)
    {
        {
            const auto *const text = "THIS IS A TEST TEXT!";
            // "THIS IS A "  T  E  S  T  ' '  T  E  X  T  !
            //  0123456789  10 11 12 13   14 15 16 17 18 19
            // Space at: 4,7,9,14

            tests.emplace_back(text,
                std::vector<string_t>{
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
                    "THIS IS A A A A TEST TEXT!",
                });
        }

        add_random(tests);
    }

    template<class int_t, class node_t, class string_t, class string_t2>
    constexpr void find_string_test(const std::vector<node_t> &nodes, const string_t &pattern, const string_t2 &text,
        std::vector<int_t> &buf, std::vector<int_t> &buf2)
    {
        const auto actual = Standard::Algorithms::Strings::find_string<int_t>(nodes, pattern, text);

        const auto fin = text.find(pattern);
        const auto expected = static_cast<int_t>(fin);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "find_string");

        Standard::Algorithms::Strings::find_all_strings(nodes, pattern, text, buf);
        std::sort(buf.begin(), buf.end());

        if (expected < 0)
        {
            ::Standard::Algorithms::ert::empty(buf, "find_all_strings");
        }
        else
        {
            buf2.clear();

            for (auto pos = std::size_t{} - static_cast<std::size_t>(1);;)
            {
                pos = text.find(pattern, pos + 1ZU);

                if (text.size() <= pos)
                {
                    break;
                }

                buf2.push_back(static_cast<int_t>(pos));
            }

            ::Standard::Algorithms::ert::are_equal(buf, buf2, "find_all_strings");
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &text = test.name();

        const Standard::Algorithms::Strings::ukkonen_suffix_tree<std::string, int_t> full_tree(text);
        const auto &nodes = full_tree.nodes();

        std::vector<int_t> buf{};
        std::vector<int_t> buf2{};

        for (const auto &patterns = test.patterns(); const auto &pattern : patterns)
        {
            find_string_test<int_t>(nodes, pattern, text, buf, buf2);
        }
    }

    constexpr auto len5 = 5;
    constexpr auto len6 = 6;
    constexpr auto len7 = 7;
    constexpr auto len8 = 8;
    constexpr auto len9 = 9;
    constexpr auto len10 = 10;

    void verify_built_tree()
    {
        const string_t text = "abcabx";
        const Standard::Algorithms::Strings::ukkonen_suffix_tree<string_t, int_t, char_t> full_tree(text);

        // 012345
        // abcabx == 1-> 5 [0,1 1,-1] -> 2 [2,5 1,0] suffix_link = 1, suffix_index = 0
        //  bcabx == 1-> 7 [1,1 1,-1] -> 3 [2,5 1,1]
        //   cabx == 1-> 4 [2,5 1,2]
        //    abx == 1-> 5 [0,1 7,-1] -> 6 [5,5 1,3] ; 5.suffix_link = 7
        //     bx == 1-> 7 [1,1 1,-1] -> 8 [5,5 1,4]
        //      x == 1-> 9 [5,5 1,5]

        std::vector<node_t> expected(len10);
        expected[0].e = static_cast<int_t>(text.size() - 1U);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1,
            {
                { 'a', len5 },
                { 'b', len7 },
                { 'c', 4 },
                { 'x', len9 },
            } };
        expected[2] = { 2, 2, len5, 1, 0 }; // suffix_link = 1, not 3!
        expected[3] = { 3, 2, len5, 1, 1 };
        expected[4] = { 4, 2, len5, 1, 2 };
        expected[len5] = { len5, 0, 1, len7, -1, // suffix_link = 7, not 1!
            {
                { 'c', 2 },
                { 'x', len6 },
            } };
        expected[len6] = { len6, len5, len5, 1, 3 };
        expected[len7] = { len7, 1, 1, 1, -1,
            {
                { 'c', 3 },
                { 'x', len8 },
            } };
        expected[len8] = { len8, len5, len5, 1, 4 };
        expected[len9] = { len9, len5, len5, 1, len5 };

        ::Standard::Algorithms::ert::are_equal(expected, full_tree.nodes(), text + " nodes");
    }

    void verify_built_tree1()
    {
        const string_t text = "a";
        const Standard::Algorithms::Strings::ukkonen_suffix_tree<string_t, int_t> full_tree(text);

        std::vector<decltype(full_tree)::node_t> expected(3);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1,
            {
                { 'a', 2 },
            } };
        expected[2] = { 2, 0, 0, 1, 0 };

        ::Standard::Algorithms::ert::are_equal(expected, full_tree.nodes(), text + " nodes");
    }

    void verify_built_tree2()
    {
        const string_t text = "ab";
        const Standard::Algorithms::Strings::ukkonen_suffix_tree<string_t, int_t> full_tree(text);

        std::vector<decltype(full_tree)::node_t> expected(4);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1,
            {
                { 'a', 2 },
                { 'b', 3 },
            } };
        expected[2] = { 2, 0, 1, 1, 0 };
        expected[3] = { 3, 1, 1, 1, 1 };

        ::Standard::Algorithms::ert::are_equal(expected, full_tree.nodes(), text + " nodes");
    }

    void verify_built_tree3()
    {
        const string_t text = "aab";
        const Standard::Algorithms::Strings::ukkonen_suffix_tree<string_t, int_t> full_tree(text);

        std::vector<decltype(full_tree)::node_t> expected(len6);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1,
            {
                { 'a', 3 },
                { 'b', len5 },
            } };
        expected[2] = { 2, 1, 2, 1, 0 };
        expected[3] = { 3, 0, 0, 1, -1,
            {
                { 'a', 2 },
                { 'b', 4 },
            } };
        expected[4] = { 4, 2, 2, 1, 1 };
        expected[len5] = { len5, 2, 2, 1, 2 };

        ::Standard::Algorithms::ert::are_equal(expected, full_tree.nodes(), text + " nodes");
    }

    void verify_built_tree40()
    {
        constexpr auto separator = '~';
        constexpr auto separator2 = '%';
        const string_t text1 = "ab";
        const string_t text2 = "ca";
        const string_t text = string_t().append(text1).append(1, separator).append(text2).append(1, separator2);

        const Standard::Algorithms::Strings::ukkonen_suffix_tree<string_t, int_t, char_t, separator> full_tree(text);

        std::vector<decltype(full_tree)::node_t> expected(len9);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1,
            {
                { separator2, len8 },
                { 'a', len6 },
                { 'b', 3 },
                { 'c', len5 },
                { separator, 4 },
            } };
        expected[2] = { 2, 1, len5, 1, 0 };
        expected[3] = { 3, 1, len5, 1, 1 };
        expected[4] = { 4, 2, len5, 1, 2 };
        expected[len5] = { len5, 3, len5, 1, 3 };
        expected[len6] = { len6, 0, 0, 1, -1,
            {
                { separator2, len7 },
                { 'b', 2 },
            } };
        expected[len7] = { len7, len5, len5, 1, 4 };
        expected[len8] = { len8, len5, len5, 1, len5 };

        ::Standard::Algorithms::ert::are_equal(expected, full_tree.nodes(), text + " nodes");
    }

    void verify_built_tree41()
    {
        constexpr auto separator = '~';
        constexpr auto separator2 = '%';

        const string_t text1 = "ab";
        const string_t text2 = "ca";
        const string_t text = string_t().append(text1).append(1, separator).append(text2).append(1, separator2);

        const Standard::Algorithms::Strings::ukkonen_suffix_tree<string_t, int_t, char_t, separator> full_tree(
            text, true);

        std::vector<decltype(full_tree)::node_t> expected(len9);
        expected[0].e = static_cast<int_t>(text.size() - 1);

        // id, begin, end, suffix_link, suffix_index, {children}.
        expected[1] = { 1, -1, -1, 0, -1,
            {
                { separator2, len8 },
                { 'a', len6 },
                { 'b', 3 },
                { 'c', len5 },
                { separator, 4 },
            } };
        //  ab~ca%
        //  012345
        //
        //        1    -> 4,8
        //  /     |   \;
        // b3    a6   c5
        // b~  /  a   ca%
        //   %7   |;
        //  a%    b2
        //       ab~
        expected[2] = { 2, 1, 2, 1, 0 }; // { 2, 1, 5, 1, 0 };
        expected[3] = { 3, 1, 2, 1, 1 }; // { 3, 1, 5, 1, 1 };
        expected[4] = { 4, 2, 2, 1, 2 }; // { 4, 2, 5, 1, 2 };
        expected[len5] = { len5, 3, len5, 1, 3 };
        expected[len6] = { len6, 0, 0, 1, -1,
            {
                { separator2, len7 },
                { 'b', 2 },
            } };
        expected[len7] = { len7, len5, len5, 1, 4 };
        expected[len8] = { len8, len5, len5, 1, len5 };

        ::Standard::Algorithms::ert::are_equal(expected, full_tree.nodes(), text + " nodes");
    }
} // namespace

void Standard::Algorithms::Strings::Tests::ukkonen_suffix_tree_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
    verify_built_tree();
    verify_built_tree1();
    verify_built_tree2();
    verify_built_tree3();
    verify_built_tree40();
    verify_built_tree41();
}
