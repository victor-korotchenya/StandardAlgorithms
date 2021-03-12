#pragma once
#include"../Numbers/max_probability.h"
#include"../Utilities/floating_point_type.h"
#include"../Utilities/is_debug.h"
#include"../Utilities/random.h"
#include"../Utilities/throw_exception.h"
#include<cstdint>
#include<gsl/gsl>
#include<memory>

namespace Standard::Algorithms::Graphs
{
    enum class value_kind : std::uint8_t
    {
        min,
        max,
        usual,
    };

    // Skip list is similar to BST, but uses randomization.
    template<class t_key, class t_value, std::int32_t max_levels>
    struct skip_list final
    {
        struct node_t;
        using nexts_t = std::array<node_t *, max_levels>;

        using floating_t = Standard::Algorithms::floating_point_type;
        using floating_rand_t = Standard::Algorithms::Utilities::random_t<floating_t>;

        struct node_t final
        {
            t_key key{};
            t_value value{};
            nexts_t nexts{};
        };

        constexpr skip_list(const t_key &min_key, const floating_t &probability, const t_key &max_key)
            : Probability(require_positive(probability, "probability"))
#ifdef _DEBUG
            , Min_key_debug(min_key)
            , Max_key_debug(max_key)
#endif
        {
            {
                constexpr auto min1 = 2;
                constexpr auto max1 = 1024;

                static_assert(min1 <= max_levels && max_levels <= max1);
            }
            {
                require_greater(::Standard::Algorithms::Numbers::max_probability, probability, "probability");
            }

            Head.key = min_key;
            Tail.key = max_key;

            reset_head_tail();
        }

        skip_list(const skip_list &) = delete;
        auto operator= (const skip_list &) & -> skip_list & = delete;
        skip_list(skip_list &&) noexcept = delete;
        auto operator= (skip_list &&) &noexcept -> skip_list & = delete;

