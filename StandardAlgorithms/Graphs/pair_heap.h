#pragma once
#include"../Utilities/print_utilities.h"
#include"child_left_right_visit.h"
#include<bit>
#include<gsl/gsl>
#include<memory>
#include<stdexcept>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Heaps
{
    template<class key_t1>
    struct pair_heap_node final
    {
        using key_t = key_t1;

        key_t key{};

        pair_heap_node *child{};

        // The previous in the doubly linked list, or the parent for the left-most.
        // Null only for the root.
        pair_heap_node *prev{};

        pair_heap_node *next{};
    };

    // Minimum priority heap, should be fast.
    // The skewed tree height might be large: # of nodes - 1.
    template<class key_t1, class node_t1 = pair_heap_node<key_t1>>
    struct pair_heap final
    {
        using key_t = key_t1;
        using node_t = node_t1;

        static constexpr bool has_parent = false;

        constexpr pair_heap() = default;

        // Time O(n).
        constexpr ~pair_heap() noexcept
        {
            assert_consistent();

            if (Root_node == nullptr)
            {
                return;
            }

            check_buffer_size();

            std::size_t count{};

            std::function<void(const node_t *, const gsl::owner<const node_t *>)> func =
                [&count](const node_t *const parent1, const gsl::owner<const node_t *> node)
            {
                assert(node != nullptr && node != parent1);

                ++count;

                delete node;
            };

            Standard::Algorithms::Trees::child_left_right_visit(*Root_node, Buffer, func);

            assert(Node_count == count);
        }

        // Time O(1).
        [[nodiscard]] constexpr auto top() const &noexcept
        {
            return static_cast<node_t *>(Root_node);
        }

        // Time O(1).
        [[nodiscard]] constexpr auto size() const noexcept
        {
            return Node_count;
        }

        // Average time O(1).
        constexpr auto push(const key_t &key) &noexcept(false)
        {
            assert_consistent();
            reserve(Node_count + 1U);

            auto owner = std::make_unique<node_t>();
            auto *node = owner.get();
            node->key = key;

            if (Root_node != nullptr)
            {
                merge_nodes(Root_node, owner.release());
            }
            else
            {
                Root_node = static_cast<gsl::owner<node_t *>>(owner.release());
            }

            assert(Root_node != nullptr);
            ++Node_count;

            check_buffer_size();
            assert_consistent();

            return node;
        }

        // Time O(1).
        constexpr void decrease_key(node_t &node, const key_t &key) &noexcept(false)
        {
            assert(Root_node != nullptr && Root_node->prev == nullptr);
            assert_consistent();

            check_buffer_size();

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(node_exists_slow(&node));
            }

            if (!(key < node.key))
            {
                return;
            }

            node.key = key;

            if (&node == Root_node)
            {
                return;
            }

            // Unchain the node: pre - node - node_next will become pre - node_next
            //
            // 1) pre                             2) par
            //   /                                     /
            // node - node_next        other - pre - node - node_next
            auto *pre = node.prev;
            assert(pre != nullptr);

            if (node.next != nullptr)
            {
                node.next->prev = pre;
            }

            (pre->child == &node ? pre->child : pre->next) = node.next;

            node.prev = node.next = nullptr;
            merge_nodes(Root_node, &node);

            assert_consistent();
        }

        // Average time O(1).
        constexpr void merge(pair_heap &other) noexcept
        {
            assert_consistent();
            other.assert_consistent();

            if (other.Root_node == nullptr || this == &other)
            {
                return;
            }

            if (Root_node == nullptr)
            {
                std::swap(Root_node, other.Root_node);
                std::swap(Buffer, other.Buffer);
                std::swap(Node_count, other.Node_count);

                assert_consistent();
                other.assert_consistent();
                return;
            }

            reserve(Node_count + other.Node_count);

            merge_nodes(Root_node, other.Root_node);

            Node_count += other.Node_count;
            assert(0U < Node_count);

            other.Root_node = nullptr;
            other.Node_count = 0;

            assert_consistent();
            other.assert_consistent();
        }

        // Amortized time O(log(n)).
        constexpr void pop() &noexcept(false)
        {
            assert(
                Root_node != nullptr && 0U < Node_count && (Root_node->child != nullptr || Root_node->next == nullptr));

            assert_consistent();
            check_buffer_size();

            auto *new_root = Root_node->child != nullptr ? two_pass_sibling_merge(*(Root_node->child)) : nullptr;

            delete Root_node;
            Root_node = static_cast<gsl::owner<node_t *>>(new_root);
            --Node_count;

            assert_consistent();
        }

        // Time O(n).
        constexpr void validate(const std::string &message)
        {
            assert(!message.empty());
            assert_consistent();

            if ((Node_count == 0U) != (Root_node == nullptr)) [[unlikely]]
            {
                throw std::out_of_range(message + " The node count is " + std::to_string(Node_count) +
                    (Root_node == nullptr ? ", but no root." : ", but the root exists."));
            }

            if ((Buffer.size() & 1U) != 0U) [[unlikely]]
            {
                throw std::out_of_range(
                    message + " The buffer size " + std::to_string(Buffer.size()) + " must have been even.");
            }

            if (Buffer.size() < Node_count) [[unlikely]]
            {
                throw std::out_of_range(message + " The buffer size " + std::to_string(Buffer.size()) +
                    " cannot be less than node count " + std::to_string(Node_count) + ".");
            }

            if (Root_node == nullptr)
            {
                return;
            }

            check_buffer_size();

            if (Root_node->prev != nullptr) [[unlikely]]
            {
                throw std::out_of_range(message + " The root has prev.");
            }

            if (Root_node->next != nullptr) [[unlikely]]
            {
                throw std::out_of_range(message + " The root has next.");
            }

            std::unordered_set<const node_t *> uniq{};
            uniq.insert(nullptr);

            std::function<void(const node_t *, const node_t *)> func = [&message, &uniq](
                                                                           const node_t *parent1, const node_t *node)
            {
                assert(node != nullptr && node != parent1);

                if (node == nullptr) [[unlikely]]
                {
                    throw std::invalid_argument(message + " The node is nullptr.");
                }

                guarantee_uniqueness(message, uniq, *node);

                if (parent1 != nullptr && node->key < parent1->key) [[unlikely]]
                {
                    throw std::out_of_range(message + " The node key is less than parent's.");
                }
            };

            Standard::Algorithms::Trees::child_left_right_visit(*Root_node, Buffer, func);

            if (const auto actual = uniq.size() - 1U; Node_count != actual) [[unlikely]]
            {
                throw std::out_of_range(message + " Expect " + std::to_string(Node_count) +
                    " nodes, but actually visited " + std::to_string(actual) + ".");
            }
        }

        pair_heap(pair_heap &) = delete;
        auto operator= (pair_heap &) & -> pair_heap = delete;
        pair_heap(pair_heap &&) = delete; // todo(p3): make movable.
        auto operator= (pair_heap &&) & -> pair_heap = delete;

private:
        [[maybe_unused]] inline constexpr void assert_consistent()
        {
            assert((Root_node == nullptr) == (Node_count == 0U));
        }

        // Time O(n).
        [[nodiscard]] [[maybe_unused]] constexpr auto node_exists_slow(const node_t *const search_node) const & -> bool
        {
            if (Root_node == nullptr || search_node == nullptr)
            {
                assert(0);
                return false;
            }

            check_buffer_size();

            auto exist = false;

            // Will visit all the nodes, but the code is simpler.
            std::function<void(const node_t *, const node_t *)> func = [search_node, &exist](
                                                                           const node_t *parent1, const node_t *node)
            {
                assert(node != nullptr && node != parent1);

                if (node == search_node)
                {
                    exist = true;
                }
            };

            Standard::Algorithms::Trees::child_left_right_visit(*Root_node, Buffer, func);

            return exist;
        }

        constexpr void check_buffer_size() const noexcept
        {
            assert(!Buffer.empty());
            assert((Buffer.size() & 1U) == 0U);
            assert(Node_count <= Buffer.size());
        }

        // Time O(1).
        static constexpr void merge_nodes(gsl::owner<node_t *> &root1, node_t *child1) noexcept
        {
            assert(root1 != nullptr && root1->prev == nullptr && root1->next == nullptr && root1 != child1);

            if (child1 == nullptr)
            {
                return;
            }

            assert(child1->prev == nullptr && child1->next == nullptr);

            const auto is_child1_the_new_root = child1->key < root1->key;
            if (is_child1_the_new_root)
            {
                std::swap(root1, child1);
            }

            // Was before:
            //   root1            child1
            //    /                   /
            // rich - next       cc - next2

            // Will be:
            //       root1
            //       /
            //   child1 - rich - next
            //    /
            // cc - next2

            if (auto *rich = root1->child; rich != nullptr)
            {
                child1->next = rich;
                rich->prev = child1;
            }

            child1->prev = root1;
            root1->child = child1;
        }

        static constexpr void guarantee_uniqueness(
            const std::string &message, std::unordered_set<const node_t *> &uniq, const node_t &node)
        {
            assert(!message.empty());

            if (uniq.insert(&node).second) [[likely]]
            {
                return;
            }

            throw std::out_of_range(
                message + " The node key " + std::to_string(node.key) + " has already been processed.");
        }

        constexpr void reserve(std::size_t capacity) noexcept(false)
        {
            constexpr std::size_t min_size = 2U;

            capacity = std::max({ capacity, min_size, Buffer.size() });
            capacity = std::bit_ceil(capacity);

            assert((capacity & 1U) == 0U && !(capacity < min_size));

            if (Buffer.size() < capacity)
            {
                Buffer.resize(capacity);
            }

            check_buffer_size();
        }

        // Amortized time O(log(n)).
        constexpr auto two_pass_sibling_merge(node_t &node) noexcept(false) -> node_t *
        {
            auto *root1 = &node;

            assert(Root_node != nullptr && 0U < Node_count && root1 != nullptr && root1->prev == Root_node);

            check_buffer_size();

            if (root1->next == nullptr)
            {
                root1->prev = nullptr;
                return root1;
            }

            std::size_t sibling_count{};

            do
            {
                if (Buffer.size() <= sibling_count) [[unlikely]]
                {
                    // NOLINTNEXTLINE
                    assert(0);

                    throw std::out_of_range(
                        "The sibling count " + std::to_string(sibling_count) + " has reached the buffer size.");
                }

                Buffer[sibling_count++] = root1;

                assert(root1->prev != nullptr);

                root1->prev->next = nullptr;
                root1->prev = nullptr;
                root1 = root1->next;
            } while (root1 != nullptr);

            if ((sibling_count & 1U) != 0U)
            {
                assert(sibling_count < Buffer.size());

                Buffer.at(sibling_count++) = nullptr;
            }

            assert(1U < sibling_count);

            std::size_t count2{};

            do
            {
                assert(count2 + 1U < sibling_count);

                merge_nodes(Buffer[count2], Buffer.at(count2 + 1U));
            } while ((count2 += 2U) < sibling_count);

            while (0U < (count2 -= 2U))
            {
                assert(1U < count2 && count2 < sibling_count);

                merge_nodes(Buffer[count2 - 2U], Buffer.at(count2));
            }

            auto *result = Buffer[0];
            assert(result != nullptr);

            return result;
        }

        gsl::owner<node_t *> Root_node{};
        std::vector<node_t *> Buffer{};
        std::size_t Node_count{};
    };
} // namespace Standard::Algorithms::Heaps
