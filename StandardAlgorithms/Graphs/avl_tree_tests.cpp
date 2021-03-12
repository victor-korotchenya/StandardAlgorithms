#include"avl_tree_tests.h"
#include"avl_tree.h"
#include"bst_tests.h"

namespace Standard::Algorithms::Trees
{
    template<class key_t>
    auto operator<< (std::ostream &str, const avl_node<key_t> &node) -> std::ostream &
    {
        str << "avl key '" << node.key << "', count " << node.count << ", height " << node.height << ", has left "
            << (node.left != nullptr) << ", has right " << (node.right != nullptr);
        return str;
    }
} // namespace Standard::Algorithms::Trees

void Standard::Algorithms::Trees::Tests::avl_tree_tests()
{
    using key_t = std::int16_t;
    using tree_t = Standard::Algorithms::Trees::avl_tree<key_t>;
    using rg_t = Standard::Algorithms::Utilities::random_t<key_t>;

    {
        using node_t = tree_t::node_t;
        static_assert(Standard::Algorithms::Trees::Inner::can_count_v<node_t>);
    }

    bst_tests<key_t, tree_t, rg_t>();
}