        // todo(p4): constexpr
        ~skip_list() noexcept
        {
            clear();
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Size;
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool
        {
            return 0U == Size;
        }

        [[nodiscard]] constexpr auto find(const t_key &key) const -> const t_value *
        {
            verify_order("Before find.");

            const auto value_kind = get_value_kind(key);

            const auto *const result = value_kind::usual == value_kind ? find_usual(key) : find_special(value_kind);

            return result;
        }

        // todo(p3): use shared_ptr<> to make it safe!
        constexpr void add(const t_key &key, const t_value &value)
        {
            verify_order("Before add.");

            const auto value_kind = get_value_kind(key);

            if (value_kind::usual == value_kind) [[likely]]
            {
                add_usual(key, value);
            }
            else
            {
                add_special(value, value_kind);
            }

            verify_order("After add.");
        }

        constexpr auto erase(const t_key &key) -> bool
        {
            verify_order("Before delete.");

            const auto value_kind = get_value_kind(key);
            auto result = value_kind::usual == value_kind ? delete_usual(key) : delete_special(value_kind);

            verify_order("After delete.");

            return result;
        }

        constexpr void clear() noexcept
        {
            auto *current = Head.nexts[0];

            while (&Tail != current)
            {
                assert(current != nullptr);

                Nobody = static_cast<gsl::owner<node_t *>>(current);
                current = current->nexts[0];

                delete Nobody;
            }

            Size = {};
            Max_current_level = {};
            Has_min_max_keys = {};

            reset_head_tail();
        }

        template<class visit_function_t>
        // bool(*)(const TKey& key, const TValue& value);
        constexpr void visit(visit_function_t visit_function) const
        {
            throw_if_null_skip_non_pointer("The 'visitFunction' must be not null.", visit_function);

            auto shall_continue = true;

            if (Has_min_max_keys[0])
            {
                shall_continue = visit_function(Head.key, Head.value);
            }

            const auto *current = Head.nexts[0];

            while (shall_continue && (&Tail) != current)
            {
                throw_if_null("current in visit", current);

                shall_continue = visit_function(current->key, current->value);
                current = current->nexts[0];
            }

            if (shall_continue && Has_min_max_keys[1])
            {
                shall_continue = visit_function(Tail.key, Tail.value);

                // NOLINTNEXTLINE NOLINT
                if (shall_continue)
                {
                }
            }
        }

private:
        constexpr void add_usual(const t_key &key, const t_value &value)
        {
            nexts_t nexts{};

            if (const auto *const current = &(find_closest(key, &nexts)); key == current->key)
            {
                if (value == current->value)
                {
                    return;
                }

                throw_add_key(current->value, key, value);
                return;
            }

            if (const auto newlevel = random_level(); Max_current_level < newlevel)
            {
                for (std::int32_t level = Max_current_level + 1; level <= newlevel; ++level)
                {
                    nexts[level] = &Head;
                }

                Max_current_level = newlevel;
            }

            auto uni = std::make_unique<node_t>();
            auto *new_node = uni.get();
            assert(new_node != nullptr);

            new_node->key = key;
            new_node->value = value;

            set_nexts(*new_node, nexts);

            uni.release();

            ++Size;
            assert(0U < Size);
        }

        constexpr void set_nexts(node_t &node, nexts_t &nexts) const noexcept
        {
            for (std::int32_t level{}; level <= Max_current_level; ++level)
            {
                auto *nex = nexts[level];
                assert(nex != nullptr);

                auto *&ne_le = nex->nexts[level];
                assert(ne_le != nullptr);

                node.nexts[level] = ne_le;
                ne_le = &node;
            }
        }

        [[nodiscard]] constexpr auto find_usual(const t_key &key) const -> const t_value *
        {
            const auto *current = &(find_closest(key));

            const auto *const result = key == current->key ? &(current->value) : nullptr;

            return result;
        }

        constexpr auto delete_usual(const t_key &key) -> bool
        {
            nexts_t nexts{};

            const auto &node = find_closest(key, &nexts);

            // NOLINTNEXTLINE
            auto *current = const_cast<node_t *>(&node);

            if (!(key == current->key))
            {
                return false;
            }

            for (std::int32_t level{}; level <= Max_current_level; ++level)
            {
                auto *nex = nexts[level];
                assert(nex != nullptr);

                auto *&ne_le = nex->nexts[level];
                assert(ne_le != nullptr);

                if (current != ne_le)
                {
                    break;
                }

                ne_le = current->nexts[level];
            }

            Nobody = static_cast<gsl::owner<node_t *>>(current);
            delete Nobody;

            assert(0U < Size);
            --Size;

            fix_useless_levels();

            return true;
        }

        constexpr void fix_useless_levels()
        {
            while (0 < Max_current_level && (&Tail) == Head.nexts[Max_current_level])
            {
                --Max_current_level;
            }
        }

        constexpr void add_special(const t_value &value, const value_kind value_kind)
        {
            const auto index = static_cast<std::int32_t>(value_kind);
            auto &ref1 = value_kind::min == value_kind ? Head : Tail;

            if (Has_min_max_keys.at(index))
            {
                if (value == ref1.value)
                {
                    return;
                }

                throw_add_key(ref1.value, ref1.key, value);
                return;
            }

            ref1.value = value;
            Has_min_max_keys.at(index) = true;
            ++Size;
            assert(0U < Size);
        }

        [[nodiscard]] constexpr auto find_special(const value_kind value_kind) const -> const t_value *
        {
            const auto index = static_cast<std::int32_t>(value_kind);
            const auto &ref1 = value_kind::min == value_kind ? Head : Tail;

            if (Has_min_max_keys.at(index))
            {
                return &(ref1.value);
            }

            return nullptr;
        }

        [[nodiscard]] constexpr auto delete_special(const value_kind value_kind) -> bool
        {
            const auto index = static_cast<std::int32_t>(value_kind);
            if (!Has_min_max_keys.at(index))
            {
                return false;
            }

            Has_min_max_keys.at(index) = false;

            assert(0U < Size);
            --Size;

            return true;
        }

        [[nodiscard]] constexpr auto find_closest(const t_key &key, nexts_t *nexts = nullptr) const -> const node_t &
        {
            const auto *result = &Head;

            for (std::int32_t level = Max_current_level; 0 <= level; --level)
            {
                assert(result != nullptr && result->nexts[level] != nullptr);

                while (result->nexts[level]->key < key)
                {
                    result = result->nexts[level];
                    throw_if_null("result in FindClosest", result);
                }

                if (nexts != nullptr)
                {
                    (*nexts)[level] =
                        // NOLINTNEXTLINE
                        const_cast<node_t *>(result);
                }
            }

            assert(result != nullptr);

            result = throw_if_null("final result in FindClosest", result->nexts[0]);

            return *result;
        }

        [[nodiscard]] constexpr auto random_level() -> std::int32_t
        {
            auto level = 1;

            while (level < max_levels
                // It does not make sense to go too far.
                && level <= Max_current_level + 1 && Rnd() < Probability)
            {
                ++level;
            }

            return level - 1;
        }

        constexpr void reset_head_tail() noexcept
        {
            for (std::int32_t level{}; level < max_levels; ++level)
            {
                Head.nexts[level] = &Tail;

                // Must never get there.
                Tail.nexts[level] = nullptr;
            }
        }

        [[nodiscard]] constexpr auto get_value_kind(const t_key &key) const noexcept -> value_kind
        {
            if (key == Head.key)
            {
                return value_kind::min;
            }

            if (key == Tail.key)
            {
                return value_kind::max;
            }

            return value_kind::usual;
        }

        [[noreturn]] static constexpr void throw_add_key(
            const t_value &old_value, const t_key &key, const t_value &new_value)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The key (" << key << ") has the old value (" << old_value
                << ") while trying to insert a new different value(" << new_value << ").";
            throw_exception(str);
        }

        [[maybe_unused]] [[nodiscard]] constexpr auto verify_lowest_level_debug(const char *const message) const
            -> std::size_t
        {
            assert(message != nullptr);

            std::size_t result{};

            auto previous_key = Head.key;
            auto *current = throw_if_null("VerifyLowestLevel_Debug Head.nexts", Head.nexts[0]);

            while (&Tail != current)
            {
                if (current->key <= previous_key)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error: current->key (" << current->key << ") <= previousKey (" << previous_key
                        << ") at index " << result << ". " << message;

                    throw_exception(str);
                }

                previous_key = current->key;
                ++result;

                current = throw_if_null("VerifyLowestLevel_Debug current", current->nexts[0]);
            }

            return result;
        }

        constexpr void verify_order([[maybe_unused]] const char *const message) const
        {
            if constexpr (!::Standard::Algorithms::is_debug)
            {
                return;
            }

            assert(message != nullptr);

#if _DEBUG
            if (Min_key_debug != Head.key)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error: (" << Min_key_debug << ") min key debug != Head.key(" << Head.key << "). " << message;

                throw_exception(str);
            }

            if (Max_key_debug != Tail.key)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error: (" << Max_key_debug << ") max key debug != Tail.key(" << Tail.key << "). " << message;

                throw_exception(str);
            }

            auto index = verify_lowest_level_debug(message);
            if (Has_min_max_keys[0])
            {
                ++index;
            }

            if (Has_min_max_keys[1])
            {
                ++index;
            }

            if (Size != index)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error: (" << Size << ") Size != index (" << index << "). " << message;

                throw_exception(str);
            }
#endif
        }

        const floating_t Probability;

        std::size_t Size{};

        // Can go from 0 up to (max_levels - 1) inclusively.
        std::int32_t Max_current_level{};

        floating_rand_t Rnd = floating_rand_t(0.0, 1.0);

        // Minus infinity.
        node_t Head{};

        // Plus infinity.
        // It allows avoiding an extra comparison with null.
        node_t Tail{};

        // Min and max key insertions require special handling - for now.
        std::array<bool, 2> Has_min_max_keys{};

        gsl::owner<node_t *> Nobody{}; // Nobody matters too.

#ifdef _DEBUG
        t_key Min_key_debug;
        t_key Max_key_debug;
#endif
    };
} // namespace Standard::Algorithms::Graphs
