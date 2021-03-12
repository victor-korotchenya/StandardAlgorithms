#pragma once
/*
#include"binary_tree_utilities.h" // free_tree
#include"multiway_point.h"
#include<cassert>
#include<functional>
#include<gsl/gsl>
#include<memory>
#include<stdexcept>
#include<utility>

namespace Standard::Algorithms::Trees
{
    // todo(p1): impl.


    template<class tuple_t>
    struct range_node final
    {
        tuple_t point{};
        range_node *left{};
        range_node* right{};
    };

    // Faster search than that in "k_d_tree.h".
    template<class tuple_t1, std::int32_t dimensions, class node_t1 = range_node<tuple_t1>>
    requires(2 <= dimensions)
    struct range_tree final
    {
        using tuple_t = tuple_t1;
        using node_t = node_t1;

        constexpr range_tree() : Unique_root(nullptr, free_tree<node_t>)
        {
        }

        constexpr ~range_tree() noexcept = default;

        // Time O(1).
        [[nodiscard]] constexpr auto size() const noexcept
        {
            return Size;
        }

        constexpr void find(const tuple_t &low,
            std::function<void(const node_t&)> &func,
            const tuple_t &high) const
        {
        }

        // todo(p1): impl [[nodiscard]] count() -> std::size_t

        // todo(p1): impl nearest_neighbour() -> node_t*

       // todo(p1): impl k_nearest_neighbours()

        constexpr void insert(const tuple_t &point) noexcept(false)
        {
        }

        // todo(p3): erase

        constexpr void clear()
        {
            Unique_root.reset(nullptr);
            Size = 0U;
        }

        range_tree(const range_tree&) = delete;
        auto operator= (const range_tree&) & -> range_tree& = delete;
        //todo(p3): make movable.
        range_tree(range_tree&&) noexcept = delete;
        auto operator= (range_tree&&) & noexcept -> range_tree& = delete;

private:
        std::unique_ptr<node_t, void(*)(node_t*)> Unique_root;
        std::size_t Size{};
    };
} // namespace Standard::Algorithms::Trees
*/
