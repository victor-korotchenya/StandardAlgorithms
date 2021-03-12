#pragma once
#include "../Utilities/StreamUtilities.h"
#include "BinaryTreeNode.h"

namespace Privet::Algorithms::Trees
{
    template <typename Number = int, typename TNode = BinaryTreeNode<Number>>
    class DoubleLinkedListUtilities final
    {
        DoubleLinkedListUtilities() = delete;

    public:

        //This is only for doubly-linked lists.
        //
        //If yes, returns the pair of nulls.
        //Else, the returned nodes:
        // - either have different values,
        // - or one pointer is null.
        static std::pair< const TNode* const, const TNode* const > AreIsomorphic(
            const TNode* const root1, const TNode* const root2);
    };

    template <typename Number, typename TNode >
    std::pair< const TNode* const, const TNode* const >
        DoubleLinkedListUtilities< Number, TNode >::AreIsomorphic(
            const TNode* const root1,
            const TNode* const root2)
    {
        using TPair = std::pair< TNode*, TNode* >;
        using TPairResult = std::pair< const TNode* const, const TNode* const >;

        TNode* node1 = const_cast<TNode*>(root1);
        TNode* node2 = const_cast<TNode*>(root2);

        while (node1 != node2)
        {
            if (nullptr == node1)
            {
                const TPairResult result = TPairResult(
                    const_cast<const TNode* const>(nullptr),
                    const_cast<const TNode* const>(node2));
                return result;
            }

            if (nullptr == node2)
            {
                const TPairResult result = TPairResult(
                    const_cast<const TNode* const>(node1),
                    const_cast<const TNode* const>(nullptr));
                return result;
            }

            if ((node1->Value) != (node2->Value))
            {
                const TPairResult result = TPairResult(
                    const_cast<const TNode* const>(node1),
                    const_cast<const TNode* const>(node2));
                return result;
            }

            node1 = node1->Right;
            node2 = node2->Right;
        }

        return TPairResult(nullptr, nullptr);
    }
}