#pragma once
#include"../Utilities/project_constants.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include<iomanip>
#include<iostream>
#include<set>

namespace Standard::Algorithms::Trees
{
    void print_tree(std::ostream &str, const auto &tree);
} // namespace Standard::Algorithms::Trees

namespace Standard::Algorithms::Trees::Tests::Inner
{
    constexpr auto bst_test_runs = 1;
    static_assert(0 < bst_test_runs);

    // To run more tests, assign 0
    constexpr auto bst_is_small_test = 1;
    static_assert(0 == bst_is_small_test || 1 == bst_is_small_test);

    constexpr auto bst_tests_max_key = 25;
    static_assert(2 < bst_tests_max_key, "To have an erase conflict");

    // To be used primarily for debugging.
    template<class tree_t, class key_t = typename tree_t::key_t, class node_t = typename tree_t::node_t>
    void print_steps_tree(const auto &steps, const tree_t &tree)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << " Steps: " << steps.str() << "\n"
            << " Tree:\n";

        Standard::Algorithms::Trees::print_tree(str, tree);

        str << "\n";
        std::cout << str.str() << std::endl; // Flush the buffer.
    }

    template<class key_t, class tree_t, class rand_gen_t>
    constexpr void insert_test(std::set<key_t> &uniq, auto &steps, tree_t &tree, rand_gen_t &rnd)
    {
        steps << " Insert ";

        const auto key = rnd();
        steps << key;

        const auto has = uniq.insert(key).second;
        steps << (has ? "." : " not.");

        if (has)
        {
            // print_steps_tree<tree_t>(steps, tree);
        }

        const auto actual = tree.insert(key);
        ::Standard::Algorithms::ert::are_equal(has, actual.second, "is inserted");
        ::Standard::Algorithms::ert::is_not_null_ptr(actual.first, "node");
        ::Standard::Algorithms::ert::are_equal(key, actual.first->key, "node key");

        if (has)
        {
            // print_steps_tree<tree_t>(steps, tree);
        }

        tree.validate();
    }

    template<class key_t, class tree_t, class rand_gen_t, class node_t>
    constexpr void find_test(
        const std::set<key_t> &uniq, auto &steps, tree_t &tree, rand_gen_t &rnd, const node_t *const sentinel)
    {
        steps << " Find ";

        const auto key = rnd();
        steps << key;

        const auto has = uniq.contains(key);
        steps << (has ? "." : " not.");

        const auto *const node = tree.find(key);
        {
            const auto actual = node != nullptr && node != sentinel && node->key == key;

            ::Standard::Algorithms::ert::are_equal(has, actual, "found");
        }

        if (uniq.empty())
        {
            ::Standard::Algorithms::ert::are_equal(true, sentinel == node, "sentinel vs node pointer");
        }
        else
        {
            ::Standard::Algorithms::ert::is_not_null_ptr(node, "node");
        }

        tree.validate();
    }

    template<class key_t, class tree_t>
    constexpr auto erase_test(std::set<key_t> &uniq, auto &steps, tree_t &tree, const key_t &key) -> bool
    {
        steps << " Erase " << key;

        const auto has = uniq.erase(key) != 0U;
        steps << (has ? "." : " not.");

        if (has)
        {
            // print_steps_tree<tree_t>(steps, tree);
        }

        const auto actual = tree.erase(key);
        ::Standard::Algorithms::ert::are_equal(has, actual, "is erased");

        if (has)
        {
            // print_steps_tree<tree_t>(steps, tree);
        }

        tree.validate();
        return has;
    }

    template<class key_t, class tree_t, class rand_gen_t, class node_t>
    constexpr void bst_tests_impl(const node_t *const sentinel, const key_t &min, auto &steps, const key_t &max)
    {
        std::set<key_t> uniq{};
        tree_t tree{};
        rand_gen_t rnd(min, max);

        try
        {
            constexpr auto small_size = 10;
            constexpr auto large_size = ::Standard::Algorithms::Utilities::stack_max_size;

            constexpr auto max_size = small_size * bst_is_small_test + large_size * (1 - bst_is_small_test);

            static_assert(1 < max_size && max_size <= large_size);

            for (auto index = 1; index < max_size; ++index)
            {
                steps << index;
                ::Standard::Algorithms::Trees::Tests::Inner::find_test(uniq, steps, tree, rnd, sentinel);

                {
                    const auto key = rnd();

                    ::Standard::Algorithms::Trees::Tests::Inner::erase_test(uniq, steps, tree, key);
                }

                ::Standard::Algorithms::Trees::Tests::Inner::insert_test(uniq, steps, tree, rnd);
                steps << "\n";
            }

            steps << " Start popping\n";

            while (!uniq.empty())
            {
                const auto key = *uniq.cbegin();
                {
                    const auto was_deleted =
                        ::Standard::Algorithms::Trees::Tests::Inner::erase_test(uniq, steps, tree, key);

                    ::Standard::Algorithms::ert::are_equal(true, was_deleted, std::to_string(key) + " was deleted");
                }

                ::Standard::Algorithms::Trees::Tests::Inner::find_test(uniq, steps, tree, rnd, sentinel);
            }
        }
        catch (const std::exception &exc)
        {
            steps << "\nGot an error: ";
            steps << exc.what();

            throw std::runtime_error(steps.str());
        }
    }
} // namespace Standard::Algorithms::Trees::Tests::Inner

namespace Standard::Algorithms::Trees::Tests
{
    template<class key_t, class tree_t, class rand_gen_t, class node_t = typename tree_t::node_t>
    constexpr void bst_tests(
        const node_t *const sentinel = nullptr, const key_t &min = 0, const key_t &max = Inner::bst_tests_max_key)
    {
        require_greater(max, min, "max vs min");

        auto steps = ::Standard::Algorithms::Utilities::w_stream();

        for (std::int32_t index{}; index < Inner::bst_test_runs; ++index)
        {
            steps.str(std::string{});
            steps.clear();

            ::Standard::Algorithms::Trees::Tests::Inner::bst_tests_impl<key_t, tree_t, rand_gen_t, node_t>(
                sentinel, min, steps, max);
        }
    }
} // namespace Standard::Algorithms::Trees::Tests
