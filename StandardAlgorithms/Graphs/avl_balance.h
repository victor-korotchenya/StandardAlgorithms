#pragma once
#include<cassert>
#include<cstdint>

namespace Standard::Algorithms::Trees
{
    [[nodiscard]] constexpr auto avl_balance(const auto &node) noexcept -> std::int32_t
    {
        auto bal = (node.right != nullptr ? node.right->height : 0) - (node.left != nullptr ? node.left->height : 0);

        {
            constexpr auto min_edge = -2;
            constexpr auto max_edge = 2;

            assert(min_edge <= bal && bal <= max_edge);
        }

        return bal;
    }
} // namespace Standard::Algorithms::Trees
