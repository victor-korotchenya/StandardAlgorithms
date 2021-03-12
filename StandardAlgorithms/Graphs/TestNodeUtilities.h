#pragma once
#include <string>
#include <vector>
#include "BinaryTreeNode.h"
#include "../Utilities/StreamUtilities.h"
#include "../Assert.h"

namespace Privet::Algorithms::Trees
{
    template <typename Node, typename Number>
    Node* SetValueGetNode(std::vector<Node>& nodes, const Number& id)
    {
        if (nodes.size() <= id)
        {
            std::ostringstream ss;
            ss << "Error: nodes.size=" << nodes.size()
                << " <= id=" << id << ".";
            StreamUtilities::throw_exception(ss);
        }

        Node* result = &nodes[id];

        Assert::AreEqual<Number>(Number(0), result->Value,
            "result->Value is already set to " + std::to_string(result->Value)
            + " for id=" + std::to_string(id));

        result->Value = id;
        return result;
    }

    template <typename Pair>
    void RequireIsomorphic(const Pair& p, const std::string& message = "")
    {
        const auto equal = nullptr == p.first && nullptr == p.second;
        if (equal)
            return;

        std::ostringstream ss;

        auto PrintNode = [&ss](auto node)
        {
            if (nullptr == node)
                ss << "null";
            else
                ss << (*node);
        };

        ss << " Trees are not equal, not isomorphic: first node='";
        PrintNode(p.first);

        ss << "', second node='";
        PrintNode(p.second);

        ss << "'.";

        StreamUtilities::throw_exception(ss, message);
    }
}