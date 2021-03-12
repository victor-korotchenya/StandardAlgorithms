#include"prefix_tree.h"
#include"../Numbers/binary_search.h"
#include"../Numbers/sort.h"
#include"../Utilities/require_utilities.h"
#include<queue>
#include<unordered_set>

namespace
{
    using node_t = Standard::Algorithms::Trees::prefix_node;

    using string_node_pair_t = std::pair<std::string, node_t>;

    constexpr auto find_node_by_first_char(const std::vector<node_t> &children, const char first_symbol)
        -> const node_t *
    {
        const auto spa = std::span(children.data(), children.size());

        using node_to_char_cast = char (*)(const node_t &node);

        // interpolation_search seems to be 3% slower than binary_search on 58K words.
        const auto *const result =
            Standard::Algorithms::Numbers::binary_search_with_cast<const node_t, char, node_to_char_cast>(
                spa, first_symbol, Standard::Algorithms::Trees::first_symbol);

        return result;
    }

    constexpr void add_word_as_child(std::string &&word, std::vector<node_t> &children)
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            Standard::Algorithms::throw_if_empty("word", word);
        }

        const auto was_empty = children.empty();

        {
            node_t prefix_node;
            prefix_node.value = std::move(word);
            prefix_node.has_word = true;

            children.push_back(std::move(prefix_node));
        }

        if (was_empty)
        {
            return;
        }

        // todo(p4): Not sure that "const" is applicable here?
        // NOLINTNEXTLINE
        std::span spa = children;

        Standard::Algorithms::Numbers::insertion_sort_step<node_t>(spa, children.size() - 1U);
    }

    constexpr auto first_matching_symbols_count(const std::size_t word_position, const std::string &word,
        const std::size_t left_to_match, const std::string &node_value, const std::size_t node_value_size)
        -> std::size_t
    {
        const auto min_length = std::min(left_to_match, node_value_size);

        constexpr std::size_t skip_first_matching_symbol = 1;
        auto result = skip_first_matching_symbol;

        while (result < min_length && node_value[result] == word[word_position + result])
        {
            ++result;
        }

        return result;
    }

    constexpr auto find_lowest_node(const std::string &word, const node_t &start_node0)
        -> Standard::Algorithms::Trees::find_node_result
    {
        const auto *start_node = &start_node0;

        const auto word_size = word.size();
        std::size_t word_position{};

        for (;;)
        {
            assert(start_node != nullptr);

            const auto &node_value = start_node->value;
            const auto node_value_size = node_value.size();

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (0U == node_value_size) [[unlikely]]
                {
                    throw std::runtime_error("0 == nodeValueSize");
                }

                if (word[word_position] != node_value[0]) [[unlikely]]
                {
                    throw std::runtime_error("The first symbol must match, but word[wordPosition] != nodeValue[0].");
                }
            }

            const auto left_to_match = word_size - word_position;

            const auto matched_symbol_count =
                first_matching_symbols_count(word_position, word, left_to_match, node_value, node_value_size);

            if (const auto is_partial_match = matched_symbol_count != node_value_size; is_partial_match)
            {
                Standard::Algorithms::Trees::find_node_result result;
                result.node = start_node;

                result.word_position = word_position;
                result.matched_symbol_count = matched_symbol_count;

                result.is_partial_match = true;

                result.has_match = left_to_match == matched_symbol_count;

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    validate(result);
                }

                return result;
            }

            if (const auto is_complete_match = word_size == word_position + node_value_size; is_complete_match)
            {
                Standard::Algorithms::Trees::find_node_result result;
                result.node = start_node;

                result.word_position = word_position;
                result.matched_symbol_count = node_value_size;

                result.is_partial_match = false;
                result.has_match = true;

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    validate(result);
                }

                return result;
            }

            // Continue searching in the node's children.
            const auto *const child_node =
                find_node_by_first_char(start_node->children, word[word_position + node_value_size]);

            if (nullptr == child_node)
            {
                Standard::Algorithms::Trees::find_node_result result;
                result.node = start_node;

                result.word_position = word_position;
                result.matched_symbol_count = node_value_size;

                result.is_partial_match = false;
                result.has_match = false;

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    validate(result);
                }

                return result;
            }

            start_node = child_node;
            word_position += node_value_size;
        }
    }

    constexpr void split_partial_match(
        std::string &&word, const std::size_t word_position, node_t &node, const std::size_t matched_symbol_count)
    {
        assert(!word.empty());

        {
            node_t middle_node;
            middle_node.value = node.value.substr(matched_symbol_count);
            middle_node.has_word = node.has_word;

            node.value = node.value.substr(0U, matched_symbol_count);

            std::swap(middle_node.children, node.children);

            // No need to sort as there is  only one child node.
            node.children.push_back(std::move(middle_node));
        }

        const auto word_size = word.size();
        const auto is_complete_match = word_size == word_position + matched_symbol_count;

        node.has_word = is_complete_match;

        if (!is_complete_match)
        {
            auto reminder = word.substr(word_position + matched_symbol_count);
            add_word_as_child(std::move(reminder), node.children);
        }
    }

    constexpr void add_word_to_node(std::string &&word, node_t &start_node)
    {
        const auto find_node_result = find_lowest_node(word, start_node);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            validate(find_node_result);
        }

        auto *node =
            // NOLINTNEXTLINE
            const_cast<node_t *>(find_node_result.node);

        assert(node != nullptr);

        if (find_node_result.is_partial_match)
        {
            split_partial_match(
                std::move(word), find_node_result.word_position, *node, find_node_result.matched_symbol_count);

            return;
        }

        if (find_node_result.has_match)
        {
            node->has_word = true;
            return;
        }

        const auto offset = find_node_result.word_position + find_node_result.matched_symbol_count;
        auto sub_word = word.substr(offset);

        add_word_as_child(std::move(sub_word), node->children);
    }

    constexpr void enqueue_children(
        std::string &&parent_string, const std::vector<node_t> &children, std::queue<string_node_pair_t> &que)
    {
        for (const auto &node : children)
        {
            // todo(p4): std::move(parent_string) for the last node.
            que.emplace(parent_string, node);
        }
    }

    constexpr void check_unique_prefixes([[maybe_unused]] const std::vector<node_t> &children)
    {
        if constexpr (!::Standard::Algorithms::is_debug)
        {
            return;
        }

        constexpr std::size_t skip_first_item = 1;
        const auto length = children.size();

        for (auto index = skip_first_item; index < length; ++index)
        {
            const auto &previous_node = children[index - skip_first_item];
            const auto previous_symbol = first_symbol(previous_node);

            const auto &current_node = children[index];
            const auto current_symbol = first_symbol(current_node);

            if (previous_symbol < current_symbol)
            {
                continue;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The terms are not sorted properly at index " << index << ", previous value '" << previous_node.value
                << "', current value '" << current_node.value << "' in the prefix tree.";

            Standard::Algorithms::throw_exception(str);
        }
    }

    void insert_unique_word(const std::string &word, const std::string &prefix, std::unordered_set<std::string> &words)
    {
        Standard::Algorithms::throw_if_empty("word", word);
        Standard::Algorithms::throw_if_empty("prefix", prefix);

        const auto insert_result = words.insert(word);
        if (!insert_result.second)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The prefix '" << prefix << "' is repeated in prefix_tree when adding the word '" << word << "'.";
            Standard::Algorithms::throw_exception(str);
        }
    }

    void check_unique_words([[maybe_unused]] const std::vector<node_t> &roots)
    {
        if constexpr (!::Standard::Algorithms::is_debug)
        {
            return;
        }

        std::unordered_set<std::string> words;
        std::queue<string_node_pair_t> que;

        {
            node_t root;
            root.has_word = false;
            root.children = roots;

            que.emplace(std::string{}, std::move(root));
        }

        do
        {
            const auto top = que.front();
            que.pop();

            const auto &prefix_node = top.second;
            auto parent_string = top.first + prefix_node.value;

            if (prefix_node.has_word)
            {
                insert_unique_word(parent_string, prefix_node.value, words);
            }

            const auto &children = prefix_node.children;
            check_unique_prefixes(children);

            enqueue_children(std::move(parent_string), children, que);
        } while (!que.empty());
    }

    constexpr void check_search_result([[maybe_unused]] const std::string &prefix,
        [[maybe_unused]] const std::vector<std::string> &result, [[maybe_unused]] const std::size_t count_limit)
    {
        if constexpr (!::Standard::Algorithms::is_debug)
        {
            return;
        }

        assert(0U < count_limit);

        if (result.empty())
        {
            return;
        }

        if (count_limit < result.size())
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << count_limit << "countLimit < result.size() " << result.size()
                << " in the prefix_tree.search. prefix '" << prefix << "'.";

            Standard::Algorithms::throw_exception(str);
        }

        const auto prefix_size = Standard::Algorithms::require_positive(prefix.size(), "prefix size");

        std::unordered_set<std::string> words;

        for (const auto &word : result)
        {
            const auto insert_result = words.insert(word);
            if (!insert_result.second)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The word '" << word << "' is repeated in the result of prefix_tree.search. prefix '" << prefix
                    << "'.";
                Standard::Algorithms::throw_exception(str);
            }

            const auto word_size = word.size();
            if (word_size < prefix_size)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The word '" << word << "' cannot have a prefix '" << prefix << "'.";
                Standard::Algorithms::throw_exception(str);
            }

            const std::string_view word_view{ word };
            const auto word_substring = word_view.substr(0U, prefix_size);
            if (word_substring != prefix) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The resulting word '" << word << "' must have a prefix '" << prefix << "'.";

                throw std::runtime_error(str.str());
            }
        }
    }

    [[nodiscard]] constexpr auto form_root_word(
        const std::string &prefix, const std::size_t last_word_position, const std::string &node_value) -> std::string
    {
        if (last_word_position == 0U)
        {
            return node_value;
        }

        auto root_word = prefix.substr(0U, last_word_position) + node_value;
        return root_word;
    }

    void append_children_to_result(const std::size_t count_limit, const std::string &root_word,
        const node_t *const node, std::vector<std::string> &result)
    {
        assert(0U < count_limit);

        std::queue<string_node_pair_t> que;
        {
            node_t root;
            root.has_word = node->has_word;
            root.children = node->children;

            que.emplace(root_word, std::move(root));
        }

        do
        {
            const auto top = que.front();
            que.pop();

            const auto &prefix_node = top.second;
            auto parent_string = top.first + prefix_node.value;

            if (prefix_node.has_word)
            {
                if (count_limit <= result.size() + 1U)
                {
                    result.push_back(std::move(parent_string));
                    return;
                }

                result.push_back(parent_string);
            }

            const auto &children = prefix_node.children;
            check_unique_prefixes(children);

            enqueue_children(std::move(parent_string), children, que);
        } while (!que.empty());
    }
} // namespace

