#include <unordered_set>
#include "SparseGraph.h"
#include "SparseGraphTests.h"
#include "../Utilities/RandomGenerator.h"
#include "../Numbers/hash_utilities.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms;

namespace
{
    using Number = long;

    template<typename Number>
    SparseGraph<Number> create_random_sparse_graph()
    {
        RandomGenerator<Number, uniform_int_distribution<Number>> random;

        const Number vertex_count = random(2, 100);
        const Number edge_count = random(1, vertex_count >> 1);

        SparseGraph<Number> result(vertex_count,
            //Added twice
            edge_count << 2);

        using Pair = pair<Number, Number>;

        unordered_set<Pair, std::pair_hash> edges;

        auto i = 0;
        Number a, b;
        do
        {
        Label_1:
            a = random(0, vertex_count - 1);
            do
            {
                b = random(0, vertex_count - 1);
            } while (a == b);

            if (b < a)
            {
                swap(a, b);
            }

            const auto it = edges.insert(make_pair(a, b));
            if (!it.second)
            {
                goto Label_1;
            }

            result.add_edge(a, b);
            result.add_edge(b, b);
        } while (++i < edge_count);

        return result;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            namespace Tests
            {
                void SparseGraphTests()
                {
                    const auto graph = create_random_sparse_graph<Number>();

                    //TODO: p1. finish.
                }
            }
        }
    }
}