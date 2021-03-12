#pragma once
#include<algorithm>
#include<cassert>
#include<cstddef>
#include<stdexcept>
#include<string>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Trees::Inner
{
    template<class letter_t, class frequency_t, class node_t>
    constexpr void add_node(std::vector<node_t> &nodes, std::size_t &ids, std::vector<node_t> &heap,
        const letter_t &letter, const frequency_t &frequency, const std::size_t left = 0U, const std::size_t right = 0U)
    {
        ++ids;
        assert(ids != 0U);

        // todo(p3): uncomment when Clang 'construct_at' ready.
        // nodes.emplace_back(letter, frequency, ids, left, right);
        // heap.emplace_back(letter, frequency, ids, left, right);
        nodes.push_back({ letter, frequency, ids, left, right });
        heap.push_back({ letter, frequency, ids, left, right });
    }

    template<class node_t, class comparer_t>
    constexpr void pop_heap_helper(std::vector<node_t> &heap, comparer_t &comparer)
    {
        std::pop_heap(heap.begin(), heap.end(), comparer);

        heap.pop_back();
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    template<class letter_t, class frequency_t>
    struct prefix_code_node final
    {
        letter_t letter{};
        frequency_t frequency{};
        std::size_t id{};
        std::size_t left{};
        std::size_t right{};
    };

    // Huffman coding or optimal prefix code tree. Given positive frequencies or counts of letters,
    // generate a min cost binary tree using a prefix-free variable-length code.
    // Can be used in text compression.
    // frequency_t, code_t must be large enough not to overflow.
    // Time O(n*log(n)).
    template<class code_t, class frequency_t, class letter_t, class letter_frequencies_t, class letter_code_t,
        class node_t>
    constexpr void prefix_codes(const letter_t &zero, const letter_frequencies_t &frequencies, const letter_t &one,
        letter_code_t &codes, std::size_t &root_position, std::vector<node_t> &nodes)
    {
        assert(zero != one);

        // todo(p3): maybe sort + an extra arr will be faster?
        codes.clear();
        nodes.clear();

        if (frequencies.size() <= 1U)
        {
            root_position = 0U;

            if (!frequencies.empty())
            {
                const auto &[letter, frequency] = *frequencies.cbegin();
                codes[letter] = code_t(1, zero);

                // todo(p3): uncomment when Clang 'construct_at' ready.
                // nodes.emplace_back(letter, frequency, 1U);
                nodes.push_back({ letter, frequency, 1U });
            }

            return;
        }

        // Here will be at least 1 internal node.
        nodes.reserve(frequencies.size() << 1U);

        std::vector<node_t> heap;
        heap.reserve(frequencies.size());

        const auto comparer = [] [[nodiscard]] (const node_t &one, const node_t &two)
        {
            return two.frequency < one.frequency;
        };

        using comparer_t = decltype(comparer);

        std::size_t ids{};

        for (const auto &freq : frequencies)
        {
            Inner::add_node<letter_t, frequency_t, node_t>(nodes, ids, heap, freq.first, freq.second);
        }

        assert(frequencies.size() == ids);

        std::make_heap(heap.begin(), heap.end(), comparer);

        while (2U <= heap.size())
        {
            const auto left = heap[0];
            Inner::pop_heap_helper<node_t, comparer_t>(heap, comparer);

            const auto right = heap[0];
            Inner::pop_heap_helper<node_t, comparer_t>(heap, comparer);

            const auto &some_letter = left.letter;
            const auto sum = static_cast<frequency_t>(left.frequency + right.frequency);

            Inner::add_node<letter_t, frequency_t, node_t>(nodes, ids, heap, some_letter, sum, left.id, right.id);
        }

        assert(heap.size() == 1U && frequencies.size() < heap[0].id);

        root_position = heap.at(0).id - 1U;

        std::vector<std::pair<std::size_t, code_t>> id_prefixes;
        id_prefixes.reserve(frequencies.size() << 1U);
        id_prefixes.emplace_back(heap[0].id, code_t{});

        do
        {
            auto &back = id_prefixes.back();
            const auto position = back.first - 1U;
            assert(position < nodes.size());

            const auto &node = nodes.at(position);
            assert(node.id == back.first);

            auto &prefix = back.second;
            if (position < frequencies.size())
            {
                assert(!prefix.empty());
                codes[node.letter] = std::move(prefix);
                id_prefixes.pop_back();
                continue;
            }

            prefix.push_back(zero);
            back.first = node.left;

            auto prefix2 = prefix;
            prefix2.back() = one;

            id_prefixes.emplace_back(node.right, std::move(prefix2));
        } while (!id_prefixes.empty());
    }

    template<class letter_t, class node_t>
    constexpr void serialize_preorder_tree_structure_only(const letter_t &zero, const std::vector<node_t> &nodes,
        const letter_t &one, const std::size_t root_position, auto &seri)
    {
        assert(zero != one && root_position < nodes.size());

        seri.clear();
        seri.reserve(nodes.size());

        const auto visitor = [&zero, &nodes, &one, &seri](
                                 const std::size_t ide, auto &itself, const std::size_t depth = {}) -> void
        {
            if (constexpr auto max_depth = 1'000U; max_depth < depth) [[unlikely]]
            {
                auto err = "Too large depth " + std::to_string(depth) + " in tree structure serialize.";

                throw std::runtime_error(err);
            }

            const auto &node = nodes.at(ide);
            const auto has_left = 0U < node.left;
            const auto has_right = 0U < node.right;

            {
                const auto is_leaf = !has_left && !has_right;
                seri.push_back(is_leaf ? one : zero);
            }

            if (has_left)
            {
                itself(node.left - 1U, itself, depth + 1U);
            }

            if (has_right)
            {
                itself(node.right - 1U, itself, depth + 1U);
            }
        };

        visitor(root_position, visitor);
    }
} // namespace Standard::Algorithms::Trees
