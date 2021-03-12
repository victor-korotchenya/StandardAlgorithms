#pragma once
#include"avl_node.h"
// The 'node' must come before the 'tree'.
#include"avl_tree_base.h"

namespace Standard::Algorithms::Trees
{
    // Duplicates are not allowed.
    template<class key_t, class node_t = avl_node<key_t>>
    using avl_tree = avl_tree_base<key_t, node_t>;
} // namespace Standard::Algorithms::Trees
