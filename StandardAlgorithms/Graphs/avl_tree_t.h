#pragma once
#include <memory>
#include "avl_node_t.h"

namespace Privet::Algorithms::Trees
{
    // AVL tree subtree heights can differ by at most 1.
    // Duplicates are not allowed.
    // AVL has faster selects than red-black does.
    template<class value_t, class node_t = avl_node_t<value_t>>
    struct avl_tree_t final
    {
        avl_tree_t() : _unique_root(nullptr, delete_tree)
        {}

        // found = (result.first && result.first->key == key)
        std::pair<const node_t*, int> lower_bound(const value_t& key) const
        {
            auto root = _unique_root.get();
            if (!root)
                return {};

            auto ordinal = 0;
            for (;;)
            {
                if (key < root->key)
                {
                    if (!root->left)
                        break;
                    root = root->left;
                    continue;
                }

                if (root->key < key)
                {
                    ordinal += (root->left ? root->left->count : 0) + 1;
                    if (!root->right)
                    {
                        ++ordinal;
                        break;
                    }

                    root = root->right;
                    continue;
                }

                assert(root->key == key);
                ordinal += (root->left ? root->left->count : 0) + 1;
                break;
            }

            ordinal = ordinal > 0 ? ordinal - (root->key <= key) : 0;
            return { root, ordinal };
        }

        const node_t* select(int rank) const
        {
            auto root = _unique_root.get();
            ++rank;
            for (;;)
            {
                assert(root && rank > 0);
                if (!root)
                    return nullptr;

                const auto lc = (root->left ? root->left->count : 0) + 1;
                if (rank < lc)
                {
                    assert(root->left);
                    root = root->left;
                    continue;
                }

                if (rank > lc)
                {
                    assert(lc > 0 && root->right);
                    rank -= lc;
                    root = root->right;
                    continue;
                }

                assert(rank == lc);
                return root;
            }
        }

        std::pair<const node_t*, bool> insert(const value_t& key)
        {
            auto root = _unique_root.get();
#ifdef _DEBUG
            assert(is_valid(root));
#endif
            auto root2 = insert(root, key);

            assert(root2.first && (!root2.second || root2.second->key == key));
            if (root != root2.first)
            {
                _unique_root.release();
                _unique_root.reset(root2.first);
            }

#ifdef _DEBUG
            assert(is_valid(root2.first));
#endif
            return { root2.second ? root2.second : root2.first, root2.second };
        }

        bool erase(const value_t& key)
        {
            auto root = _unique_root.get();
#ifdef _DEBUG
            assert(is_valid(root));
#endif
            auto root2 = erase(root, key);
            assert(!root2.second || root2.second && root2.second->key == key);
            if (root != root2.first)
            {
                _unique_root.release();
                _unique_root.reset(root2.first);
            }

            delete root2.second;
#ifdef _DEBUG
            assert(is_valid(root2.first));
#endif
            return root2.second;
        }

        void clear()
        {
            _unique_root.reset(nullptr);
        }

    private:
        std::unique_ptr<node_t, void(*)(node_t*)> _unique_root;

        avl_tree_t* operator=(avl_tree_t&) = delete;
        avl_tree_t* operator=(avl_tree_t&&) = delete;
        avl_tree_t(avl_tree_t&) = delete;
        avl_tree_t(avl_tree_t&&) = delete;

        static void delete_tree(node_t* node)
        {
            if (!node)
                return;

            delete_tree(node->left);
            delete_tree(node->right);

            delete node;
        }

