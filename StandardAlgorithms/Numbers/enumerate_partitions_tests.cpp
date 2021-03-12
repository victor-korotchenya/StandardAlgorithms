#include <set>
#include "../Utilities/PrintUtilities.h"
#include "enumerate_partitions.h"
#include "enumerate_partitions_tests.h"
#include "../test_utilities.h"
using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    void verify_subsets(const set<vector<vector<int>>>& set1, const vector<int>& elements)
    {
        vector<int> buf(elements.size());
        set<int> un;
        const set<int> expected(elements.begin(), elements.end());

        for (const auto& a : set1)
        {
            buf.clear();
            un.clear();
            Assert::Greater(a.size(), 0, "Set size");

            for (const auto& b : a)
            {
                Assert::Greater(b.size(), 0, "Subset size");
                for (const auto& c : b)
                {
                    const auto added = un.insert(c);
                    Assert::AreEqual(true, added.second, "Subset items must be unique.");
                }
            }

            Assert::AreEquals(expected, un, "The subsets must be complete.");
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void enumerate_partitions_tests()
                {
                    auto lam0 = [](const vector<vector<int>>& items0, set<vector<vector<int>>>& set0, int& cnt0)
                        -> void
                    {
                        Assert::Greater(items0.size(), 0, "Partition cannot be empty.");

                        auto items_copy = items0;
                        for (auto& c : items_copy)
                            sort(c.begin(), c.end());

                        sort(items_copy.begin(), items_copy.end());

                        set0.insert(items_copy);
                        ++cnt0;
                        Assert::AreEqual(cnt0, set0.size(), "Partition cannot repeat.");

                        ////Print items
                        //auto h0 = false;
                        //for (const auto &list : items0)
                        //{
                        //  assert(list.size());
                        //  cout << (h0 ? " (" : "(");
                        //  h0 = true;
                        //  auto h1 = false;
                        //  for (const auto &e : list)
                        //  {
                        //    cout << (h1 ? " " : "") << e;
                        //    h1 = true;
                        //  }
                        //  cout << ")";
                        //}
                        //cout << '\n';
                    };

                    auto cnt1 = 0;
                    set<vector<vector<int>>> set1;

                    auto la1 = [&cnt1, &set1, &lam0](const vector<vector<int>>& els)-> void
                    {
                        lam0(els, set1, cnt1);
                    };
                    const vector<int> elements{ 1,2,3,4 };
                    {
                        auto elements_copy = elements;
                        enumerate_partitions_all(elements_copy, la1);
                        Assert::AreEqual(15, cnt1, "enumerate_partitions_all");

                        verify_subsets(set1, elements);
                    }

                    auto cnt2 = 0;
                    set<vector<vector<int>>> set2;
                    auto la2 = [&cnt2, &set2, &lam0](const vector<vector<int>>& els)-> void
                    {
                        lam0(els, set2, cnt2);
                    };

                    enumerate_partitions_by_one(elements, la2);
                    Assert::AreEqual(cnt1, cnt2, "enumerate_partitions_by_one");
                    Assert::AreEquals(set1, set2, "sets");
                }
            }
        }
    }
}