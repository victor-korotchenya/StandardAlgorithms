#include "del_min_side_items_2min_gt_max.h"
#include "del_min_side_items_2min_gt_max_tests.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/Random.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = short;
    using int_t2 = int64_t;

    struct test_case final : base_test_case
    {
        vector<int_t> ar;
        pair<int, int> expected;

        test_case(string&& name,
            vector<int_t>&& ar, int left = -1, int right = -1)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<int_t>>(ar)),
            expected({ left, right })
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            Assert::GreaterOrEqual(expected.first, -1, "expected.first");
            Assert::GreaterOrEqual(expected.second, -1, "expected.second");

            const auto size = static_cast<int>(ar.size());
            Assert::GreaterOrEqual(size, expected.first + expected.second, "ar expected");
            Assert::GreaterOrEqual(size, expected.first, "expected.x");
            Assert::GreaterOrEqual(size, expected.second, "expected.y");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print("ar", ar, str);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        ShortRandom r;
        vector<int_t> ar;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(1, 10);
            FillRandom(ar, n, int_t(100));

            // Make positive.
            const auto mi = *min_element(ar.begin(), ar.end());
            for (auto& a : ar)
                a += 1 - mi;

            test_cases.emplace_back("random" + to_string(att), move(ar));
        }

        test_cases.push_back({ "some", { 10, 1, 12, 100 }, 0, 3 });
        test_cases.push_back({ "all", { -10, -1, -12, -100, 0 }, 5, 0 });
        test_cases.push_back({ "none", { 1, 1, 1, 1 }, 0, 0 });
        test_cases.push_back({ "base1", { 1, 10, 1, 1, 1, 5,7,80 }, 2, 3 });
    }

    template<class pair_t>
    void check_cut(const pair_t& p, const vector<int_t>& ar, const string& name)
    {
        const auto size = static_cast<int>(ar.size());

        Assert::Greater(size, 0, "ar size. " + name);

        Assert::GreaterOrEqual(p.first, 0, name);
        Assert::GreaterOrEqual(size, p.first, name);

        Assert::GreaterOrEqual(p.second, 0, name);
        Assert::GreaterOrEqual(size, p.second, name);

        if (size == p.first + p.second)
            return;

        Assert::Greater(size, p.first + p.second, "total removed " + name);

        auto mi = ar[p.first], ma = mi;
        for (auto i = p.first + 1; i < size - p.second; ++i)
        {
            const auto& a = ar[i];
            if (mi > a)
                mi = a;
            else if (ma < a)
                ma = a;
        }

        Assert::Greater(mi * int64_t(2), ma, "2*mi > ma, " + name);
    }

    template<class pair_t, class pair_t2>
    void compare_lengths(const pair_t& expected, const pair_t2& actual, const vector<int_t>& ar, const string& name)
    {
        if (expected == actual)
            return;

        if (expected.second + expected.first != actual.second + actual.first)
            Assert::AreEqual(expected, actual, name);

        check_cut(expected, ar, "expected " + name);
        check_cut(actual, ar, "actual " + name);
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = del_min_side_items_2min_gt_max<int_t2>(test.ar);
        if (test.expected.first >= 0)
            compare_lengths(test.expected, actual, test.ar, "del_min_side_items_2min_gt_max");

        const auto slow = del_min_side_items_2min_gt_max_slow<int_t2>(test.ar);
        compare_lengths(actual, slow, test.ar, "del_min_side_items_2min_gt_max_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::del_min_side_items_2min_gt_max_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}