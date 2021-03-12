#pragma once
#include <ostream>

namespace Privet::Algorithms::Trees
{
    //A binary tree node.
    template <typename T>
    struct BinaryTreeNode final
    {
        using value_type = T;

        T Value;

        BinaryTreeNode* Left;
        BinaryTreeNode* Right;

        BinaryTreeNode(const T& value = {},
            BinaryTreeNode* left = {},
            BinaryTreeNode* right = {});

        inline bool operator ==(const BinaryTreeNode& b) const
        {
            const bool result = Value == b.Value;
            return result;
        }

        inline friend std::ostream& operator << (
            std::ostream& str, const BinaryTreeNode& node)
        {
            str << node.Value;

            return str;
        }
    };

    template <typename T >
    BinaryTreeNode< T >::BinaryTreeNode(
        const T& value,
        BinaryTreeNode* left,
        BinaryTreeNode* right)
        : Value(value), Left(left), Right(right)
    {
    }
}