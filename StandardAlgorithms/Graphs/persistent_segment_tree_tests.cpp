#include "../Utilities/Random.h"
#include "persistent_segment_tree_tests.h"
#include "persistent_segment_tree.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = int64_t;

    struct oper_t final
    {
        int left, right_incl;
        union
        {
            int_t sum;
            int ad;
        };

        unsigned version;
        bool is_add;

        void validate() const
        {
            RequireNonNegative(left, "left");
            RequireNotGreater(left, right_incl, "right_incl");

            if (is_add)
            {
                RequirePositive(version, "version");
                Assert::NotEqual(ad, 0, "ad");
            }
        }
    };

    oper_t ad(int left, int right_incl, int ad, unsigned version)
    {
        assert(version > 0 && ad);

        oper_t op;
        op.left = left;
        op.right_incl = right_incl;
        op.ad = ad;
        op.version = version;
        op.is_add = true;

        op.validate();
        return op;
    }

    oper_t su(int left, int right_incl, unsigned version, int_t sum)
    {
        oper_t op;
        op.left = left;
        op.right_incl = right_incl;
        op.sum = sum;
        op.version = version;
        op.is_add = false;

        op.validate();
        return op;
    }

    ostream& operator <<(ostream& str, oper_t const& op)
    {
        if (op.is_add)
            str << "Add [" << op.left << ", " << op.right_incl << "] " << op.ad << " new version " << op.version;
        else
            str << "Sum [" << op.left << ", " << op.right_incl << "] version " << op.version << " expect " << op.sum;

        return str;
    }

    struct test_case final : base_test_case
    {
        vector<oper_t> operations;
        int n;

        test_case(string&& name,
            int n,
            vector<oper_t>&& operations)
            : base_test_case(forward<string>(name)),
            operations(forward<vector<oper_t>>(operations)),
            n(n)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(n, "n");
            RequirePositive(operations.size(), "operations");
            Assert::AreEqual(false, operations.back().is_add, "last operation");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            PrintValue(str, "n", n);
            ::Print("operations", operations, str);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.emplace_back("base1", 1, vector<oper_t>{
            su(0, 0, 0, 0),
                ad(0, 0, 10, 1),
                ad(0, 0, 300, 2),
                su(0, 0, 0, 0),
                su(0, 0, 1, 10),
                su(0, 0, 2, 310),
        });

        IntRandom r;
        vector<oper_t> ops;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(1, 10), m = r(1, 10);
            unsigned version = 0;
            ops.resize(m);

            for (auto i = 0; i < m; ++i)
            {
                const auto left = r(0, n - 1),
                    // todo: p1. "Add L R A" now works only L=R.
                    //right = r(left, n - 1);
                    right = left;

                ops[i] = (r() & 1) || i == m - 1
                    ? su(left, right, version, 0)
                    : ad(left, right, r(1, 20), ++version);
            }

            test_cases.emplace_back("random" + to_string(att), n, move(ops));
        }
    }

    template<class tree_a, class tree_b, class tree_c>
    void run_add(tree_a& tree,
        tree_b& tree_they,
        tree_c& tree_slow,
        const oper_t& op,
        unsigned& version)
    {
        Assert::AreEqual(version, tree.version(), "before tree add");
        Assert::AreEqual(version, tree_they.version(), "before tree_they add");
        Assert::AreEqual(version, tree_slow.version(), "before tree_slow add");
        tree.add(op.left, op.right_incl, op.ad);
        tree_they.add(op.left, op.right_incl, op.ad);
        tree_slow.add(op.left, op.right_incl, op.ad);

        ++version;
        Assert::AreEqual(version, op.version, "add op version");
        Assert::AreEqual(version, tree.version(), "after tree add ");
        Assert::AreEqual(version, tree_they.version(), "after tree_they add ");
        Assert::AreEqual(version, tree_slow.version(), "after tree_slow add");
    }

    template<class tree_a, class tree_b, class tree_c>
    void run_sum(const tree_a& tree,
        const tree_b& tree_they,
        const tree_c& tree_slow,
        const oper_t& op,
        const unsigned version)
    {
        Assert::GreaterOrEqual(version, op.version, "sum op.version");
        const auto actual = tree.sum(op.left, op.right_incl, op.version);
        if (op.sum)
            Assert::AreEqual(op.sum, actual, "tree sum");

        const auto they = tree_they.sum(op.left, op.right_incl, op.version);
        Assert::AreEqual(actual, they, "tree_they sum");

        const auto slow = tree_slow.sum(op.left, op.right_incl, op.version);
        Assert::AreEqual(actual, slow, "tree_slow sum");
    }

    void run_test_case(const test_case& test)
    {
        auto tree = persistent_segment_tree_slow<int_t>(test.n);
        assert(static_cast<int>(tree.size()) == test.n && !tree.version());

        auto tree_they = persistent_segment_tree_they<int_t>(test.n);
        assert(static_cast<int>(tree_they.size()) == test.n && !tree_they.version());

        auto tree_slow = persistent_segment_tree_slow<int_t>(test.n);
        assert(static_cast<int>(tree_slow.size()) == test.n && !tree_slow.version());

        unsigned version{}, index{};
        for (const auto& op : test.operations)
        {
            try
            {
                if (op.is_add)
                    run_add(tree, tree_they, tree_slow, op, version);
                else
                    run_sum(tree, tree_they, tree_slow, op, version);
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

void Privet::Algorithms::Trees::Tests::persistent_segment_tree_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}