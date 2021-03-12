#pragma once
// "move_to_front_encoding.h"
#include"../Graphs/avl_tree.h"
#include"../Utilities/is_debug.h"
#include"key_only_compare.h"
#include<algorithm>
#include<array>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<limits>
#include<memory> // 2-byte coder may be large - will have to use heap, not stack.
#include<stdexcept>
#include<string>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::unsigned_integral index_t, index_t total_tize>
    requires(1U < total_tize)
    constexpr void move_right(const std::unsigned_integral auto &key, auto &data, index_t index)
    {
        assert(0U < index && index < total_tize && key < total_tize);

        do
        {
            data.at(index) = std::move(data.at(index - 1U));
        } while (0U < --index);

        data[0] = key; // Might throw.
    }

    constexpr std::uint8_t is_chunk_start_mask = 1U;

    template<std::unsigned_integral key_t1>
    struct mtf_node final
    {
        using key_t = key_t1;

        key_t previous{};

        // The next position might be found by:
        // 1) Either via the next pointer.
        // 2) Or via going to the next chunk start, and then traversing backwards - it might be too slow.
        key_t next{};

        // It is faster to check a flag, than to search in a list.
        // todo(p4): del it; use 256 bits (== 32 bytes) array to save 256-32 = 224 bytes; measure performance.
        std::uint8_t flags{};
    };

    static_assert(3 == sizeof(mtf_node<std::uint8_t>));

    template<std::unsigned_integral key_t, key_t chunk_mask>
    requires(0U < chunk_mask && // todo(p3): ensure the mask is like 00001111.
        1U == (chunk_mask & 1U) && 2U == (chunk_mask & 2U))
    [[nodiscard]] constexpr auto is_chunk_start(const key_t &key) noexcept -> bool
    {
        auto isc = 0U == (key & chunk_mask);
        return isc;
    }

    template<std::unsigned_integral key_t>
    [[nodiscard]] constexpr auto is_chunk_start(const mtf_node<key_t> &node) noexcept -> bool
    {
        assert(node.flags <= is_chunk_start_mask);

        auto isc = 0U != node.flags;
        return isc;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Initially, all keys e.g. [0..255] equal their ranks.
    // After a key is retrieved:
    // - all keys with lower rank have their ranks increased by 1.
    // - the rank of the key becomes 0.
    // It can be used in text compression because often used keys have lower ranks.
    // E.g. "good mood", in hex 676F'6F64 20 6D6F'6F64, is encoded as
    // 676F'0066 23 6E03'0003.
    template<class coder_t, class key_t = typename coder_t::key_t>
    concept encoder = std::default_initializable<coder_t> &&
        // Avoid 70 chars long prefixes.
        // NOLINTNEXTLINE
        requires(coder_t &coder, const key_t &key) {
            {
                coder.encode(key)
                } -> std::convertible_to<key_t>;
            {
                coder.decode(key)
                } -> std::convertible_to<key_t>;
            {
                coder.clear()
            };
        };

    // O(log(n)) time per encode or decode.
    // It seams that an asymptotically slower sqrt(n) move_to_front_encoder_still
    // beats this log(n) 5X times on small dictionary sizes such as 256 std::uint8_t!
    // todo(p4): improve AVL Tree: allocate an array of 256 in advance; no new, or unique_ptr.
    // The index_t must be large enough to support index_t::max() ops.
    template<std::unsigned_integral key_t1, std::unsigned_integral index_t1 = std::uint64_t>
    requires(sizeof(key_t1) <= sizeof(std::uint16_t) && sizeof(key_t1) < sizeof(index_t1))
    struct move_to_front_encoder final
    {
        using key_t = key_t1;
        using index_t = index_t1;

        // Note that key_t is the value of a tree node.
        // std::pair would also suit.
        using pair_t = Standard::Algorithms::Graphs::key_only_compare<index_t, key_t>;

        // Any tree with ranking suits.
        using tree_t = Standard::Algorithms::Trees::avl_tree<pair_t>;

        static constexpr key_t max_value = std::numeric_limits<key_t>::max();
        static constexpr auto total_tize = static_cast<index_t>(max_value + static_cast<index_t>(1));
        static constexpr index_t max_index = std::numeric_limits<index_t>::max();

        static_assert(0U < max_value && max_value < total_tize && total_tize < max_index);

        constexpr move_to_front_encoder()
        {
            clear();
        }

        [[nodiscard]] constexpr auto front_key() const &noexcept -> const key_t &
        {
            return Front_key;
        }

        // todo(p4): It is possible to initialize a range [Az..Ja] starting with 0.
        // todo(p4): For small inputs, it may be faster to backtrack than to clear.
        constexpr void clear()
        {
            Tree.clear(); // todo(p4): reuse an array as an allocator.

            Front_priority = {};
            Front_key = {};

            for (auto iter = Priorities.rbegin(); iter != Priorities.rend(); ++iter)
            {
                --Front_key;
                --Front_priority;
                assert(0U < Front_priority);

                *iter = Front_priority;
                [[maybe_unused]] const auto ins_res = Tree.insert(pair_t{ Front_priority, Front_key });

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(ins_res.first != nullptr && ins_res.second);
                }
            }

            assert(0U < Front_priority && 0U == Front_key);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }
        }

        // Return the old rank of the key, bringing the key to the front.
        [[nodiscard]] constexpr auto encode(const key_t &key) -> key_t
        {
            assert(0U < Front_priority && key < total_tize);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }

            if (Front_key == key)
            {
                return {};
            }

            const pair_t par{ Priorities.at(key), key };
            const auto [node, rank] = Tree.lower_bound_rank(par);

            // todo(p4): re-use the node to erase faster.
            assert(node != nullptr && 0U < rank && rank < total_tize);

            code_end(par);

            return rank;
        }

        // Return the key for the given rank, moving the key to the front.
        [[nodiscard]] constexpr auto decode(const key_t &rank) -> key_t
        {
            assert(rank < total_tize);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }

            if (0U == rank)
            {
                return Front_key;
            }

            const auto *const node = Tree.select(rank);
            if (nullptr == node) [[unlikely]]
            {
                auto err = "A node with rank (" + std::to_string(rank) +
                    ") must exist in the move_to_front_encoder::Tree error.";

                throw std::runtime_error(err);
            }

            // todo(p4): re-use the node to erase faster.
            // No reference, as the node is to be erased.
            const auto par = node->key;
            const auto &key = par.value;

            assert(Priorities.at(key) == par.key);

            code_end(par);

            return key;
        }

