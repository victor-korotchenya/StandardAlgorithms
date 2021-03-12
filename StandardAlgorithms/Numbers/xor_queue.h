#pragma once
#include"../Utilities/require_utilities.h"

namespace Standard::Algorithms::Numbers
{
    // The node_t class must have a public field "Next" of type "node_t*".
    template<class node_t>
    concept node_with_next_pointer = requires(node_t &node)
    // Avoid 70 chars long prefixes.
    // NOLINTNEXTLINE
    {
        {
            node.next
            } -> std::same_as<node_t *&>;
    };

    // A queue, storing items as XORed linked list.
    // It uses less memory than a traditional queue, but it is slower.
    template<node_with_next_pointer node_t1>
    struct xor_queue final
    {
        using node_t = node_t1;
        using address_type = std::uintptr_t;

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Size;
        }

        constexpr void enqueue(node_t &node) &noexcept
        {
            assert(Size < std::numeric_limits<std::size_t>::max());

            switch (Size)
            {
            case 0U:
                assert(Head == nullptr && Tail == nullptr);

                node.next = nullptr;
                Head = &node;
                break;
            case 1U:
                assert(Head != nullptr && Tail == nullptr);

                node.next = Head;
                Tail = &node;
                Head->next = Tail;
                break;
            default:
                assert(Head != nullptr && Tail != nullptr);

                node.next = Tail;
                {
                    auto address1 = as_address(Tail->next);
                    auto address2 = as_address(&node);
                    Tail->next = as_pointer(address1 ^ address2);
                }

                Tail = &node;
                break;
            }

            ++Size;
        }

        [[nodiscard]] constexpr auto dequeue() &noexcept -> node_t *
        {
            auto *result = Head;

            switch (Size)
            {
            case 0U:
                assert(Head == nullptr && Tail == nullptr);

                return nullptr;
            case 1U:
                assert(Head != nullptr && Tail == nullptr);

                Head = nullptr;
                break;
            case 2U:
                assert(Head != nullptr && Tail != nullptr);

                Head = Tail;
                Head->next = Tail = nullptr;
                break;
            default:
                assert(Head != nullptr && Head->next != nullptr && Tail != nullptr);

                Head = Head->next;

                auto difference = as_address(Head->next);
                auto addr = as_address(result);

                Head->next = as_pointer(difference ^ addr);

                assert(Head->next != nullptr);
                break;
            }

            --Size;
            result->next = nullptr;

            return result;
        }

private:
        [[nodiscard]] static constexpr auto as_address(void *pointer) noexcept -> address_type
        {
            address_type address1 = // We need the cast.
                                    // NOLINTNEXTLINE
                reinterpret_cast<address_type>(pointer);

            return address1;
        }

        [[nodiscard]] static constexpr auto as_pointer(address_type address1) noexcept -> node_t *
        {
            auto *node = // We need it again.
                         // NOLINTNEXTLINE
                reinterpret_cast<node_t *>(address1);

            return node;
        }

        node_t *Head{};
        node_t *Tail{};
        std::size_t Size{};
    };
} // namespace Standard::Algorithms::Numbers