void Standard::Algorithms::Trees::prefix_tree::clear() noexcept
{
    Roots.clear();
}

void Standard::Algorithms::Trees::prefix_tree::add_word(std::string &&word) &
{
    throw_if_empty("word", word);

    auto *start_node =
        // NOLINTNEXTLINE
        const_cast<node_t *>(find_node_by_first_char(Roots, word[0]));
    if (nullptr == start_node)
    {
        add_word_as_child(std::move(word), Roots);
    }
    else
    {
        add_word_to_node(std::move(word), *start_node);
    }

    check_unique_words(Roots);
}

void Standard::Algorithms::Trees::prefix_tree::search(
    const std::string &prefix, std::vector<std::string> &result, std::size_t count_limit) const &noexcept(false)
{
    throw_if_empty("prefix", prefix);

    if (0U == count_limit)
    {
        count_limit = std::numeric_limits<std::size_t>::max();
    }

    result.clear();

    const auto *start_node = find_node_by_first_char(Roots, prefix[0]);
    if (nullptr == start_node)
    {
        return;
    }

    const auto find_node_result = find_lowest_node(prefix, *start_node);

    if constexpr (::Standard::Algorithms::is_debug)
    {
        validate(find_node_result);
    }

    const auto *lowest_node = find_node_result.node;

    if (const auto not_found = !find_node_result.has_match || nullptr == lowest_node; not_found)
    {
        return;
    }

    auto root_word = form_root_word(prefix, find_node_result.word_position, lowest_node->value);

    append_children_to_result(count_limit, root_word, lowest_node, result);

    // todo(p4): Not sure that "const" is applicable here?
    // NOLINTNEXTLINE
    std::span spa = result;

    // BFS can get unsorted data.
    Standard::Algorithms::Numbers::insertion_sort(spa);

    check_search_result(prefix, result, count_limit);
}
