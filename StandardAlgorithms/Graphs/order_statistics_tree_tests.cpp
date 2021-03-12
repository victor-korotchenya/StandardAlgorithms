// #include<iostream>
#include"order_statistics_tree_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"avl_tree.h"
#include"order_statistics_tree_slow.h"

namespace
{
    using key_t = std::int16_t;

    template<class slow_tree_t, class tree_t>
    constexpr void erase_ost(std::string &description, const key_t &key, slow_tree_t &slow, tree_t &tree)
    {
        description += " Erase ";
        description += std::to_string(key);

        const auto expected = slow.erase(key);
        description += expected ? " yes" : " no";

        const auto actual = tree.erase(key);
        ::Standard::Algorithms::ert::are_equal(expected, actual, "erase");
    }

    template<class slow_tree_t, class tree_t>
    constexpr void insert_ost(std::string &description, const key_t &key, slow_tree_t &slow, tree_t &tree)
    {
        description += ", insert ";
        description += std::to_string(key);

        const auto expected = slow.insert(key);
        description += expected ? " yes" : " no";

        const auto actual = tree.insert(key);
        ::Standard::Algorithms::ert::are_equal(expected, actual.second, "insert");
    }

    template<class slow_tree_t, class tree_t>
    constexpr void lower_bound_ost(std::string &description, const key_t &key, slow_tree_t &slow, const tree_t &tree)
    {
        description += ", lower bound ";
        description += std::to_string(key);

        const auto temp = tree.lower_bound_rank(key);

        ::Standard::Algorithms::ert::is_not_null_ptr(temp.first, "tree lower_bound_rank");

        assert(temp.first != nullptr);

        const auto actual = std::make_pair(temp.first->key, temp.second);

        description += " slow";

        const auto expected = slow.lower_bound_rank(key);
        ::Standard::Algorithms::ert::are_equal(expected, actual, "lower_bound_rank");
    }

    template<class rg_t, class slow_tree_t, class tree_t>
    constexpr void select_ost(std::string &description, rg_t &rnd, slow_tree_t &slow, const tree_t &tree)
    {
        assert(!slow.is_empty());

        description += ", select ";

        const auto rank1 = rnd(key_t{}, static_cast<key_t>(slow.size() - 1U));
        description += std::to_string(rank1);
        description += " size ";
        description += std::to_string(slow.size());

        const auto node = tree.select(rank1);
        ::Standard::Algorithms::ert::is_not_null_ptr(node, "tree select");
        assert(node);

        const auto &actual = node->key;

        description += " slow";
        // std::cout << description << "\n\n\n";        ::Standard::Algorithms::print("Data", slow.data(), std::cout);

        const auto expected = slow.select(rank1);
        ::Standard::Algorithms::ert::are_equal(expected, actual, "select");
    }

    template<class tree_t>
    constexpr void ost_random_test(const std::string &name, std::string &description)
    {
        assert(!name.empty());

        description.clear();

        Standard::Algorithms::Trees::order_statistics_tree_slow<key_t> slow;
        description += "Created slow\n";

        tree_t tree{};
        description += "Created ";
        description += name;

        Standard::Algorithms::Utilities::random_t<std::int16_t> rnd{};

        constexpr auto max_steps = 50;

        for (std::int32_t step{}; step < max_steps; ++step)
        {
            description += "\n";
            tree.validate();

            erase_ost(description, rnd(), slow, tree);
            insert_ost(description, rnd(), slow, tree);
            lower_bound_ost(description, rnd(), slow, tree);
            select_ost(description, rnd, slow, tree);
        }

        while (!slow.is_empty())
        {
            lower_bound_ost(description, rnd(), slow, tree);
            select_ost(description, rnd, slow, tree);

            const auto key = slow.data().at(0);
            erase_ost(description, key, slow, tree);
            tree.validate();
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::order_statistics_tree_tests()
{
    std::string description;
    try
    {
        ost_random_test<Standard::Algorithms::Trees::avl_tree<key_t>>("AVL tree", description);

        // todo(p3): red_black_tree.select(std::size_t rank)
    }
    catch (const std::exception &exc)
    {
        description += "\n Error: ";
        description += exc.what();

        throw std::runtime_error(description);
    }
}
