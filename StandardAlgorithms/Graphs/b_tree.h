#pragma once
#include"b_tree_node.h"
#include<limits>

namespace Standard::Algorithms::Trees
{
    // B-tree is a multi-way search tree, that can be used in external memory apps.
    // Since B-tree is half-full, the tree height is logarithmic, thus the tree itself is well balanced.
    // Key duplicates are allowed here.
    // Each leaf has the same distance to the root.
    // The root node is either null, or has [1 .. min_degree*2-1] keys.
    // Non-root node key size is [min_degree-1 .. min_degree*2-1], and children size is 1 larger.
    template<class key_t1, std::unsigned_integral int_t1, class node_t1 = b_tree_node<key_t1, int_t1>>
    requires(sizeof(int_t1) <= sizeof(std::uint32_t))
    struct b_tree final
    {
        using key_t = key_t1;
        using int_t = int_t1;
        using node_t = node_t1;

        static constexpr int_t lowest_min_degree = node_t::lowest_min_degree;
        static constexpr int_t highest_min_degree = std::numeric_limits<int_t>::max() / 2 - 2;

        static_assert(1U < lowest_min_degree && lowest_min_degree <= highest_min_degree);

        constexpr explicit b_tree(int_t min_degree)
            : Unique_root(nullptr, free_b_tree<key_t, int_t, node_t>)
            , Min_degree(require_less_equal(
                  require_greater(min_degree, lowest_min_degree - 1U, "min degree"), highest_min_degree, "min degree"))
        {
        }

        b_tree(const b_tree &) = delete;
        auto operator= (const b_tree &) & -> b_tree & = delete;
        b_tree(b_tree &&) noexcept = delete;
        auto operator= (b_tree &&) &noexcept -> b_tree & = delete;

        constexpr ~b_tree() noexcept = default;

        [[nodiscard]] constexpr auto size() const noexcept
        {
            return Size;
        }

        [[nodiscard]] constexpr auto contains(const key_t &key) const &noexcept -> bool
        {
            const auto *const node = find(key);
            return node != nullptr;
        }

        // Insert at the leaf level. Move the medium key to the parent node when splitting a full node.
        constexpr void insert(key_t &&key) &
        {
            auto *root = Unique_root.get();
            assert((root == nullptr) == (Size == 0U));

            if (root == nullptr)
            {
                auto unique2 = std::make_unique<node_t>(Min_degree);
                root = unique2.get();
                root->keys().at(0) = std::move(key);
                root->key_count() = 1;
                Unique_root.reset(unique2.release());
                ++Size;
                return;
            }

            if (root->is_full(Min_degree))
            {// Split to make a single pass in order to have fewer disk ops.
                auto parent = std::make_unique<node_t>(Min_degree);
                parent->children().at(0) = root;

                parent->split_full_child(Min_degree, 0);
                root = parent.get();

                assert(root != nullptr && !root->is_full(Min_degree) && root->children()[0] && root->children()[1] &&
                    root->children()[0] != root->children()[1]);

                Unique_root.release();
                Unique_root.reset(parent.release());
            }

            root->insert_into_not_full(Min_degree, std::move(key));
            ++Size;
        }

        [[maybe_unused]] constexpr auto erase(const key_t &key) & -> bool
        {
            auto *root = Unique_root.get();
            if (root == nullptr)
            {
                assert(Size == 0U);
                return false;
            }

            assert(0U < Size);

            const auto erased = root->erase(Min_degree, key);

            if (root->key_count() == 0U)
            {// todo(p3): There might be unnecessary movements even if nothing's been erased.
                {
                    [[maybe_unused]] const auto &child_1 = root->children()[1];
                    assert(child_1 == nullptr);
                }

                auto &child = root->children()[0];
                auto *root2 = child;
                child = nullptr;
                Unique_root.reset(root2);
            }

            Size -= erased ? 1U : 0U;
            return erased;
        }

        constexpr void clear() &noexcept
        {
            Unique_root.reset(nullptr);
            Size = 0U;
        }

        constexpr void validate() const &
        {
            const auto *const root = Unique_root.get();
            if (root == nullptr)
            {
                if (Size == 0U) [[likely]]
                {
                    return;
                }

                throw std::runtime_error(std::string("B tree has no root, but ") + std::to_string(Size) + " size.");
            }

            const auto actual_count =
                validate_b_tree_return_size<key_t, int_t, node_t>(gsl::make_strict_not_null(root));

            if (Size != actual_count) [[unlikely]]
            {
                throw std::runtime_error("B tree has " + std::to_string(Size) + " keys, but the actual count is " +
                    std::to_string(actual_count) + ".");
            }
        }

private:
        [[nodiscard]] constexpr auto find(const key_t &key) const &noexcept -> const node_t *
        {
            const auto *const root = Unique_root.get();
            const auto *const node = root == nullptr ? nullptr : root->find(key);
            return node;
        }

        std::unique_ptr<node_t, void (*)(node_t *)> Unique_root;
        std::size_t Size{};
        const int_t Min_degree;
    };
} // namespace Standard::Algorithms::Trees
