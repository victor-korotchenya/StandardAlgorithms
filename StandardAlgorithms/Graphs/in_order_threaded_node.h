#pragma once
#include"../Utilities/require_utilities.h"
#include"../Utilities/tag_kind.h"
#include<stack>

namespace Standard::Algorithms::Trees
{
    // Represent a node in an in-order threaded binary tree.
    // The in-order tree traversal does not require a stack
    // - it might be faster, but 1 extra byte per node is used.
    //
    // "Left" points to in-order predecessor,
    //  when (tag_kind::left == (tag_kind::left &Tag));
    // Otherwise, Left is left node(vertex).
    //
    // "Right" .. successor,
    // when (tag_kind::right == (tag_kind::right &Tag)).
    template<class type_of_data1>
    struct in_order_threaded_node final
    {
        using type_of_data = type_of_data1;
        using tag_kind = ::Standard::Algorithms::Utilities::tag_kind;

        [[nodiscard]] constexpr auto datum() const &noexcept -> const type_of_data &
        {
            return Datum;
        }

        [[nodiscard]] constexpr auto datum() &noexcept -> type_of_data &
        {
            return Datum;
        }

        // Remember to repair the tree e.g.
        // by calling "in_order_visit_and_set_tags".
        constexpr void set_left(in_order_threaded_node *left1) noexcept
        {
            Left = left1;
            Tag -= tag_kind::left;
        }

        // Remember to repair the tree e.g.
        // by calling "in_order_visit_and_set_tags".
        constexpr void set_right(in_order_threaded_node *right1) noexcept
        {
            Right = right1;
            Tag -= tag_kind::right;
        }

        [[nodiscard]] constexpr auto get_left() const &noexcept -> const in_order_threaded_node *
        {
            return has_flag(Tag, tag_kind::left) ? nullptr : Left;
        }

        // todo(p3): del copy-paste.
        [[nodiscard]] constexpr auto get_left() &noexcept -> in_order_threaded_node *
        {
            const auto *root1 = this;

            // NOLINTNEXTLINE
            return const_cast<in_order_threaded_node *>(root1->get_left());
        }

        [[nodiscard]] constexpr auto get_right() const &noexcept -> const in_order_threaded_node *
        {
            return has_flag(Tag, tag_kind::right) ? nullptr : Right;
        }

        // todo(p3): del copy-paste.
        [[nodiscard]] constexpr auto get_right() &noexcept -> in_order_threaded_node *
        {
            const auto *root1 = this;

            // NOLINTNEXTLINE
            return const_cast<in_order_threaded_node *>(root1->get_right());
        }

        [[nodiscard]] constexpr auto predecessor() const &noexcept -> const in_order_threaded_node *
        {
            return has_flag(Tag, tag_kind::left) ? Left : nullptr;
        }

        [[nodiscard]] constexpr auto successor() const &noexcept -> const in_order_threaded_node *
        {
            return has_flag(Tag, tag_kind::right) ? Right : nullptr;
        }

        [[nodiscard]] constexpr auto in_order_successor() const &noexcept -> const in_order_threaded_node *
        {
            if (has_flag(Tag, tag_kind::right))
            {
                return Right;
            }

            const auto *result = get_right();

            if (result != nullptr)
            {
                const auto *left1 = result->get_left();

                while (left1 != nullptr)
                {
                    result = left1;
                    left1 = left1->get_left();
                }
            }

            return result;
        }

        [[nodiscard]] constexpr auto operator== (const in_order_threaded_node &other) const noexcept -> bool
        {
            auto result = Left == other.Left && Right == other.Right && Tag == other.Tag && Datum == other.Datum;

            return result;
        }

        friend auto operator<< (std::ostream &str, const in_order_threaded_node &node) -> std::ostream &
        {
            str << "Datum " << node.Datum;
            return str;
        }

        // "This" node will be considered the root of the tree.
        //
        // When Left (or Right) is null, it might be set and
        // the Tag be updated properly.
        constexpr void in_order_visit_and_set_tags()
        {
            in_order_visit_slow(set_neighbors);
        }

        // This is fast - is it so? O(n) time and O(n) space.
        template<class visit_function_t, class... arguments_t>
        constexpr void in_order_visit_fast(visit_function_t visit_function, arguments_t &&...arguments) const
        {
            throw_if_null("visitFunction", visit_function);

            const in_order_threaded_node *previous{};
            const auto *current = find_lowest_element();

            while (current != nullptr)
            {
                visit_function(previous, current,
                    // no std::forward<arguments_t>(arguments)...);
                    arguments...);

                previous = current;
                current = current->in_order_successor();
            }
        }

        // Better use "in_order_visit_fast".
        template<class visit_function_t, class... arguments_t>
        constexpr void in_order_visit_slow(visit_function_t visit_function, arguments_t &&...arguments) const
        {
            in_order_visit_slow_impl<const in_order_threaded_node *, visit_function_t, arguments_t...>(
                this, visit_function, std::forward<arguments_t>(arguments)...);
        }

        // Better use "in_order_visit_fast" or the const version instead of this function.
        template<class visit_function_t, class... arguments_t>
        constexpr void in_order_visit_slow(visit_function_t visit_function, arguments_t &&...arguments)
        {
            in_order_visit_slow_impl<in_order_threaded_node *, visit_function_t, arguments_t...>(
                this, visit_function, std::forward<arguments_t>(arguments)...);
        }

private:
        template<class ptr_t, class visit_function_t, class... arguments_t>
        requires(std::is_pointer_v<ptr_t>)
        static constexpr void in_order_visit_slow_impl(
            ptr_t root1, visit_function_t visit_function, arguments_t &&...arguments)
        {
            throw_if_null("visitFunction", visit_function);

            ptr_t previous{};
            std::stack<ptr_t> sta;

            for (;;)
            {
                while (root1 != nullptr)
                {
                    sta.push(root1);
                    root1 = root1->get_left();
                }

                if (sta.empty())
                {
                    break;
                }

                root1 = sta.top();
                sta.pop();

                visit_function(previous, root1, std::forward<arguments_t>(arguments)...);

                previous = root1;
                root1 = root1->get_right();
            }
        }

        static constexpr void set_neighbors(in_order_threaded_node *previous, in_order_threaded_node *current)
        {
            if (nullptr == previous || nullptr == current)
            {
                return;
            }

            if (nullptr == previous->get_right())
            {
                previous->Tag |= tag_kind::right;
                previous->Right = current;
            }

            if (nullptr == current->get_left())
            {
                current->Tag |= tag_kind::left;
                current->Left = previous;
            }
        }

        [[nodiscard]] constexpr auto find_lowest_element() const &noexcept -> const in_order_threaded_node *
        {
            const auto *current = this;
            const auto *previous = current->get_left();

            while (previous != nullptr)
            {
                current = previous;
                previous = current->get_left();
            }

            return current;
        }

        in_order_threaded_node *Left{};
        in_order_threaded_node *Right{};

        type_of_data Datum{};

        // todo(p2): if pointers are always 4 or 8-byte aligned, use 2 lowest bits as a tag.
        tag_kind Tag{};
    };
} // namespace Standard::Algorithms::Trees
