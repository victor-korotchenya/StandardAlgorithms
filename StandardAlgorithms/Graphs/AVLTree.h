#pragma once
#include <memory> // unique_ptr
#include "../Utilities/ExceptionUtilities.h"
#include "BinaryTreeUtilities.h"
#include "BinarySearchTreeUtilities.h"
#include "AVLTreeNode.h"

namespace Privet::Algorithms::Trees
{
    //IN AVL tree, subtree heights can differ by at most 1.
    //Duplicates are not allowed.
    //AVL has faster selects than red-black does.
    template <typename TValue >
    struct AVLTree final
    {
        typedef AVLTreeNode< TValue > Node;

    private:
        size_t _Size;

        std::unique_ptr<Node, void(*)(Node*)> _UniqueRoot;

        Node* _Root;

        //TODO: p3. implement copy constructor.
        AVLTree(const AVLTree&) = delete;

        //TODO: p3. implement assign constructor.
        AVLTree& operator =(const AVLTree&) = delete;

    public:
        AVLTree();

        ~AVLTree();

        size_t size() const;

        void clear();

        bool Find(const TValue& value) const;

        //Returns whether the value has been deleted.
        bool Add(const TValue& value);

        //Returns whether the value has been deleted.
        bool Delete(const TValue& value);

        //TODO: p2. void Union();
    };

    template <typename TValue>
    AVLTree<TValue>::AVLTree()
        : _Size(0),
        _UniqueRoot(nullptr, FreeTree<Node>),
        _Root(nullptr)
    {
    }

    template <typename TValue >
    AVLTree< TValue >::~AVLTree()
    {
        clear();
    }

    template <typename TValue>
    size_t AVLTree<TValue>::size() const
    {
        return _Size;
    }

    template <typename TValue >
    void AVLTree< TValue >::clear()
    {
        _UniqueRoot.reset(nullptr);
        _Root = nullptr;
        _Size = 0;
    }

    template <typename TValue >
    bool AVLTree< TValue >::Find(const TValue& value) const
    {
        Node* result = const_cast<Node*>(
            BinarySearchTreeUtilities< TValue, Node >::Find(
                _Root, value));

        return nullptr != result;
    }

    template <typename TValue >
    bool AVLTree< TValue >::Add(const TValue& value)
    {
        if (0 == _Size)
        {
            _UniqueRoot.reset(new Node(value));

            _Root = _UniqueRoot.get();
            ThrowIfNull(_Root, "Cannot create the root node.");

            ++_Size;
            return true;
        }

        //TODO: p1. Finish Add and Delete in order to fix the tests.

        Node* node = _Root;
        ThrowIfNull(node, "Inner error: The root node is null.");

        Node* parent;
        do
        {
            if (value == node->Value)
            {
                return false;
            }

            parent = node;

            if (value < node->Value)
            {
                node = node->Left;
            }
            else
            {
                node = node->Right;
            }
        } while (node);

        ThrowIfNull(parent, "Inner error: The parent node is null.");

        Node** nodeToChange = value < parent->Value
            ? &(parent->Left) : &(parent->Right);

#ifdef _DEBUG
        if (nullptr != *nodeToChange)
        {
            throw std::exception("Inner error: nodeToChange must be null.");
        }
#endif
        * nodeToChange = new Node(value);

        ++_Size;

#ifdef _DEBUG
        {
            const bool isBST = BinarySearchTreeUtilities< TValue, Node >::
                IsBSTWithoutDuplicates(_Root);
            if (!isBST)
            {
                throw std::exception("AVLTree is not BST after addition.");
            }
        }
#endif

        return true;
    }

    template <typename TValue >
    bool AVLTree< TValue >::Delete(const TValue& value)
    {
        if (0 == _Size)
        {
            return false;
        }

        Node* node = _Root;
        ThrowIfNull(node, "Inner error: The root node is null.");

        Node* parent = nullptr;
        do
        {
            if (value == node->Value)
            {
                if (1 == _Size)
                {
                    clear();
                    return true;
                }

                Node* toDelete;

                if (nullptr == parent)
                {
#ifdef _DEBUG
                    if (node != _Root)
                    {
                        throw std::exception("Inner error: node != _Root.");
                    }
#endif
                    if (nullptr == node->Left && nullptr == node->Right)
                    {
                        std::ostringstream ss;
                        ss << "Inner error: There are " << _Size
                            << " nodes, but the root has null for both children.";
                        StreamUtilities::throw_exception(ss);
                    }

                    if (nullptr == node->Left)
                    {
                        toDelete = node->Right;
                        memcpy(node, toDelete, sizeof(Node));
                    }
                    else if (nullptr == node->Right)
                    {
                        toDelete = node->Left;
                        memcpy(node, toDelete, sizeof(Node));
                    }
                    else
                    {//Both left and right.
                        if (nullptr == (node->Right)->Left)
                        {
                            toDelete = node->Right;
                            node->Value = toDelete->Value;
                            //node->Right = (node->Right)->Right;
                        }
                        else
                        {
                        }
                    }
                }
                else
                {
                }

                //delete toDelete;
                --_Size;

#ifdef _DEBUG
                {
                    const bool isBST = BinarySearchTreeUtilities< TValue, Node >::
                        IsBSTWithoutDuplicates(_Root);
                    if (!isBST)
                    {
                        throw std::exception("AVLTree is not BST after deletion.");
                    }
                }
#endif
                return true;
            }

            parent = node;

            if (value < node->Value)
            {
                node = node->Left;
            }
            else
            {
                node = node->Right;
            }
        } while (node);

        return false;
    }
}