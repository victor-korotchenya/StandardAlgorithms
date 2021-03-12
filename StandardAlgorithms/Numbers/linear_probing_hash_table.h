#pragma once
#include"hash_function.h"
#include<algorithm>
#include<cassert>
#include<stdexcept>
#include<string>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Good for CPU cache locality.
    template<class key_t, class hash_func_t>
    requires(hash_function<hash_func_t, key_t>)
    struct linear_probing_hash_table final
    {
        constexpr explicit linear_probing_hash_table(
            const std::size_t initial_capacity = 0U, hash_func_t hash_func = {})
            : Data(initial_capacity == 0U ? 0U : std::max(initial_capacity, guarantee_one_empty_slot))
            , Used_bits(Data.size())
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
            auto has = !is_empty() && slot_find(key).second;
            return has;
        }

        constexpr auto insert(const key_t &key) -> bool
        {
            [[maybe_unused]] const auto was_empty = is_empty();

            auto added = insert_impl(key, false);
            assert((was_empty == added) || added);

            demand_consistency();

            if constexpr (::Standard::Algorithms::is_debug)
            {
                slow_check_size();
            }

            return added;
        }

        constexpr auto erase(const key_t &key) -> bool
        {
            if (is_empty())
            {
                return false;
            }

            auto [slot, exists] = slot_find(key);
            if (!exists)
            {
                return false;
            }

            const auto cap = capacity();

            assert(0U < Size && Size <= cap && slot < cap && Used_bits[slot]);

            Data.at(slot) = {}; // todo(p2). call the destructor
            Used_bits.at(slot) = false;
            --Size;

            wipe_off_cluster(slot);
            demand_consistency();

            if constexpr (::Standard::Algorithms::is_debug)
            {
                slow_check_size();
            }

            return true;
        }

        constexpr void clear() // todo(p3): It should have been 'noexcept ', but it might be not?
                               // noexcept
        {
            demand_consistency();

            // todo(p2). call the destructors in the Data?
            Used_bits.assign(Used_bits.size(), false);
            Size = 0U;

            demand_consistency();
        }

