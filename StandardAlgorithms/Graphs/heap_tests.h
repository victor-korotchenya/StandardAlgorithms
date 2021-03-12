#pragma once
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include<iostream>
#include<map>
#include<set>
#include<tuple>

namespace Standard::Algorithms::Heaps::Tests
{
    constexpr auto shall_print_heap = false;
    constexpr auto heap_test_mask = static_cast<std::int16_t>(0x7f);

    template<class key_t, class node_t, class heap_t>
    struct heap_context final
    {
        heap_t heap{};
        std::uint32_t size{};

        // Use map<,> to avoid another duplicate deletion in decrease_key().
        std::map<key_t, std::set<node_t *>> key_nodes{};

        std::vector<std::tuple<key_t, key_t, std::int16_t>> steps{};

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};
    };

    template<class context_t>
    constexpr void verify_size(const context_t &context)
    {
        ::Standard::Algorithms::ert::are_equal(context.size, context.heap.size(), "heap.size");
    }

    template<class nt>
    constexpr void add_node(std::set<nt> &nodes, nt &node)
    {
        assert(node != nullptr);

        const auto ins = nodes.insert(node);
        ::Standard::Algorithms::ert::are_equal(true, ins.second, "The node must have been inserted.");
    }

    template<class context_t, class key_t, class list_t, class node_t>
    constexpr void remove_node(context_t &context, const key_t &key, list_t &nodes, node_t &node)
    {
        assert(node != nullptr && context.key_nodes.contains(key) && nodes.contains(node));

        {
            const auto del_count = nodes.erase(node);
            ::Standard::Algorithms::ert::are_equal(1U, del_count, "The node must have been erased.");
        }

        if (!nodes.empty())
        {
            return;
        }

        const auto del_count_2 = context.key_nodes.erase(key);
        ::Standard::Algorithms::ert::are_equal(1U, del_count_2, "The key must have been erased.");
    }

    template<class context_t, class key_t, class list_t, class node_t>
    constexpr void try_decrease_key(context_t &context, const key_t &old_key, list_t &nodes, node_t &node)
    {
        assert(node != nullptr && node->key == old_key && context.key_nodes.contains(old_key) && nodes.contains(node) &&
            context.key_nodes[old_key] == nodes);

        const auto new_key = static_cast<key_t>(context.rnd() & heap_test_mask);
        context.steps.emplace_back(new_key, old_key, 3);

        if constexpr (shall_print_heap)
        {
            std::string descr(
                "Before decrease from " + std::to_string(old_key) + " to " + std::to_string(new_key) + ". ");

            context.heap.print(descr);
            descr += '\n';
            std::cout << descr;
        }

        context.heap.decrease_key(*node, new_key);
        context.heap.validate();

        if (!(new_key < old_key))
        {
            return;
        }

        ::Standard::Algorithms::ert::are_equal(new_key, node->key, "The key must have been decreased.");
        remove_node(context, old_key, nodes, node);

        auto &nodes2 = context.key_nodes[new_key];
        add_node(nodes2, node);
    }

    template<class key_t, class context_t>
    constexpr auto add_key_step(context_t &context, const key_t &key)
    {
        if constexpr (shall_print_heap)
        {
            std::string des("Before add. ");
            context.heap.print(des);
            des += '\n';
            std::cout << des;
        }

        context.steps.emplace_back(key, 0, 0);

        auto *node = context.heap.push(key);
        assert(node != nullptr && node->key == key);
        context.heap.validate();

        auto &nodes = context.key_nodes[key];
        add_node(nodes, node);
        ++context.size;
        verify_size(context);

        return node;
    }

    template<bool can_decrease_key, class key_t, class node_t, class context_t>
    constexpr void add_keys(context_t &context)
    {
        const auto rnd = static_cast<std::uint32_t>(context.rnd());
        const auto size = 1U + (rnd & 1U);

        for (std::uint32_t index{}; index < size; ++index)
        {
            if constexpr (shall_print_heap)
            {// cppcheck-suppress [knownConditionTrueFalse] // NOLINTNEXTLINE
                if (auto shall_debug = false; shall_debug)
                {
                    std::string des;
                    context.heap.print(des);
                }
            }

            const auto key = static_cast<key_t>(context.rnd() & heap_test_mask);
            auto node = add_key_step<key_t, context_t>(context, key);

            if constexpr (can_decrease_key)
            {
                if (!(rnd & (2U << index)))
                {
                    continue;
                }

                auto &nodes = context.key_nodes[key];
                assert(nodes.size() && node && node->key == key);

                try_decrease_key(context, key, nodes, node);
            }
        }
    }

    template<class key_t, class context_t>
    constexpr void try_decrease_key_last(context_t &context)
    {
        assert(context.key_nodes.size() && context.heap.size());

        auto ite = std::prev(context.key_nodes.end());
        const auto key = ite->first;
        auto &nodes = ite->second;
        assert(nodes.size());

        auto node_it = std::prev(nodes.end());
        auto node = *node_it;
        assert(node && node->key == key);

        try_decrease_key(context, key, nodes, node);
    }

    template<bool can_decrease_key, class key_t, class context_t>
    constexpr void remove_top(context_t &context)
    {
        assert(0U < context.size);

        const auto rnd = static_cast<std::uint32_t>(context.rnd()) & 1U;
        const auto steps = std::min(1U + rnd, context.size);

        for (std::size_t index{}; index < steps; ++index)
        {
            if constexpr (shall_print_heap)
            {
                std::string descr("Before remove. ");
                context.heap.print(descr);
                descr += '\n';
                std::cout << descr;
            }

            verify_size(context);

            auto ite = context.key_nodes.begin();
            const auto key = ite->first;
            context.steps.emplace_back(key, 0, 2);

            auto *top = context.heap.top();
            assert(top != nullptr);
            ::Standard::Algorithms::ert::are_equal(key, top->key, "heap.top");

            auto &nodes = ite->second;
            remove_node(context, key, nodes, top);
            --context.size;
            context.heap.pop();
            context.heap.validate();
            verify_size(context);

            if (context.key_nodes.empty())
            {
                return;
            }

            assert(context.size);

            if constexpr (can_decrease_key)
            {
                try_decrease_key_last<key_t>(context);
            }
        }
    }

    template<bool can_decrease_key, class key_t, class node_t, class heap_t> // todo(p2): order params
    constexpr void heap_tests()
    {
        heap_context<key_t, node_t, heap_t> context;

        try
        {
            constexpr auto max_attempts = 50;

            for (std::int32_t att{}; att < max_attempts; ++att)
            {
                add_keys<can_decrease_key, key_t, node_t>(context);
                remove_top<can_decrease_key, key_t>(context);
            }

            while (0U < context.size)
            {
                remove_top<can_decrease_key, key_t>(context);
            }
        }
        catch (...)
        {
            // todo(p2): ::Standard::Algorithms::print(context.steps, "steps");
            throw;
        }
    }
} // namespace Standard::Algorithms::Heaps::Tests
