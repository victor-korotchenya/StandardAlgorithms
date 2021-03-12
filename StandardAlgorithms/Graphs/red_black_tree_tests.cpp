#include"red_black_tree_tests.h"
#include"red_black_tree.h"
// Must respect the include order.
#include"bst_tests.h"

namespace Standard::Algorithms::Trees
{
    template<class key_t>
    auto operator<< (std::ostream &str, const red_black_node<key_t> &node) -> std::ostream &
    {
        str << "'" << node.key << (node.is_red ? "' red" : "'");
        return str;
    }
} // namespace Standard::Algorithms::Trees

void Standard::Algorithms::Trees::Tests::red_black_tree_tests()
{
    using key_t = std::uint32_t;
    using tree_t = Standard::Algorithms::Trees::red_black_tree<key_t>;
    using node_t = tree_t::node_t;

    static_assert(!Standard::Algorithms::Trees::Inner::can_count_v<node_t>);

    using rg_t = Standard::Algorithms::Utilities::random_t<key_t>;

    /*{
        tree_t tree{};
        tree.insert(2);
        tree.insert(1);
        tree.insert(3);
        tree.insert(0);
        tree.validate();

        Standard::Algorithms::Trees::print_tree<key_t, node_t>(std::cout, tree);

        tree.erase(3);

        Standard::Algorithms::Trees::print_tree<key_t, node_t>(std::cout, tree);

        tree.validate();
    }*/

    constexpr const auto &sentinel = tree_t::sentinel_node();

    bst_tests<key_t, tree_t, rg_t>(&sentinel);
}