private:
        constexpr void code_end(const pair_t &par)
        {
            if (0U == Front_priority) [[unlikely]]
            {
                auto err = std::string("The index_t (") + typeid(index_t).name() +
                    ") is too small in move_to_front_encoder error.";

                throw std::runtime_error(err);
            }

            const auto &[priority, key] = par;
            assert(Front_priority < priority && key < total_tize);

            {
                [[maybe_unused]] const auto erased = Tree.erase(par);

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(erased);
                }
            }

            --Front_priority;
            Priorities.at(key) = Front_priority;
            Front_key = key;

            {
                [[maybe_unused]] const auto ins_res = Tree.insert(pair_t{ Front_priority, Front_key });

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(ins_res.first != nullptr && ins_res.second);
                }
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }
        }

        [[maybe_unused]] constexpr void ensure_pair_exists_in_tree(const index_t &priority, const key_t &key) const
        {
            if (const pair_t par{ priority, key }; Tree.contains(par)) [[likely]]
            {
                return;
            }

            auto err = "The priority " + std::to_string(priority) + " for the key " + std::to_string(key) +
                " must exist in the tree error.";

            throw std::runtime_error(err);
        }

        [[maybe_unused]] constexpr void check() const
        {
            assert(Front_key < total_tize);

            {
                auto pricop = Priorities;
                std::sort(pricop.begin(), pricop.end());

                const auto iter = std::adjacent_find(pricop.begin(), pricop.end());
                if (iter != pricop.end()) [[unlikely]]
                {
                    auto err = "The priority " + std::to_string(*iter) + " is duplicated error.";

                    throw std::runtime_error(err);
                }
            }

            if (const auto siz = Tree.size(); total_tize != siz) [[unlikely]]
            {
                auto err = "The tree size (" + std::to_string(siz) + ") should have been " +
                    std::to_string(total_tize) + " error.";

                throw std::runtime_error(err);
            }

            Tree.validate();

            ensure_pair_exists_in_tree(Front_priority, Front_key);

            for (key_t key{}; const auto &priority : Priorities)
            {
                ensure_pair_exists_in_tree(priority, key);
                ++key;
            }
        }

        std::array<index_t, total_tize> Priorities // To be initialized in ctor, not here.
