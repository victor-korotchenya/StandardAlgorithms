#pragma once
#include"../Utilities/zu_string.h"
#include<cstddef>
#include<string>

namespace Standard::Algorithms::Heaps
{
    template<class key_t>
    struct fib_node final
    {
#if _DEBUG
        key_t key{};
        std::size_t children_count{};
        fib_node *parent{};
        fib_node *child{};
        fib_node *left{};
        fib_node *right{};
#else
        std::size_t children_count{};
        fib_node *parent{};
        fib_node *child{};
        fib_node *left{};
        fib_node *right{};
        key_t key{};
#endif
        bool marked{};
    };

    template<class key_t>
    [[nodiscard]] constexpr auto node_to_string(const ::Standard::Algorithms::Heaps::fib_node<key_t> &node) -> std::string
    {
        return ::Standard::Algorithms::Utilities::zu_string(node.key)
            .append(" (")
            .append(::Standard::Algorithms::Utilities::zu_string(node.children_count))
            .append(")");
    }
} // namespace Standard::Algorithms::Heaps
