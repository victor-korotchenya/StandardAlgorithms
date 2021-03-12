#pragma once

namespace Privet::Algorithms::Trees
{
    //A binary tree node with parent.
    template <typename T >
    struct BinaryTreeNodeWithParent final
    {
        using value_type = T;

        T Value;

        BinaryTreeNodeWithParent* Left;
        BinaryTreeNodeWithParent* Right;

        BinaryTreeNodeWithParent* Parent;

        BinaryTreeNodeWithParent(const T value) :
            Value(value), Left(nullptr), Right(nullptr), Parent(nullptr)
        {
        }
    };
}