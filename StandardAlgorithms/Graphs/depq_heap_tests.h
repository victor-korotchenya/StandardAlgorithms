#pragma once
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"multiset_heap_depq.h"
#include<vector>

namespace Standard::Algorithms::Heaps::Tests::Inner
{
    constexpr auto pop_min_key = 0;
    constexpr auto add_key = 1;
    constexpr auto pop_max_key = 2;

    template<class key_t, class heap_t>
    constexpr void depq_test_min_max(const heap_t &heap, const multiset_heap_depq<key_t> &msh)
    {
        ::Standard::Algorithms::ert::are_equal(false, heap.is_empty(), "empty heap in test");
        ::Standard::Algorithms::ert::are_equal(false, msh.is_empty(), "empty msh in test");
        ::Standard::Algorithms::ert::are_equal(msh.size(), heap.size(), "heap size");
        assert(msh.size());

        {
            const auto &expected = msh.min();
            const auto &actual = heap.min();
            ::Standard::Algorithms::ert::are_equal(expected, actual, "min");
        }
        {
            const auto &expected = msh.max();
            const auto &actual = heap.max();
            ::Standard::Algorithms::ert::are_equal(expected, actual, "max");
        }
    }

    template<bool shall_slow_copy_before, class random_t, class key_t, class heap_t>
    constexpr void depq_add(std::vector<std::pair<std::int32_t, key_t>> &data, std::vector<key_t> &before, heap_t &heap,
        multiset_heap_depq<key_t> &msh, random_t &rnd)
    {
        const auto item = static_cast<key_t>(rnd());
        data.emplace_back(add_key, item);
        msh.push(item);

        if constexpr (shall_slow_copy_before)
        {
            before = heap.data();
        }

        heap.push(item);

        depq_test_min_max(heap, msh);
        heap.validate();
    }

    template<bool shall_slow_copy_before, class random_t, class key_t, class heap_t>
    constexpr void try_depq_erase(std::vector<std::pair<std::int32_t, key_t>> &data, std::vector<key_t> &before,
        heap_t &heap, multiset_heap_depq<key_t> &msh, random_t &rnd, const bool can_skip)
    {
        ::Standard::Algorithms::ert::are_equal(false, heap.is_empty(), "empty heap before erase");
        ::Standard::Algorithms::ert::are_equal(msh.size(), heap.size(), "heap size");
        assert(msh.size());

        const auto key = static_cast<std::uint32_t>(rnd()) & 3U;

        if (can_skip && 1U < key)
        {
            return;
        }

        if constexpr (shall_slow_copy_before)
        {
            before = heap.data();
        }

        if ((key & 1U) == 0U)
        {
            const auto &min1 = msh.min();
            data.emplace_back(pop_min_key, min1);

            msh.pop_min();
            heap.pop_min();
        }
        else
        {
            const auto &max1 = msh.max();
            data.emplace_back(pop_max_key, max1);

            msh.pop_max();
            heap.pop_max();
        }

        if (!msh.is_empty())
        {
            depq_test_min_max(heap, msh);
        }

        heap.validate();
    }

    template<bool shall_slow_copy_before, class random_t, class key_t, class heap_t>
    constexpr void depq_random_test(std::vector<std::pair<std::int32_t, key_t>> &data, std::vector<key_t> &before,
        heap_t &heap, multiset_heap_depq<key_t> &msh)
    {
        random_t rnd{};
        const auto total_steps = 100;

        for (std::int32_t step{}; step < total_steps; ++step)
        {
            if (!msh.is_empty())
            {
                try_depq_erase<shall_slow_copy_before>(data, before, heap, msh, rnd, true);
            }

            depq_add<shall_slow_copy_before>(data, before, heap, msh, rnd);
        }

        while (!msh.is_empty())
        {
            try_depq_erase<shall_slow_copy_before>(data, before, heap, msh, rnd, false);
        }

        ::Standard::Algorithms::ert::are_equal(0U, heap.size(), "heap size after pop all");
    }
} // namespace Standard::Algorithms::Heaps::Tests::Inner

namespace Standard::Algorithms::Heaps::Tests
{
    template<class random_t, class key_t, class heap_t>
    constexpr void depq_heap_tests()
    {
        std::vector<std::pair<std::int32_t, key_t>> data;
        heap_t heap;
        auto before = heap.data();
        multiset_heap_depq<key_t> msh;

        constexpr auto shall_slow_copy_before = false;
        try
        {
            ::Standard::Algorithms::Heaps::Tests::Inner ::depq_random_test<shall_slow_copy_before, random_t, key_t,
                heap_t>(data, before, heap, msh);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "\nError: " << exc.what();
            ::Standard::Algorithms::print("operations", data, str);

            if constexpr (shall_slow_copy_before)
            {
                ::Standard::Algorithms::print("before heap", before, str);
            }

            ::Standard::Algorithms::print("after heap", heap.data(), str);
            ::Standard::Algorithms::print_value("msh.size", str, msh.size());

            throw std::runtime_error(str.str());
        }
    }
} // namespace Standard::Algorithms::Heaps::Tests
