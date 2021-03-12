#pragma once
#include <sal.h>
#include <ostream>

namespace Privet::Algorithms::Trees
{
    template <typename TValue>
    struct AVLTreeNode final
    {
        TValue Value;

        char Height;

        AVLTreeNode* Left;
        AVLTreeNode* Right;

        AVLTreeNode(const TValue& value = {},
            const char height = char(0),
            _In_opt_ AVLTreeNode* left = nullptr,
            _In_opt_ AVLTreeNode* right = nullptr)
            : Value(value), Height(height), Left(left), Right(right)
        {
        }

        inline bool operator ==(const AVLTreeNode& b) const
        {
            const bool result = Value == b.Value;
            return result;
        }

        friend std::ostream& operator << (
            std::ostream& str, const AVLTreeNode& node)
        {
            str << node.Value;

            return str;
        }
    };
}