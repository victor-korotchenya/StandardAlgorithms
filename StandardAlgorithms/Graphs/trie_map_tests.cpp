#include "trie_map_tests.h"
#include "trie_map.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/Random.h"

//#define trie_map_tests_load_flag 1

#if trie_map_tests_load_flag
#include <iostream>
#include "../elapsed_time_ns.h"
#endif

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using string_t = string;
    using key_t = string_t::value_type;
    using value_t = int;
    constexpr auto is_add_flag = 1, expected_flag = 2;

    struct oper_t final
    {
        string_t key;
        value_t value{};
        char flags{};

        bool is_add() const
        {
            return flags & is_add_flag;
        }

        bool expected() const
        {
            return flags & expected_flag;
        }
    };

    ostream& operator<<(ostream& str, const oper_t& op)
    {
        str << (op.is_add() ? "add '" : "find '")
            << op.key << "' " << op.value << " expect " << op.expected();

        return str;
    }

    oper_t ad(const string_t& key, const value_t& value)
    {
        oper_t op;
        op.key = key;
        op.value = value;
        op.flags = is_add_flag;
        return op;
    }

    oper_t fin(const string_t& key, const bool expected, const value_t& value = {})
    {
        oper_t op;
        op.key = key;
        op.value = value;
        op.flags = expected ? expected_flag : 0;
        return op;
    }

    struct test_case final : base_test_case
    {
        vector<oper_t> operations;

        test_case(string&& name,
            vector<oper_t>&& operations)
            : base_test_case(forward<string>(name)),
            operations(forward<vector<oper_t>>(operations))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int>(operations.size());
            RequirePositive(size, "operations.size");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("operations", operations, str);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "base1", {
            ad("", 1),
            fin("c", false, 3),
            ad("c", 4),
            ad("c", 5),
            fin("c", true, 5),
            fin("c", true, 5),
            ad("dfg", 60),
            fin("cd", false),
            fin("d", false),
            fin("df", false),
            fin("f", false),
            fin("fg", false),
            fin("dfg", true, 60),
            fin("dfgdfg", false),
            fin("dfgd", false),
        } });
    }

    void run_test_case(const test_case& test_case)
    {
        trie_map<key_t, value_t> trie;

        for (auto i = 0u; i < test_case.operations.size(); ++i)
        {
            const auto& op = test_case.operations[i];
            const auto& key = op.key;
            if (op.is_add())
            {
                trie[key] = op.value;
                continue;
            }

            const auto name = "op " + to_string(i);

            value_t value{};
            const auto actual = trie.find(key, value);
            Assert::AreEqual(op.expected(), actual, name + " expected");

            if (actual)
                Assert::AreEqual(op.value, value, name + " value");
        }
    }

#if trie_map_tests_load_flag
    void trie_map_tests_load_tests()
    {
        constexpr auto count = 25000, length = 4;
        constexpr auto min_char = '0', max_char = '9';
        vector<string_t> words(count);

        IntRandom r;
        for (auto att = 0, att_max = 10; att < att_max; ++att)
        {
            for (auto i = 0; i < count; ++i)
                words[i] = random_string<decltype(r), min_char, max_char>(r, length, length);

            using node_t = simple_trie_map_node_t<key_t, value_t, min_char, max_char>;

            elapsed_time_ns t0;
            trie_map<key_t, int, node_t> trie_map(count * length);
            {
                auto i = 0;
                for (const auto& word : words)
                    trie_map[word] = ++i;
            }
            const auto elapsed0 = t0.elapsed();

            elapsed_time_ns t1;
            unordered_map<string_t, int> m;
            m.reserve(count);

            {
                auto i = 0;
                for (const auto& word : words)
                    m[word] = ++i;
            }
            const auto elapsed1 = t1.elapsed();

            const auto ratio = elapsed0 > 0 ? double(elapsed1) / elapsed0 : 0.0;

            cout << " att " + to_string(att) + ", t1 " + to_string(elapsed1) + " / t0 " + to_string(elapsed0) + " = " + to_string(ratio) + "\n";
        }
    }
#endif
}

void Privet::Algorithms::Trees::Tests::trie_map_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);

#if trie_map_tests_load_flag
    trie_map_tests_load_tests();
#endif
}