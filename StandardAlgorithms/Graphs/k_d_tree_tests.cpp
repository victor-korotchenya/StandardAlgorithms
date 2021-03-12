#include"k_d_tree_tests.h"
#include"k_d_tree.h"
#include"multiway_search_tree_tests.h"

void Standard::Algorithms::Trees::Tests::k_d_tree_tests()
{
    using tree_t = Standard::Algorithms::Trees::k_d_tree<tuple_t, dimensions>;
    multiway_search_tree_tests<tree_t>();
}
