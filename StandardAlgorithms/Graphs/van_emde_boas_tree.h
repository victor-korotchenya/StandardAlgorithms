#pragma once
#include<algorithm>
#include<cassert>
#include<cmath>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<gsl/gsl>
#include<limits>
#include<map>
#include<memory>
#include<stdexcept>
#include<string>
#include<type_traits>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    // todo(p3): Full [0..(2**n - 1)] range.
    // todo(p4): If D <= 64, mask std::uint64_t, .., std::uint8_t.
    //
    // The size depends on the key domain size D
    // which should not be much larger than the number of keys N.
    // D is split into b or (b-1) blocks of size b = ceiling(sqrt(D)).
    // No key duplicates - the keys are unique, distinct.
    // Time is O(log(log(D)) which is usually faster than AVL, RB trees.
    template<std::unsigned_integral int_t1,
        // For small sizes, std::vector is faster than std::map, so set prefer_speed to true.
        bool prefer_speed1 = true, int_t1 max_value = std::numeric_limits<int_t1>::max()>
    requires(1U < max_value) // todo(p3): There is a solution for std::sqrt(std::int64_t).
    struct van_emde_boas_tree final
    {
        using int_t = int_t1;
        static constexpr auto prefer_speed = prefer_speed1;

        constexpr explicit van_emde_boas_tree(const int_t &size)
            : Domain_size(std::max<int_t>(size, 1))
            , Block_size(static_cast<int_t>(std::ceil(
                  // todo(p3): use more reliable library?
                  std::sqrt(Domain_size))))
            ,
            // todo(p3): if (prefer_speed
            Blocks(prefer_speed ? count_blocks(Domain_size, Block_size) : 0U)
        // todo(p3): else: Block_map()
        {
            assert(0U < Block_size && 0U < Domain_size && Domain_size <= max_value);

            if (Domain_size <= 2U)
            {
                return;
            }

            Summary.reset(new van_emde_boas_tree(Block_size));

            if constexpr (prefer_speed)
            {
                for (auto &block : Blocks)
                {
                    block.reset(new van_emde_boas_tree(Block_size));
                }
            }
        }

        [[nodiscard]] static constexpr auto count_blocks(const int_t &domain_size1, const int_t &block) noexcept
            -> int_t
        {
            assert(0U < block && block <= domain_size1 &&
                // todo(p3): overflow.
                domain_size1 <= block * block);

            if (domain_size1 <= 2U)
            {
                return int_t{};
            }

            const auto is_small = domain_size1 <= block * (block - static_cast<int_t>(1));

            const auto block_count = static_cast<int_t>(block - (is_small ? 1U : 0U));
            assert(1U < block_count && (block_count == block - 1U || block_count == block));

            return block_count;
        }

        [[nodiscard]] constexpr auto domain_size() const &noexcept -> const int_t &
        {
            return Domain_size;
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool
        {
            return Minimum == max_value;
        }

        [[nodiscard]] constexpr auto contains(const int_t &key) const -> bool
        {
            if (Domain_size <= key)
            {
                return {};
            }

            assert(max_value != key);

            const auto *tree = this;
            auto key1 = key;

            for (;;)
            {
                assert(tree != nullptr);

                if (tree->Minimum == key1 || tree->Maximum == key1)
                {
                    return true;
                }

                if (key1 < tree->Minimum || tree->Maximum < key1 || tree->Domain_size <= std::max<int_t>(key1, 2))
                {
                    return {};
                }

                const auto hilo = tree->high_low(key1);
                const auto *blid = tree->block_by_id(hilo.first);
                if (blid == nullptr)
                {
                    return {};
                }

                assert(blid->Domain_size < tree->Domain_size);
                tree = blid;
                key1 = hilo.second;
            }
        }

        [[nodiscard]] constexpr auto min() const -> std::pair<int_t, bool>
        {
            return is_empty() ? std::make_pair(int_t{}, false) : std::make_pair(Minimum, true);
        }

        [[nodiscard]] constexpr auto max() const -> std::pair<int_t, bool>
        {
            return Maximum == max_value ? std::make_pair(int_t{}, false) : std::make_pair(Maximum, true);
        }

        [[nodiscard]] constexpr auto prev(const int_t &key) const -> std::pair<int_t, bool>
        {
            if (is_empty() || key <= Minimum || Domain_size <= 2U)
            {
                const auto has = Minimum == 0U && 0U < key;
                return std::make_pair(int_t{}, has);
            }

            if (Maximum < key)
            {
                return std::make_pair(Maximum, true);
            }

            const auto key1 = std::min(static_cast<int_t>(Domain_size - 1), key);
            assert(max_value != key1 && Summary != nullptr);

            const auto hilo = high_low(key1);
            {
                const auto *blid = block_by_id(hilo.first);
                if (blid != nullptr)
                {
                    const auto min_block = blid->min();

                    if (min_block.second && min_block.first < hilo.second)
                    {// There must be a smaller key.
                        const auto low1 = blid->prev(hilo.second);
                        assert(low1.second);

                        const auto ind1 = to_index(hilo.first, low1.first);
                        assert(ind1 < key);

                        return std::make_pair(ind1, true);
                    }
                }
            }

            const auto *sum = Summary.get();
            const auto previous_block = sum->prev(hilo.first);

            if (previous_block.second)
            {
                const auto *blid = block_by_id(previous_block.first);
                assert(blid != nullptr);

                const auto largest = blid->max();
                assert(largest.second);

                const auto ind = to_index(previous_block.first, largest.first);
                assert(ind < key);

                return std::make_pair(ind, true);
            }

            return Minimum < key1 ? std::make_pair(Minimum, true) : std::make_pair(int_t{}, false);
        }

        [[nodiscard]] constexpr auto next(const int_t &key) const -> std::pair<int_t, bool>
        {
            if (is_empty() || Maximum <= key || Domain_size <= 2U)
            {
                return key == 0U && Maximum == 1U ? std::make_pair(static_cast<int_t>(1), true)
                                                  : std::make_pair(int_t{}, false);
            }

            if (key < Minimum)
            {
                return std::make_pair(Minimum, true);
            }

            assert(Summary != nullptr);

            const auto hilo = high_low(key);
            {
                const auto *blid = block_by_id(hilo.first);
                if (blid != nullptr)
                {
                    const auto max_block = blid->max();
                    if (hilo.second < max_block.first)
                    {
                        const auto next_lo = blid->next(hilo.second);
                        assert(max_block.second && next_lo.second);

                        const auto ind1 = to_index(hilo.first, next_lo.first);
                        assert(key < ind1);

                        return std::make_pair(ind1, true);
                    }
                }
            }

            const auto *sum = Summary.get();
            const auto next_block = sum->next(hilo.first);
            if (!next_block.second)
            {
                return std::make_pair(int_t{}, false);
            }

            const auto *blid2 = block_by_id(next_block.first);
            assert(blid2 != nullptr);

            const auto smallest = blid2->min();
            assert(smallest.second);

            const auto ind = to_index(next_block.first, smallest.first);
            assert(key < ind);

            return std::make_pair(ind, true);
        }

        [[maybe_unused]] constexpr auto push(const int_t &key) -> bool
        {
            if (Domain_size <= key) [[unlikely]]
            {
                throw std::invalid_argument("Too large key '" + std::to_string(key) + "'.");
            }

            assert(max_value != key);

            if (is_empty())
            {
                Minimum = Maximum = key;
                return true;
            }

            auto key1 = key;
            auto res = false;
            if (key < Minimum)
            {
                std::swap(Minimum, key1);
                res = true;
            }
            else if (key == Minimum || key == Maximum)
            {
                return false;
            }

            if (2U < Domain_size)
            {
                const auto hilo = high_low(key1);
                auto *blid = block_by_id_add(hilo.first);
                assert(blid != nullptr && Summary != nullptr);

                if (blid->is_empty())
                {
                    auto *sum = Summary.get();
                    const auto added = sum->push(hilo.first);
                    assert(added);

                    blid->Minimum = blid->Maximum = hilo.second;
                    res = true;
                }
                else if (blid->push(hilo.second))
                {
                    res = true;
                }
            }

            if (Maximum < key1)
            {
                Maximum = key1;
                res = true;
            }

            return res;
        }

        [[maybe_unused]] constexpr auto pop(const int_t &key) -> bool
        {
            if (is_empty() || Domain_size <= key)
            {
                return false;
            }

            assert(max_value != key);

            if (Minimum == Maximum)
            {
                if (key != Minimum)
                {
                    return false;
                }

                Minimum = Maximum = max_value;
                return true;
            }

            assert(1U < Domain_size && Minimum < Maximum);

            if (Domain_size == 2U)
            {
                Maximum = Minimum = key == 0U ? 1U : 0U;
                return true;
            }

            assert(Summary != nullptr);

            auto key1 = key;
            const auto is_min = key == Minimum;

            if (is_min)
            {
                key1 = find_larger_than_minimum(key);
            }

            const auto hilo = high_low(key1);
            auto *blid = block_by_id(hilo.first);

            if (blid == nullptr)
            {
                assert(!is_min);
                return false;
            }

            if (!blid->pop(hilo.second))
            {
                assert(!is_min);
                return false;
            }

            if (const auto nonempty = blid->min(); nonempty.second)
            {
                return nonempty_adjust_maximum(key1, *blid, hilo.first);
            }

            auto *sum = Summary.get();
            const auto cleared_summary = sum->pop(hilo.first);
            assert(cleared_summary);

            if (key1 != Maximum)
            {
                return true;
            }

            const auto max2 = sum->max();
            if (!max2.second)
            {
                Maximum = Minimum;
                return true;
            }

            const auto *blid3 = block_by_id(max2.first);
            assert(blid3 != nullptr);

            const auto max3 = blid3->max();
            assert(max3.second);

            const auto ind3 = to_index(max2.first, max3.first);
            assert(ind3 < Maximum);

            Maximum = ind3;
            return true;
        }

private:
        [[nodiscard]] constexpr auto high_low(const int_t &ind) const -> std::pair<int_t, int_t>
        {
            const auto high = static_cast<int_t>(ind / Block_size);
            const auto low = static_cast<int_t>(ind % Block_size);

            assert(ind < Domain_size && high < Block_size && low < Block_size);

            return std::make_pair(high, low);
        }

        [[nodiscard]] constexpr auto to_index(const int_t &high, const int_t &low) const -> int_t
        {
            const auto ind = Block_size * high + low; // todo(p3): overflow
            const auto ind_cast = static_cast<int_t>(ind);

            assert(ind_cast < Domain_size && high < Block_size && low < Block_size);

            return ind_cast;
        }

        [[nodiscard]] constexpr auto block_by_id(const int_t &high) const -> van_emde_boas_tree *
        {
            assert(high < Block_size);

            if constexpr (prefer_speed)
            {
                const auto &block = Blocks[high];
                auto *blid = block.get();
                assert(blid != nullptr);

                return blid;
            }
            else
            {
                const auto &iter = Block_map.find(high);
                if (iter == Block_map.end())
                {
                    return nullptr;
                }

                const auto &block = iter->second;
                auto *blid = block.get();
                assert(blid != nullptr);

                return blid;
            }
        }

        [[nodiscard]] constexpr auto block_by_id_add(const int_t &high) -> van_emde_boas_tree *
        {
            assert(high < Block_size);

            if constexpr (prefer_speed)
            {
                auto *blid = block_by_id(high);
                assert(blid != nullptr);

                return blid;
            }
            else
            {
                auto ins = Block_map.emplace(high, std::unique_ptr<van_emde_boas_tree>());
                auto &block = ins.first->second;
                assert(!block == ins.second);

                if (ins.second)
                {
                    try
                    {
                        block.reset(new van_emde_boas_tree(Block_size));
                    }
                    catch (...)
                    {
                        Block_map.erase(high);
                        throw;
                    }
                }

                auto *blid = block.get();
                assert(blid != nullptr);

                return blid;
            }
        }

        [[nodiscard]] constexpr auto find_larger_than_minimum(const int_t &key) -> int_t
        {
            assert(!is_empty() && Summary != nullptr && Minimum < Maximum && 2U < Domain_size);

            const auto *sum = Summary.get();
            const auto smallest = sum->min();
            assert(smallest.second);

            const auto *blid = block_by_id(smallest.first);
            assert(blid != nullptr);

            const auto mi1 = blid->min();
            assert(mi1.second);

            const auto ind1 = to_index(smallest.first, mi1.first);
            assert(key < ind1);

            // Store a new greater min in the 'minimum',
            // thus remove it from the block.
            Minimum = ind1;

            return ind1;
        }

        [[nodiscard]] constexpr auto nonempty_adjust_maximum(const int_t &key1, const van_emde_boas_tree &blid,
            const int_t &high) -> bool // todo(p3): rename?
        {
            if (key1 != Maximum)
            {
                return true;
            }

            const auto max2 = blid.max();
            assert(max2.second);

            const auto ind1 = to_index(high, max2.first);
            assert(ind1 < Maximum);

            Maximum = ind1;

            return true;
        }

        const int_t Domain_size;
        const int_t Block_size;
        int_t Minimum = max_value;
        int_t Maximum = max_value;

        std::unique_ptr<van_emde_boas_tree> Summary{};

        // todo(p3): if (prefer_speed)
        std::vector<std::unique_ptr<van_emde_boas_tree>> Blocks{};
        // todo(p3): else
        std::map<int_t, std::unique_ptr<van_emde_boas_tree>> Block_map{};
    };
} // namespace Standard::Algorithms::Graphs