#ifndef __clang__
            {}
#endif
        ;

        tree_t Tree{};

        index_t Front_priority{};
        key_t Front_key{};
    };

    static_assert(encoder<move_to_front_encoder<std::uint8_t>>);

    // All chars form a circular ring; only sqrt(n) chunk beginnings are kept in-sync:
    // 16 chunks of size 16 for std::uint8_t; 256 * 256 for std::uint16_t.
    // O(sqrt(n)) time per encode or decode,
    // but 3X greater memory usage compared with the slower version.
    // GCC release build is 1.4 times faster on encoding,
    // 1.05 on decoding than that of slow on 1MB random '0'..'z' strings.
    // However, Clang build is 2 times slower than the slower version.
    // In tests there were up to ('z' - '0' + 1) = 75 unique chars.
    //
    // It is 17 times faster on 0.1MB random vector of std::uint16_t.
    template<std::unsigned_integral key_t1>
    requires(sizeof(key_t1) <= sizeof(std::uint16_t))
    struct move_to_front_encoder_still final
    {
        using key_t = key_t1;
        using index_t = std::uint32_t;
        using node_t = Inner::mtf_node<key_t>;
        static_assert(sizeof(key_t) < sizeof(index_t), "Avoid an overflow");

        static constexpr key_t max_value = std::numeric_limits<key_t>::max();
        static constexpr auto total_tize = static_cast<index_t>(max_value + static_cast<index_t>(1));
        static constexpr auto chunk_shift = static_cast<key_t>(sizeof(key_t1) < sizeof(std::uint16_t) ? 4 : 8);
        static constexpr auto chunk_size = static_cast<key_t>(1LLU << chunk_shift);
        static constexpr auto chunk_mask = static_cast<key_t>(chunk_size - 1U);

        static_assert(0U < max_value && max_value < total_tize);
        static_assert(0U < chunk_size && chunk_size < max_value);
        static_assert(chunk_size * chunk_size == total_tize && chunk_size == total_tize / chunk_size);
        static_assert(0U < chunk_mask && chunk_mask < chunk_size && chunk_mask + 1U == chunk_size);

        constexpr move_to_front_encoder_still() noexcept(!::Standard::Algorithms::is_debug)
        {
            clear();
        }

        // todo(p4): It is possible to initialize a range [Az..Ja] starting with 0.
        constexpr void clear() noexcept(!::Standard::Algorithms::is_debug)
        {
            for (key_t ide{}; auto &start_ide : Chunk_starts)
            {
                start_ide = ide;
                ide += chunk_size;
            }

            assert(0U == std::as_const(*this).front_key() && total_tize - chunk_size == Chunk_starts.back());

            for (key_t key{}; auto &node : Nodes)
            {
                node.previous = static_cast<key_t>(key - 1U);
                node.next = static_cast<key_t>(key + 1U);
                node.flags = Inner::is_chunk_start<key_t, chunk_mask>(key) ? Inner::is_chunk_start_mask : 0U;

                ++key;
            }

            assert(Inner::is_chunk_start_mask == Nodes.front().flags && 0U == Nodes.back().flags);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }
        }

        [[nodiscard]] constexpr auto front_key() const &noexcept -> const key_t &
        {
            const auto &key = Chunk_starts[0];
            assert(key < total_tize);
            return key;
        }

