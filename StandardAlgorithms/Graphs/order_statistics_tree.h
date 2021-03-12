#pragma once
#include <cassert>
#include <functional>
#include <stdexcept>
// todo: p1. end
namespace Privet::Algorithms::Trees
{
    /// order statistics tree
    template <typename t, typename less_t = std::less<t>>
    class order_statistics_tree final
    {
        struct node_t final
        {
            node_t* left = nullptr, * right = nullptr, * parent;
            t value;
            int size = 1;

            node_t(node_t* parent, t value) : parent(parent), value(value)
            {
            }

            ~node_t()
            {
                delete left;
                delete right;
            }
        };

        less_t less;
        node_t* root = nullptr;

        //TODO: p3. implement copy constructor.
        order_statistics_tree(const order_statistics_tree&) = delete;

        //TODO: p3. implement assignment constructor.
        order_statistics_tree& operator =(const order_statistics_tree&) = delete;

        //TODO: p3. implement move constructor.
        order_statistics_tree(const order_statistics_tree&&) = delete;

        //TODO: p3. implement move assignment constructor.
        order_statistics_tree& operator =(const order_statistics_tree&&) = delete;

    public:
        explicit order_statistics_tree(less_t less = {})
            : less(less)
        {
        }

        ~order_statistics_tree()
        {
            delete root;
            root = nullptr;
        }

        int size() const noexcept
        {
            return root == nullptr ? 0 : root->size;
        }

        // todo: return rank?
        bool insert(const t& v)
        {
            if (root == nullptr)
            {
                root = new node_t(nullptr, v);
                return true;
            }

            auto node = root;
            for (;;)
            {
                if (less(v, node->value))
                {
                    if (node->left == nullptr)
                    {
                        node->left = new node_t(node, v);
                        break;
                    }

                    node = node->left;
                    continue;
                }

                if (!less(node->value, v))
                    return false;

                if (node->right == nullptr)
                {
                    node->right = new node_t(node, v);
                    break;
                }

                node = node->right;
            }

            do
            {
                ++(node->size);
                // todo: balance.
                node = node->parent;
            } while (node);

            return true;
        }

        bool erase(const t& v)
        {
            auto node = root;
            for (;;)
            {
                if (node == nullptr)
                    return false;

                if (less(v, node->value))
                {
                    node = node->left;
                }
                else if (less(node->value, v))
                {
                    node = node->right;
                }
                else
                    break;
            }

            assert(node);
            if (root == node)
            {
                if (!root->left)
                {
                    node = root->right;
                    root->right = node->parent = nullptr;
                    delete root;
                    root = node;
                    return true;
                }

                if (!root->right)
                {
                    node = root->left;
                    root->left = node->parent = nullptr;
                    delete root;
                    root = node;
                    return true;
                }

                // del root, with 2 children.
                node_t* op;
                int delta;
                if (node->left->size <= node->right->size)
                {
                    op = node = node->right;
                    delta = root->left->size;
                    while (op->left)
                        op = op->left;

                    op->left = root->left;
                    op->left->parent = op;
                }
                else
                {
                    op = node = node->left;
                    delta = root->right->size;
                    while (op->right)
                        op = op->right;

                    op->right = root->right;
                    op->right->parent = op;
                }

                root->left = root->right = node->parent = nullptr;
                delete root;
                root = node;

                assert(op && delta > 0);
                do
                {
                    // todo: balance.
                    op->size += delta;
                    op = op->parent;
                } while (op);
                return true;
            }

            // erase non root.
            assert(node->parent);
            if (node->left == nullptr)
            {
                if (node->right == nullptr)
                {
                    auto p = node->parent;
                    if (p->left == node)
                        p->left = nullptr;
                    else
                    {
                        assert(p->right == node);
                        p->right = nullptr;
                    }

                    do
                    {
                        --(p->size);
                        p = p->parent;
                    } while (p);

                    delete node;
                    return true;
                }

                {// right only
                    auto p = node->parent;
                    if (p->left == node)
                    {
                        p->left = node->right;
                    }
                    else
                    {
                        assert(p->right == node);
                        p->right = node->right;
                    }
                    node->right->parent = p;

                    do
                    {
                        --(p->size);
                        p = p->parent;
                    } while (p);

                    node->right = nullptr;
                    delete node;
                    return true;
                }
            }

            if (node->right == nullptr)
            {//left only
                auto p = node->parent;
                if (p->left == node)
                {
                    p->left = node->left;
                }
                else
                {
                    assert(p->right == node);
                    p->right = node->left;
                }
                node->left->parent = p;

                do
                {
                    --(p->size);
                    p = p->parent;
                } while (p);

                node->left = nullptr;
                delete node;
                return true;
            }

            // both
            auto node2 = node->left;
            while (node2->right)
                node2 = node2->right;

            node->left = node->right = nullptr;
            delete node;
            // todo: balance.
            return true;
        }

        // todo: del it for 'rank()'?
        bool find(const t& v) const
        {
            const auto r = rank(v);
            return r >= 0;
        }

        const t& find_k_smallest(int rank) const
        {
            if (rank < 0 || rank >= size())
                throw std::runtime_error("invalid rank");

            auto node = root;
            auto r = rank;
            for (;;)
            {
                assert(node && r >= 0 && node->size > 0);
                if (node->left)
                {
                    if (r < node->left->size)
                    {
                        node = node->left;
                        continue;
                    }
                    if (r == node->left->size)
                        return node->value;

                    r -= 1 + node->left->size;
                    assert(node->right);
                    node = node->right;
                    continue;
                }

                if (r == 0)
                    return node->value;

                assert(r > 0 && node->right);
                --r;
                node = node->right;
            }
        }

        int rank(const t& v) const
        {
            auto node = root;
            auto r = 0;
            for (;;)
            {
                if (node == nullptr)
                    return -1;

                if (less(v, node->value))
                {
                    node = node->left;
                }
                else if (less(node->value, v))
                {
                    r += 1 + (node->left ? node->left->size : 0);
                    node = node->right;
                }
                else
                {
                    r += node->left ? node->left->size : 0;
                    return r;
                }
            }
        }
    };
}