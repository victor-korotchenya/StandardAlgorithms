#pragma once
#include"prefix_tree.h"

namespace Standard::Algorithms::Trees
{
    // todo(p3): mov to strings.

    // todo(p3): make it linear, not n*n.
    // The words are case-insensitive.
    struct suffix_tree final
    {
        void set_word(const std::string &word);

        void search(const std::string &suffix, std::vector<std::string> &result, std::size_t count_limit = 0U) const;

private:
        prefix_tree Prefix_tree{};
        std::string Word{};
    };
} // namespace Standard::Algorithms::Trees
