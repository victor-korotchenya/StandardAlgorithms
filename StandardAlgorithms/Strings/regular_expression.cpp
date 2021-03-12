#include <array>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../Numbers/Arithmetic.h"
#include "../Utilities/StreamUtilities.h"
#include "regular_expression.h"

using namespace std;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms;

namespace
{
    bool are_stars_only(const string_view& s)
    {
        const auto pos = s.find_first_not_of('*');
        const auto stars_only = pos == string::npos;
        return stars_only;
    }

    string_view one_less_size(const string_view& s)
    {
        assert(s.size());
        return string_view(s.begin(), s.end() - 1);
    }

    // Go from the end.
    void is_wildcard_match_slow_rec(vector<vector<signed char>>& buf,
        const string_view& pattern, const string_view& text)
    {
        auto& val = buf[pattern.size()][text.size()];
        assert(val < 0 && pattern.size());

        if (text.empty())
        {
            val = are_stars_only(pattern);
            return;
        }

        if (const auto& pc = pattern.back(); pc != '*')
        {
            if (pc != '?' && pc != text.back())
            {
                val = 0;
                return;
            }

            auto& val2 = buf[pattern.size() - 1][text.size() - 1];
            if (val2 < 0)
                is_wildcard_match_slow_rec(buf, one_less_size(pattern), one_less_size(text));

            val = val2;
            assert(val >= 0);
            return;
        }

        {
            auto& skip = buf[pattern.size() - 1][text.size()];
            if (skip < 0)
                is_wildcard_match_slow_rec(buf, one_less_size(pattern), text);

            if (skip > 0)
            {
                val = 1;
                return;
            }
        }

        auto& take = buf[pattern.size()][text.size() - 1];
        if (take < 0)
            is_wildcard_match_slow_rec(buf, pattern, one_less_size(text));
        val = take > 0;
    }

    // NFA, NFM, NFSA, NFSM, NFSSA, NDFSM
    auto build_nfa(const string_view& pattern)
    {
        assert(pattern.size());

        vector<vector<pair<size_t, char>>> automaton;
        automaton.reserve(pattern.size() + 1);
        automaton.emplace_back();

        for (size_t i{}; i < pattern.size();)
        {
            if (const auto& c = pattern[i]; c != '*' && c != '?')
            {
                automaton.back().emplace_back(automaton.size(), c);
                automaton.emplace_back();
                ++i;
                continue;
            }

            // "?*?**?" -> "???*"
            size_t required{};
            auto has_star = false;
            do
            {
                required += pattern[i] == '?';
                has_star = has_star || pattern[i] == '*';
            } while (++i < pattern.size() && (pattern[i] == '*' || pattern[i] == '?'));

            assert(required || has_star);
            while (required--)
            {
                automaton.back().emplace_back(automaton.size(), '?');
                automaton.emplace_back();
            }

            if (has_star) // Remain in the current state.
                automaton.back().emplace_back(automaton.size() - 1, '*');
        }

        assert(automaton.size());
        return automaton;
    }

    bool accept(const vector<vector<pair<size_t, char>>>& automaton, const string_view& text)
    {
        assert(automaton.size() && text.size());

        array<unordered_set<size_t>, 2> buf;
        buf[0].insert(0);

        auto pos = 0;
        for (size_t i{};;)
        {
            const auto& prev = buf[pos];
            assert(prev.size());

            auto& cur = buf[pos ^ 1];
            cur.clear();

            const auto& c = text[i];

            for (const auto& pr : prev)
            {
                assert(pr < automaton.size());

                const auto& states = automaton[pr];

                // The last state might be empty.
                assert(states.size() || pr + 1 == automaton.size());

                for (const auto& state : states)
                {
                    if (state.second == '*')
                    {// Stay.
                        assert(pr == state.first);

                        cur.insert(pr);
                    }
                    else
                    {
                        assert(pr + 1 == state.first && state.first < automaton.size());
                        if (state.second == '?' || state.second == c)
                            // Move to the next state.
                            cur.insert(state.first);
                    }
                }
            }

            if (cur.empty())
                return false;

            if (++i == text.size())
            {
                const auto has = cur.count(automaton.size() - 1);
                return has > 0;
            }

            pos ^= 1;
        }
    }
}

bool Privet::Algorithms::Strings::is_wildcard_match_slow_still(const string_view& pattern, const string_view& text)
{
    // It can be cached.
    if (are_stars_only(pattern))
        return true; // Including pattern.empty()

    if (text.empty()) // At least 1 symbol unmatched.
        return false;

    // It can be cached.
    const auto automaton = build_nfa(pattern);

    const auto result = accept(automaton, text);
    return result;
}

bool Privet::Algorithms::Strings::is_wildcard_match_slow(const string_view& pattern, const string_view& text)
{
    if (pattern.empty())
        return true;

    vector<vector<signed char>> buf(pattern.size() + 1,
        vector<signed char>(text.size() + 1, -1));

    for (size_t i = 0; i <= text.size(); ++i)
        buf[0][i] = !i;

    const auto& result = buf.back().back();
    if (result < 0)
        is_wildcard_match_slow_rec(buf, pattern, text);

    assert(result >= 0);
    return result > 0;
}