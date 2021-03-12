#pragma once
#include<ostream>

namespace Standard::Algorithms::Trees
{
    template<class value_type1>
    struct binary_tree_node final
    {
        using value_type = value_type1;

        value_type value{};
        binary_tree_node *left{};
        binary_tree_node *right{};
    };

    template<class value_type>
    [[nodiscard]] constexpr auto operator== (
        const binary_tree_node<value_type> &one, const binary_tree_node<value_type> &two) noexcept -> bool
    {
        auto result = one.value == two.value;
        return result;
    }

    template<class value_type>
    auto operator<< (std::ostream &str, const binary_tree_node<value_type> &node) -> std::ostream &
    {
        str << node.value;
        return str;
    }
} // namespace Standard::Algorithms::Trees
