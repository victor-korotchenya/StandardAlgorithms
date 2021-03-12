#pragma once
#include"../Utilities/require_utilities.h"
#include<queue>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    // Bipartite unweighted graph maximum matching due to Karzanov, Hopcroft, Karp.
    // Similar to Dinic's flow.
    // The vertices are 1-based.
    // Can use a stack of size of the left part size.
    // Time O(n**2.5).
    template<std::unsigned_integral int_t1, class edge_t1 = std::pair<int_t1, int_t1>>
    struct max_2c_matching_karzanov_hopcroft_karp final
    {
        using int_t = int_t1;
        using edge_t = edge_t1;

        constexpr explicit max_2c_matching_karzanov_hopcroft_karp(const std::pair<int_t, int_t> &left_right_part_sizes)
            : Left_neighbors(require_positive(
                  require_positive(left_right_part_sizes.first, "left part size") + 1, "left part size + 1"))
            , Right_matches(require_positive(
                  require_positive(left_right_part_sizes.second, "right part size") + 1, "right part size + 1"))
            , Left_distances(left_right_part_sizes.first + 1)
        {
        }

        // The graph must be simple.
        // The right part neighbors are not stored at all.
        constexpr void add_edge(const std::pair<int_t, int_t> &from_to)
        {
            const auto &[left_from, right_to] = from_to;

            assert(fake_vertex < left_from && left_from < Left_neighbors.size());
            assert(fake_vertex < right_to && right_to < Right_matches.size());

            auto &edges = Left_neighbors.at(left_from);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                const auto ite = std::find(edges.cbegin(), edges.cend(), right_to);
                assert(ite == edges.cend()); // No multi-edges.
            }

            edges.push_back(right_to);
        }

        constexpr void max_matching(std::vector<edge_t> &matchings)
        {
            Left_matches.assign(Left_neighbors.size(), fake_vertex);
            Right_matches.assign(Right_matches.size(), fake_vertex);

            [[maybe_unused]] std::size_t matching_size{};

            while (compute_distances_find_augmenting_path_bfs())
            {
                [[maybe_unused]] bool has_any_matching{};

                for (int_t from = 1; from < Left_neighbors.size(); ++from)
                {
                    if (is_from_matched(from))
                    {
                        continue;
                    }

                    if (has_augmenting_path_dfs(from))
                    {
                        if constexpr (::Standard::Algorithms::is_debug)
                        {
                            ++matching_size;
                            has_any_matching = true;

                            assert(0ZU < matching_size);
                        }
                    }
                }

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(has_any_matching);
                }
            }

            // The restoration can be skipped if only the matching_size is needed.
            restore_matchings(matchings);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(matching_size == matchings.size());
            }
        }

private:
        [[nodiscard]] constexpr auto is_from_matched(const int_t &from) const -> bool
        {
            assert(from < Left_matches.size());

            const auto &right_to = Left_matches.at(from);
            return right_to != fake_vertex;
        }

        constexpr void reset_left_distances_enque_unmatched()
        {
            while (!From_queue.empty())
            {
                From_queue.pop();
            }

            Left_distances.at(fake_vertex) = infinite_distance;

            for (int_t from = 1; from < Left_neighbors.size(); ++from)
            {
                auto &dist = Left_distances[from];

                if (is_from_matched(from))
                {
                    dist = infinite_distance;
                }
                else
                {// A free vertex.
                    dist = {};
                    From_queue.push(from);
                }
            }
        }

        [[nodiscard]] constexpr auto compute_distances_find_augmenting_path_bfs() -> bool
        {
            reset_left_distances_enque_unmatched();

            const auto &shortest_augmenting_path_dist = Left_distances[fake_vertex];

            while (!From_queue.empty())
            {
                const auto from_free_vertex = From_queue.front();
                From_queue.pop();

                const auto &left_dist = Left_distances[from_free_vertex];

                if (!(left_dist < shortest_augmenting_path_dist))
                {
                    continue;
                }

                for (const auto &rights = Left_neighbors[from_free_vertex]; const auto &right_to : rights)
                {
                    assert(right_to < Right_matches.size());

                    // from_free_vertex -> right_to -> match_from
                    const auto &match_from = Right_matches[right_to];
                    assert(match_from < Left_neighbors.size());

                    auto &match_dist = Left_distances[match_from];

                    if (match_dist == infinite_distance)
                    {
                        match_dist = left_dist + 1;
                        From_queue.push(match_from);
                    }
                }
            }

            auto are_augmenting_paths = shortest_augmenting_path_dist != infinite_distance;
            return are_augmenting_paths;
        }

        [[nodiscard]] constexpr auto has_augmenting_path_dfs(const int_t from) -> bool
        {
            assert(fake_vertex < from && from < Left_distances.size());

            auto &from_dist = Left_distances[from];

            for (const auto &rights = Left_neighbors[from]; const auto &right_to : rights)
            {
                // from -> right_to -> match_from
                auto &match_from = Right_matches[right_to];
                const auto &match_dist = Left_distances[match_from];

                if (from_dist + 1 != match_dist)
                {
                    continue;
                }

                if (match_from == fake_vertex || has_augmenting_path_dfs(match_from))
                {
                    match_from = from;
                    Left_matches[from] = right_to;
                    return true;
                }
            }

            // No path in a DAG - ban the vertex.
            from_dist = infinite_distance;
            return false;
        }

        constexpr void restore_matchings(std::vector<edge_t> &matchings) const
        {
            matchings.clear();

            for (int_t from = 1; from < Left_neighbors.size(); ++from)
            {
                const auto &right_to = Left_matches.at(from);
                if (right_to != fake_vertex)
                {
                    assert(right_to < Right_matches.size() && Right_matches.at(right_to) == from);

                    matchings.emplace_back(from, right_to);
                }
            }
        }

        static constexpr int_t fake_vertex{};
        static constexpr auto infinite_distance = std::numeric_limits<int_t>::max();

        static_assert(int_t{} < infinite_distance);

        std::vector<std::vector<int_t>> Left_neighbors;

        // For a left part vertex, has a matching right part vertex, or fake_vertex if no match.
        std::vector<int_t> Left_matches{};
        std::vector<int_t> Right_matches;

        std::vector<int_t> Left_distances;
        std::queue<int_t> From_queue{};
    };
} // namespace Standard::Algorithms::Graphs
