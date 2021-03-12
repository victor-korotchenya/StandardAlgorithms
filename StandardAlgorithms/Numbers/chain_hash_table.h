#pragma once
#include"../Utilities/is_debug.h"
#include"hash_function.h"
#include<algorithm>
#include<cassert>
#include<forward_list>
#include<numeric>
#include<stdexcept>
#include<string>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Since the list elements might be stored anywhere in the memory, cache locality suffers.
    template<class key_t, class hash_func_t>
    requires(hash_function<hash_func_t, key_t>)
    struct chain_hash_table final
    {
        using list_t = std::forward_list<key_t>;

        constexpr explicit chain_hash_table(const std::size_t initial_capacity = 0U, hash_func_t hash_func = {})
            : Data(initial_capacity)
            , Hash_func(hash_func)
        {
            demand_consistency();
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Size;
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool
        {
            return 0U == Size;
        }

        [[nodiscard]] constexpr auto capacity() const noexcept -> std::size_t
        {
            return Data.size();
        }

        [[nodiscard]] constexpr auto contains(const key_t &key) const noexcept -> bool
        {
            auto has = !is_empty() && slot_prev_find(key).second;
            return has;
        }

        constexpr auto insert(const key_t &key) -> bool
        {
            [[maybe_unused]] const auto was_empty = is_empty();

            auto added = insert_impl(key, false);

            demand_consistency();

            assert((was_empty == added) || added);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                slow_check_size();
            }

            return added;
        }

        constexpr auto erase(const key_t &key) noexcept -> bool
        {
            if (is_empty())
            {
                return false;
            }

            const auto [slot_prev, exists] = slot_prev_find(key);
            if (!exists)
            {
                return false;
            }

            const auto [slot, prev_iter] = slot_prev;
            assert(0U < Size && slot < capacity());

            auto &items = Data[slot];

            assert(!items.empty() && prev_iter != items.end() && key == *std::next(prev_iter));

            items.erase_after(prev_iter);
            --Size;

            demand_consistency();

            if constexpr (::Standard::Algorithms::is_debug)
            {
                slow_check_size();
            }

            return true;
        }

        // Time O(capacity + Size).
        constexpr void clear() noexcept
        {
            demand_consistency();

            for (auto &items : Data)
            {
                items.clear();
            }

            Size = 0U;

            demand_consistency();
        }

private:
        // Precondition: the capacity must be positive.
        [[nodiscard]] constexpr auto slot_prev_find(const key_t &key) const noexcept
        {
            demand_consistency();

            const auto cap = capacity();
            assert(0U < cap);

            const auto slot = static_cast<std::size_t>(Hash_func(key)) % cap;
            const auto &items = Data[slot];
            const auto end_8 = items.end();

            for (auto prev = items.before_begin();;)
            {
                auto next_1 = std::next(prev);

                if (next_1 == end_8)
                {
                    return std::make_pair(std::make_pair(slot, prev), false);
                }

                if (*next_1 == key)
                {
                    return std::make_pair(std::make_pair(slot, prev), true);
                }

                prev = next_1;
            }
        }

        [[nodiscard]] constexpr auto insert_impl(const key_t &key, const bool is_expanding) -> bool
        {
            if (const auto draft_size = (Size << shift) | guarantree_non_zero_size; capacity() < draft_size)
                [[unlikely]]
            {// todo(p4): Remove these checks when expanding?
                if (is_expanding) [[unlikely]]
                {
                    throw std::runtime_error("Cannot expand the already expanding chain hash table.");
                }

                const auto new_size = std::max(guarantree_non_zero_size, capacity() << shift);
                expand_to_ensure_small_load_factor(new_size);
            }

            auto [slot_prev, exists] = slot_prev_find(key);
            if (exists)
            {
                return false;
            }

            const auto [slot, prev_iter] = slot_prev;
            assert(slot < capacity());

            auto &items = Data[slot];
            assert(prev_iter != items.end());

            items.insert_after(prev_iter, key);
            ++Size;

            assert(0U < Size);

            return true;
        }

        constexpr void expand_to_ensure_small_load_factor(const std::size_t new_size)
        {
            demand_consistency();

            if (const auto cap = capacity(); !(cap < new_size)) [[unlikely]]
            {
                auto error = "Too large hash table size " + std::to_string(cap);
                throw std::runtime_error(error);
            }

            auto old_data = Data;
            Data = std::vector<list_t>(new_size);
            // todo(p2): better exception guarantee - restore the old size, ..
            Size = 0U;

            for (auto &items : old_data)
            {
                for (auto &item : items)
                {
                    [[maybe_unused]] const auto added =
                        insert_impl(std::move(item), true); // todo(p3): insert_impl(&&key_t, bool)

                    assert(added);
                }
            }

            demand_consistency();
        }

        constexpr void demand_consistency() const noexcept
        {
            assert(Size <= capacity());
        }

        [[maybe_unused]] constexpr void slow_check_size() const noexcept
        {
            [[maybe_unused]] auto actual = std::accumulate(Data.begin(), Data.end(), std::size_t{},
                [] [[nodiscard]] (std::size_t prev_sum, const auto &items)
                {
                    auto list_count = std::distance(items.begin(), items.end());
                    return prev_sum + static_cast<std::size_t>(list_count);
                });

            assert(actual == Size);
        }

        // Assume that the load factor is 0.5.
        static constexpr auto shift = 1U;
        static constexpr std::size_t guarantree_non_zero_size = 1U;

        std::vector<list_t> Data;
        hash_func_t Hash_func;
        std::size_t Size{};
    };
} // namespace Standard::Algorithms::Numbers
