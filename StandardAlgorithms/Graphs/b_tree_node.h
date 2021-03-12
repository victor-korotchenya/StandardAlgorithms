#pragma once
#include"../Utilities/is_debug.h"
#include"../Utilities/project_constants.h"
#include"../Utilities/require_utilities.h"
#include<gsl/gsl>
#include<memory>
#include<vector>

namespace Standard::Algorithms::Trees
{
    // Sorted vector is used as the secondary structure.
    // todo(p2): AVL tree instead of vector.
    template<class key_t1, std::unsigned_integral int_t1>
    requires(std::is_nothrow_move_assignable_v<key_t1> && (sizeof(int_t1) <= sizeof(std::uint32_t)))
    struct b_tree_node final
    {
        using key_t = key_t1;
        using int_t = int_t1;

        static constexpr int_t lowest_min_degree = 2U;

        constexpr explicit b_tree_node(int_t min_degree)
            : Children(max_degree(min_degree))
            , Keys(Children.size() - 1ZU)
        {
        }

        b_tree_node(const b_tree_node &) = delete;
        auto operator= (const b_tree_node &) & -> b_tree_node & = delete;
        b_tree_node(b_tree_node &&) noexcept = delete;
        auto operator= (b_tree_node &&) &noexcept -> b_tree_node & = delete;

        constexpr ~b_tree_node() noexcept = default;

        [[nodiscard]] constexpr auto is_leaf() const &noexcept -> bool
        {
            const auto isl = Children[0] == nullptr;
            return isl;
        }

        [[nodiscard]] constexpr auto children() const &noexcept -> const std::vector<b_tree_node *> &
        {
            return Children;
        }

        [[nodiscard]] constexpr auto keys() const &noexcept -> const std::vector<key_t> &
        {
            return Keys;
        }

        [[nodiscard]] constexpr auto key_count() const &noexcept -> const int_t &
        {
            return Key_count;
        }

        [[nodiscard]] [[maybe_unused]] constexpr auto has_keys() const &noexcept -> bool
        {
            constexpr int_t zero{};

            return zero < Key_count;
        }

        [[nodiscard]] constexpr auto children() &noexcept -> std::vector<b_tree_node *> &
        {
            return Children;
        }

        [[nodiscard]] constexpr auto keys() &noexcept -> std::vector<key_t> &
        {
            return Keys;
        }

        [[nodiscard]] constexpr auto key_count() &noexcept -> int_t &
        {
            return Key_count;
        }

        [[nodiscard]] constexpr auto find(const key_t &key) const &noexcept -> const b_tree_node *
        {
            for (const b_tree_node *node = this;;)
            {
                assert(node != nullptr && node->has_keys() && node->Key_count <= node->Keys.size());

                const auto index = node->key_lower_bound(key);

                if (index < node->Key_count && node->Keys.at(index) == key)
                {
                    return node;
                }

                node = node->Children.at(index);

                if (node == nullptr)
                {
                    return nullptr;
                }
            }
        }

        [[nodiscard]] constexpr auto is_full(const int_t min_degree) const noexcept -> bool
        {
            const auto max_key_count = static_cast<int_t>(max_degree(min_degree) - int_t(1));

            assert(Key_count <= max_key_count);

            auto full = Key_count == max_key_count;
            return full;
        }

        [[nodiscard]] constexpr auto smallest_key() const &noexcept -> const key_t &
        {
            for (const auto *node = this;;)
            {
                assert(node != nullptr && node->has_keys());

                if (node->is_leaf())
                {
                    const auto &key = node->Keys.at(0);
                    return key;
                }

                node = node->Children.at(0);
            }
        }

        [[nodiscard]] constexpr auto smallest_key() &noexcept -> key_t &
        {
            const auto &cur = *this;

            // Want no copy-past.
            // NOLINTNEXTLINE
            auto &key = const_cast<key_t &>(cur.smallest_key());
            return key;
        }

