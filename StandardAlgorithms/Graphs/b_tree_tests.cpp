#include"b_tree_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"b_tree.h"
#include<iostream>
#include<unordered_set>

namespace
{
    using key_t = std::int32_t;
    using int_t = std::uint16_t;
    using tree_t = Standard::Algorithms::Trees::b_tree<key_t, int_t>;

    constexpr auto test_runs = 1;
    constexpr auto steps = 250;

    constexpr key_t min_value = 1;
    constexpr key_t max_value = 100;
    static_assert(min_value <= max_value);

    constexpr int_t min_degr = tree_t::lowest_min_degree;
    constexpr int_t max_degr = 8;
    static_assert(min_degr <= max_degr);

    constexpr auto shall_print_b_tree = false;

    struct b_tree_test_context final
    {
        std::string description{};
        std::unordered_multiset<key_t> s{};
        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{ min_value, max_value };
        const int_t min_degree = rnd(min_degr, max_degr);
    };

    void add_key(b_tree_test_context &context, tree_t &tree)
    {
        const key_t key = context.rnd();
        auto &descr = context.description;
        descr += ", ";
        descr += ::Standard::Algorithms::Utilities::zu_string(key);

        if constexpr (shall_print_b_tree)
        {
            std::cout << descr;
        }

        context.s.insert(key);

        tree.insert(key + key_t{});
        ::Standard::Algorithms::ert::are_equal(context.s.size(), tree.size(), "tree size");
        tree.validate();
    }

    void erase_key(b_tree_test_context &context, tree_t &tree, const key_t &key)
    {
        auto &descr = context.description;
        descr += ", -";
        descr += ::Standard::Algorithms::Utilities::zu_string(key);

        auto iter = context.s.find(key);
        const auto expected = iter != context.s.end();
        // descr += expected ? " true" : " false";

        if constexpr (shall_print_b_tree)
        {
            std::cout << descr;
        }

        if (expected)
        {
            context.s.erase(iter);
        }

        const auto actual = tree.erase(key + key_t{});
        ::Standard::Algorithms::ert::are_equal(expected, actual, "is erased");
        ::Standard::Algorithms::ert::are_equal(context.s.size(), tree.size(), "tree size");
        tree.validate();
    }

    void erase_all_keys(b_tree_test_context &context, std::vector<key_t> &temp, tree_t &tree)
    {
        temp.clear();
        temp.insert(temp.begin(), context.s.begin(), context.s.end());

        while (!temp.empty())
        {
            const auto index = context.rnd(0, static_cast<key_t>(temp.size() - 1U));
            const auto &key = temp.at(index);
            erase_key(context, tree, key);
            temp.erase(temp.begin() + index);
        }
    }

    void find_key(b_tree_test_context &context, const tree_t &tree)
    {
        const key_t key = context.rnd();

        const auto expected = context.s.contains(key);
        const auto actual = tree.contains(key);

        ::Standard::Algorithms::ert::are_equal(
            expected, actual, "contains " + ::Standard::Algorithms::Utilities::zu_string(key));
    }

    void random_test(b_tree_test_context &context, std::vector<key_t> &temp)
    {
        auto &descr = context.description;
        descr += "Create b tree, min degree ";
        descr += ::Standard::Algorithms::Utilities::zu_string(context.min_degree);
        descr += ". Keys { ";

        tree_t tree(context.min_degree);
        tree.validate();

        for (std::int32_t index{}; index < steps; ++index)
        {
            find_key(context, tree);
            {
                const key_t key = context.rnd();
                erase_key(context, tree, key);
            }
            add_key(context, tree);
        }

        erase_all_keys(context, temp, tree);

        tree.validate();
        descr += "\nDone test\n";

        if constexpr (shall_print_b_tree)
        {
            std::cout << descr;
        }
    }

    constexpr void random_tests()
    {
        std::vector<key_t> temp;

        for (std::int32_t index{}; index < test_runs; ++index)
        {
            b_tree_test_context context;

            auto &descr = context.description;
            descr += "\nStart random test " + ::Standard::Algorithms::Utilities::zu_string(index) + "\n";

            if constexpr (shall_print_b_tree)
            {
                constexpr auto nomagic = 1023U;

                if (0U == (static_cast<std::uint64_t>(index) & nomagic))
                {
                    std::cout << "  Run random test " << ::Standard::Algorithms::Utilities::zu_string(index) << " of "
                              << test_runs << "\n";
                }
            }

            try
            {
                random_test(context, temp);
            }
            catch (const std::exception &exc)
            {
                descr += "\nError in random b tree test: ";
                descr += exc.what();
                throw std::runtime_error(descr);
            }
        }
    }

    void predefined_test()
    {
        constexpr int_t min_degree = 3;
        tree_t tree(min_degree);

        const std::vector<std::int32_t> data{ 9, 2, 5, -5, 8, 6, 7, 10, 1, 4, 3, 10, 6, 7, 4, 3, 3, 7, 8, -9, 5, -5, 7,
            4, 6, 4, 10, 4, 9, 10, 3, -8, 3, -7, 4, 2, -2, 1, -6 };

        for (const auto &key : data)
        {
            tree.validate();

            if (0 < key)
            {
                tree.insert(key + 0);
            }
            else
            {
                assert(key < 0);
                tree.erase(key);
            }
        }

        tree.validate();
    }
} // namespace

void Standard::Algorithms::Trees::Tests::b_tree_tests()
{
    predefined_test();
    random_tests();
}
