#pragma once
#include"../Utilities/project_constants.h"
#include<algorithm>
#include<stdexcept>
#include<string>
#include<utility>

namespace Standard::Algorithms::Trees
{
    // Can be used as a locator because, once assigned, the key sticks to a node and never changes.
    template<class coord_t1, class key_t1 = std::pair<coord_t1, coord_t1>>
    requires(std::is_default_constructible_v<coord_t1> && std::is_default_constructible_v<key_t1>)
    struct interval_node final
    {
        using coord_t = coord_t1;
        using key_t = key_t1;

        key_t key{};

        // Should be O(log(count)).
        std::int32_t height = 1;

        std::size_t count = 1U;
        interval_node *left{};
        interval_node *right{};
        coord_t subtree_max{}; // Including this node.
    };

    [[nodiscard]] constexpr auto is_valid_interval(const auto &interval) noexcept -> bool
    {
        auto valid = !(interval.second < interval.first);
        return valid;
    }

    constexpr void require_valid_interval(const auto &interval)
    {
        if (!is_valid_interval(interval)) [[unlikely]]
        {
            auto err = "The interval [" + std::to_string(interval.first) + ", " + std::to_string(interval.second) +
                "] is invalid.";

            throw std::runtime_error(err);
        }
    }

    template<class coord_t, class key_t>
    constexpr void avl_update_height_int(interval_node<coord_t, key_t> &node)
    {
        assert(is_valid_interval(node.key));

        node.height =
            1 + std::max(node.left != nullptr ? node.left->height : 0, node.right != nullptr ? node.right->height : 0);

        node.count = static_cast<std::size_t>(1) + (node.left != nullptr ? node.left->count : 0U) +
            (node.right != nullptr ? node.right->count : 0U);

        assert(0 < node.height && node.height <= Standard::Algorithms::Utilities::stack_max_size && 0U < node.count);

        const auto &curmax = node.key.second;
        assert(!(curmax < node.key.first));

        node.subtree_max = // Might throw.
            std::max({ curmax, node.left != nullptr ? node.left->subtree_max : curmax,
                node.right != nullptr ? node.right->subtree_max : curmax });
    }

    [[nodiscard]] constexpr auto are_intervals_overlapping(const auto &one, const auto &two) noexcept -> bool
    {
        assert(is_valid_interval(one));
        assert(is_valid_interval(two));

        const auto &begin1 = std::max(one.first, two.first);
        const auto &end1 = std::min(one.second, two.second);

        auto crossing = !(end1 < begin1);
        return crossing;
    }
} // namespace Standard::Algorithms::Trees