        [[nodiscard]] constexpr auto largest_key() const &noexcept -> const key_t &
        {
            for (const auto *node = this;;)
            {
                assert(node != nullptr && node->has_keys());

                const auto cnt = node->Key_count;

                if (node->is_leaf())
                {
                    const auto pos = cnt - 1U;
                    const auto &key = node->Keys.at(pos);
                    return key;
                }

                node = node->Children.at(cnt);
            }
        }

        [[nodiscard]] constexpr auto largest_key() &noexcept -> key_t &
        {
            const auto &cur = *this;

            // Want no copy-past.
            // NOLINTNEXTLINE
            auto &key = const_cast<key_t &>(cur.largest_key());
            return key;
        }

        constexpr void split_full_child(const int_t min_degree, const std::int32_t child_index) &noexcept(false)
        {
            assert(!(child_index < 0) && std::uint32_t(child_index) < max_degree(min_degree) - 1U);

            auto *ptr = Children.at(child_index);
            assert(ptr);

            if (ptr == nullptr) [[unlikely]]
            {
                throw std::runtime_error("B node child at " + std::to_string(child_index) + " is null.");
            }

            auto &child1 = *ptr;
            auto child_two = std::make_unique<b_tree_node>(min_degree);

            split_full_child_safe(child_index, child1, min_degree, std::move(child_two));
        }

        constexpr void insert_into_not_full(const int_t min_degree, key_t &&key) &noexcept(false)
        {
            for (b_tree_node *node = this;;)
            {
                assert(node != nullptr && !node->is_full(min_degree));

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    [[maybe_unused]] const auto *const read_only_node = node;

                    assert(read_only_node != nullptr && read_only_node->has_keys());
                }

                auto pos = static_cast<std::int32_t>(node->Key_count - 1U);
                auto &nks = node->Keys;

                if (node->is_leaf())
                {
                    while (!(pos < 0) && key < nks[pos])
                    {// Shift greater keys to the right.
                        nks[pos + 1] = std::move(nks[pos]);
                        --pos;
                    }

                    nks[++pos] = std::move(key);
                    ++(node->Key_count);
                    return;
                }

                while (!(pos < 0) && key < nks[pos])
                {
                    --pos;
                }

                ++pos;

                assert(!(pos < 0) && std::uint32_t(pos) < max_degree(min_degree) - 1U);

                auto *child = node->Children.at(pos);
                assert(child);

                if (child->is_full(min_degree))
                {
                    node->split_full_child(min_degree, pos);
                    pos += nks[pos] < key ? 1 : 0;
                }

                node = node->Children.at(pos);
            }
        }

        [[maybe_unused]] constexpr auto erase(const int_t min_degree, const key_t &key
#if _DEBUG
            ,
            std::uint32_t depth = {}
#endif
            ) & -> bool
        {
            check_degree(min_degree);

#if _DEBUG
            if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
            {
                throw std::runtime_error("Too deep stack in b node erase.");
            }
#endif

            for (auto *node = this;;)
            {
                assert(node != nullptr && node->has_keys());

                auto index = node->key_lower_bound(key);

                if (index < node->Key_count && node->Keys.at(index) == key)
                {
                    if (node->is_leaf())
                    {
                        for (auto ind_2 = index + 1LLU; ind_2 < node->Key_count; ++ind_2)
                        {// Move left.
                            node->Keys[ind_2 - 1U] = std::move(node->Keys[ind_2]);
                        }

                        node->Key_count--;
                        // todo(p3): The deleted key destructor might actually not be called here.
                    }
                    else
                    {
                        node->erase_from_non_leaf_child(min_degree, index, key
#if _DEBUG
                            ,
                            depth
#endif
                        );
                    }

                    return true;
                }

                if (node->is_leaf())
                {
                    return false;
                }

                if (auto *child = node->Children[index]; child->Key_count < min_degree)
                {
                    node->remedy_underflow(min_degree, index);
                }

                index -= node->Key_count < index ? 1U : 0U;
                node = node->Children.at(index);
            }
        }

private:
        static constexpr auto check_degree(const int_t &min_degree) noexcept -> const int_t &
        {
            assert(!(min_degree < lowest_min_degree) && min_degree < min_degree * static_cast<int_t>(2));

            return min_degree;
        }