        static std::pair<node_t*, node_t*> insert(node_t* node, const value_t& key)
        {
            if (!node)
            {
                auto t = new node_t(key);
                return { t, t };
            }

            node_t** t;
            if (key < node->key)
                t = &(node->left);
            else if (node->key < key)
                t = &(node->right);
            else
                return { node, nullptr };

            auto ins = insert(*t, key);
            if (!ins.second)
                return { node, nullptr };

            *t = ins.first;

            const auto balance = node->get_balance();
            if (balance < -1)
            {
                if (key < node->left->key)
                    return { right_rotate(node), ins.second };

                assert(node->left->key < key);
                node->left = left_rotate(node->left);
                return { right_rotate(node), ins.second };
            }

            if (balance > 1)
            {
                if (node->right->key < key)
                    return { left_rotate(node), ins.second };

                assert(key < node->right->key);
                node->right = right_rotate(node->right);
                return { left_rotate(node), ins.second };
            }

            node->update_height();
            return { node, ins.second };
        }

        static std::pair<node_t*, node_t*> erase(node_t* node, const value_t& key)
        {
            if (!node)
                return { nullptr, nullptr };

            node_t* del;
            if (key < node->key)
            {
                auto p = erase(node->left, key);
                if (!p.second)
                    return { node, nullptr };

                node->left = p.first;
                del = p.second;
            }
            else if (node->key < key)
            {
                auto p = erase(node->right, key);
                if (!p.second)
                    return { node, nullptr };

                node->right = p.first;
                del = p.second;
            }
            else
            {
                if (node->left && node->right)
                {
                    auto next_larger = node->right;
                    while (next_larger->left)
                        next_larger = next_larger->left;

                    std::swap(node->key, next_larger->key);

                    auto p = erase(node->right, next_larger->key);
                    assert(p.second);
                    node->right = p.first;
                    del = p.second;
                }
                else
                {
                    auto temp = node->left ? node->left : node->right;
                    if (!temp) //leaf
                        return { nullptr, node };

                    del = node;
                    node = temp;
                }
            }

            const auto balance = node->get_balance();
            if (balance < -1)
            {
                const auto balance2 = node->left->get_balance();
                if (balance2 <= 0)
                    return { right_rotate(node), del };

                node->left = left_rotate(node->left);
                return { right_rotate(node), del };
            }

            if (balance > 1)
            {
                const auto balance2 = node->right->get_balance();
                if (balance2 >= 0)
                    return { left_rotate(node), del };

                node->right = right_rotate(node->right);
                return { left_rotate(node), del };
            }

            node->update_height();
            return { node, del };
        }

        static node_t* left_rotate(node_t* x)
        {
            assert(x && x->right);
            auto z = x->right;
            auto y = z->left;

            z->left = x;
            x->right = y;

            x->update_height();
            z->update_height();
            return z;
        }

        static node_t* right_rotate(node_t* z)
        {
            assert(z && z->left);
            //       z6             x4
            //       / \            /  \
                    //     x4   7          2   z6
                    //     / \       -->  / \  / \
                    //    2  y5          1   3 y5 7
                    //   / \
                    //  1   3
            auto x = z->left;
            auto y = x->right;

            x->right = z;
            z->left = y;

            z->update_height();
            x->update_height();
            return x;
        }

#ifdef _DEBUG
        static bool is_valid(node_t* root)
        {
            if (nullptr == root)
                return true;

            auto min_node = root;
            while (min_node->left)
                min_node = min_node->left;

            auto max_node = root;
            while (max_node->right)
                max_node = max_node->right;

            assert(min_node->key <= max_node->key);
            const auto iv = is_valid(min_node, root, max_node);
            return iv;
        }

        static bool is_valid(node_t* min_node, node_t* node, node_t* max_node)
        {
            assert(min_node && node && max_node);
            const auto& min = min_node->key;
            const auto& cur = node->key;
            const auto& max = max_node->key;
            if (!(min < cur))
            {
                if (node != min_node)
                    return false;
            }

            if (!(cur < max))
            {
                if (node != max_node)
                    return false;
            }

            const auto is_left = nullptr == node->left
                || is_valid(min_node, node->left, node);
            if (!is_left)
                return false;

            const auto r = nullptr == node->right || is_valid(node, node->right, max_node);
            return r;
        }
#endif
    };
}