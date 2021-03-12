#include"graph_girth.h"
#include<limits>
#include<queue>
#include<sstream>
#include<stdexcept>
#include<string>

namespace
{
    constexpr auto girth_min = 3U;

    constexpr auto girth_max = std::numeric_limits<std::uint64_t>::max();

    constexpr std::size_t skip_one_vertex = 1;
    constexpr std::size_t plus_infinity = 0ZU - skip_one_vertex;
    constexpr auto no_parent = plus_infinity;

    struct girth_context final
    {
        const Standard::Algorithms::Graphs::graph<std::int32_t, std::uint32_t> &graf;
        std::vector<std::size_t> &cycle_vertices;

        std::uint64_t result = girth_max;

        // To bound the search.
        std::uint64_t semi_distance = girth_max;

        std::vector<bool> visited{};
        std::vector<std::size_t> parents{};
        std::vector<std::size_t> parents_temp{};
        std::vector<std::size_t> distances{};
        std::queue<std::size_t> que{};
    };

    constexpr void fill_parents(
        const std::vector<std::size_t> &parents, std::size_t start_vertex, std::vector<std::size_t> &data)
    {
        data.clear();

        do
        {
            data.push_back(start_vertex);
            start_vertex = parents[start_vertex];
        } while (no_parent != start_vertex);
    }

    constexpr void append_reversed_but_last(
        const std::vector<std::size_t> &parents_temp, std::vector<std::size_t> &cycle_vertices)
    {
        const auto parents_temp_reversed_end = parents_temp.crend();
        auto ite = parents_temp.crbegin();

        if (ite == parents_temp_reversed_end)
        {
            return;
        }

        while ((++ite) != parents_temp_reversed_end)
        {
            const auto &vertex = *ite;
            cycle_vertices.push_back(vertex);
        }
    }

    void girth_prepare_to_run_bfs(const std::size_t source_vertex, girth_context &context)
    {
        const auto &adjacency_lists = context.graf.adjacency_lists();
        const auto vertex_count = adjacency_lists.size();
        assert(source_vertex < vertex_count);

        // todo(p3): Get rid of distances by using 2 queues:
        //  que1 - current level.
        //  que2 - next level.
        context.visited.assign(vertex_count, false);
        context.parents.assign(vertex_count, no_parent);
        context.distances.assign(vertex_count, plus_infinity);
        context.distances[source_vertex] = {};

        // Run BFS.
        context.que = {};
        context.que.push(source_vertex);
    }

    constexpr void girth_explore_neighbors(
        [[maybe_unused]] const std::size_t source_vertex, girth_context &context, const std::size_t current_vertex)
    {
        const auto current_vertex_parent = context.parents[current_vertex];
        const auto next_weight = context.distances[current_vertex] + 1U;

        const auto &edges = context.graf.adjacency_lists()[current_vertex];

        for (const auto &edge : edges)
        {
            const auto &tod = edge.vertex;

            if (current_vertex_parent == static_cast<std::size_t>(tod))
            {// Skip the edge (parents[currentVertex], currentVertex) which becomes
              // (currentVertex, parents[currentVertex]).
                continue;
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                const auto distance2 = context.distances[tod];
                const auto dis = static_cast<std::int64_t>(next_weight) - static_cast<std::int64_t>(1) -
                    static_cast<std::int64_t>(distance2);

                const auto abs_delta = std::abs(dis);

                const auto is_ok_distance2 = plus_infinity == distance2 || abs_delta <= 2;
                if (!is_ok_distance2)
                {
                    const auto *const func_name = static_cast<const char *>(__FUNCTION__);

                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Bad absDelta " << abs_delta << " in " << func_name << ", sourceVertex " << source_vertex
                        << ", to " << tod << ", currentVertex " << current_vertex << ", distance2 " << distance2
                        << ", (nextWeight - 1) " << (next_weight - 1) << ".";

                    Standard::Algorithms::throw_exception(str);
                }
            }

            if (const auto is_cycle_found = context.visited[tod]; is_cycle_found)
            {
                const std::uint64_t current_distance = next_weight + context.distances[tod];

                if (!(current_distance < context.result))
                {
                    continue;
                }

                if (girth_max == context.result)
                {
                    context.cycle_vertices.reserve(current_distance);

                    const std::uint64_t estimated_size = (current_distance >> 1U) + 1U;
                    context.parents_temp.reserve(estimated_size);
                }

                fill_parents(context.parents, current_vertex, context.cycle_vertices);
                fill_parents(context.parents, tod, context.parents_temp);
                append_reversed_but_last(context.parents_temp, context.cycle_vertices);

                context.result = current_distance;

                if (girth_min == current_distance)
                {// The smallest value is found - no sense to go on.
                    return;
                }

                context.semi_distance = (current_distance - 1U) >> 1U;
            }
            else if (next_weight < context.distances[tod])
            {// Push only once as there can be several arcs to a vertex.
                context.que.push(tod);
                context.parents[tod] = current_vertex;
                context.distances[tod] = next_weight;
            }
        }
    }

    void girth_explore_vertex(const std::size_t source_vertex, girth_context &context)
    {
        girth_prepare_to_run_bfs(source_vertex, context);

        do
        {
            const auto current_vertex = context.que.front();
            context.que.pop();

            if (context.semi_distance < context.distances[current_vertex] || girth_min == context.result)
            {// The next level won't produce a smaller size cycle.
                return;
            }

            context.visited[current_vertex] = true;

            girth_explore_neighbors(source_vertex, context, current_vertex);
        } while (!context.que.empty());
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Graphs::graph_girth::find_girth_unweighted(
    const graph<std::int32_t, std::uint32_t> &graf, std::vector<std::size_t> &cycle_vertices) -> std::uint64_t
{
    const auto &adjacency_lists = graf.adjacency_lists();
    const auto vertex_count = adjacency_lists.size();

    if (vertex_count < girth_min) [[unlikely]]
    {
        throw std::runtime_error("The graph must have at least 3 vertices.");
    }

    cycle_vertices.clear();

    girth_context context{ graf, cycle_vertices };

    for (auto source_vertex = skip_one_vertex; source_vertex < vertex_count; ++source_vertex)
    {
        girth_explore_vertex(source_vertex, context);

        if (girth_min == context.result)
        {
            return girth_min;
        }
    }

    if (const auto is_no_cycle = girth_max == context.result; is_no_cycle)
    {
        context.result = {};
    }

    return context.result;
}
