#include "../Utilities/Random.h"
#include "persistent_trie_tests.h"
#include "persistent_trie.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using string_t = string;

    struct oper_t final
    {
        string s;
        size_t version{};
        bool is_add{}, expected{};

        void validate() const
        {
            RequirePositive(s.size(), "s.size");

            if (is_add)
                RequirePositive(version, "version");
        }
    };

    ostream& operator <<(ostream& str, oper_t const& op)
    {
        if (op.is_add)
            str << "Add '" << op.s << "' " << op.expected << " new version " << op.version;
        else
            str << "Find '" << op.s << "' version " << op.version << " " << op.expected;

        return str;
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

            RequirePositive(operations.size(), "operations");
            Assert::AreEqual(false, operations.back().is_add, "last operation");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("operations", operations, str);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.emplace_back("base1", vector<oper_t>{
            { "a", 0, false, false},
            { "a", 1, true, true },
            { "a", 1, false, true },
            { "a", 1, true, false },//2nd add
            { "a", 1, false, true },//2nd search

            { "ab", 2, true, true },
            { "cd", 3, true, true },
            { "ce", 4, true, true },
            { "cd", 4, true, false },
                // Searches
            { "a", 1, false, true },
            { "a", 2, false, true },
            { "a", 3, false, true },
            { "a", 4, false, true },
            { "c", 3, false, false },
            { "c", 4, false, false },
            { "cd", 2, false, false },
            { "cd", 3, false, true },
            { "cd", 4, false, true },
            { "ce", 3, false, false },
            { "ce", 4, false, true },
            { "aB", 4, false, false },
            { "ab", 1, false, false },
            { "ab", 2, false, true },
            { "ab", 4, false, true },
            { "abc", 4, false, false },
                //
            { "ghj", 5, true, true },
            { "gh", 5, false, false },
            { "ghj", 5, false, true },
            { "gh", 6, true, true },
            { "gh", 6, true, false },
            { "ghj", 6, true, false },
            { "gh", 6, true, false },
            { "gh", 5, false, false },
            { "gh", 6, false, true },
            { "ghj", 5, false, true },
            { "ghj", 6, false, true },
        });

        IntRandom r;
        vector<oper_t> ops;
        unordered_set<string_t> uniq;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(1, 10);
            size_t version = 0;
            ops.resize(n);
            uniq.clear();

            for (auto i = 0; i < n; ++i)
            {
                const auto s = random_string(r, 1, 5);
                const auto is_add = !(r() & 1) && i != n - 1;
                const auto expected = is_add ? uniq.insert(s).second
                    : uniq.count(s) > 0;

                version += is_add && expected;

                ops[i] = { s, version, is_add, expected };
            }

            test_cases.emplace_back("random" + to_string(att), move(ops));
        }
    }

    template<class tree_a, class tree_b>
    void run_add(tree_a& tree,
        tree_b& tree_slow,
        const oper_t& op,
        size_t& version)
    {
        Assert::AreEqual(version, tree.version(), "before tree add");
        Assert::AreEqual(version, tree_slow.version(), "before tree_slow add");

        const auto actual = tree.add(op.s);
        Assert::AreEqual(op.expected, actual, "tree add");

        const auto slow = tree_slow.add(op.s);
        Assert::AreEqual(actual, slow, "tree_slow add");

        version += actual;
        Assert::AreEqual(version, op.version, "add op version");
        Assert::AreEqual(version, tree.version(), "after tree add ");
        Assert::AreEqual(version, tree_slow.version(), "after tree_slow add");
    }

    template<class tree_a, class tree_b>
    void run_find(const tree_a& tree,
        const tree_b& tree_slow,
        const oper_t& op,
        const size_t version)
    {
        Assert::GreaterOrEqual(version, op.version, "sum op.version");
        const auto actual = tree.find(op.s, op.version);
        Assert::AreEqual(op.expected, actual, "tree find");

        const auto slow = tree_slow.find(op.s, op.version);
        Assert::AreEqual(actual, slow, "tree_slow find");
    }

    void run_test_case(const test_case& test)
    {
        auto tree = persistent_trie<string_t>();
        assert(!tree.version());

        auto tree_slow = persistent_trie_slow<string_t>();
        assert(!tree_slow.version());

        size_t version{}, index{};
        for (const auto& op : test.operations)
        {
            try
            {
                if (op.is_add)
                    run_add(tree, tree_slow, op, version);
                else
                    run_find(tree, tree_slow, op, version);
            }
            catch (const exception& ex)
            {
                const auto err = ex.what() + string(" at op index ") + to_string(index);
                throw runtime_error(err);
            }

            ++index;
        }
    }
}

void Privet::Algorithms::Trees::Tests::persistent_trie_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}