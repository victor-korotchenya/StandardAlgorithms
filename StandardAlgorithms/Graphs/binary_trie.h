#pragma once
// "binary_trie.h"
#include"binary_tree_utilities.h"
#include<cstddef>
#include<memory>
#include<stack>
#include<tuple>

namespace Standard::Algorithms::Trees::Inner
{
    template<class note_t>
    [[nodiscard]] constexpr auto find_first_fork(note_t *node) -> note_t *
    {
        assert(node != nullptr);

        for (;;)
        {
            if (node->left != nullptr)
            {
                if (node->right != nullptr)
                {
                    return node;
                }

                node = node->left;
            }
            else if (node->right != nullptr)
            {
                node = node->right;
            }
            else
            {
                return nullptr;
            }
        }
    }

    template<std::integral value_t>
    [[nodiscard]] constexpr auto shift_add_bit(value_t sum, value_t bit) -> value_t
    {
        auto result = static_cast<value_t>((sum << 1U) | bit);
        return result;
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    // Radix tree, trie (retrieval), with find maximum XOR for 2 elements.
    template<std::integral value_t>
    class binary_trie final
    {
        struct note_t final
        {
            note_t *left{};
            note_t *right{};
            bool value{};
        };

        static constexpr value_t zero{};
        static constexpr value_t one{ 1 };

        using left_right_nodes_sums_t = std::tuple<const note_t *, const note_t *, value_t, value_t>;

        using deleter_t = void (*)(note_t *);

        std::unique_ptr<note_t, deleter_t> Unique_root = [] [[nodiscard]] ()
        {// Call an immediately invoked lambda.
            std::unique_ptr<note_t, deleter_t> res(nullptr, free_tree_33<note_t>);
            res.reset(new note_t{});

            return res;
        }();

        [[nodiscard]] constexpr auto root() &noexcept -> note_t *
        {
            assert(Unique_root);
            return Unique_root.get();
        }

        [[nodiscard]] constexpr auto root() const &noexcept -> const note_t *
        {
            assert(Unique_root);
            return Unique_root.get();
        }

public:
        constexpr binary_trie() = default;
        constexpr ~binary_trie() noexcept = default;

        binary_trie(const binary_trie &) = delete;
        auto operator= (const binary_trie &) & -> binary_trie & = delete;
        binary_trie(binary_trie &&) noexcept = delete;
        auto operator= (binary_trie &&) &noexcept -> binary_trie & = delete;

        constexpr void add(const value_t &value)
        {
            constexpr auto max_bits = static_cast<std::uint32_t>(sizeof(value_t) << 3U) - 1U;

            auto node = root();
            auto bits = max_bits;

            do
            {
                assert(node != nullptr);

                const auto bit = (value >> bits) & one;

                gsl::owner<note_t *> *ptr = 0U == bit ? &(node->left) : &(node->right);

                if (gsl::owner<note_t *> &owner1 = *ptr; nullptr == owner1)
                {
                    owner1 = new note_t{ .value = zero != bit };
                }

                node = *ptr;
            } while (0U < bits--);
        }

        // Maximum XOR of 2 values, not necessarily distinct.
        // Return 0 when no values added.
        [[nodiscard]] constexpr auto max_xor() const -> value_t
        {
            auto *const node = Inner::find_first_fork(root());
            if (node == nullptr)
            {
                return {};
            }

            value_t result{};

            std::stack<left_right_nodes_sums_t> sta;
            push_tuple(node->left, zero, sta, node->right, zero);

            do
            {
                const auto &top = sta.top();

                const auto *const left = std::get<0>(top);
                const auto *const right = std::get<1>(top);
                const auto sum_left = std::get<2>(top);
                const auto sum_right = std::get<3>(top);

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    {
                        const auto is_leaf_left = nullptr == left->left && nullptr == left->right;

                        const auto is_leaf_right = nullptr == right->left && nullptr == right->right;

                        if (is_leaf_left != is_leaf_right) [[unlikely]]
                        {
                            throw std::runtime_error(
                                "Inner error: Left node has no sub-nodes, but the right one has, or vice versa.");
                        }
                    }
                }

                sta.pop();

                if (left->left != nullptr || left->right != nullptr)
                {
                    process_intermediate_nodes(left, sum_left, sta, right, sum_right);
                }
                else
                {
                    const auto temp = static_cast<value_t>(sum_left ^ sum_right);
                    if (result < temp)
                    {
                        result = temp;
                    }
                }
            } while (!sta.empty());

            return result;
        }

private:
        static constexpr void process_intermediate_nodes(const note_t *left, const value_t &sum_left,
            std::stack<left_right_nodes_sums_t> &sta, const note_t *right, const value_t &sum_right)
        {
            if (nullptr == left->left)
            {// Left has only Right
                push_tuple(left->right, sum_left, sta, nullptr == right->left ? right->right : right->left, sum_right);
            }
            else if (nullptr == left->right)
            {// Left has only Left.
                push_tuple(left->left, sum_left, sta, nullptr == right->right ? right->left : right->right, sum_right);
            }
            // Left has both.
            else if (nullptr == right->left)
            {// Right has only Right.
                push_tuple(left->left, sum_left, sta, right->right, sum_right);
            }
            else if (nullptr == right->right)
            {// Right has only Left.
                push_tuple(left->right, sum_left, sta, right->left, sum_right);
            }
            else
            {// Both exist.
                push_tuple(left->right, sum_left, sta, right->left, sum_right);

                push_tuple(left->left, sum_left, sta, right->right, sum_right);
            }
        }

        static constexpr void push_tuple(const note_t *left, const value_t &sum_left,
            std::stack<left_right_nodes_sums_t> &sta, const note_t *right, const value_t &sum_right)
        {
            const auto uno = Inner::shift_add_bit<value_t>(sum_left, left->value ? one : zero);

            const auto dupo = Inner::shift_add_bit<value_t>(sum_right, right->value ? one : zero);

            sta.emplace(left, right, uno, dupo);
        }
    };
} // namespace Standard::Algorithms::Trees
