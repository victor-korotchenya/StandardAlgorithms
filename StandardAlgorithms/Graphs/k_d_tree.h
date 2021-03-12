#pragma once
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
    template<class tuple_t>
    struct k_d_node final
    {
        tuple_t point{};
        k_d_node *left{};
        k_d_node *right{};
        // todo(p3): bool is_deleted{};
    };

    // O(size) time for many operations.
    // See also "range_tree.h".
    template<class tuple_t1, std::int32_t dimensions, class node_t1 = k_d_node<tuple_t1>>
    requires(2 <= dimensions)
    struct k_d_tree final
    {
        using tuple_t = tuple_t1;
        using node_t = node_t1;

        constexpr k_d_tree()
            : Unique_root(nullptr, free_tree<node_t>)
        {
        }

        constexpr ~k_d_tree() noexcept = default;

        // Time O(1).
        [[nodiscard]] constexpr auto size() const noexcept
        {
            return Size;
        }

        constexpr void find(const tuple_t &low, std::function<void(const node_t &)> &func, const tuple_t &high) const
        {
            if (!Unique_root)
            {
                return;
            }

            const std::pair<const tuple_t &, const tuple_t &> limits{ low, high };
            find_impl(Unique_root.get(), func, limits);
        }

        // It is assumed that a median on dim=0 is inserted first, .. , to guarantee O(log(n)) height.
        // todo(p2): impl. insert many/all.
        constexpr void insert(const tuple_t &point) noexcept(false)
        {
            if (::Standard::Algorithms::Utilities::stack_max_size <= Size) [[unlikely]]
            {
                throw std::runtime_error("todo(p2): Too many nodes in experimental k_d_tree.");
            }

            auto temp = std::make_unique<node_t>();
            temp->point = point;

            if (!Unique_root)
            {
                Unique_root.reset(temp.release());
                ++Size;
                return;
            }

            auto root = Unique_root.get();
            node_t **ppr = &root;

            for (std::int32_t dim{};; dim = next_dimention(dim))
            {
                assert(ppr != nullptr);

                auto &node = *ppr;
                if (node == nullptr)
                {
                    node = temp.release();
                    ++Size;
                    return;
                }

                const auto &key1 = point.at(dim);
                const auto &key2 = node->point.at(dim);
                ppr = key1 < key2 ? &(node->left) : &(node->right);
            }
        }

        // todo(p3): erase

        constexpr void clear()
        {
            Unique_root.reset(nullptr);
            Size = 0U;
        }

        k_d_tree(const k_d_tree &) = delete;
        auto operator= (const k_d_tree &) & -> k_d_tree & = delete;
        // todo(p3): make movable.
        k_d_tree(k_d_tree &&) noexcept = delete;
        auto operator= (k_d_tree &&) &noexcept -> k_d_tree & = delete;

private:
        inline static constexpr auto next_dimention(const std::int32_t dim)
        {
            assert(!(dim < 0) && dim < dimensions);

            return (dim == 0 ? dimensions : dim) - 1;
        }

        static constexpr void find_impl(node_t *node, std::function<void(const node_t &)> &func,
            const std::pair<const tuple_t &, const tuple_t &> limits, std::int32_t dim = 1)
        {
            for (;;)
            {
                assert(node != nullptr);
                dim = next_dimention(dim);

                if (is_between<dimensions, tuple_t>(node->point, limits))
                {
                    func(*node);
                }

                if (node->left != nullptr)
                {
                    if (node->right != nullptr)
                    {
                        find_impl(node->right, func, limits, dim);
                    }

                    if (const auto &low = limits.first; node->point[dim] < low[dim])
                    {
                        return;
                    }

                    node = node->left;
                }
                else
                {
                    if (const auto &high = limits.second; node->right == nullptr || high[dim] < node->point[dim])
                    {
                        return;
                    }

                    node = node->right;
                }
            }
        }

        /* todo(p3): static auto is_valid(node_t* root)
        -> typename std::enable_if_t<::Standard::Algorithms::is_debug, void>
        {
            if (nullptr == root)
                return true;
        } */

        std::unique_ptr<node_t, void (*)(node_t *)> Unique_root;
        std::size_t Size{};
    };
} // namespace Standard::Algorithms::Trees