private:
        [[nodiscard]] constexpr auto front_key() &noexcept -> key_t &
        {
            // cppcheck-suppress [constVariable]
            auto &key = Chunk_starts[0];
            assert(key < total_tize);
            return key;
        }

public:
        [[nodiscard]] constexpr auto nodes() const &noexcept -> const std::array<node_t, total_tize> &
        {
            return Nodes;
        }

        [[nodiscard]] constexpr auto chunk_starts() const &noexcept -> const std::array<key_t, chunk_size> &
        {
            return Chunk_starts;
        }

        // Return the old rank of the key, bringing the key to the front.
        // Still slow time O(n**0.5).
        [[nodiscard]] constexpr auto encode(const key_t &key) -> key_t
        {
            assert(key < total_tize);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }

            if (const auto &fr_key = front_key(); fr_key == key)
            {
                return {};
            }

            const auto [small_rank, chunk_start_key] = rank_in_chunk_and_start_key(key);
            const auto chunk = find_chunk(chunk_start_key);

            const index_t raw_rank = small_rank + static_cast<index_t>(chunk_size) * chunk;
            auto rank = static_cast<key_t>(raw_rank);

            assert(0U < raw_rank && raw_rank < total_tize);
            assert(0U < rank && rank < total_tize);

            code_end(key, chunk);

            return rank;
        }

        // Return the key for the given rank, moving the key to the front.
        // Still slow time O(n**0.5).
        [[nodiscard]] constexpr auto decode(const key_t &rank) -> key_t
        {
            assert(rank < total_tize);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }

            if (0U == rank)
            {
                const auto &fr_key = front_key();
                return fr_key;
            }

            const index_t chunk = rank >> chunk_shift;
            const index_t small_rank = rank & chunk_mask;
            assert(chunk < chunk_size && small_rank < chunk_size);

            auto key = Chunk_starts.at(chunk);

            for (auto remainder = small_rank; 0U < remainder; --remainder)
            {
                const auto &node = Nodes.at(key);
                key = node.next;
            }

            code_end(key, chunk);

            return key;
        }

