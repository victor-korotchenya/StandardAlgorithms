#pragma once
// "binary_tree_utilities.h"
#include"../Numbers/arithmetic.h"
#include"../Utilities/project_constants.h"
#include"../Utilities/random.h"
#include"../Utilities/throw_exception.h"
#include"binary_tree_node.h"
#include<gsl/gsl>
#include<memory>
#include<stack>
#include<tuple>
#include<unordered_map>
#include<unordered_set>

namespace Standard::Algorithms::Trees::Inner
{
    template<class node_t, class function_t>
    constexpr void post_order_double_pointer(node_t **double_root, function_t visit_function)
    {
        throw_if_null_skip_non_pointer<function_t>("visit function", visit_function);

        if (nullptr == double_root || nullptr == *double_root)
        {
            return;
        }

        auto *last_visited = *double_root; // Assign some not null value.

        std::stack<node_t **> sta;
        sta.push(double_root);

        for (;;)
        {
            auto *top2 = sta.top();

            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("post_order_double_pointer.top2", top2);
                throw_if_null("*(post_order_double_pointer.top2)", *top2);
            }

            auto *top = *top2;

            const auto is_leaf = nullptr == top->left && nullptr == top->right;
            const auto shall_visit = is_leaf || top->left == last_visited || top->right == last_visited;

            if (shall_visit)
            {
                visit_function(top2);
                last_visited = top;

                sta.pop();
                if (sta.empty())
                {
                    break;
                }
            }
            else
            {
                if (top->right != nullptr)
                {
                    sta.push(&(top->right));
                }

                if (top->left != nullptr)
                {
                    sta.push(&(top->left));
                }
            }
        }
    }

    template<class node_t,
        class visit_function_t, // void(node_t **ppp)
        class node_two_ptr = node_t **>
    constexpr void post_order_double_pointer_2 // todo(p3): del
        (node_t **p_root, visit_function_t visit_function = {})
    {
        throw_if_null_skip_non_pointer<visit_function_t>("visit function", visit_function);
        if (nullptr == p_root || nullptr == *p_root)
        {
            return;
        }

        auto *last_visited = *p_root; // Assign some not null value.

        std::stack<node_two_ptr> sta;
        sta.push(p_root);

        for (;;)
        {
            auto *p_top = sta.top();

            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("post order_double_pointer_del.pTop", p_top);
                throw_if_null("*(post order_double_pointer_del.pTop)", *p_top);
            }

            auto *top = *p_top;

            const auto shall_visit =
                // Leaf
                (nullptr == top->left && nullptr == top->right) || // After both sub-trees.
                top->right == last_visited
                // No right subtree, and left is visited.
                || // nullptr == top->right &&
                top->left == last_visited;

            if (shall_visit)
            {
                visit_function(p_top);
                last_visited = top;

                sta.pop();
                if (sta.empty())
                {
                    break;
                }
            }
            else
            {
                if (top->right != nullptr)
                {
                    sta.push(&(top->right));
                }

                if (top->left != nullptr)
                {
                    sta.push(&(top->left));
                }
            }
        }
    }

    template<class node_t>
    constexpr void free_tree_not_null(gsl::owner<node_t *> node, const node_t *const sentinel = nullptr
#if _DEBUG
        ,
        std::uint32_t depth = {}
#endif
        ) noexcept
    {
#if _DEBUG
        {
            ++depth;

            assert(0U < depth && depth <= ::Standard::Algorithms::Utilities::stack_max_size);
        }
#endif

        for (;;)
        {
            assert(node != nullptr && node != sentinel);

            if (node->left != sentinel && node->right != sentinel)
            {
                assert(node->left != nullptr && node->right != nullptr);

                gsl::owner<node_t *> cop = node;
                node = static_cast<gsl::owner<node_t *>>(node->right);

                free_tree_not_null<node_t>(node, sentinel
#if _DEBUG
                    ,
                    depth
#endif
                );

                node = cop;
                node->right = // NOLINTNEXTLINE
                    const_cast<node_t *>(sentinel);
            }

            auto rest = node->left != sentinel ? node->left : node->right;
            delete node;

            if (rest == sentinel)
            {
                return;
            }

            node = static_cast<gsl::owner<node_t *>>(rest);
        }
    }

    template<class node_t, std::integral int_t = std::size_t>
    class can_count final
    {
        template<class t>
        static constexpr auto can_impl(void *) -> typename std::is_same<int_t, decltype(std::declval<t>().count)>::type;

        template<class>
        static constexpr auto can_impl(...) -> std::false_type;

        using type = decltype(can_impl<node_t>(nullptr));

public:
        static constexpr bool value = type::value;
    };

    template<class node_t>
    constexpr bool can_count_v = can_count<node_t>::value;
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    template<class node_t>
    constexpr void free_tree_2(gsl::owner<node_t *> root, const node_t *const sentinel = nullptr) noexcept
    {
        if (sentinel != root)
        {
            Inner::free_tree_not_null(root, sentinel);
        }
    }

    template<class node_t>
    constexpr void free_tree(gsl::owner<node_t *> root) noexcept
    {
        free_tree_2<node_t>(root);
    }

    // Return { the successor's parent, successor }.
    template<class node_t, class pair_t = std::pair<const node_t *, const node_t *>>
    [[nodiscard]] constexpr auto find_successor(const node_t &cur) -> pair_t
    {
        auto *par = &cur;
        auto *successor = cur.right;

        if (successor != nullptr)
        {
            while (successor->left != nullptr)
            {
                par = successor;
                successor = successor->left;
            }
        }

        return { par, successor };
    }

    // Return { lower_bound node or max node, ordinal rank }.
    // Return nullptr as the first part only when the tree is empty.
    // Sample. Given an array { 50 }, compute: rank(4) = 0, rank(50) = 0, and rank(600) = 1.
    template<class node_t, class key_t, bool can_count = Inner::can_count_v<node_t>>
    [[nodiscard]] constexpr auto lower_bound_rank(node_t *root, const key_t &key,
        const node_t *const sentinel = nullptr) noexcept -> std::pair<node_t *, std::size_t>
    {
        if (root == sentinel)
        {
            return { root, 0ZU };
        }

        std::size_t rank1{};

        for (node_t *larger = nullptr;;)
        {
            assert(root != nullptr && root != sentinel);

            if (key < root->key)
            {
                if (root->left == sentinel)
                {
                    return { root, rank1 };
                }

                larger = root;
                root = root->left;
                continue;
            }

            if (!(root->key < key))
            {
                if constexpr (can_count)
                {
                    rank1 += root->left != sentinel ? root->left->count : 0U;
                }

                return { root, rank1 };
            }

            if constexpr (can_count)
            {
                rank1 += (root->left != sentinel ? root->left->count : 0U) + 1U;
            }

            if (root->right == sentinel)
            {
                return { larger != nullptr ? larger : root, rank1 };
            }

            root = root->right;
        }
    }

    template<class node_t>
    [[nodiscard]] constexpr auto select_by_rank(const node_t *root, std::size_t rank) -> const node_t *
    {
        if (const auto size1 = root != nullptr ? root->count : 0U; !(rank < size1)) [[unlikely]]
        {
            throw std::runtime_error(
                "Invalid rank " + std::to_string(rank) + " for size " + std::to_string(size1) + ".");
        }

        for (;;)
        {
            assert(root != nullptr);

            const auto left_count = root->left != nullptr ? root->left->count : 0U;
            if (rank < left_count)
            {
                assert(root->left != nullptr);
                root = root->left;
            }
            else if (left_count < rank)
            {
                assert(root->right != nullptr);
                rank -= left_count + 1U;
                root = root->right;
            }
            else
            {
                return root;
            }
        }
    }

    // Not very optimal.
    template<class node_t>
    constexpr void free_tree_slow2(gsl::owner<node_t *> *double_root)
    {
        if (nullptr == double_root || nullptr == *double_root)
        {
            return;
        }

        const auto deleter = [](gsl::owner<node_t *> *p_node)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("free_tree_slow2.pNode", p_node);
                throw_if_null("*(free_tree_slow2.pNode)", *p_node);
            }

            const gsl::owner<node_t *> &node = *p_node;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_not_null("free_tree_slow2 node->left", node->left);
                throw_if_not_null("free_tree_slow2 node->right", node->right);
            }

            delete node;

            // The following line raises a red flag when const is removed from '&node' declaration:
            // node = nullptr;

            // And this does not, mda.
            *p_node = nullptr;
        };

        Standard::Algorithms::Trees::Inner::post_order_double_pointer(double_root, deleter);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            throw_if_not_null("free_tree_slow2 *double_root after deletion.", *double_root);
        }
    }

    template<class node_t>
    constexpr void free_tree_33(gsl::owner<node_t *> *p_root) // todo(p3): del
    {
        if (nullptr == p_root || nullptr == *p_root)
        {
            return;
        }

        const auto vis_2 = [](gsl::owner<node_t *> *p_node) -> void
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("free_tree_33.pNode", p_node);
                throw_if_null("*(free_tree_33.pNode)", *p_node);
            }

            const gsl::owner<node_t *> &node = *p_node;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_not_null("free_tree_33 node->left", node->left);
                throw_if_not_null("free_tree_33 node->right", node->right);
            }

            delete node;
            *p_node = nullptr;
        };

        Standard::Algorithms::Trees::Inner::post_order_double_pointer_2(p_root, vis_2);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            throw_if_not_null("*pRoot after deletion.", *p_root);
        }
    }

    template<class node_t>
    constexpr void free_tree_slow(gsl::owner<node_t *> root)
    {
        if (nullptr != root)
        {
            free_tree_slow2(&root);
        }
    }

    template<class node_t>
    constexpr void free_tree_33(node_t *root) noexcept // todo(p3): del
    {
        if (nullptr == root)
        {
            return;
        }

        free_tree_33(&root);
    }

    template<class int_t = std::int32_t, class node_t = binary_tree_node<int_t>>
    struct binary_tree_utilities final
    {
        binary_tree_utilities() = delete;

        // 0-based.
        //
        //                0;   root, level0
        //       1                  2;   level1
        //   3      4         5       6;   level2
        // 7  8  9 10  11 12 13 14;   level3

        // The child index must not be 0.
        [[nodiscard]] inline static constexpr auto parent_node(std::size_t child_index) noexcept(false) -> std::size_t
        {
            require_positive(child_index, "child index");

            auto result = (child_index - 1U) >> 1U;
            return result;
        }

        // 0 has 1 and 2.
        // 1 has 3 and 4.
        // 3 has 7 and 8.
        [[nodiscard]] inline static constexpr auto left_child(std::size_t parent_index) noexcept -> std::size_t
        {
            auto result = (parent_index << 1U) | 1U;
            return result;
        }

        [[nodiscard]] inline static constexpr auto right_child(std::size_t parent_index) noexcept -> std::size_t
        {
            auto result = (parent_index << 1U) + 2U;
            return result;
        }

        // 0 -> 0
        // 1,2 -> 1
        // 3..6 -> 2
        // 7..14 -> 3
        // index -> lead_bit(index + 1)
        [[nodiscard]] inline static constexpr auto level(std::size_t index) noexcept -> std::int32_t
        {
            assert(index != std::numeric_limits<std::size_t>::max());

            constexpr auto max_bit = static_cast<std::int32_t>(sizeof(std::size_t) * (CHAR_BIT)-1);

            const auto lev = max_bit - std::countl_zero(index + 1U);
            return lev;
        }

        // 1-based.
        //
        //                1;   root
        //       2                  3
        //   4      5         6       7
        // 8  9 10  11 12 13 14 15
        [[nodiscard]] inline static constexpr auto parent1(std::size_t index) noexcept(!::Standard::Algorithms::is_debug)
            -> std::size_t
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (0U == index) [[unlikely]]
                {
                    throw std::out_of_range("Cannot get a parent for 0 index.");
                }
            }

            return index >> 1U;
        }

        [[nodiscard]] inline static constexpr auto left_child1(const std::size_t index) noexcept -> std::size_t
        {
            return index << 1U;
        }

        [[nodiscard]] inline static constexpr auto right_child1(const std::size_t index) noexcept -> std::size_t
        {
            return (index << 1U) | 1U;
        }

        [[nodiscard]] static constexpr auto calc_tree_size(std::size_t size) -> std::size_t
        {
            verify_size(size);

            return size << 1U;
        }

        [[nodiscard]] static constexpr auto calc_tree_height(std::size_t size) -> std::uint32_t
        {
            verify_size(size);

            auto result = Standard::Algorithms::Numbers::most_significant_bit(size) + 1U;

            return result;
        }

        static constexpr void check_index(
            const std::size_t index, const std::size_t max_index_exclusive, const std::string &name)
        {
            assert(!name.empty());

            if (max_index_exclusive <= index)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The index (" << index << ") must be smaller than " << max_index_exclusive << " in " << name
                    << ".";

                throw_exception<std::out_of_range>(str);
            }
        }

        static constexpr void check_indexes(std::size_t left_inclusive, const std::string &name,
            std::size_t right_exclusive, const std::size_t max_index_exclusive)
        {
            check_index(left_inclusive, max_index_exclusive, name);

            check_index(right_exclusive - 1U, max_index_exclusive, name);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (right_exclusive <= left_inclusive)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error: rightExclusive (" << right_exclusive << ") <= leftInclusive(" << left_inclusive
                        << ") in " << name << ".";

                    throw_exception<std::out_of_range>(str);
                }
            }
        }

        // To avoid a memory leak, please use try-catch,
        // and free the tree if the (*head) is not null.
        // See also build_random_tree.
        template<class new_function_t = node_t *(*)()>
        static constexpr void build_random_tree(const std::size_t size, gsl::owner<node_t *> *head,
            const int_t cutter = {}, const bool is_unique_only = {}, new_function_t new_function = &node_constructor)
        {
            throw_if_null("head", head);
            throw_if_not_null("*head", *head);

            if (0U == size)
            {
                return;
            }

            if (std::numeric_limits<std::size_t>::max() - 1U <= size) [[unlikely]]
            {
                throw std::runtime_error("The size is too big.");
            }

            std::vector<int_t> data{};
            Standard::Algorithms::Utilities::fill_random(data, size, cutter, is_unique_only);

            new_node(data[0], head, new_function);
            if (1U == size)
            {
                return;
            }

            using temp_t = std::tuple<node_t *, std::size_t, std::size_t>;

            // No constant
            // NOLINTNEXTLINE
            Standard::Algorithms::Utilities::random_t<std::size_t> sizeRandom{};
            std::stack<temp_t> sta;
            sta.emplace(*head, 0U, size);

            do
            {
                const auto &top = sta.top();
                auto *root = std::get<0>(top);

                // Both indexes are exclusive.
                const auto first_index = std::get<1>(top);
                const auto second_index = std::get<2>(top);
                sta.pop();

                const auto split = sizeRandom(first_index, second_index - 1U);

                [[maybe_unused]] std::int32_t sub_tree_count{};

                // range = (0..2)
                // split = 0, left is none; right = 1.
                // split = 1, left is 1; right is none.
                //
                // range = (0..10)
                // split = 0, left is none; right is 1: next (1..10).
                // split = 1, left is 1: stop; right is 2: next (2..10).
                // split = 4, left is 4: next (0..4); right is 5: next (5..10).
                // split = 9, left is 9: next (0..9); right none.
                if (first_index < split)
                {// Left sub-tree.
                    new_node(data[split], &(root->left), new_function);

                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        ++sub_tree_count;
                    }

                    if (first_index < split - 1U)
                    {
                        sta.emplace(root->left, first_index, split);
                    }
                }

                if (split + 1U < second_index)
                {// Right sub-tree.
                    new_node(data[split + 1U], &(root->right), new_function);

                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        ++sub_tree_count;
                    }

                    if (split + 1U < second_index - 1U)
                    {
                        sta.emplace(root->right, split + 1U, second_index);
                    }
                }

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    if (0 == sub_tree_count)
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << "Error: 0 == subTreeCount"
                            << ", split " << split << ", firstIndex " << first_index << ", secondIndex " << second_index
                            << ".";

                        throw_exception(str);
                    }
                }
            } while (!sta.empty());
        }

        template<class visit_function_t> // = void(*)(const node_t* const)
        static constexpr void pre_order(const node_t *const root, visit_function_t visit_function)
        {
            if (nullptr == root)
            {
                return;
            }

            std::stack<const node_t *> sta;
            sta.push(root);
            do
            {
                const auto *node = sta.top();
                sta.pop();

                visit_function(node);

                if (node->right != nullptr)
                {
                    sta.push(node->right);
                }

                if (node->left != nullptr)
                {
                    sta.push(node->left);
                }
            } while (!sta.empty());
        }

        template<class visit_function_t>
        static constexpr void in_order_slow(const node_t *root,
            visit_function_t visit_function) // todo(p3): del
        {
            std::stack<const node_t *> sta;

            while (root != nullptr || !sta.empty())
            {
                if (nullptr == root)
                {
                    root = sta.top();
                    assert(root != nullptr);
                    sta.pop();

                    // A better function in_order, using a reference, is below.
                    visit_function(root);

                    root = root->right;
                }
                else
                {
                    sta.push(root);
                    root = root->left;
                }
            }
        }

        // Alles ist in Ordnung, oder?
        template<class visit_function_t>
        static constexpr void in_order(const node_t *root, visit_function_t visit_function)
        {
            std::stack<const node_t *> sta;

            while (root != nullptr || !sta.empty())
            {
                if (nullptr == root)
                {
                    root = sta.top();
                    assert(root != nullptr);
                    sta.pop();

                    visit_function(*root);

                    root = root->right;
                }
                else
                {
                    sta.push(root);
                    root = root->left;
                }
            }
        }

        static constexpr void print_tree(const node_t *root, std::string &str)
        {
            auto visit = [&str](const node_t &node)
            {
                str += std::to_string(node.key);
                str += ", ";
            };

            in_order(root, visit);

            str += "\n";
        }

        template<class visit_function_t, class node_two_ptr = node_t **>
        static constexpr void post_order(node_t *root, visit_function_t visit_function)
        {
            auto vis_2 = [&visit_function](node_two_ptr ppp) -> void
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    throw_if_null("post order.ppp", ppp);
                    throw_if_null("*(post order.ppp)", *ppp);
                }

                auto *node = *ppp;
                visit_function(node);
            };

            auto *p_root = &root;

            Standard::Algorithms::Trees::Inner::post_order_double_pointer_2<node_t, decltype(vis_2), node_two_ptr>(
                p_root, vis_2);
        }

        // Use the "post_order" which uses less memory.
        template<class visit_function_t> // = void(*)(node_t*)
        static constexpr void post_order_slow(node_t *root, visit_function_t visit_function)
        {
            throw_if_null_skip_non_pointer("visitFunction", visit_function);
            if (nullptr == root)
            {
                return;
            }

            std::stack<node_t *> result_stack;
            std::stack<node_t *> temp_stack;
            temp_stack.push(root);

            do
            {
                auto *top = temp_stack.top();
                temp_stack.pop();

                result_stack.push(top);

                if (nullptr != top->left)
                {
                    temp_stack.push(top->left);
                }

                if (nullptr != top->right)
                {
                    temp_stack.push(top->right);
                }
            } while (!temp_stack.empty());

            do
            {
                auto *node = result_stack.top();
                visit_function(node);
                result_stack.pop();
            } while (!result_stack.empty());
        }

