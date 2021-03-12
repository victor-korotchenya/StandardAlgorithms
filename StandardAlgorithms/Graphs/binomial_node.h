#pragma once
#include"../Utilities/zu_string.h"
#include<cstddef>
#include<string>

namespace Standard::Algorithms::Heaps
{
    // todo(p3): Decrease key properly, no key swaps, in O(log(n)).
    template<class key_t>
    struct binomial_node final
    {
        key_t key{};
        std::size_t degree{};
        binomial_node *parent{};
        binomial_node *child{};
        binomial_node *left{};

        // Roots: Increasing degrees to make the merge easier.
        // Non roots: Decreasing degrees to append a larger degree in O(1).
        binomial_node *right{};
    };

    template<class key_t>
    [[nodiscard]] constexpr auto node_to_string(const Standard::Algorithms::Heaps::binomial_node<key_t> &node)
        -> std::string
    {
        return ::Standard::Algorithms::Utilities::zu_string(node.key)
            .append(" (")
            .append(::Standard::Algorithms::Utilities::zu_string(node.degree))
            .append(")");
    }
} // namespace Standard::Algorithms::Heaps
