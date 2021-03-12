#pragma once
#include"../Utilities/print_utilities.h"
#include"interval_node.h"
#include<cstdint>
#include<string>
#include<utility>

namespace std
{
    // todo(p2): del it.
    [[nodiscard]] auto to_string(const std::pair<std::int16_t, std::int16_t> &key) -> std::string;

    // todo(p2): del it.
    auto operator<< (std::ostream &str, const std::pair<std::int16_t, std::int16_t> &par) -> std::ostream &;
} // namespace std

namespace Standard::Algorithms::Trees
{
    // todo(p2): del it.
    constexpr void avl_update_height(interval_node<std::int16_t> &node);
} // namespace Standard::Algorithms::Trees

namespace Standard::Algorithms::Trees::Tests
{
    void interval_tree_tests();
}
