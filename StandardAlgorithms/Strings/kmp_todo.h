#pragma once
#include <algorithm>
#include <cassert>
#include <forward_list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Numbers/Arithmetic.h"

namespace
{
    constexpr auto zero_pointer = -1;

    template<typename Char, typename Size>
    std::vector<std::pair<Char, Size>> to_char_length(
        const std::string& text)
    {
        Char previous = text[0];

        std::vector<std::pair<Char, Size>> result(
            1, std::make_pair(previous, 1));

        const auto size = text.size();

        for (Size i = 1; i < size; ++i)
        {
            if (text[i] == previous)
            {
                ++(result[result.size() - 1].second);
            }
            else
            {
                previous = text[i];
                result.push_back(std::make_pair(previous, 1));
            }
        }

        return result;
    }

    template<typename Size>
    std::pair<Size, Size> calc_limits(const Size size)
    {
        const auto large = 3;//There is something between the first and last chars.
        const auto isSmall = size < large;
        const auto result = std::make_pair(isSmall ? large : 1, isSmall ? large : size - 2);
        return result;
    }

    template<typename Char, typename Size>
    std::vector<Size> calc_table(
        const std::vector<std::pair<Char, Size>>& word)
    {
        constexpr auto minus_one = static_cast<Size>(0 - static_cast<Size>(1));

        const auto size = static_cast<Size>(word.size());

        Size pos = minus_one;

        std::vector<Size> result(size + 1);
        result[0] = minus_one;

        const auto minMaxInclusive = calc_limits<Size>(size);

        Size i = 0;
        do
        {
            while (minus_one != pos && (word[pos].first != word[i].first
                || word[i].second < word[pos].second
                || is_within<Size>(minMaxInclusive.first, minMaxInclusive.second, pos)
                && word[pos].second < word[i].second))
            {
                pos = result[pos];
            }

            result[++i] = ++pos;
        } while (i < size);

        return result;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            //String search by Knuth, Morris, Pratt.
            template<typename Char,
                typename Size = size_t>
                Size KnuthMorrisPratt(
                    //TODO: p3. check both text and word are normalized
                    //e.i. any 2 consecutive chars are never the same.
                    const std::vector<std::pair<Char, Size>>& text,
                    const std::vector<std::pair<Char, Size>>& word)
            {
                const auto text_size = text.size();
                RequirePositive(text_size, "text size");

                const auto word_size = static_cast<Size>(word.size());
                RequirePositive(word_size, "word size");

                constexpr Size minus_one = static_cast<Size>(0 - static_cast<Size>(1));
                if (text_size < word_size)
                {
                    return minus_one;
                }

                const auto table = calc_table<Char, Size>(word);
                const auto minMaxInclusive = calc_limits<Size>(word_size);

                Size i = 0, pos = 0, i2 = 0;
                do
                {
                    while (minus_one != pos && (text[i].first != word[pos].first
                        || text[i].second < word[pos].second
                        || is_within<Size>(minMaxInclusive.first, minMaxInclusive.second, pos)
                        && word[pos].second < text[i].second))
                    {
                        pos = table[pos];
                    }

                    if (word_size == ++pos)
                    {
                        Size result = i2 //There might be extra symbols at the start.
                            + std::min(text[i].second, word[word_size - 1].second)
                            - word[0].second,
                            j = 0;

                        while (++j < word_size)
                        {
                            result -= word[j].second;
                        }

                        return result;
                    }

                    i2 += text[i].second;
                } while (++i < text_size);

                return minus_one;
            }

            //String search by Knuth, Morris, Pratt.
            template<typename string_t = std::string,
                typename Size = size_t>
                Size KnuthMorrisPrattStr_todo(
                    const string_t& text,
                    const string_t& word)
            {
                RequirePositive(text.size(), "text size");
                RequirePositive(word.size(), "word size");

                using Char = std::string::value_type;

                const auto textPair = to_char_length<Char, Size>(text);
                const auto subPair = to_char_length<Char, Size>(word);

                const auto result = KnuthMorrisPratt<Char, Size>(textPair, subPair);
                return result;
            }

            template<typename string_t = std::string>
            std::vector<int> knuth_morris_pratt_prep_todo(const string_t& pattern)
            {
                RequirePositive(pattern.size(), "pattern size");

                const auto n = static_cast<int>(pattern.size());
                std::vector<int> result(n + 1);
                result[0] = -1;

                auto j = 0;
                for (auto i = 1; i < n;)
                {
                    if (pattern[i] == pattern[j])
                    {
                        result[i] = result[j];
                    }
                    else
                    {
                        result[i] = j;
                        do
                            j = result[j];
                        while (j >= 0 && pattern[i] != pattern[j]);
                    }

                    ++i, ++j;
                }

                assert(j >= 0);
                result[n] = j;
                return result;
            }

            // Hm.
            template<typename string_t>
            void knuth_morris_pratt_todo(const string_t& pattern, const std::vector<int>& table,
                const string_t& text,
                std::vector<int>& result)
            {
                RequirePositive(pattern.size(), "pattern size");
                RequirePositive(text.size(), "text size");

                result.clear();
                assert(pattern.size() + 1 == table.size());

                const auto n = static_cast<int>(text.size());
                const auto m = static_cast<int>(pattern.size());
                auto i = 0, j = 0;
                do
                {
                    if (text[i] == pattern[j])
                    {
                        if (j + 1 == m)
                        {
                            result.push_back(i - j);
                            j = table[m];
                            assert(j >= 0);
                        }
                        else
                            ++j;
                    }
                    else
                    {
                        j = table[j];
                        if (0 <= j)
                            continue;

                        j = 0;
                    }
                    ++i;
                } while (i < n);
            }

            // Shift algs.
            // http://www-igm.univ-mlv.fr/~lecroq/string/node6.html
            template <typename string_t>
            void shift_and_prep(const string_t& pattern, std::vector<uint64_t>& bits)
            {
                const auto m = pattern.size();
                RequirePositive(m, "pattern size");
                if (m > 64u)
                    throw std::runtime_error("Use pattern size <= word size");

                bits.clear();

                constexpr auto bit_count = static_cast<uint64_t>(1u) << (sizeof(decltype(pattern[0])) << 3);
                static_assert(bit_count > 0); // 256
                bits.resize(bit_count);

                for (auto i = 0u; i < m; ++i)
                {
                    const auto& c = pattern[i];
                    bits[c] |= static_cast<uint64_t>(1u) << i;
                }
            }

            template <typename string_t>
            void shift_and(const string_t& pattern, const string_t& text,
                std::vector<uint64_t>& bits,
                std::vector<int>& result)
            {
                shift_and_prep(pattern, bits);

                const auto m = pattern.size();
                const auto n = text.size();
                const auto p = static_cast<uint64_t>(1u) << (m - 1u);
                result.clear();

                for (uint64_t i = 0u, state = 0u; i < n; ++i)
                {
                    const auto& c = text[i];
                    constexpr auto empty_string = static_cast<uint64_t>(1u);
                    state = (state << 1 | empty_string) & bits[c];
                    if (state & p)
                        result.push_back(static_cast<int>(i + 1u - m));
                }
            }

            template <typename string_t>
            uint64_t shift_or_prep(const string_t& pattern, std::vector<uint64_t>& bits)
            {
                const auto m = pattern.size();
                RequirePositive(m, "pattern size");
                if (m > 64u)
                    throw std::runtime_error("Use pattern size <= word size");

                bits.clear();

                constexpr auto bit_count = static_cast<uint64_t>(1u) << (sizeof(decltype(pattern[0])) << 3);
                static_assert(bit_count > 0);
                bits.resize(bit_count, ~static_cast<uint64_t>(0u));

                for (auto i = 0u; i < m; ++i)
                {
                    const auto& c = pattern[i];
                    bits[c] &= ~(static_cast<uint64_t>(1u) << i);
                }

                const auto f = (static_cast<uint64_t>(1u) << (m
                    // avoid the undefined behavior when m=64.
                    - 1)) << 1;
                const auto lim = ~((f - static_cast<uint64_t>(1u)) >> 1);
                return lim;
            }

            template <typename string_t>
            void shift_or(const string_t& pattern, const string_t& text,
                std::vector<uint64_t>& bits,
                std::vector<int>& result)
            {
                const auto lim = shift_or_prep<string_t>(pattern, bits);
                const auto m = pattern.size();
                const auto n = text.size();

                result.clear();

                for (uint64_t i = 0u, state = ~static_cast<uint64_t>(0u); i < n; ++i)
                {
                    const auto& c = text[i];
                    state = state << 1 | bits[c];
                    if (state < lim)
                        result.push_back(static_cast<int>(i + 1u - m));
                }
            }

            // Boyer-Moore
            template <typename string_t>
            void bad_character_rule_rev(const string_t& pattern,
                std::map<char, int>& last_positions_rev)
            {
                last_positions_rev.clear();

                const auto m = static_cast<int>(pattern.size());

                for (auto i = 0; i < m - 1; ++i)
                {
                    const auto& c = pattern[i];
                    last_positions_rev[c] = m - 1 - i;
                }
            }

            template <typename string_t>
            void bad_character_rule(const string_t& pattern,
                std::map<char, int>& last_positions)
            {
                last_positions.clear();

                const auto m = static_cast<int>(pattern.size());
                for (auto i = 0; i < m - 1; ++i)
                {
                    const auto& c = pattern[i];
                    last_positions[c] = i;
                }
            }

            template <typename string_t>
            void boyer_moore_horspool(const string_t& pattern, const string_t& text,
                std::map<char, int>& last_positions_rev,
                std::vector<int>& result)
            {
                const auto m = static_cast<int>(pattern.size());
                RequirePositive(m, "pattern size");

                const auto n = static_cast<int>(text.size());
                RequirePositive(n, "text size");

                bad_character_rule_rev<string_t>(pattern, last_positions_rev);
                result.clear();

                for (auto i = 0; i <= n - m;)
                {
                    const auto& c = text[i + m - 1];
                    if (pattern[m - 1] == c &&
                        !text.compare(i, m - 1, pattern, 0, m - 1))
                        result.push_back(i);

                    const auto f = last_positions_rev.find(c);
                    const auto delta = f == last_positions_rev.end() ? m : f->second;
                    assert(delta > 0);
                    i += delta;
                }
            }

            // http://www-igm.univ-mlv.fr/~lecroq/string/node14.html
            // find the longest x s.t. pattern = axy = bx, where |y| = m - 1 - i.
            // suff[m - 1] = m.
            // E.g. pattern = "ABABABAB", m = 8, suff = {0,2,0,4,0,6,0,8}.
            // E.g. pattern = "BBCBACBBCB", m = 10, suff = {1,1,0,4,0,0,2,1,0,10}.
            template <typename string_t>
            void suffixes(const string_t& pattern, const int m,
                std::vector<int>& suff)
            {
                assert(m > 0);
                suff.resize(m);
                suff[m - 1] = m;

                for (auto i = m - 2, left = m - 2,
                    right = 0; // can be anything - pacify the compiler.
                    i >= 0; --i)
                {
                    if (left < i)
                    {
                        // Already calculated - try to reuse.
                        const auto i2 = m - 1 + i - right;
                        const auto& s2 = suff[i2];
                        const auto delta = i - left;
                        if (s2 < delta)
                        {
                            // Inside the window.
                            suff[i] = s2;
                            continue;
                        }

                        // The suffix might be longer.
                    }

                    left = std::min(left, i);
                    const auto shift = m - 1 - i;

                    while (left >= 0 && pattern[left] == pattern[shift + left])
                        --left;
                    suff[i] = i - left;
                    right = i;
                }
            }

            template <typename string_t>
            void good_rule(const string_t& pattern, const int m,
                std::vector<int>& suff, std::vector<int>& good_chars)
            {
                assert(m > 0);
                suffixes<string_t>(pattern, m, suff);

                good_chars.clear();
                good_chars.resize(m, m);

                // E.g. pattern = "ABABABAB", m = 8, suff = {0,2,0,4,0,6,0,8}.
                for (auto i = m - 1, j = 0; i >= 0; --i)
                {
                    if (suff[i] != i + 1)
                        continue;

                    // Prefix == suffix.
                    const auto delta = m - 1 - i;
                    for (; j < delta; ++j)
                        if (good_chars[j] == m)
                            good_chars[j] = delta;
                }

                // Now, good_chars={2,2,4,4,6,6,8,8}
                for (auto i = 0; i < m - 1; ++i)
                {
                    // aaaXXXXXbbXXXXX  s=5 the suffix length
                    //        i k    (m - 1)
                    //         ___v___

                    const auto& s = suff[i];  // i=6, s2=0
                    const auto k_before_suffix = m - 1 - s; // k=7
                    const auto v_dist_to_right_edge = m - 1 - i;// val=1
                    good_chars[k_before_suffix] = v_dist_to_right_edge;
                }

                // good_chars={2,2,4,4,6,6,8,1}
#ifdef _DEBUG
                for (auto i = 0; i < m; ++i)
                    assert(good_chars[i] > 0);
#endif

                // AABCAACAAA, m=10, good_chars={8,8,8,8,8,8,8,1,2,3}
            }

            template <typename string_t>
            void boyer_moore(const string_t& pattern, const string_t& text,
                std::map<char, int>& last_positions,
                std::vector<int>& suff,
                std::vector<int>& good_chars,
                std::vector<int>& result)
            {
                const auto m = static_cast<int>(pattern.size());
                RequirePositive(m, "pattern size");

                const auto n = static_cast<int>(text.size());
                RequirePositive(n, "text size");

                bad_character_rule<string_t>(pattern, last_positions);
                good_rule<string_t>(pattern, m, suff, good_chars);

                result.clear();
                for (auto i = 0; i <= n - m;)
                {
                    auto j = m - 1;
                    while (j >= 0 && pattern[j] == text[i + j])
                        --j;

                    if (j < 0)
                    {
                        result.push_back(i);
                        // Add the period.
                        i += good_chars[0];
                        continue;
                    }

                    const auto& c_failed = text[i + j];
                    const auto f = last_positions.find(c_failed);
                    const auto x = (f == last_positions.end() ? 1 : -f->second) + j;
                    // x could be < 0.

                    const auto y = good_chars[j];
                    const auto ax = std::max(x, y);
                    assert(ax > 0);
                    i += ax;
                }
            }

            // BNDM http://www-igm.univ-mlv.fr/~lecroq/string/bndm.html
            // where DAWG is directed acyclic word graph.
            template <typename string_t>
            void backward_nondeterministic_dawg_matching(const string_t& pattern, const string_t& text,
                std::vector<uint64_t>& bits,
                std::vector<int>& result)
            {
                const auto m = static_cast<int>(pattern.size());
                RequirePositive(m, "pattern size");
                if (m > 64)
                    throw std::runtime_error("Use pattern size <= word size");

                const auto n = static_cast<int>(text.size());
                RequirePositive(n, "text size");

                constexpr auto bit_count = static_cast<uint64_t>(1u) << (sizeof(decltype(pattern[0])) << 3);
                static_assert(bit_count > 0); // 256
                bits.clear();
                bits.resize(bit_count);

                result.clear();

                uint64_t s = 1u;
                for (auto i = m - 1; i >= 0; --i)
                {
                    const auto& c = pattern[i];
                    bits[c] |= s;
                    s <<= 1;
                }

                for (auto i = 0; i <= n - m; )
                {
                    auto j = m - 1;
                    auto last = m;
                    auto d = ~static_cast<uint64_t>(0);
                    do
                    {
                        const auto& c = text[i + j];
                        d &= bits[c];
                        if (d)
                        {
                            d <<= 1;
                            if (j == 0)
                                result.push_back(i);
                            else
                                last = j;
                        }
                    } while (--j >= 0 && d);

                    i += last;
                }
            }

            // todo: p1. review all code below.

            // BOM http://www-igm.univ-mlv.fr/~lecroq/string/bom.html
            template <typename string_t>
            int get_transition(const char* x,
                const std::vector<int>& tempz,
                const std::vector<std::pair<int, int>>& bom_cells,
                int p, const char c)
            {
                if (p > 0 && x[p - 1] == c)
                    return p - 1;

                //List cell = L[p];
                //while (cell != nullptr)
                //  if (x[cell->element] == c)
                //    return cell->element;
                //  else
                //    cell = cell->next;
                p = tempz[p];
                while (p >= 0)
                {
                    const auto& cell = bom_cells[p];
                    if (x[cell.first] == c)
                        return cell.first;

                    assert(p != cell.second);
                    p = cell.second;
                }

                return zero_pointer;
            }

            template <typename string_t>
            void set_transition(std::vector<int>& tempz,
                std::vector<std::pair<int, int>>& bom_cells,
                int p, int q)
            {
                assert(p >= 0 && q >= 0);
                assert(bom_cells.size() < tempz.size());

                //auto cell = new _cell();
                //cell->element = q;
                //cell->next = L[p];
                //L[p] = cell;
                bom_cells.push_back(std::make_pair(q, tempz[p]));
                tempz[p] = static_cast<int>(bom_cells.size() - 1);
            }

            template <typename string_t>
            void oracle(const char* x, const int m,
                std::vector<int>& S,
                std::vector<int>& tempz,
                std::vector<bool>& bools,
                std::vector<std::pair<int, int>>& bom_cells)
            {
                S[m] = m + 1;
                auto q = 0;

                for (auto i = m; i > 0; --i)
                {
                    auto c = x[i - 1];
                    auto p = S[i];

                    while (p <= m &&
                        (q = get_transition<string_t>(x, tempz, bom_cells, p, c)) < 0)
                    {
                        set_transition<string_t>(tempz, bom_cells, p, i - 1);
                        p = S[p];
                    }

                    S[i - 1] = p == m + 1 ? m : q;
                }

                auto p = 0;
                while (p <= m)
                {
                    bools[p] = true;
                    p = S[p];
                }
            }

            template <typename string_t>
            void backward_oracle_matching(const string_t& pattern, const string_t& text,
                std::vector<int>& S,
                std::vector<int>& tempz,
                std::vector<bool>& bools,
                std::vector<std::pair<int, int>>& bom_cells,
                std::vector<int>& result)
            {
                const auto m = static_cast<int>(pattern.size());
                RequirePositive(m, "pattern size");

                const auto n = static_cast<int>(text.size());
                RequirePositive(n, "text size");

                S.clear();
                S.resize(m + 1);

                tempz.clear();
                tempz.resize(m + 1, zero_pointer);

                bools.clear();
                bools.resize(m + 1);

                bom_cells.clear();
                bom_cells.reserve(m + 1);

                result.clear();

                const auto x = pattern.data();
                const auto y = text.data();

                auto period = 0;
                oracle<string_t>(x, m, S, tempz, bools, bom_cells);
                //
                auto j = 0;
                while (j <= n - m)
                {
                    auto i = m - 1, p = m,
                        shift = m;
                    int q;

                    while (i + j >= 0 &&
                        (q = get_transition<string_t>(x, tempz, bom_cells, p, y[i + j])) >= 0)
                    {
                        p = q;
                        if (bools[p])
                        {
                            period = shift;
                            shift = i;
                        }

                        --i;
                    }

                    if (i < 0)
                    {
                        result.push_back(j);
                        shift = period;
                    }

                    j += shift;
                }
            }

            template<class char_t, class string_t>
            std::vector<std::unordered_map<char_t, int>> automaton_pattern_matching_build_slow(const string_t& pattern)
            {
                const auto size = static_cast<int>(pattern.size());
                RequirePositive(size, "pattern size");

                std::vector<std::unordered_map<char_t, int>> result(size + 1);
                std::unordered_set<char_t> prev_chars;
                auto i = 0;
                do
                {
                    auto& states = result[i];

                    for (const auto& c2 : prev_chars)
                    {
                        // Skip the first character for the last state.
                        int start = i == size;
                        do
                        {
                            // The best next state is for
                            // the longest suffix of "pattern[0..i - 1]c2"
                            // which is also a prefix of the pattern.
                            const auto len1 = i - start;
                            assert(len1 >= 0 && len1 < size);
                            if (pattern[len1] != c2)
                                continue;

                            auto pos = 0;
                            while (pos < len1 && pattern[pos] == pattern[pos + start])
                                ++pos;

                            if (pos == len1)
                            {
                                states[c2] = len1 + 1;
                                break;
                            }
                        } while (++start <= i);
                    }

                    if (i < size)
                    {
                        const auto& c = pattern[i];
                        states[c] = i + 1;
                        prev_chars.insert(c);
                    }
                } while (++i <= size);

                return result;
            }

            template<class map_t, class char_t>
            int automaton_next_state(const map_t& automaton, const char_t& c)
            {
                const auto it = automaton.find(c);
                const auto r = it == automaton.end() ? 0 : it->second;
                assert(r >= 0);
                return r;
            }

            template<class char_t, class string_t>
            std::vector<std::unordered_map<char_t, int>> automaton_pattern_matching_build(const string_t& pattern)
            {
                const auto size = static_cast<int>(pattern.size());
                RequirePositive(size, "pattern size");

                std::vector<std::unordered_map<char_t, int>> result(size + 1);

                for (auto i = 0, max_fix_state = 0; i <= size; ++i)
                {
                    auto& states = result[i];
                    states = result[max_fix_state];

                    if (i < size)
                    {
                        const auto& c = pattern[i];
                        assert(max_fix_state < size);

                        max_fix_state = automaton_next_state(result[max_fix_state], c);
                        assert(max_fix_state >= 0 && max_fix_state < size);

                        states[c] = i + 1;
                    }
                }

                return result;
            }

            template<class map_t, class string_t>
            void automaton_pattern_matching(const map_t& automaton,
                const string_t& text,
                std::vector<int>& result)
            {
                RequireGreater(automaton.size(), 1, "automaton size");
                result.clear();

                const auto n = static_cast<int>(text.size());
                const auto m = static_cast<int>(automaton.size() - 1);
                assert(m > 0);
                if (n < m)
                    return;

                for (auto i = 0, state = 0; i < n; ++i)
                {
                    const auto& c = text[i];
                    const auto& cur = automaton[state];
                    state = automaton_next_state(cur, c);
                    assert(state >= 0 && state <= m);

                    if (m == state)
                    {
                        result.push_back(i + 1 - m);
                        assert(result.back() >= 0 && result.back() < n);
                    }
                }
            }
        }
    }
}