        [[nodiscard]] inline static constexpr auto max_degree(const int_t min_degree) noexcept -> int_t
        {
            check_degree(min_degree);

            auto maxd = static_cast<int_t>(min_degree << 1U);
            assert(int_t{} < min_degree && min_degree < maxd);

            return maxd;
        }

        // Assume min degree = 3, then max degree = max children.size() = 6,
        // child1 is full, and has 5 keys:
        // - 2 of those at indexes 0, 1 are left untouched.
        // - 1 key at the middle 2 and the child pointer are moved to this node, the parent of both child1 and
        // child_two.
        // - 2 keys at indexes 3, 4 are to be moved to child_two, a new node.
        // If child1 is not a leaf, the 3 highest of 6 children are also to be transferred to child_two.
        constexpr void split_full_child_safe(const std::int32_t child_index, b_tree_node &child1,
            const int_t min_degree, std::unique_ptr<b_tree_node> &&child_two) noexcept
        {
            assert(!is_full(min_degree) && child1.is_full(min_degree));

            auto *child2 = child_two.get();

            assert(!(child_index < 0) && std::uint32_t(child_index) < max_degree(min_degree) - 1U && child2);

            const auto middle = static_cast<int_t>(min_degree - int_t(1));
            child2->Key_count = child1.Key_count = middle;

            for (std::uint32_t index{}; index < middle; ++index)
            {
                child2->Keys[index] = std::move(child1.Keys[min_degree + index]);
            }

            if (!child1.is_leaf())
            {
                for (std::uint32_t index{}; index < min_degree; ++index)
                {
                    auto &source = child1.Children[min_degree + index];
                    child2->Children[index] = source;
                    source = nullptr;
                }
            }

            // Let Key_count = 4, child_index = 1.
            // In order to make place for the child1 middle:
            // - keys at indexes 1,2,3 must be shifted to 2,3,4;
            // - children at indexes 2,3,4 to 3,4,5.
            for (std::int32_t index = Key_count; child_index + 1 <= index; --index)
            {// Insertion into a vector might require shifting - poor speed.
                Keys[index] = std::move(Keys[index - 1]);
                Children[index + 1] = Children[index];
            }

            Keys.at(child_index) = std::move(child1.Keys[middle]);
            Children[child_index + 1] = child_two.release();

            ++Key_count;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                [[maybe_unused]] const auto &read_only_node = *this;

                assert(read_only_node.has_keys());
            }
        }

        [[nodiscard]] constexpr auto key_lower_bound(const key_t &key) const &noexcept -> std::uint32_t
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                [[maybe_unused]] const auto &read_only_node = *this;

                assert(read_only_node.Key_count <= read_only_node.Keys.size());
            }

            std::uint32_t index{};

            while (index < Key_count && Keys[index] < key)
            {
                ++index;
            }

            return index;
        }

        constexpr void erase_from_non_leaf_child(const int_t min_degree, const std::uint32_t index, const key_t &key
#if _DEBUG
            ,
            const std::uint32_t depth
#endif
        )
        {
            auto &key0 = Keys.at(index);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                [[maybe_unused]] const auto &read_only_node = *this;

                assert(read_only_node.has_keys());
                assert(index < read_only_node.Keys.size());
            }

            assert(key == key0);

            auto *child = Children[index + 1U];
            assert(child != nullptr && child->has_keys());

            if (min_degree <= child->Key_count)
            {
                auto &key1 = child->smallest_key();
                assert(!(key1 < key));

                std::swap(key0, key1);
            }
            else if (child = Children[index]; child != nullptr && min_degree <= child->Key_count)
            {
                auto &key2 = child->largest_key();
                assert(!(key < key2));

                std::swap(key0, key2);
            }
            else
            {// Left and right child each has (min degree - 1) keys.
                join_key_next_child(min_degree, index);

                assert(child != nullptr);

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    [[maybe_unused]] const auto &readonly_node = *this;
                    [[maybe_unused]] const auto &raw_child = *child;

                    assert(child == readonly_node.Children.at(index) && raw_child.is_full(min_degree));
                    {
                        [[maybe_unused]] const auto &key3 = raw_child.Keys.at(min_degree - 1U);

                        assert(key == key3);
                    }
                }
            }

