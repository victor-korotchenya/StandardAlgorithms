#include "centroid_decomposition.h"
#include "centroid_decomposition_tests.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = int;

    struct test_case : base_test_case
    {
        vector<vector<Number>> tree;
        vector<vector<int>> expected_centroid_tree;
        int root;

        test_case(string&& name,
            vector<vector<Number>>&& tree,
            vector<vector<int>>&& expected_centroid_tree,
            int root)
            : base_test_case(forward<string>(name)),
            tree(forward<vector<vector<Number>>>(tree)),
            expected_centroid_tree(forward<vector<vector<int>>>(expected_centroid_tree)),
            root(root)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int>(tree.size());
            RequirePositive(size, "tree size");
            Assert::AreEqual(true, is_tree(tree), "is tree");

            Assert::AreEqual(size, static_cast<int>(expected_centroid_tree.size()), "expected_centroid_tree size");
            Assert::AreEqual(true, is_tree(expected_centroid_tree), "is expected_centroid_tree");

            Assert::GreaterOrEqual(root, 0, "root");
            Assert::Greater(size, root, "root");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", tree=", tree, str);
            ::Print(", expected_centroid_tree=", expected_centroid_tree, str);
            ::PrintValue(str, ", root=", root);
        }
    };

    void generate_test_cases(vector<test_case>& tests)
    {
        tests.push_back(
            {
              "One",
              {{}
              },
              {{}
              },
              0
            });
        tests.push_back(
            {
              "Edge",
              {
                {1},
                {0},
              },
              {
                {1},
                {0},
              },
              0
            });
        tests.push_back(
            {
              "three_1",
              {
                {1},
                {0,2},
                {1},
              },
              {
                {1},
                {0,2},
                {1},
              },
              1
            });
        tests.push_back(
            {
              "three_0",
              {
                {1,2},
                {0},
                {0},
              },
              {
                {1,2},
                {0},
                {0},
              },
              0
            });
        tests.push_back(
            {
              "three_2",
              {
                {2},
                {2},
                {0,1},
              },
              {
                {2},
                {2},
                {0,1},
              },
              2
            });

        tests.push_back(
            {
              "path7",
              {
                {1},
                {0,2},//1
                {1,3},
                {2,4},//3
                {3,5},
                {4,6},//5
                {5,},
              },
              {
                {1},
                {0,2,3},
                {1},
                {1,5},//3
                {5},
                {4,6,3},
                {5}
              },
              3
            });
        tests.push_back(
            {
              "path8",
              {
                {1},
                {0,2},//1
                {1,3},
                {2,4},//3
                {3,5},
                {4,6},//5
                {5,7},
                {6},//7
              },
              {
                {1},
                {0,2,3},//1
                {1},
                {1,5},//3
                {5},
                {4,6,3},//5
                {7,5},
                {6},
              },
              3
            });

        tests.push_back(
            {
              "base_8",
              {
                {1},//0
                {0,2,3},
                {1,4,5,6},
                {1},//3
                {2,7},
                {2},
                {2},//6
                {4},
            },
            /* 0 - 1 - 3
                   |
                   2
                 / | \
           7 - 4 - 5 - 6
            */
            {
                {1},
                {0,3,2},//1
                {1,4,5,6},
                {1},//3
                {7,2},
                {2},//5
                {2},
                {4},
            },
            2 });
    }

    void run_test_case(const test_case& test)
    {
        centroid_decomposition<Number> dec(test.tree);
        const auto& centroid_tree = dec.get_centroid_tree();

        Assert::AreEqual(test.expected_centroid_tree, centroid_tree, "get_centroid_tree");

        const auto root = dec.get_root();
        Assert::AreEqual(test.root, root, "get_root");
    }
}

namespace Privet::Algorithms::Trees::Tests
{
    void centroid_decomposition_tests()
    {
        test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
    }
}