private:
        [[nodiscard]] static constexpr auto min_size(const std::size_t old_size) noexcept -> std::size_t
        {
            auto new_size = std::max(guarantee_one_empty_slot, old_size << 1U);

            assert(old_size < new_size);

            return new_size;
        }

        [[nodiscard]] static constexpr auto disdance(
            const std::size_t original, const std::size_t cap, const std::size_t slut) noexcept -> std::size_t
        {
            const auto is_slut = slut < original;
            auto disco = slut - original + (is_slut ? cap : 0U);

            assert(original < cap && slut < cap && disco < cap);

            return disco;
        }

        // Precondition: the capacity must be positive.
        [[nodiscard]] constexpr auto slot_find(const key_t &key) const noexcept
            // slot, whether found
            -> std::pair<std::size_t, bool>
        {
            demand_consistency();

            const auto cap = capacity();
            assert(0U < cap);

            const auto original_hash = static_cast<std::size_t>(Hash_func(key)) % cap;

            if (Size == 0U)
            {
                return { original_hash, false };
            }

            std::size_t count1{};
            auto slot1 = original_hash;

            for (;;)
            {
                assert(slot1 < cap);

                if (!Used_bits.at(slot1))
                {
                    return { slot1, false };
                }

                if (Data.at(slot1) == key)
                {
                    return { slot1, true };
                }

                if (!(++count1 < cap)) [[unlikely]]
                {// All slots have been taken - it should have never happened.
                    assert(false);

                    return { original_hash, false };
                }

                if (++slot1 == cap)
                {
                    slot1 = {};
                }
            }
        }

        [[nodiscard]] constexpr auto insert_impl(const key_t &key, const bool is_expanding) -> bool
        {
            if (const auto draft_size = min_size(Size); capacity() < draft_size) [[unlikely]]
            {
                if (is_expanding) [[unlikely]]
                {
                    throw std::runtime_error("Cannot expand the already expanding linear probing hash table.");
                }

                const auto new_size = min_size(capacity());
                expand_to_ensure_small_load_factor(new_size);
            }

            auto [slot, exists] = slot_find(key);
            if (exists)
            {
                return false;
            }

            assert(slot < capacity());

            if (Used_bits.at(slot)) [[unlikely]]
            {
                assert(false);

                throw std::runtime_error("Inner error. All slots are taken in a linear probe hash table.");
            }

            Data.at(slot) = key;
            Used_bits.at(slot) = true;
            ++Size;

            assert(0U < Size);

            return true;
        }

        constexpr void expand_to_ensure_small_load_factor(const std::size_t new_size)
        {// Avoid a gigantic cluster-f@ck.
            demand_consistency();

            if (const auto cap = capacity(); !(cap < new_size)) [[unlikely]]
            {
                auto error = "Too large hash table size " + std::to_string(cap);
                throw std::runtime_error(error);
            }

            auto old_data = Data;
            const auto old_used_bits = Used_bits;

            std::vector<key_t> new_data(new_size);
            std::vector<bool> new_used_bits(new_size);

            std::swap(new_data, Data);
            std::swap(new_used_bits, Used_bits);

            // todo(p2): better exception guarantee - restore the old size, ..
            Size = 0U;

            for (std::size_t index{}; index < old_used_bits.size(); ++index)
            {
                if (!old_used_bits[index])
                {
                    continue;
                }

                auto &old = old_data[index];

                [[maybe_unused]] const auto added =
                    insert_impl(std::move(old), true); // todo(p3): insert_impl(&&key_t, bool)

                assert(added);
            }

            demand_consistency();
        }

        constexpr void wipe_off_cluster(std::size_t slot)
        {
            // Add: 0a, 1b, 0c, 2d.
            // 0a, 1b, 0c, 2d ; before 0a erasion
            // _  , 1b, 0c, 2d
            // 0c , 1b, _ , 2d
            // 0c , 1b , 2d, _ ; after

            // Add: 0a, 1b, 1c.
            // 0a, 1b, 1c ; before 0a erasion
            // _  , 1b, 1c ; after

            // Let cap = 10.
            // Add: 8a, 9b,   8c, 0d, 9e.
            // 8a, 9b,   8c, 0d, 9e ; before 8a erasion
            // _  , 9b,   8c, 0d, 9e
            // 8c, 9b,   _  , 0d, 9e
            // 8c, 9b,   0d, _  , 9e
            // 8c, 9b,   0d, 9e ,  _ ; after

            // Add: 8a, 9b,   0c, 1d, 1e, 7f.
            // 8a, 9b,   0c, 1d, 1e, 7f ; before 8a erasion
            // _  , 9b,   0c, 1d, 1e, 7f ; after

            const auto cap = capacity();
            assert(Size < cap); // Now should be at least 2 free slots.

            for (;;)
            {
                assert(slot < cap && !Used_bits.at(slot));

                auto slot_2 = slot;

                for (;;)
                {
                    if (++slot_2 == cap)
                    {
                        slot_2 = {};
                    }

                    assert(slot != slot_2 && slot_2 < cap);

                    if (!Used_bits.at(slot_2))
                    {
                        return;
                    }

                    assert(0U < Size && Size < cap);

                    const auto &key_2 = Data.at(slot_2);
                    const auto original_hash = static_cast<std::size_t>(Hash_func(key_2)) % cap;

                    const auto initial = disdance(original_hash, cap, slot);
                    const auto curr = disdance(original_hash, cap, slot_2);

                    if (const auto can_move_ahead = initial <= curr; can_move_ahead)
                    {
                        break;
                    }
                }

                assert(0U < Size && slot < cap && slot_2 < cap && !Used_bits.at(slot) && Used_bits.at(slot_2));

                Data.at(slot) = std::move(Data.at(slot_2));
                Data.at(slot_2) = {}; // todo(p2). call the destructor ?
                Used_bits.at(slot) = true;
                Used_bits.at(slot_2) = false;
                slot = slot_2;
            }
        }

        constexpr void demand_consistency() const noexcept
        {
            assert(capacity() == Used_bits.size() && Size <= capacity());
        }

        [[maybe_unused]] constexpr void slow_check_size() const noexcept
        {
            [[maybe_unused]] auto actual =
                static_cast<std::size_t>(std::count(Used_bits.begin(), Used_bits.end(), true));

            assert(actual == Size);
        }

        // Capacity = 4, Size = 2, 2 empty slots. Add an item, no rehashing. Now Size = 3.
        // Capacity = 4, Size = 3, 1 empty slot. Since 4 < 3*2,
        // a rehashing takes place before adding an item: Capacity = 8, and there are 5 empty slots.
        static constexpr std::size_t guarantee_one_empty_slot = 4U;

        std::vector<key_t> Data;
        std::vector<bool> Used_bits;
        hash_func_t Hash_func;
        std::size_t Size{};
    };
} // namespace Standard::Algorithms::Numbers
