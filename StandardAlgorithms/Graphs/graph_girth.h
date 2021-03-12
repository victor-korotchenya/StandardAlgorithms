#pragma once
#include"graph.h"

namespace Standard::Algorithms::Graphs
{
    // Graph girth is the length of the shortest cycle, or infinity otherwise.
    struct graph_girth final
    {
        graph_girth() = delete;

        // 0 means there is no girth or no cycle.
        // todo(p5): The first vertex in "cycleVertices" should be the minimum one.
        [[nodiscard]] static auto find_girth_unweighted(
            const graph<std::int32_t, std::uint32_t> &graf, std::vector<std::size_t> &cycle_vertices) -> std::uint64_t;
    };
} // namespace Standard::Algorithms::Graphs
