#pragma once

#include <algorithm>
#include <cassert>
#include <cctype> // tolower
#include <map>
#include <queue>
#include <utility>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/VectorUtilities.h"

namespace
{
    constexpr int not_found = -1;

    template<typename t>
    void merge_remove_duplicates(const std::vector<t>& source,
        std::vector<t>& buffer,
        std::vector<t>& target)
    {
        if (source.empty())
            return;

        if (target.empty())
        {
            target.insert(target.end(), source.begin(), source.end());
            return;
        }

        std::swap(buffer, target);
        target.clear();

        std::merge(source.begin(), source.end(),
            buffer.begin(), buffer.end(), std::back_inserter(target));

        Privet::Algorithms::VectorUtilities::remove_duplicates(target);
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            template<//typename iterator_t,
                typename string_t = std::string,
                typename char_t = typename string_t::value_type>
                class aho_corasick final
            {
                std::vector<int> _failures;
                std::vector<string_t> _keywords;
                std::vector<std::vector<int>> _state_keywords;
                std::vector<std::map<char_t, int>> _transitions;

            public:
                explicit aho_corasick(std::vector<string_t>&& keywords
                    //iterator_t begin, iterator_t end
                ) : _keywords(std::move(keywords))
                {
                    const auto max_states = total_word_length(_keywords) + 1;
                    _state_keywords.resize(max_states);
                    _transitions.resize(max_states);

                    const auto actual_states = build_tree() + 1;
                    resize(actual_states);

                    auto q = enqueue_root();
                    bfs(q);
                }

                void find(const string_t& text,
                    std::vector<std::pair<const string_t*,
                    // word start index.
                    int>> &result) const
                {
                    result.clear();

                    auto state = 0, i = 0;
                    for (const auto& t : text)
                    {
                        const auto c = static_cast<char_t>(std::tolower(t));
                        state = next(state, c);
                        const auto& keywords = _state_keywords[state];
                        ++i;
                        for (const auto& j : keywords)
                        {
                            const auto& word = _keywords[j];
                            assert(word.size() > 0);
                            const auto beginning = i - static_cast<int>(word.size());
                            assert(beginning >= 0 && beginning < static_cast<int>(text.size()));

                            result.push_back(std::make_pair(&word, beginning));
                        }
                    }
                }

            private:
                static int total_word_length(const std::vector<string_t>& keywords)
                {
                    RequirePositive(keywords.size());

                    int64_t total_length{};
                    for (const auto& w : keywords)
                    {
                        const auto size = static_cast<int>(w.size());
                        if (size <= 0)
                            throw std::runtime_error("Empty string/too large keywords not allowed.");
                        total_length += size;

                        if (total_length + 1 >= std::numeric_limits<int>::max())
                            throw std::runtime_error("Too large keywords total length.");
                    }

                    assert(total_length > 0 && total_length < std::numeric_limits<int>::max());
                    return static_cast<int>(total_length);
                }

                int build_tree()
                {
                    auto count = 0, i = 0;
                    for (const auto& keyword : _keywords)
                    {
                        auto state = 0;
                        for (const auto& k : keyword)
                        {
                            const auto c = static_cast<char_t>(std::tolower(k));
                            auto& trans = _transitions[state];
                            auto& val = trans[c];
                            if (!val)
                                val = ++count;

                            state = val;
                        }

                        auto& state_keys = _state_keywords[state];
                        state_keys.push_back(i);
                        ++i;
                    }

                    return count;
                }

                void resize(int states)
                {
                    _state_keywords.resize(states);
                    _transitions.resize(states);
                    _failures.resize(states, not_found);

                    _state_keywords.shrink_to_fit();
                    _transitions.shrink_to_fit();
                }

                std::queue<int> enqueue_root()
                {
                    assert(_transitions[0].size());
                    std::queue<int> q;

                    for (const auto& p : _transitions[0])
                    {
                        // The first letters of each word.
                        _failures[p.second] = 0;
                        q.push(p.second);
                    }

                    assert(q.size());
                    return q;
                }

                void bfs(std::queue<int>& q)
                {
                    std::vector<int> buffer;

                    while (q.size())
                    {
                        const auto state = q.front();
                        q.pop();
                        const auto& trans = _transitions[state];

                        for (const auto& p : trans)
                        {
                            const auto f = find_failure(state, p.first);
                            _failures[p.second] = f;
                            q.push(p.second);

                            const auto& source = _state_keywords[f];
                            auto& target = _state_keywords[p.second];
                            merge_remove_duplicates(source, buffer, target);
                            assert(_failures[0] == not_found);
                        }

                        _state_keywords[state].shrink_to_fit();
                    }
                }

                int find_failure(int state, char_t c) const
                {
                    assert(state >= 0);
                    state = _failures[state];
                    if (state < 0)
                        return 0;

                    const auto result = next(state, c);
                    return result;
                }

                int next(int state, char_t c) const
                {
                    assert(state >= 0);
                    for (;;)
                    {
                        const auto& trans = _transitions[state];
                        const auto it = trans.find(c);
                        if (it != trans.end())
                            return it->second;

                        state = _failures[state];
                        if (state < 0)
                            return 0;
                    }
                }
            };
        }
    }
}