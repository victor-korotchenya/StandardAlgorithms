#include"suffix_tree_tests.h"
#include"../Utilities/ert.h"
#include"suffix_tree.h"
#include<limits>

void Standard::Algorithms::Trees::Tests::suffix_tree_tests()
{
    const std::string word = "Bananas";

    suffix_tree tree;
    tree.set_word(word);

    constexpr auto count_limit = std::numeric_limits<std::uint32_t>::max();

    std::vector<std::string> actual;
    {
        const std::string suffix = "BN";
        tree.search(suffix, actual, count_limit);
        ::Standard::Algorithms::ert::empty(actual, suffix);
    }
    {
        const std::string suffix = "Ananas";
        tree.search(suffix, actual, count_limit);

        const std::vector<std::string> expected = { word };
        ::Standard::Algorithms::ert::are_equal(expected, actual, suffix);
    }
}
