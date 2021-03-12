#pragma once
#include<algorithm>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<stdexcept>
#include<string>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    template<class cuckoo_hash_func_t, class key_t>
    concept cuckoo_hash_family = std::default_initializable<cuckoo_hash_func_t> &&
        // Avoid 70 chars long prefixes.
        // NOLINTNEXTLINE
        requires(cuckoo_hash_func_t func, const key_t &key, std::int32_t func_index) {
            {
                func.func_size()
                } -> std::convertible_to<std::int32_t>;
            {
                func.regenerate()
            };
            {
                func.hash(key, func_index)
                } noexcept -> std::convertible_to<std::size_t>;
        };

    // todo(p3): precalc primes: 2, 5, 11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, ..
    template<class int_t>
    [[nodiscard]] auto calc_prime(const int_t &value) -> int_t;

    // O(1) search, erase time.
    // todo(p4): make it faster than linear probing?
    // Random numbers provide no guarantee here - insertion can run out of memory.
    template<class key_t, class random_t, cuckoo_hash_family<key_t> cuckoo_hash_func_t>
    struct cuckoo_hash_table final
    {
        constexpr explicit cuckoo_hash_table(const std::size_t initial_prime = 0U)
            : Data(initial_prime == 0U ? 0U : calc_prime(initial_prime))
            , Used_bits(Data.size())
            , Func_size(Cuckoo_hash_func.func_size())
            , Rnd(0, Func_size - 1)
        {
            if (Func_size < min_hash_functions) [[unlikely]]
            {
                auto error = "The number of hash functions(" + std::to_string(Func_size) + ") must be at least " +
                    std::to_string(min_hash_functions);

                throw std::runtime_error(error);
            }

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
            return Data.capacity();
        }

        [[nodiscard]] constexpr auto contains(const key_t &key) const noexcept -> bool
        {
            const auto slot = find_slot(key);
            return slot != npos;
        }

        [[maybe_unused]] constexpr auto insert(const key_t &key) -> bool
        {
            if (contains(key))
            {
                return false;
            }

            {
                const auto edge = (Size << 1U) | 1LLU;
                assert(Size < edge);

                if (Data.size() <= edge)
                {
                    expand();
                }
            }

            insert_impl(key);

            return true;
        }

        [[maybe_unused]] constexpr auto erase(const key_t &key) noexcept -> bool
        {
            const auto slot = find_slot(key);
            if (slot == npos)
            {
                return false;
            }

            assert(0U < Size && Size <= Data.size() && slot < Data.size() && Used_bits[slot]);

            Used_bits.at(slot) = false;
            // todo(p2). call the destructor
            --Size;

            return true;
        }

        constexpr void clear() // todo(p3): It should have been 'noexcept ', but it might be not?
                               // noexcept
        {
            demand_consistency();

            // todo(p2). call the destructors in the Data?
            Used_bits.assign(Used_bits.size(), false);

            Size = 0U;
        }

private:
        [[nodiscard]] constexpr auto data_hash(const key_t &key, const std::int32_t func_index) const noexcept
            -> std::size_t
        {
            assert(!Data.empty() && 0 <= func_index && func_index < Func_size);

            const auto hash1 = Cuckoo_hash_func.hash(key, func_index);
            return static_cast<std::size_t>(hash1) % Data.size();
        }

        [[nodiscard]] constexpr auto find_slot(const key_t &key) const noexcept -> std::size_t
        {
            demand_consistency();

            if (Size == 0U)
            {
                return npos;
            }

            assert(!Data.empty());

            for (std::int32_t func_index{}; func_index < Func_size; ++func_index)
            {
                const auto hash1 = data_hash(key, func_index);

                if (!Used_bits.at(hash1))
                {
                    continue;
                }

                if (const auto &datum = Data[hash1]; datum == key)
                {
                    return hash1;
                }
            }

            return npos;
        }

        constexpr void move_around(std::size_t &last_h, key_t &key)
        {
            demand_consistency();

            assert(!Data.empty());

            constexpr std::int32_t max_attempts = 8;

            std::size_t hash1{};

            for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
            {
                const auto func_index = static_cast<std::int32_t>(Rnd());

                hash1 = data_hash(key, func_index);
                assert(Used_bits.at(hash1));

                if (hash1 != last_h)
                {
                    break;
                }
            }

            std::swap(key, Data.at(hash1));
            last_h = hash1;
        }

        constexpr void insert_impl(const key_t &key0)
        {
            assert(!Data.empty() && Data.size() == Used_bits.size());

            for (auto key = key0;;)
            {
                auto last_h = npos;

                constexpr auto max_attempts = 20;

                for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
                {
                    for (std::int32_t func_index{}; func_index < Func_size; ++func_index)
                    {
                        const auto hash1 = data_hash(key, func_index);

                        if (Used_bits.at(hash1))
                        {
                            continue;
                        }

                        Data[hash1] = std::move(key);
                        Used_bits[hash1] = true;
                        ++Size;

                        return;
                    }

                    move_around(last_h, key);
                }

                constexpr std::int32_t max_rehashes = 10;

                if (max_rehashes <= ++Rehashes)
                {
                    expand();
                }
                else
                {
                    rehash_same_size();
                }
            }
        }

        constexpr void rehash_same_size()
        {
            Cuckoo_hash_func.regenerate();

            rehash(Data.size());
        }

        constexpr void rehash(std::size_t new_size)
        {
            demand_consistency();

            new_size = std::max(new_size, static_cast<std::size_t>(2));

            assert(Data.size() <= new_size);

            auto old_data = Data;
            old_data.resize(new_size);

            auto old_used_bits = Used_bits;
            {
                std::vector<bool> new_bits(new_size);

                std::swap(old_data, Data);
                std::swap(new_bits, Used_bits);
            }

            // todo(p2): better exception guarantee - restore the old size, ..
            Size = 0U;

            for (std::size_t index{}; index < old_used_bits.size(); ++index)
            {
                if (old_used_bits[index])
                {
                    auto &old = old_data[index];

                    [[maybe_unused]] const auto added = insert(std::move(old)); // todo(p3): insert(&&key_t)

                    assert(added);
                }
            }
        }

        constexpr void expand()
        {
            const auto old_size = Data.size();
            const auto news = static_cast<std::size_t>((old_size << 1U) | 1LLU);

            if (!(old_size < news)) [[unlikely]]
            {
                auto error = "Too large size " + std::to_string(old_size);
                throw std::runtime_error(error);
            }

            const auto prime = calc_prime(news);

            Rehashes = 0;
            rehash(prime);
            Rehashes = 0;
        }

        constexpr void demand_consistency() const noexcept
        {
            assert(Data.size() == Used_bits.size() && Size <= Data.size());
            assert(min_hash_functions <= Func_size);
            assert(0 <= Rehashes);
        }

        static constexpr auto npos = 0U - static_cast<std::size_t>(1);

        static constexpr auto min_hash_functions = 2;

        std::vector<key_t> Data;
        std::vector<bool> Used_bits;
        std::size_t Size{};
        cuckoo_hash_func_t Cuckoo_hash_func{};

        const std::int32_t Func_size;
        std::int32_t Rehashes{};
        random_t Rnd;
    };
} // namespace Standard::Algorithms::Numbers