private:
        // Still slow time O(n**0.5).
        [[nodiscard]] constexpr auto rank_in_chunk_and_start_key(key_t key) const -> std::pair<index_t, key_t>
        {
            index_t rank{};

            for (;;)
            {
                const auto &node = Nodes.at(key);
                if (Inner::is_chunk_start<key_t>(node))
                {
                    return std::make_pair(rank, key);
                }

                ++rank;
                assert(rank < chunk_size);

                const auto &prev_key = node.previous;
                key = prev_key;
            }
        }

        // Still slow time O(n**0.5).
        [[nodiscard]] constexpr auto find_chunk(const key_t &start_key) const -> key_t
        {
            assert(start_key < total_tize);

            const auto iter = std::find(Chunk_starts.begin(), Chunk_starts.end(), start_key);

            if (Chunk_starts.end() == iter) [[unlikely]]
            {
                auto err = "The key " + std::to_string(start_key) + " should have started a chunk.";

                throw std::runtime_error(err);
            }

            auto chunk = static_cast<key_t>(iter - Chunk_starts.begin());

            assert(chunk < chunk_size);

            return chunk;
        }

        // Instead of e.g. 48, specify 47 as the chunk start.
        constexpr void move_chunk_to_prev_node(const index_t &chunk)
        {
            assert(chunk < chunk_size);

            auto &start_key = Chunk_starts.at(chunk);
            auto &chunk_node = Nodes.at(start_key);

            const auto &pre_key = chunk_node.previous;
            auto &pre_node = Nodes.at(pre_key);

            assert(start_key != pre_key && start_key != chunk_node.next);
            assert(pre_node.next == start_key);
            assert(Inner::is_chunk_start<key_t>(chunk_node));
            assert(!Inner::is_chunk_start<key_t>(pre_node));

            start_key = pre_key;
            chunk_node.flags = {};
            pre_node.flags = Inner::is_chunk_start_mask;
        }

        constexpr void check_nodes() const
        {
            std::vector<bool> prev_bits(total_tize);
            std::vector<bool> nex_bits(total_tize);
            index_t chunk_start_count{};
            index_t key{};
            --key;

            for (const auto &node : Nodes)
            {
                ++key;

                {
                    const auto &pre = node.previous;
                    if (key == pre) [[unlikely]]
                    {
                        auto err =
                            "The key " + std::to_string(key) + " has the same previous in move_to_front_encoder_still.";

                        throw std::runtime_error(err);
                    }

                    if (prev_bits.at(pre)) [[unlikely]]
                    {
                        auto err = "The key " + std::to_string(key) + " has a duplicated previous " +
                            std::to_string(pre) + " in move_to_front_encoder_still.";
                        throw std::runtime_error(err);
                    }

                    prev_bits.at(pre) = true;
                }
                {
                    const auto &nex = node.next;
                    if (key == nex) [[unlikely]]
                    {
                        auto err =
                            "The key " + std::to_string(key) + " has the same next in move_to_front_encoder_still.";

                        throw std::runtime_error(err);
                    }

                    if (nex_bits.at(nex)) [[unlikely]]
                    {
                        auto err = "The key " + std::to_string(key) + " has a duplicated next " + std::to_string(nex) +
                            " in move_to_front_encoder_still.";
                        throw std::runtime_error(err);
                    }

                    nex_bits.at(nex) = true;
                }

                if (!Inner::is_chunk_start<key_t>(node))
                {
                    continue;
                }

                if (chunk_size < ++chunk_start_count) [[unlikely]]
                {
                    auto err = "There should have been " + std::to_string(chunk_size) +
                        " chunks, but there is at least one more in move_to_front_encoder_still.";

                    throw std::runtime_error(err);
                }
            }

            if (chunk_start_count < chunk_size) [[unlikely]]
            {
                auto err = "There should have been " + std::to_string(chunk_size) + " chunks, but there actually are " +
                    std::to_string(chunk_start_count) + " in move_to_front_encoder_still.";

                throw std::runtime_error(err);
            }
        }

        constexpr void check_chunk_starts() const
        {
            std::vector<bool> bits(total_tize);

            for (const auto &key : Chunk_starts)
            {
                if (bits.at(key)) [[unlikely]]
                {
                    auto err = "The key " + std::to_string(key) +
                        " is duplicated in move_to_front_encoder_still::Chunk_starts.";
                    throw std::runtime_error(err);
                }

                bits.at(key) = true;

                const auto &node = Nodes.at(key);

                if (!Inner::is_chunk_start<key_t>(node)) [[unlikely]]
                {
                    auto err = "The node " + std::to_string(key) +
                        " should have started a chunk in move_to_front_encoder_still.";

                    throw std::runtime_error(err);
                }
            }
        }

        constexpr void check() const
        {
            check_nodes();
            check_chunk_starts();
        }

        constexpr void code_end(const key_t &key, const index_t &chunk)
        {
            assert(key < total_tize && chunk < chunk_size);

            for (auto chu = chunk; 0U < chu; --chu)
            {
                move_chunk_to_prev_node(chu);
            }

            {
                auto &node = Nodes.at(key);
                {// Remove the node from the ring.
                    const auto &pre_key = node.previous;
                    const auto &next_key = node.next;

                    assert(key != pre_key && key != next_key && pre_key != next_key);

                    auto &pre_node = Nodes.at(pre_key);
                    auto &next_node = Nodes.at(next_key);

                    // pre_node <-> node <-> next_node ; was
                    // pre_node <-> next_node ; will be
                    pre_node.next = next_key;
                    next_node.previous = pre_key;
                }
                {// Insert the node before the front.
                    const auto &fr_key = front_key();
                    assert(key != fr_key);

                    auto &fr_node = Nodes.at(fr_key);
                    const auto &bef_fr_key = fr_node.previous;
                    auto &bef_fr_node = Nodes.at(bef_fr_key);

                    // bef_fr_node <-> fr_node ; was
                    // bef_fr_node <-> node <-> fr_node ; will be
                    node.previous = bef_fr_key;
                    node.next = fr_key;

                    bef_fr_node.next = fr_node.previous = key;
                }
            }
            {
                constexpr index_t chu{};
                move_chunk_to_prev_node(chu);
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }
        }

        std::array<node_t, total_tize> Nodes // To be initialized in ctor, not here.
