#pragma once
#include"../Utilities/check_size.h"
#include"../Utilities/remove_duplicates.h"
#include<cctype> // tolower
#include<map>
#include<queue>

namespace Standard::Algorithms::Strings::Inner
{
    template<class item_t>
    constexpr void merge_remove_duplicates(
        const std::vector<item_t> &source, std::vector<item_t> &buffer, std::vector<item_t> &destination)
    {
        if (source.empty())
        {
            return;
        }

        if (destination.empty())
        {
            destination.insert(destination.end(), source.begin(), source.end());
            return;
        }

        std::swap(buffer, destination);
        destination.clear();

        std::merge(source.begin(), source.end(), buffer.begin(), buffer.end(), std::back_inserter(destination));

        ::Standard::Algorithms::remove_duplicates(destination);
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    // Case-insensitive search.
    template<class string_t, class char_t = typename string_t::value_type,
        std::signed_integral word_start_index_t = std::int32_t>
    struct aho_corasick final
    {
        static constexpr word_start_index_t not_found = -word_start_index_t{ 1 };

        constexpr explicit aho_corasick(std::vector<string_t> &&words)
            : Keywords(std::move(words))
        {
            const auto max_positions = total_word_length(Keywords) + 1LL;
            Position_keyword_indexes.resize(max_positions);
            Transitions.resize(max_positions);

            const auto actual_positions = build_tree() + 1LL;
            resize(actual_positions);

            auto que = enqueue_root();
            bfs(que);
        }

        constexpr void find(
            const string_t &text, std::vector<std::pair<const string_t *, word_start_index_t>> &result) const
        {
            result.clear();

            word_start_index_t position{};
            word_start_index_t index{};

            for (const auto &raw : text)
            {
                const auto cha = static_cast<char_t>(std::tolower(raw));
                position = next(position, cha);

                const auto &keyword_indexes = Position_keyword_indexes[position];
                ++index;

                for (const auto &chosen : keyword_indexes)
                {
                    const auto &word = Keywords[chosen];
                    assert(!word.empty());

                    const auto beginning = index - static_cast<word_start_index_t>(word.size());

                    assert(!(beginning < word_start_index_t{}) &&
                        beginning < static_cast<word_start_index_t>(text.size()));

                    result.emplace_back(&word, beginning);
                }
            }
        }

private:
        [[nodiscard]] static constexpr auto total_word_length(const std::vector<string_t> &keywords)
            -> word_start_index_t
        {
            require_positive(keywords.size(), "keywords size");

            constexpr word_start_index_t max_size = std::numeric_limits<word_start_index_t>::max() - 1;

            static_assert(word_start_index_t{} < max_size);

            std::int64_t total_length{};

            for (const auto &word : keywords)
            {
                const auto size = Standard::Algorithms::Utilities::check_size<word_start_index_t, max_size>(
                    "keyword size", word.size());

                if (!(word_start_index_t{} < size)) [[unlikely]]
                {
                    throw std::runtime_error("Empty keyword is not allowed.");
                }

                total_length += size;
                assert(0 < total_length);

                {
                    [[maybe_unused]] const auto unused =
                        Standard::Algorithms::Utilities::check_size<word_start_index_t, max_size>(
                            "First keywords size", total_length);
                }
            }

            assert(0 < total_length && total_length < std::numeric_limits<word_start_index_t>::max());

            return static_cast<word_start_index_t>(total_length);
        }

        [[nodiscard]] constexpr auto build_tree() -> word_start_index_t
        {
            word_start_index_t position_count{};
            word_start_index_t index{};

            for (const auto &keyword : Keywords)
            {
                word_start_index_t position{};

                for (const auto &raw : keyword)
                {
                    const auto cha = static_cast<char_t>(std::tolower(raw));
                    auto &trans = Transitions[position];
                    auto &val = trans[cha];
                    if (val == word_start_index_t{})
                    {
                        val = ++position_count;
                    }

                    position = val;
                }

                auto &position_keys = Position_keyword_indexes[position];
                position_keys.push_back(index);
                ++index;
            }

            return position_count;
        }

        constexpr void resize(word_start_index_t positions)
        {
            Position_keyword_indexes.resize(positions);
            Transitions.resize(positions);
            Failures.resize(positions, not_found);

            Position_keyword_indexes.shrink_to_fit();
            Transitions.shrink_to_fit();
        }

        [[nodiscard]] constexpr auto enqueue_root() -> std::queue<word_start_index_t>
        {
            assert(!Transitions.empty());

            const auto &start = Transitions.at(0);
            assert(!start.empty());

            std::queue<word_start_index_t> que;

            for (const auto &pai : start)
            {// The first letters of each word.
                Failures[pai.second] = {};
                que.push(pai.second);
            }

            assert(!que.empty());
            return que;
        }

        constexpr void bfs(std::queue<word_start_index_t> &que)
        {
            std::vector<word_start_index_t> buffer;

            while (!que.empty())
            {
                const auto position = que.front();
                que.pop();

                const auto &trans = Transitions[position];

                for (const auto &pai : trans)
                {
                    const auto fail = find_failure(position, pai.first);
                    Failures[pai.second] = fail;
                    que.push(pai.second);

                    const auto &source = Position_keyword_indexes[fail];
                    auto &destination = Position_keyword_indexes[pai.second];
                    Inner::merge_remove_duplicates(source, buffer, destination);
                    assert(Failures.at(0) == not_found);
                }

                Position_keyword_indexes[position].shrink_to_fit();
            }
        }

        [[nodiscard]] constexpr auto find_failure(word_start_index_t position, char_t cha) const -> word_start_index_t
        {
            assert(!(position < word_start_index_t{}));

            position = Failures[position];
            if (position < word_start_index_t{})
            {
                return {};
            }

            auto result = next(position, cha);
            return result;
        }

        [[nodiscard]] constexpr auto next(word_start_index_t position, char_t cha) const -> word_start_index_t
        {
            assert(!(position < word_start_index_t{}));

            for (;;)
            {
                const auto &trans = Transitions[position];
                const auto iter = trans.find(cha);
                if (iter != trans.end())
                {
                    return iter->second;
                }

                position = Failures[position];
                if (position < word_start_index_t{})
                {
                    return {};
                }
            }
        }

        std::vector<string_t> Keywords{};
        std::vector<word_start_index_t> Failures{};
        std::vector<std::vector<word_start_index_t>> Position_keyword_indexes{};
        std::vector<std::map<char_t, word_start_index_t>> Transitions{};
    };
} // namespace Standard::Algorithms::Strings