private:
        static constexpr void verify_size(const std::size_t size)
        {
            constexpr auto max_size = std::numeric_limits<std::size_t>::max() >> 1U;

            if (0U == size || max_size <= size)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The tree size (" << size << ") must positive and smaller than " << max_size << ".";

                throw_exception<std::out_of_range>(str);
            }
        }

        [[nodiscard("new")]] static constexpr auto node_constructor() -> gsl::owner<node_t *>
        {
            return new node_t{}; // todo(p2): del it - no naked new.
        }

        template<class new_function_t = node_t *(*)()>
        static constexpr void new_node(
            const int_t &value, node_t **result, new_function_t new_function = &node_constructor)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                throw_if_null("New Node**", result);
                throw_if_not_null("New Node *result", *result);
            }

            auto *node = (*result) = new_function();
            assert(node != nullptr && node->left == nullptr && node->right == nullptr);

            node->value = value;
        }

        //"maxNodeCount" allows protecting against
        // a broken tree when there is a cycle.
        // inline static void CheckStackSize(const std::size_t size, const std::size_t maxNodeCount)
        //{
        //    if (size > maxNodeCount) [[unlikely]]
        //    {
        //        auto str = ::Standard::Algorithms::Utilities::w_stream();
        //        str << "The tree might be circular: stack size (" << size
        //            << ") has exceeded maximum value (" << maxNodeCount << ").";
        //        throw runtime_error(str.str());
        //    }
        //}
    };
} // namespace Standard::Algorithms::Trees
