#pragma once
#include <ostream>
#include <stack>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Trees
{
    //Represents a node in an in-order threaded binary tree.
    //The in-order tree traversal does not require a stack
    // - it might be faster, but 1 extra byte per node is used.
    //
    //"Left" points to in-order predecessor,
    //  when (TagKind::Left == (TagKind::Left  &Tag));
    // Otherwise, Left is left node(vertex).
    //
    //"Right" ... successor,
    //  when (TagKind::Right == (TagKind::Right  &Tag)).
    template <typename T>
    struct InOrderThreadedNode final
    {
        enum class TagKind final : char
        {
            None = 0, Left = 1, Right = 2, Both = 3
        };

        using TypeOfData = T;

        T Datum;

    private:

        struct InOrderThreadedNode* Left;
        struct InOrderThreadedNode* Right;

        TagKind Tag;

    public:

        InOrderThreadedNode(
            T datum = {},
            struct InOrderThreadedNode* left = {},
            struct InOrderThreadedNode* right = {},
            TagKind tag = TagKind::None)
            : Datum(datum), Left(left), Right(right), Tag(tag)
        {
        }

        //Remember to repair the tree e.g.
        //by calling "InOrderVisitAndSetTags".
        inline void SetLeft(InOrderThreadedNode* left)
            noexcept
        {
            Left = left;
            ResetFlag(TagKind::Left, Tag);
        }
        //Remember to repair the tree e.g.
        //by calling "InOrderVisitAndSetTags".
        inline void SetRight(InOrderThreadedNode* right)
            noexcept
        {
            Right = right;
            ResetFlag(TagKind::Right, Tag);
        }

        inline struct InOrderThreadedNode* GetLeft() const noexcept
        {
            InOrderThreadedNode* result = HasFlag(TagKind::Left, Tag) ? nullptr : Left;
            return result;
        }
        inline struct InOrderThreadedNode* GetRight() const noexcept
        {
            InOrderThreadedNode* result = HasFlag(TagKind::Right, Tag) ? nullptr : Right;
            return result;
        }
        inline struct InOrderThreadedNode* Predecessor() const noexcept
        {
            InOrderThreadedNode* result = HasFlag(TagKind::Left, Tag) ? Left : nullptr;
            return result;
        }
        inline struct InOrderThreadedNode* Successor() const noexcept
        {
            InOrderThreadedNode* result = HasFlag(TagKind::Right, Tag) ? Right : nullptr;
            return result;
        }

        struct InOrderThreadedNode* InOrderSuccessor() const;

        inline bool operator == (const InOrderThreadedNode& b) const
        {
            const bool result = Datum == b.Datum
                && Left == b.Left && Right == b.Right
                && Tag == b.Tag;
            return result;
        }

        inline friend std::ostream& operator <<
            (std::ostream& str, const InOrderThreadedNode& b)
        {
            str
                << "Datum=" << b.Datum;
            return str;
        }

        //"This" node will be considered the root of the tree.
        //
        //When Left( or Right) is null, it might be set and
        // the Tag be updated properly.
        void InOrderVisitAndSetTags();

        using VisitFunction = void(*)(
            const InOrderThreadedNode* const previous,
            const InOrderThreadedNode* const current);

        //This is fast - is it so? O(n) time and O(n) space.
        void InOrderVisitFast(VisitFunction visitFunction) const;

        //Use "InOrderVisitFast" instead of this function.
        void InOrderVisitSlow(VisitFunction visitFunction) const;

    private:

        inline static char HasFlag(const TagKind flag, const TagKind value)
            noexcept
        {
            const char result = static_cast<char>(flag) & static_cast<char>(value);
            return result;
        }
        inline static TagKind AddFlag(const TagKind flag, const TagKind value)
            noexcept
        {
            const char result = static_cast<char>(flag) | static_cast<char>(value);
            return static_cast<TagKind>(result);
        }
        inline static void ResetFlag(
            const TagKind flag, TagKind& value)
            noexcept
        {
            const char result = (~static_cast<char>(flag)) & static_cast<char>(value);
            value = static_cast<TagKind>(result);
        }

        static void SetNeighbors(
            const InOrderThreadedNode* const previous,
            const InOrderThreadedNode* const current);

        const InOrderThreadedNode* FindLowestElement() const;
    };

    template <typename T>
    void InOrderThreadedNode<T>::SetNeighbors(
        const InOrderThreadedNode* const previous,
        const InOrderThreadedNode* const current)
    {
        if (nullptr == previous || nullptr == current)
        {
            return;
        }

        InOrderThreadedNode* previousEdit = const_cast<InOrderThreadedNode*>(previous);
        InOrderThreadedNode* currentEdit = const_cast<InOrderThreadedNode*>(current);

        if (nullptr == previous->GetRight())
        {
            previousEdit->Tag = AddFlag(previousEdit->Tag, TagKind::Right);
            previousEdit->Right = currentEdit;
        }

        if (nullptr == current->GetLeft())
        {
            currentEdit->Tag = AddFlag(currentEdit->Tag, TagKind::Left);
            currentEdit->Left = previousEdit;
        }
    }

    template <typename T>
    void InOrderThreadedNode<T>::InOrderVisitSlow(
        VisitFunction visitFunction) const
    {
        ThrowIfNull(visitFunction, "visitFunction");

        InOrderThreadedNode* previous = nullptr;
        InOrderThreadedNode* root = const_cast<InOrderThreadedNode*>(this);

        std::stack <InOrderThreadedNode*> s;
        for (;;)
        {
            while (root)
            {
                s.push(root);
                root = root->GetLeft();
            }

            if (s.empty())
            {
                break;
            }

            root = s.top();
            s.pop();

            visitFunction(previous, root);

            previous = root;
            root = root->GetRight();
        }
    }

    template <typename T>
    void InOrderThreadedNode<T>::InOrderVisitAndSetTags()
    {
        InOrderVisitSlow(SetNeighbors);
    }

    template <typename T>
    const InOrderThreadedNode<T>* InOrderThreadedNode<T>::FindLowestElement() const
    {
        InOrderThreadedNode* current = const_cast<InOrderThreadedNode*>(this);
        InOrderThreadedNode* previous = current->GetLeft();
        while (previous)
        {
            current = previous;
            previous = current->GetLeft();
        }

        return const_cast<const InOrderThreadedNode<T>*>(current);
    }

    template <typename T >
    InOrderThreadedNode<T>* InOrderThreadedNode<T>::
        InOrderSuccessor() const
    {
        InOrderThreadedNode<T>* result;

        if (HasFlag(TagKind::Right, Tag))
        {
            result = Right;
            return result;
        }

        result = GetRight();
        if (result)
        {
            InOrderThreadedNode<T>* left = result->GetLeft();
            while (left)
            {
                result = left;
                left = left->GetLeft();
            }
        }

        return result;
    }

    template <typename T >
    void InOrderThreadedNode<T>::InOrderVisitFast(
        VisitFunction visitFunction) const
    {
        ThrowIfNull(visitFunction, "visitFunction");

        InOrderThreadedNode* previous = nullptr;
        InOrderThreadedNode* current = const_cast<InOrderThreadedNode*>
            (FindLowestElement());

        while (current)
        {
            visitFunction(previous, current);

            previous = current;
            current = current->InOrderSuccessor();
        }
    }
}