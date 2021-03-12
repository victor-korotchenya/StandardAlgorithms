#include"suffix_tree.h"
#include"../Strings/string_utilities.h" // to_lower
#include"../Utilities/require_utilities.h"
#include<stdexcept>

void Standard::Algorithms::Trees::suffix_tree::set_word(const std::string &word)
{
    const auto size = require_positive(word.size(), "word size");

    auto word_lower_case = word;
    ::Standard::Algorithms::Strings::to_lower(word_lower_case);

    this->Word = word;
    Prefix_tree.clear();

    // For now, use a simple method.
    for (std::size_t start = 1; start < size; ++start)
    {
        auto sub_string = word_lower_case.substr(start);
        Prefix_tree.add_word(std::move(sub_string));
    }

    Prefix_tree.add_word(std::move(word_lower_case));
}

void Standard::Algorithms::Trees::suffix_tree::search(
    const std::string &suffix, std::vector<std::string> &result, std::size_t count_limit) const
{
    require_positive(suffix.size(), "suffix size");

    if (0U == count_limit)
    {
        count_limit = std::numeric_limits<std::size_t>::max();
    }

    result.clear();
    if (Word.empty() || Prefix_tree.is_empty())
    {
        return;
    }

    auto suffix_lower_case = suffix;
    ::Standard::Algorithms::Strings::to_lower(suffix_lower_case);

    std::vector<std::string> temp;
    Prefix_tree.search(suffix_lower_case, temp, count_limit);

    if (!temp.empty())
    {
        result.push_back(Word);
    }
}