#ifndef __clang__
            {}
#endif
        ;

        std::array<key_t, chunk_size> Chunk_starts // To be initialized in ctor, not here.
#ifndef __clang__
            {}
#endif
        ;
    };

    static_assert(sizeof(move_to_front_encoder_still<std::uint8_t>) == // NOLINTNEXTLINE
        256 * 3 + 16);

    static_assert(encoder<move_to_front_encoder_still<std::uint8_t>>);

    // Slow time O(n), but low memory usage: 256 bytes, or 64KB for std::uint16_t.
    template<std::unsigned_integral key_t1>
    requires(sizeof(key_t1) <= sizeof(std::uint16_t))
    struct move_to_front_encoder_slow final
    {
        using key_t = key_t1;
        using index_t = std::uint32_t;
        static_assert(sizeof(key_t) < sizeof(index_t), "Avoid an overflow");

        static constexpr key_t max_value = std::numeric_limits<key_t>::max();
        static constexpr auto total_tize = static_cast<index_t>(max_value + static_cast<index_t>(1));

        static_assert(0U < max_value && max_value < total_tize);

        constexpr move_to_front_encoder_slow() noexcept(!::Standard::Algorithms::is_debug)
        {
            clear();
        }

        // todo(p4): It is possible to initialize a range [Az..Ja] starting with 0.
        constexpr void clear() noexcept(!::Standard::Algorithms::is_debug)
        {
            for (key_t key{}; auto &datum : Data)
            {
                datum = key++;
            }

            assert(0U == Data.front() && max_value == Data.back());

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::array<key_t, total_tize> &
        {
            return Data;
        }

        // Return the old rank of the key, bringing the key to the front.
        // Slow time O(n).
        [[nodiscard]] constexpr auto encode(const key_t &key) -> key_t
        {
            assert(key < total_tize);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }

            if (Data[0] == key)
            {
                return {};
            }

            auto rank = find_rank(key);
            assert(index_t{} < rank && rank < total_tize);

            Inner::move_right<index_t, total_tize>(key, Data, rank);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }

            return rank;
        }

        // Return the key for the given rank, moving the key to the front.
        // Slow time O(n).
        [[nodiscard]] constexpr auto decode(const key_t &rank) -> key_t
        {
            assert(rank < total_tize);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }

            if (0U == rank)
            {
                return Data[0];
            }

            auto key = Data.at(rank); // Might throw.
            Inner::move_right<index_t, total_tize>(key, Data, rank);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check();
            }

            return key;
        }