#if _DEBUG
            {
                const auto is_erased = child->erase(min_degree, key, depth + 1);
                assert(is_erased);
            }
#else
            child->erase(min_degree_, key);
#endif
        }

        constexpr void join_key_next_child(const int_t min_degree, const std::uint32_t index) &
        {
            // todo(p1): Fix Clang warning. Make the children nodes a vector of gsl::owner<b_tree_node*>, or ?
            gsl::owner<b_tree_node *> next_chi = Children.at(index + 1U);

            auto *child = Children[index];

            assert(int_t{} < min_degree && next_chi != nullptr && child != nullptr && next_chi != child);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                [[maybe_unused]] const auto &read_only_node = *this;

                assert(read_only_node.has_keys());
            }

            child->Keys[min_degree - 1U] = std::move(Keys[index]);

            for (std::uint32_t ind_2{}; ind_2 < next_chi->Key_count; ++ind_2)
            {
                child->Keys.at(ind_2 + min_degree) = std::move(next_chi->Keys[ind_2]);
            }

            for (auto ind_2 = index + 1U; ind_2 < Key_count; ++ind_2)
            {
                Keys[ind_2 - 1U] = std::move(Keys[ind_2]);
                Children[ind_2] = Children[ind_2 + 1U];
            }

            {
                auto *&last = Children[Key_count];
                assert(last != nullptr);

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    [[maybe_unused]] const auto &read_only_node = *this;

                    assert(read_only_node.Key_count + 1U == max_degree(min_degree) ||
                        (read_only_node.Key_count + 1U < max_degree(min_degree) &&
                            read_only_node.Children.at(read_only_node.Key_count + 1U) == nullptr));
                }

                last = nullptr;
            }

            --Key_count;
            child->Key_count += 1U + next_chi->Key_count;

            if (!child->is_leaf())
            {
                for (std::uint32_t ind_2{}; ind_2 <= next_chi->Key_count; ++ind_2)
                {
                    child->Children.at(ind_2 + min_degree) = next_chi->Children[ind_2];
                }
            }

            delete next_chi;
        }

        constexpr void remedy_underflow(const int_t min_degree, std::uint32_t index) &
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                [[maybe_unused]] const auto &read_only_node = *this;

                assert(read_only_node.has_keys());
            }

            if (0U < index && min_degree <= Children[index - 1U]->Key_count)
            {
                prev_key_transfer(min_degree, index);
            }
            else if (index < Key_count && min_degree <= Children[index + 1U]->Key_count)
            {
                next_key_transfer(min_degree, index);
            }
            else
            {// Too few keys: join both siblings.
                index -= index == Key_count ? 1U : 0U;
                join_key_next_child(min_degree, index);
            }
        }

        constexpr void prev_key_transfer(const int_t min_degree, const std::uint32_t index) &
        {
            auto *pre = Children.at(index - 1U);

            assert(0U < index && pre != nullptr);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                {
                    [[maybe_unused]] const auto &read_only_node = *this;

                    assert(read_only_node.has_keys());
                }
                {
                    [[maybe_unused]] const auto &raw_pre = *pre;

                    assert(min_degree <= raw_pre.Key_count);
                }
            }

            auto *nex = Children.at(index);

            assert(nex != nullptr && pre->is_leaf() == nex->is_leaf() && nex->Key_count < min_degree);

            auto &nks = nex->Keys;

            for (auto ind_2 = nex->Key_count; 0U < ind_2; --ind_2)
            {// Move right.
                nks.at(ind_2) = std::move(nks[ind_2 - 1U]);
            }

            auto &cur_key = Keys[index - 1U];
            nks[0] = std::move(cur_key);

            // Having transfered a key from this current node to next,
            // do from previous to this.
            cur_key = std::move(pre->Keys[pre->Key_count - 1U]);

            if (!nex->is_leaf())
            {
                auto &n_ch = nex->Children;

                for (auto ind_2 = nex->Key_count;;)
                {// Move right.
                    n_ch[ind_2 + 1U] = n_ch[ind_2];

                    if (ind_2-- == 0U)
                    {
                        break;
                    }
                }

                auto *&last = pre->Children[pre->Key_count];
                n_ch[0] = last;
                last = nullptr;
            }

            --(pre->Key_count);
            ++(nex->Key_count);
        }

        constexpr void next_key_transfer(const int_t min_degree, const std::uint32_t index) &
        {
            auto *nex = Children.at(index + 1U);
            auto *pre = Children[index];

            assert(nex != nullptr && pre != nullptr);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                {
                    [[maybe_unused]] const auto &read_only_node = *this;

                    assert(read_only_node.has_keys());
                }
                {
                    [[maybe_unused]] const auto &raw_nex = *nex;

                    assert(min_degree <= raw_nex.Key_count);

                    [[maybe_unused]] const auto &raw_pre = *pre;

                    assert(raw_pre.is_leaf() == raw_nex.is_leaf() && raw_pre.Key_count < min_degree);
                }
            }

            {
                auto &nks = nex->Keys;
                {
                    auto &cur_key = Keys[index];
                    auto &pks = pre->Keys;

                    pks.at(pre->Key_count) = std::move(cur_key);
                    cur_key = std::move(nks[0]);
                }

                for (auto ind_2 = 1U; ind_2 < nex->Key_count; ++ind_2)
                {// Move left.
                    nks[ind_2 - 1U] = std::move(nks[ind_2]);
                }
            }

            if (!pre->is_leaf())
            {
                auto &n_chi = nex->Children;
                pre->Children.at(pre->Key_count + 1U) = n_chi[0];

                for (auto ind_2 = 1U; ind_2 <= nex->Key_count; ++ind_2)
                {
                    n_chi[ind_2 - 1U] = n_chi[ind_2];
                }

                n_chi[nex->Key_count] = nullptr;
                assert(n_chi[0] != nullptr);
            }

            --(nex->Key_count);
            ++(pre->Key_count);
        }

        std::vector<b_tree_node *> Children;
        std::vector<key_t> Keys;
        int_t Key_count{};
    };

    namespace Inner
    {
        template<class key_t, class int_t, class node_t = b_tree_node<key_t, int_t>>
        constexpr auto validate_b_tree_impl(const key_t &min, const node_t &node, const key_t &max,
            std::uint32_t depth = {}, const bool is_root = true) -> std::size_t
        {
            if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
            {
                throw std::runtime_error("Too deep stack in B tree validation.");
            }

            constexpr int_t zero{};

            const auto &key_count = node.key_count();

            if (key_count == zero) [[unlikely]]
            {
                throw std::runtime_error("B tree node has 0 key count.");
            }

            assert(node.has_keys());

            if (const auto min_key_count = node.keys().size() >> 1U; !is_root && key_count < min_key_count) [[unlikely]]
            {
                throw std::runtime_error("B tree non-root node has " + std::to_string(key_count) + " keys < " +
                    std::to_string(min_key_count) + " min count.");
            }

            {
                const auto ksize = node.keys().size();
                if (ksize < key_count) [[unlikely]]
                {
                    throw std::runtime_error(
                        "B tree size " + std::to_string(ksize) + " < " + std::to_string(key_count) + " node keys.");
                }

                if (const auto chi_s = node.children().size(); ksize + 1U != chi_s) [[unlikely]]
                {
                    throw std::runtime_error(std::string("B tree node keys ") + std::to_string(ksize) +
                        " + 1 != " + std::to_string(chi_s) + " children size.");
                }
            }

            for (std::uint32_t index{}; index < key_count; ++index)
            {
                const auto &key = node.keys().at(index);

                if (key < min || max < min || max < key) [[unlikely]]
                {
                    throw std::runtime_error("Error at b tree node key '" + std::to_string(key) + "', max '" +
                        std::to_string(max) + "', min '" + std::to_string(min) + "'");
                }
            }

            std::size_t count = key_count;

            const auto null_index = node.is_leaf() ? 0U : key_count + 1U;

            for (std::uint32_t index{}; index < node.children().size(); ++index)
            {
                const auto &chi = node.children()[index];

                if (null_index <= index)
                {
                    if (chi == nullptr) [[likely]]
                    {
                        continue;
                    }

                    throw std::runtime_error("B tree node children[" + std::to_string(index) +
                        "] is not null; key count " + std::to_string(key_count) + "; is leaf " +
                        std::to_string(node.is_leaf()));
                }

                if (chi == nullptr) [[unlikely]]
                {
                    throw std::runtime_error("B tree node children[" + std::to_string(index) + "] is null; key count " +
                        std::to_string(key_count) + "; is leaf " + std::to_string(node.is_leaf()));
                }

                const auto &min2 = index == 0U ? min : node.keys().at(index - 1U);
                const auto &max2 = index < key_count ? node.keys().at(index) : max;

                // todo(p3): overflow check.
                count += validate_b_tree_impl<key_t, int_t, node_t>(min2, *chi, max2, depth, false);
            }

            return count;
        }
    } // namespace Inner

    template<class key_t, class int_t, class node_t = b_tree_node<key_t, int_t>>
    requires(sizeof(std::int32_t) <= sizeof(std::size_t))
    [[nodiscard]] constexpr auto validate_b_tree_return_size(const gsl::strict_not_null<const node_t *> root)
        -> std::size_t
    {
        if (root == nullptr || !root->has_keys()) [[unlikely]]
        {
            throw std::runtime_error("B tree root node is null or has 0 key count.");
        }

        const auto &min_key = root->smallest_key();
        const auto &max_key = root->largest_key();

        auto count = Inner::validate_b_tree_impl<key_t, int_t, node_t>(min_key, *root, max_key);

        return count;
    }

    namespace Inner
    {
        template<class key_t, class int_t, class node_t>
        constexpr void free_b_tree_not_null(gsl::owner<node_t *> node
#if _DEBUG
            ,
            std::uint32_t depth = {}
#endif
            ) noexcept
        {
            assert(node != nullptr);

#if _DEBUG
            ++depth;
            assert(0U < depth && depth <= ::Standard::Algorithms::Utilities::stack_max_size);
#endif

            if constexpr (::Standard::Algorithms::is_debug)
            {
                for (std::size_t index{}; index < node->children().size(); ++index)
                {
                    const auto &chi = node->children()[index];
                    const auto is_null = chi == nullptr;
                    const auto expected = node->is_leaf() || node->key_count() < index;

                    assert(is_null == expected);
                }
            }

            if (!node->is_leaf())
            {
                for (std::size_t index{}; index <= node->key_count(); ++index)
                {
                    auto *chi = node->children().at(index);
                    assert(chi != nullptr);

                    free_b_tree_not_null<key_t, int_t, node_t>(chi
#if _DEBUG
                        ,
                        depth
#endif
                    );
                }
            }

            delete node;
        }
    } // namespace Inner

    template<class key_t, class int_t, class node_t>
    constexpr void free_b_tree(gsl::owner<node_t *> root) noexcept
    {
        if (nullptr != root)
        {
            Inner::free_b_tree_not_null<key_t, int_t, node_t>(root);
        }
    }
} // namespace Standard::Algorithms::Trees
