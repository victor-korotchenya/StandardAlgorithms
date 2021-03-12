#pragma once
#include <vector>
#include "../disable_constructors.h"
#include "graph.h"

// Graph girth is the length of the shortest cycle, or infinity otherwise.
class Graph_Girth final
{
    Graph_Girth() = delete;

    static const size_t NoParent = 0 - size_t(1);

public:

    //0 means there is no girth or no cycle.
    //TODO: p3. The first vertex in "cycleVertices" will be the minimum one.
    unsigned long long static FindGirthUnweighted(
        const Privet::Algorithms::Graph<int, unsigned int>& g,
        std::vector<size_t>& cycleVertices);

private:

    static inline void FillParents(
        const std::vector<size_t>& parents,
        const size_t startVertex,
        std::vector<size_t>& data);

    static inline void AppendReversedButLast(
        const std::vector<size_t>& parentsTemp,
        std::vector<size_t>& cycleVertices);
};

void FindGirthTests();