private:
        // Slow time O(n).
        [[nodiscard]] constexpr auto find_rank(const key_t &key) const -> key_t
        {
            assert(key < total_tize);

            for (index_t rank{};;)
            {
                assert(rank < total_tize);

                if (const auto &cand = Data.at(rank); key == cand)
                {
                    return static_cast<key_t>(rank);
                }

                if (max_value < ++rank) [[unlikely]]
                {
                    auto err =
                        "The key " + std::to_string(key) + " must exist in move_to_front_encoder_slow::find_rank.";

                    throw std::runtime_error(err);
                }
            }
        }

        constexpr void check() const
        {
            std::vector<bool> bits(total_tize);

            for (const auto &key : Data)
            {
                if (bits.at(key)) [[unlikely]]
                {
                    auto err = "The key " + std::to_string(key) + " is duplicated in move_to_front_encoder_slow.";
                    throw std::runtime_error(err);
                }

                bits.at(key) = true;
            }

            if (const auto iter = std::find(bits.begin(), bits.end(), false); bits.end() != iter) [[unlikely]]
            {
                const auto key = static_cast<std::size_t>(iter - bits.begin());
                assert(key < total_tize);

                auto err = "The key " + std::to_string(key) + " must exist in move_to_front_encoder_slow.";
                throw std::runtime_error(err);
            }
        }

        std::array<key_t, total_tize> Data // To be initialized in ctor, not here.
#ifndef __clang__
            {}
#endif
        ;
    };

    static_assert(sizeof(move_to_front_encoder_slow<std::uint8_t>) == // NOLINTNEXTLINE
        256);

    static_assert(encoder<move_to_front_encoder_slow<std::uint8_t>>);

    template<std::unsigned_integral key_t, encoder<key_t> coder_t, class encoded_t = std::vector<key_t>>
    requires(sizeof(key_t) <= sizeof(std::uint16_t))
    constexpr void encode_mtf(const auto &text, coder_t &coder, encoded_t &encoded, const bool shall_clear = true)
    {
        const auto size = text.size();
        encoded.clear();

        if (0U == size)
        {
            return;
        }

        encoded.reserve(size);

        if (shall_clear)
        {
            coder.clear();
        }

        for ([[maybe_unused]] std::size_t index{}; const auto &cha : text)
        {
            const auto key = static_cast<key_t>(cha);
            auto rank = coder.encode(key);
            encoded.push_back(rank);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                ++index;
            }
        }
    }

    template<std::unsigned_integral key_t, encoder<key_t> coder_t, class encoded_t = std::vector<key_t>>
    requires(sizeof(key_t) <= sizeof(std::uint16_t))
    [[nodiscard]] constexpr auto encode_mtf_slow(const auto &text) -> encoded_t
    {
        encoded_t encoded{};
        auto ucoder = std::make_unique<coder_t>();

        {
            constexpr auto already_clean_shall_clear = false;

            encode_mtf<key_t, coder_t, encoded_t>(text, *ucoder, encoded, already_clean_shall_clear);
        }

        return encoded;
    }

    template<class cha_t, std::unsigned_integral key_t, encoder<key_t> coder_t, class text_t = std::vector<cha_t>>
    requires(sizeof(key_t) <= sizeof(std::uint16_t))
    constexpr void decode_mtf(const auto &encoded, coder_t &coder, text_t &text, const bool shall_clear = true)
    {
        const auto size = encoded.size();
        text.clear();

        if (0U == size)
        {
            return;
        }

        text.reserve(size);

        if (shall_clear)
        {
            coder.clear();
        }

        for ([[maybe_unused]] std::size_t index{}; const auto &rank : encoded)
        {
            const auto key = static_cast<key_t>(rank);
            auto raw_cha = coder.decode(key);
            text.push_back(static_cast<cha_t>(raw_cha));

            if constexpr (::Standard::Algorithms::is_debug)
            {
                ++index;
            }
        }
    }

    template<class cha_t, std::unsigned_integral key_t, encoder<key_t> coder_t, class text_t = std::vector<cha_t>>
    requires(sizeof(key_t) <= sizeof(std::uint16_t))
    [[nodiscard]] constexpr auto decode_mtf_slow(const auto &encoded) -> text_t
    {
        text_t text{};

        auto ucoder = std::make_unique<coder_t>();

        {
            constexpr auto already_clean_shall_clear = false;

            decode_mtf<cha_t, key_t, coder_t, text_t>(encoded, *ucoder, text, already_clean_shall_clear);
        }

        return text;
    }
} // namespace Standard::Algorithms::Numbers
