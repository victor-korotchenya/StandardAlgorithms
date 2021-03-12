#include <array>
#include <functional>
#include <memory>
#include <unordered_set>
#include "order_statistics_tree.h"
#include "order_statistics_tree_slow.h"
#include "order_statistics_tree_tests.h"
#include "../Assert.h"
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;
// todo: p1. end
namespace
{
    using number_t = short;

    template<typename tree_t>
    void order_statistics_tree_test_plain(const string& name)
    {
        auto p_tree = make_unique<tree_t>();
        auto& tree = *p_tree.get();

        string s;
        for (number_t i = 1; i <= 5; i += 2)
        {
            const auto added = tree.insert(i);
            s.clear();
            s.append(name);
            s.append(" Insert ");
            s.append(to_string(i));
            Assert::AreEqual(added, true, s);

            const auto added2 = tree.insert(i);
            s.append(" second");
            Assert::AreEqual(added2, false, s);
        }

        const number_t to_delete = 3;
        for (auto i = 0; i < 2; ++i)
        {
            const auto found = tree.find(to_delete);
            s.clear();
            s.append(name);
            s.append(" Find ");
            const auto i_string = to_string(i);
            s.append(i_string);
            Assert::AreEqual(found, i == 0, s);

            const auto erased = tree.erase(to_delete);
            s.clear();
            s.append(name);
            s.append(" Erase ");
            s.append(i_string);
            Assert::AreEqual(erased, i == 0, s);

            s.clear();
            s.append(name);
            s.append(" Size after erase ");
            s.append(i_string);
            Assert::AreEqual(tree.size(), 2, s);
        }

        const initializer_list<number_t> ar{ 1,5 };
        {
            auto rank = 0;
            for (const auto& i : ar)
            {
                const auto ks = tree.find_k_smallest(rank++);
                Assert::AreEqual(ks, i, name + " find_k_smallest");
            }
        }

        for (number_t i = 1; i <= 5; i += 2)
        {
            const auto rank = tree.rank(i);
            const auto expected = i == to_delete ? -1 : i / 5;
            Assert::AreEqual(rank, expected, name + "rank");
        }
    }

    struct test_case final : base_test_case
    {
        vector<number_t> Data;

        test_case(string&& name, const vector<number_t>& data)
            : base_test_case(forward<string>(name)),
            Data(data)
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print(", Data=", Data, str);
        }
    };

    void generate_test_cases(vector<test_case>& testCases)
    {
        const auto size = 20;
        vector<number_t> data;
        FillRandom<number_t>(data, size, 0, true);
        testCases.push_back({ "random", data });
    }

    void order_statistics_tree_test_random(const test_case& testCase)
    {
        auto p_slow = make_unique<order_statistics_tree_slow<number_t>>();
        auto& slow = *p_slow.get();

        auto p_fast = make_unique<order_statistics_tree<number_t>>();
        auto& fast = *p_fast.get();

        string s;
        auto i = 0;
        for (const auto& value : testCase.Data)
        {
            const auto added = fast.insert(value);
            s.clear();
            s.append(" Insert random ");
            s.append(to_string(value));
            Assert::AreEqual(added, true, s);

            const auto added2 = slow.insert(value);
            s.append(" slow ");
            Assert::AreEqual(added2, true, s);

            const auto smallest = fast.find_k_smallest(i);
            const auto smallest2 = slow.find_k_smallest(i);
            const auto i_str = to_string(i);
            Assert::AreEqual(smallest, smallest2, "random find_k_smallest " + i_str);

            const auto rank = fast.rank(value);
            const auto rank2 = slow.rank(value);
            Assert::AreEqual(rank, rank2, "random rank " + i_str);
            ++i;
        }
    }
}

void Privet::Algorithms::Trees::Tests::order_statistics_tree_tests()
{
    order_statistics_tree_test_plain<order_statistics_tree_slow<number_t>>("slow");
    //order_statistics_tree_test_plain<order_statistics_tree<number_t>>("fast");

    //test_utilities<TestCase>::run_tests(order_statistics_tree_test_random, GenerateTestCases);
}