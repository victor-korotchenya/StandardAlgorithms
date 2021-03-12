#pragma once
#include"../Numbers/inplace_merge.h"
#include"../Utilities/same_sign_leq_size.h"
#include<algorithm>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<limits>
#include<numeric>
#include<stdexcept>
#include<string>
#include<type_traits>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<class graph_t, class long_int_t>
    struct slow_min_cut_context final
    {
        const graph_t &graph;

        std::size_t index{}; // Will start from graph.size().
        std::vector<bool> froms{};
        std::size_t froms_count{};

        std::vector<bool> min_chosen{};
        std::size_t min_chosen_count = std::numeric_limits<std::size_t>::max();
        long_int_t min_cost = std::numeric_limits<long_int_t>::max();
    };

    template<class graph_t, class long_int_t>
    constexpr void min_cut_slow_eval(slow_min_cut_context<graph_t, long_int_t> &context)
    {
        assert(1U < context.graph.size() && context.froms_count <= context.graph.size());

        if (const auto no_cut = context.froms_count == 0U || context.froms_count == context.graph.size(); no_cut)
        {
            return;
        }

        long_int_t sum{};

        for (std::size_t from{}; from < context.graph.size(); ++from)
        {
            if (!context.froms[from])
            {
                continue;
            }

            const auto &edges = context.graph[from];
            assert(edges.size() == context.graph.size());

            for (std::size_t tod{}; tod < context.graph.size(); ++tod)
            {
                if (context.froms[tod])
                {
                    continue;
                }

                const auto &cost = edges[tod];
                sum += cost;

                assert(!(sum < long_int_t{}));
            }
        }

        if (sum < context.min_cost || (sum == context.min_cost && context.froms_count < context.min_chosen_count))
        {
            context.min_cost = sum;
            context.min_chosen = context.froms;
            context.min_chosen_count = context.froms_count;
        }
    }

    template<class graph_t, class long_int_t>
    constexpr void min_cut_slow_rec(slow_min_cut_context<graph_t, long_int_t> &context)
    {
        assert(1U < context.graph.size() && 0U < context.index && context.index <= context.graph.size());

        if (0U < --context.index)
        {
            min_cut_slow_rec<graph_t, long_int_t>(context);
        }
        else
        {
            min_cut_slow_eval<graph_t, long_int_t>(context);
        }

        // Add.
        ++context.froms_count;
        context.froms[context.index] = true;

        if (0U < context.index)
        {
            min_cut_slow_rec<graph_t, long_int_t>(context);
        }
        else
        {
            min_cut_slow_eval<graph_t, long_int_t>(context);
        }

        // Remove.
        --context.froms_count;
        context.froms[context.index] = false;

        ++context.index;
    }

    template<class int_t>
    [[nodiscard]] constexpr auto slow_chosen_to_index_vector(const std::vector<bool> &chosen) -> std::vector<int_t>
    {
        assert(chosen.size() < static_cast<std::uint64_t>(std::numeric_limits<int_t>::max()));

        std::vector<int_t> indexes;

        for (std::size_t index{}; index < chosen.size(); ++index)
        {
            if (chosen[index])
            {
                indexes.push_back(index);
            }
        }

        return indexes;
    }

    inline constexpr void cook_set_2(const auto &set_1, const std::size_t size, auto &set_2)
    {
        assert(1ZU < size);
        assert(!set_1.empty() && set_1.size() < size);
        assert(std::is_sorted(set_1.begin(), set_1.end()));

        const auto the_end = set_1.cend();
        auto beg = set_1.cbegin();

        set_2.clear();

        for (std::size_t column{}; column < size; ++column)
        {
            while (beg != the_end && *beg < column)
            {
                ++beg;
            }

            if (beg == the_end || column != *beg)
            {
                set_2.push_back(column);
            }
        }

        assert(!set_2.empty() && set_2.size() <= set_1.size());
    }

    template<std::signed_integral int_t>
    constexpr void merge_two_mincostedge_sets(std::vector<std::vector<int_t>> &graph, std::vector<int_t> &temp_costs,
        const std::size_t merges, std::vector<std::vector<std::size_t>> &vertex_sets,
        std::vector<std::size_t> &min_vertices, int_t &min_cost)
    {
        const auto size = graph.size();

        assert(0U < merges && size == graph.at(0).size() && size == temp_costs.size());

        [[maybe_unused]] constexpr int_t zero{};
        constexpr auto vertex_ban = std::numeric_limits<int_t>::min();

        static_assert(vertex_ban < zero); // The graph weights are non-negative.

        auto &edges_0 = graph[0];
        std::copy(edges_0.cbegin(), edges_0.cend(), temp_costs.begin());

        std::size_t sink{}; // The edge {sink, source} will have the minimum cost edge in the last merge.
        std::size_t source{}; // Both {sink, source} belong to either different or same sets of a min cut.

        for (std::size_t merg{}; merg < merges; ++merg)
        {
            temp_costs[source] = vertex_ban;
            sink = source;

            constexpr auto safe_skip_zero = 1ZU;

            source = // todo(p3): a heap is faster.
                static_cast<std::size_t>(
                    std::max_element(temp_costs.cbegin() + safe_skip_zero, temp_costs.cend()) - temp_costs.cbegin());

            assert(sink != source);

            const auto &edges = graph[source];

            for (std::size_t column{}; column < size; ++column)
            {
                const auto &edge_cost = edges[column];
                assert(!(edge_cost < zero));

                auto &cost = temp_costs[column]; // Can be negative.
                // todo(p4): skip already banned?
                cost += edge_cost;
            }
        }

        {
            const auto &source_cost = temp_costs[source];

            // It is faster to subtract self, than to check whether (source == column).
            const auto &self = graph[source][source];
            const auto candidate = static_cast<int_t>(source_cost - self);
            assert(!(candidate < zero));

            const auto &set_1 = vertex_sets[source];

            if (const auto set_1_size = set_1.size(), set_2_size = size - set_1_size,
                min_size = std::min(set_1_size, set_2_size);
                candidate < min_cost || (candidate == min_cost && min_size < min_vertices.size()))
            {
                if (set_1_size < set_2_size)
                {
                    min_vertices = set_1;
                }
                else
                {
                    cook_set_2(set_1, size, min_vertices);
                }

                assert(!min_vertices.empty() && min_vertices.size() < graph.size());

                min_cost = candidate;
            }
        }

        // Merge the cheapest edge from the source into the sink, also joining the vertex sets.
        {
            const auto &pres = graph[source];
            auto &currs = graph[sink];

            for (std::size_t column{}; column < size; ++column)
            {
                const auto &pre = pres[column];
                auto &curr = currs[column];
                curr += pre;

                // todo(p4): del? E.g. use data[0][2] instead of data[2][0].
                graph[column][sink] = curr;
            }
        }

        edges_0[source] = vertex_ban;

        {// todo(p4): dsu faster.
            auto &pre = vertex_sets[source];
            auto &curr = vertex_sets[sink];

            if (curr.capacity() < pre.capacity())
            {// The source vertex set won't be needed - can speed up.
                std::swap(curr, pre);
            }

            // Make the vertex set sorted to cook the 2nd set easily.
            ::Standard::Algorithms::Numbers::inplace_merge(pre, curr);
        }
    }

    template<std::integral int_t>
    constexpr void prepare_flow_graph(auto &source_graph, const int_t &size, auto &max_flow_graph)
    {
        constexpr int_t zero{};

        max_flow_graph.source(zero);
        max_flow_graph.clear();

        for (int_t from{}; from < size; ++from)
        {
            const auto &edges = source_graph[from];

            for (int_t tod{}; // Skip (source == zero).
                 ++tod < size;)
            {
                const auto &cost = edges[tod];
                if (zero < cost)
                {
                    max_flow_graph.add_edge(from, tod, cost);
                }
            }
        }
    }

    template<std::integral int_t>
    constexpr void compute_min_size_vertices(const std::size_t size, const auto &flow_graph, const int_t &sink,
        std::vector<std::size_t> &sta, std::vector<bool> &reachables, std::vector<std::size_t> &min_vertices)
    {
        constexpr int_t zero{};

        assert(1ZU < size && size <= flow_graph.size());

        auto reach_count = 1ZU;
        reachables.assign(size, false);
        reachables.at(zero) = true;

        sta.assign(1, zero);

        do
        {
            const auto from = sta.back();
            sta.pop_back();

            for (const auto &edges = flow_graph.at(from); const auto &edge : edges)
            {
                if (edge.flow < edge.capacity && !reachables.at(edge.to))
                {
                    reachables.at(edge.to) = true;
                    sta.push_back(edge.to);
                    ++reach_count;

                    assert(0ZU < reach_count && reach_count < size);
                }
            }
        } while (!sta.empty());

        assert(!reachables.at(sink));
        min_vertices.clear();

        const auto is_set1_smallest = reach_count < size - reach_count;

        for (std::size_t vertex{}; vertex < size; ++vertex)
        {
            if (is_set1_smallest == reachables.at(vertex))
            {
                min_vertices.push_back(vertex);
            }
        }

        assert(!min_vertices.empty() && min_vertices.size() < size);
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Given an undirected, connected, non-negative weighted graph, represented as a square matrix,
    // Stoer Wagner compute a global min cost cut (min weight cut, edge connectivity). See also max flow.
    // 0-based vertices.
    // Sum of both disjoint sets: set1 + set2 = {0, 1, .. , (n-1)}.
    // int_t must be large enough not to overflow. todo(p3): overflow checks?
    // Time O(n**3), space O(n**2).
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto min_cut_stoer_wagner( // The graph is edited.
        std::vector<std::vector<int_t>> &graph)
        -> std::pair<std::vector<std::size_t>, int_t> // set1 vertices, cut cost.
    {
        const auto size = graph.size();
        if (size <= 1U)
        {
            return {};
        }

        constexpr int_t zero{};

        if (const auto sink_iter = std::find_if(graph.cbegin(), graph.cend(),
                [size, zero](const auto &edges)
                {
                    assert(edges.size() == size);

                    auto zeroes_only = std::all_of(edges.begin(), edges.end(),
                        [zero](const auto &edge_cost) noexcept
                        {
                            assert(!(edge_cost < zero));
                            return edge_cost == zero;
                        });

                    return zeroes_only;
                });
            sink_iter != graph.cend())
        {
            auto node = static_cast<std::size_t>(sink_iter - graph.cbegin());
            return { std::vector<std::size_t>{ node }, zero };
        }

        std::vector<std::size_t> min_vertices{ 0ZU };
        auto min_cost = std::accumulate(graph[0ZU].cbegin(), graph[0ZU].cend(), zero);
        assert(zero < min_cost);

        std::vector<int_t> temp_costs(size);
        std::vector<std::vector<std::size_t>> vertex_sets(size); // dsu.

        for (std::size_t index{}; index < size; ++index)
        {
            vertex_sets[index].push_back(index);
        }

        for (auto merges = size; 0ZU < --merges;)
        {
            Inner::merge_two_mincostedge_sets<int_t>(graph, temp_costs, merges, vertex_sets, min_vertices, min_cost);
        }

        assert(!min_vertices.empty() && min_vertices.size() < graph.size()); // todo(p5): check no duplicates.
        assert(!(min_cost < zero));

        return { std::move(min_vertices), min_cost };
    }

    // Slow still.
    template<std::integral int_t, class max_flow_graph_t>
    [[nodiscard]] constexpr auto min_cut_via_flow(const std::vector<std::vector<int_t>> &graph,
        max_flow_graph_t &max_flow_graph) -> std::pair<std::vector<std::size_t>, int_t>
    {
        const auto size = max_flow_graph.size();
        assert(graph.size() == static_cast<std::size_t>(size));

        if (size <= 1)
        {
            return {};
        }

        constexpr int_t zero{};

        std::vector<std::size_t> min_vertices{ 0ZU };
        std::vector<std::size_t> temp_vertices(size - 1);
        auto min_cost = std::accumulate(graph[0ZU].cbegin(), graph[0ZU].cend(), zero);
        assert(!(min_cost < zero));

        std::vector<std::size_t> sta(size);
        std::vector<bool> reachables;

        Inner::prepare_flow_graph<int_t>(graph, size, max_flow_graph);

        for (auto sink = size - 1;;)
        {
            max_flow_graph.sink(sink);
            max_flow_graph.zero_flow();

            const auto cand = max_flow_graph.max_flow();
            assert(!(cand < zero));

            if (!(min_cost < cand))
            {
                const auto &flow_graph = max_flow_graph.graph();

                Inner::compute_min_size_vertices<int_t>(size, flow_graph, sink, sta, reachables, temp_vertices);

                if (cand < min_cost || temp_vertices.size() < min_vertices.size())
                {
                    std::swap(min_vertices, temp_vertices);
                    min_cost = cand;
                }
            }

            if (zero == min_cost || zero == --sink)
            {
                assert(!min_vertices.empty() && min_vertices.size() < graph.size()); // todo(p5): check no duplicates.

                return { std::move(min_vertices), min_cost };
            }
        }
    }

    // Time O(2**n * n * n).
    template<class long_int_t, class int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto min_cut_slow(const std::vector<std::vector<int_t>> &graph)
        -> std::pair<std::vector<std::size_t>, long_int_t>
    {
        const auto size = graph.size();
        if (size <= 1U)
        {
            return {};
        }

        if (constexpr auto max_size = 25U; max_size < size) [[unlikely]]
        {
            throw std::invalid_argument("Too large graph size " + std::to_string(size));
        }

        using graph_t = std::vector<std::vector<int_t>>;

        Inner::slow_min_cut_context<graph_t, long_int_t> context{ graph, graph.size(),
            std::vector<bool>(graph.size(), false) };

        assert(context.froms.size() == graph.size() && long_int_t{} < context.min_cost);

        Inner::min_cut_slow_rec<graph_t, long_int_t>(context);

        assert(0U < context.min_chosen_count && context.min_chosen_count < graph.size() &&
            !(context.min_cost < long_int_t{}));

        auto chosen_vertices = Inner::slow_chosen_to_index_vector<std::size_t>(context.min_chosen);

        assert(!chosen_vertices.empty() && chosen_vertices.size() < graph.size());

        return { std::move(chosen_vertices), context.min_cost };
    }
} // namespace Standard::Algorithms::Graphs
