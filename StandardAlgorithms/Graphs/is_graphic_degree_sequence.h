#pragma once
#include <algorithm>
#include <cassert>
#include <vector>

namespace
{
    template <typename number_t>
    std::vector<std::vector<number_t>> check_input(const std::vector<number_t>& degree_sorted_desc)
    {
        const auto size = static_cast<number_t>(degree_sorted_desc.size());
        if (static_cast<size_t>(size) != degree_sorted_desc.size()) return {}; // overflow.

        std::vector<std::vector<number_t>> degree_list(size);

        std::int64_t degree_sum = 0;
        auto id = static_cast<number_t>(0);
        auto previous_degree = degree_sorted_desc.front();

        for (const auto& d : degree_sorted_desc)
        {
            if (previous_degree < d || size <= d || d < 0) return {};
            degree_sum += d, previous_degree = d;
            if (0 != d) degree_list[d].push_back(id++);
        }

        if (degree_sum & 1) return {};
        return degree_list;
    }

    template <typename number_t>
    bool match_vertices(std::vector<std::vector<number_t>>& graph, std::vector<std::vector<number_t>>& degree_list, const number_t first_deg, const number_t u, std::vector<std::pair<number_t, number_t>>& buf)
    {
        buf.clear();
        auto deg = first_deg;
        auto left = first_deg;

        while (0 < deg)
        {
            auto& list = degree_list[deg];
            while (list.size())
            {
                const auto v = list.back();
                list.pop_back();
                assert(u != v);
                graph[u].push_back(v);
                graph[v].push_back(u);

                if (deg - 1) buf.push_back(std::make_pair(v, deg - 1));

                if (0 == --left)
                {
                    for (const auto& p : buf)
                    {
                        degree_list[p.second].push_back(p.first);
                    }

                    return true;
                }
            }

            --deg;
        }

        assert(0 < left);
        return false;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            // Havel–Hakimi algorithm.
            template <typename number_t>
            bool is_graphic_degree_sequence(const std::vector<number_t>& degree_sorted_desc, std::vector<std::vector<number_t>>& graph)
            {
                graph.clear();
                if (degree_sorted_desc.empty()) return true;

                const auto size = static_cast<number_t>(degree_sorted_desc.size());
                if (static_cast<size_t>(size) != degree_sorted_desc.size()) return false; // overflow.

                graph.resize(size);
                auto degree_list = check_input(degree_sorted_desc);
                if (degree_list.empty()) return false;

                std::vector<std::pair<number_t, number_t>> buf;
                for (number_t deg = size - 1; 0 < deg; --deg)
                {
                    auto& list = degree_list[deg];
                    while (list.size())
                    {
                        const auto u = list.back();
                        list.pop_back();
                        if (!match_vertices<number_t>(graph, degree_list, deg, u, buf)) return false;
                    }
                }

                return true;
            }
        }
    }
}
