#include"treap_tests.h"
#include"bst_tests.h"
#include"treap.h"

namespace Standard::Algorithms::Heaps
{
    template<class key_t, std::integral priority_t>
    auto operator<< (std::ostream &str, const treap_node<key_t, priority_t> &node) -> std::ostream &
    {
        str << "treap_node key " << node.key << ", has parent " << (node.parent != nullptr) << ", has left "
            << (node.left != nullptr) << ", has right " << (node.right != nullptr);

        return str;
    }
} // namespace Standard::Algorithms::Heaps

void Standard::Algorithms::Heaps::Tests::treap_tests()
{
    using key_t = std::uint16_t;
    using key_rg_t = Standard::Algorithms::Utilities::random_t<key_t>;

    using priority_t = std::int64_t;
    using prior_rg_t = Standard::Algorithms::Utilities::random_t<priority_t>;

    using tree_t = Standard::Algorithms::Heaps::treap<prior_rg_t, key_t>;

    {
        using node_t = tree_t::node_t;
        static_assert(!Standard::Algorithms::Trees::Inner::can_count_v<node_t>);
    }

    ::Standard::Algorithms::Trees::Tests::bst_tests<key_t, tree_t, key_rg_t>();
}
