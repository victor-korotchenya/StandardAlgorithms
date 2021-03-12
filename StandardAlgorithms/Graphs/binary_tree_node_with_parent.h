#pragma once

namespace Standard::Algorithms::Trees
{
    template<class value_type1>
    struct binary_tree_node_with_parent final
    {
        using value_type = value_type1;

        value_type value{};

        binary_tree_node_with_parent *left{};
        binary_tree_node_with_parent *right{};
        binary_tree_node_with_parent *parent{};
    };
} // namespace Standard::Algorithms::Trees
