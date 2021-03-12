#pragma once
// "kmp_todo.h"
#include"../Numbers/arithmetic.h"
#include<map>
#include<span>
#include<unordered_map>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Strings::Inner
{
    constexpr auto zero_pointer = -1;

    // "aaaaaH" -> {{'a', 5}, {'H', 1}}
    template<class char_t, class pos_t>
    [[nodiscard]] constexpr auto to_char_length(const std::string &text) -> std::vector<std::pair<char_t, pos_t>>
    {
        char_t previous = text.at(0);

        std::vector<std::pair<char_t, pos_t>> result(1, std::make_pair(previous, 1));

        const auto size = text.size();

        for (pos_t index = 1; index < size; ++index)
        {
            if (text[index] == previous)
            {
                ++(result[result.size() - 1].second);
            }
            else
            {
                previous = text[index];
                result.emplace_back(previous, 1);
            }
        }

        return result;
    }

    template<class pos_t>
    constexpr auto calc_limits(const pos_t size) -> std::pair<pos_t, pos_t>
    {
        constexpr auto large = 3; // There is something between the first and last chars.
        const auto is_small = size < large;
        auto result = std::make_pair(is_small ? large : 1, is_small ? large : size - 2);

        return result;
    }

    template<class char_t, class pos_t>
    constexpr auto calc_table(const std::vector<std::pair<char_t, pos_t>> &word) -> std::vector<pos_t>
    {
        constexpr auto minus_one = static_cast<pos_t>(0 - static_cast<pos_t>(1));

        const auto size = static_cast<pos_t>(word.size());
        auto matches = minus_one;

        std::vector<pos_t> result(size + 1LL);
        result.at(0) = minus_one;

        const auto min_max_inclusive = calc_limits<pos_t>(size);
        pos_t index{};

        do
        {
            while (minus_one != matches &&
                (word[matches].first != word[index].first || word[index].second < word[matches].second ||
                    (Standard::Algorithms::Numbers::is_within_inclusive<pos_t>(matches, min_max_inclusive) &&
                        word[matches].second < word[index].second)))
            {
                matches = result[matches];
            }

            result[++index] = ++matches;
        } while (index < size);

        return result;
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    constexpr auto bit_match_pattern_max_size = 64U;

    // String search by Knuth, Morris, Pratt.
    template<class char_t, class pos_t = std::size_t>
    constexpr auto knuth_morris_pratt_todo2(
        // todo(p3): check both text and word are normalized
        // e.g. any 2 consecutive chars are never the same.
        const std::vector<std::pair<char_t, pos_t>> &text, const std::vector<std::pair<char_t, pos_t>> &word) -> pos_t
    {
        const auto text_size = text.size();
        require_positive(text_size, "text size");

        const auto word_size = static_cast<pos_t>(word.size());
        require_positive(word_size, "word size");

        constexpr auto minus_one = static_cast<pos_t>(0 - static_cast<pos_t>(1));
        if (text_size < word_size)
        {
            return minus_one;
        }

        const auto table = Inner::calc_table<char_t, pos_t>(word);
        const auto min_max_inclusive = Inner::calc_limits<pos_t>(word_size);

        pos_t index{};
        pos_t matches{};
        pos_t ind_two{};
        do
        {
            while (minus_one != matches &&
                (text[index].first != word[matches].first || text[index].second < word[matches].second ||
                    (Standard::Algorithms::Numbers::is_within_inclusive<pos_t>(matches, min_max_inclusive) &&
                        word[matches].second < text[index].second)))
            {
                matches = table[matches];
            }

            if (word_size == ++matches)
            {
                pos_t result = ind_two // There might be extra symbols at the start.
                    + std::min(text[index].second, word[word_size - 1].second) - word[0].second;

                pos_t ind_3{};
                while (++ind_3 < word_size)
                {
                    result -= word[ind_3].second;
                }

                return result;
            }

            ind_two += text[index].second;
        } while (++index < text_size);

        return minus_one;
    }

    template<class string_t, class pat_string_t, class pos_t = std::size_t,
        class char_t = typename std::string::value_type>
    constexpr auto knuth_morris_pratt_str_todo(const string_t &text, const pat_string_t &word) -> pos_t
    {
        require_positive(text.size(), "text size");
        require_positive(word.size(), "word size");

        const auto text_pair = Inner::to_char_length<char_t, pos_t>(text);
        const auto sub_pair = Inner::to_char_length<char_t, pos_t>(word);

        auto result = knuth_morris_pratt_todo2<char_t, pos_t>(text_pair, sub_pair);
        return result;
    }

    template<class string_t>
    constexpr auto knuth_morris_pratt_prep_todo(const string_t &pattern) -> std::vector<std::int32_t>
    {
        require_positive(pattern.size(), "pattern size");

        const auto size = static_cast<std::int32_t>(pattern.size());
        std::vector<std::int32_t> result(size + 1LL);
        result.at(0) = -1;

        std::int32_t ind_3{};

        for (auto index = 1; index < size;)
        {
            if (pattern[index] == pattern[ind_3])
            {
                result[index] = result[ind_3];
            }
            else
            {
                result[index] = ind_3;
                do
                {
                    ind_3 = result[ind_3];
                } while (!(ind_3 < 0) && pattern[index] != pattern[ind_3]);
            }

            ++index, ++ind_3;
        }

        assert(!(ind_3 < 0));

        result[size] = ind_3;
        return result;
    }

    template<class string_t>
    constexpr void knuth_morris_pratt_todo(const string_t &pattern, const std::vector<std::int32_t> &table,
        const string_t &text, std::vector<std::int32_t> &result)
    {
        require_positive(pattern.size(), "pattern size");
        require_positive(text.size(), "text size");

        result.clear();
        assert(pattern.size() + 1U == table.size());

        const auto text_size = static_cast<std::int32_t>(text.size());
        const auto pat_size = static_cast<std::int32_t>(pattern.size());
        std::int32_t index{};
        std::int32_t ind_3{};

        do
        {
            if (text[index] == pattern[ind_3])
            {
                if (ind_3 + 1 == pat_size)
                {
                    result.push_back(index - ind_3);
                    ind_3 = table[pat_size];

                    assert(!(ind_3 < 0));
                }
                else
                {
                    ++ind_3;
                }
            }
            else
            {
                ind_3 = table[ind_3];
                if (0 <= ind_3)
                {
                    continue;
                }

                ind_3 = 0;
            }

            ++index;
        } while (index < text_size);
    }

    // Shift algs.
    // http://www-igm.univ-mlv.fr/~lecroq/string/node6.html
    template<class string_t>
    constexpr void shift_and_prep(const string_t &pattern, std::vector<std::uint64_t> &bits)
    {
        const auto pat_size = pattern.size();
        require_positive(pat_size, "pattern size");

        if (bit_match_pattern_max_size < pat_size) [[unlikely]]
        {
            throw std::runtime_error("Use pattern size <= word size");
        }

        constexpr auto bit_count = static_cast<std::uint64_t>(1) << (sizeof(decltype(pattern[0])) << 3U);

        static_assert(0U < bit_count); // 256

        bits.assign(bit_count, 0);

        for (std::size_t index{}; index < pat_size; ++index)
        {
            const auto &cha = pattern[index];
            bits.at(cha) |= static_cast<std::uint64_t>(1) << index;
        }
    }

    template<class string_t>
    constexpr void shift_and(const string_t &pattern, std::vector<std::uint64_t> &bits, const string_t &text,
        std::vector<std::int32_t> &result)
    {
        shift_and_prep(pattern, bits);

        const auto pat_size = pattern.size();
        const auto text_size = text.size();
        const auto mask = static_cast<std::uint64_t>(1) << (pat_size - 1U);
        result.clear();

        for (std::uint64_t index{}, position{}; index < text_size; ++index)
        {
            constexpr auto empty_string = static_cast<std::uint64_t>(1);

            const auto &cha = text[index];
            position = ((position << 1U) | empty_string) & bits.at(cha);

            if (position & mask)
            {
                result.push_back(static_cast<std::int32_t>(index + 1U - pat_size));
            }
        }
    }

    template<class string_t>
    constexpr auto shift_or_prep(const string_t &pattern, std::vector<std::uint64_t> &bits) -> std::uint64_t
    {
        const auto pat_size = pattern.size();
        require_positive(pat_size, "pattern size");

        if (bit_match_pattern_max_size < pat_size) [[unlikely]]
        {
            throw std::runtime_error("Use pattern size <= word size");
        }

        constexpr auto bit_count = static_cast<std::uint64_t>(1) << (sizeof(decltype(pattern[0])) << 3U);

        static_assert(0U < bit_count);

        bits.assign(bit_count, ~static_cast<std::uint64_t>(0));

        for (std::size_t index{}; index < pat_size; ++index)
        {
            const auto &cha = pattern[index];
            bits.at(cha) &= ~(static_cast<std::uint64_t>(1) << index);
        }

        const auto temp = (static_cast<std::uint64_t>(1) << (pat_size
                               // avoid the undefined behavior when pat_size=64.
                               - 1U))
            << 1U;
        auto lim = ~((temp - static_cast<std::uint64_t>(1)) >> 1U);
        return lim;
    }

    template<class string_t>
    constexpr void shift_or(const string_t &pattern, std::vector<std::uint64_t> &bits, const string_t &text,
        std::vector<std::int32_t> &result)
    {
        const auto lim = shift_or_prep<string_t>(pattern, bits);
        const auto pat_size = pattern.size();
        const auto text_size = text.size();

        result.clear();

        for (std::uint64_t index{}, position = ~static_cast<std::uint64_t>(0); index < text_size; ++index)
        {
            const auto &cha = text[index];
            position = (position << 1U) | bits.at(cha);

            if (position < lim)
            {
                result.push_back(static_cast<std::int32_t>(index + 1U - pat_size));
            }
        }
    }

    // Boyer-Moore explore the pattern backward; used in string matching.
    template<class string_t>
    constexpr void bad_character_rule(const string_t &pattern, std::map<char, std::int32_t> &last_positions)
    {
        last_positions.clear();

        const auto pat_size = static_cast<std::int32_t>(pattern.size());

        for (std::int32_t index{}; index < pat_size - 1; ++index)
        {
            const auto &cha = pattern[index];
            last_positions[cha] = index;
        }
    }

    template<class string_t>
    constexpr void bad_character_rule_rev(const string_t &pattern, std::map<char, std::int32_t> &last_positions_rev)
    {
        last_positions_rev.clear();

        const auto pat_size = static_cast<std::int32_t>(pattern.size());

        for (std::int32_t index{}; index < pat_size - 1; ++index)
        {
            const auto &cha = pattern[index];
            last_positions_rev[cha] = pat_size - 1 - index;
        }
    }

    // It can be slow time O(text_size * pat_size), but it is usually good on average.
    template<class string_t>
    constexpr void boyer_moore_horspool(const string_t &pattern, const string_t &text,
        std::map<char, std::int32_t> &last_positions_rev, std::vector<std::int32_t> &result)
    {
        const auto pat_size = static_cast<std::int32_t>(pattern.size());
        require_positive(pat_size, "pattern size");

        const auto text_size = static_cast<std::int32_t>(text.size());
        require_positive(text_size, "text size");

        bad_character_rule_rev<string_t>(pattern, last_positions_rev);
        result.clear();

        for (std::int32_t index{}; index <= text_size - pat_size;)
        {
            const auto &cha = text[index + pat_size - 1];

            if (pattern[pat_size - 1] == cha &&
                // It can be slow time O(pat_size).
                0 == text.compare(index, pat_size - 1, pattern, 0, pat_size - 1))
            {
                result.push_back(index);
            }

            const auto iter = last_positions_rev.find(cha);
            const auto delta = iter == last_positions_rev.end() ? pat_size : iter->second;
            assert(0 < delta);

            index += delta;
        }
    }

    // http://www-igm.univ-mlv.fr/~lecroq/string/node14.html
    // find the longest x s.t. pattern = axy = bx, where |y| = pat_size - 1 - index.
    // suff[pat_size - 1] = pat_size.
    // E.g. pattern = "ABABABAB", pat_size = 8, suff = {0,2,0,4,0,6,0,8}.
    // E.g. pattern = "BBCBACBBCB", pat_size = 10, suff = {1,1,0,4,0,0,2,1,0,10}.
    template<class string_t>
    constexpr void suffixes(const string_t &pattern, std::vector<std::int32_t> &suff)
    {
        const auto pat_size = static_cast<std::int32_t>(pattern.size());
        assert(0 < pat_size);

        suff.resize(pat_size);
        suff.back() = pat_size;

        for (std::int32_t index = pat_size - 2, left = pat_size - 2, right{}; // can be anything - pacify the compiler.
             0 <= index; --index)
        {
            if (left < index)
            {// Already calculated - try to reuse.
                const auto indd = pat_size - 1 + index - right;
                const auto &su2 = suff.at(indd);
                const auto delta = index - left;

                if (const auto is_inside_window = su2 < delta; is_inside_window)
                {
                    suff[index] = su2;
                    continue;
                }

                // The suffix might be longer.
            }

            const auto shift = pat_size - 1 - index;
            left = std::min(left, index);

            while (!(left < 0) && pattern[left] == pattern[shift + left])
            {
                --left;
            }

            suff[index] = index - left;
            right = index;
        }
    }

    template<class string_t>
    constexpr void good_rule(
        std::vector<std::int32_t> &suff, const string_t &pattern, std::vector<std::int32_t> &good_chars)
    {
        const auto pat_size = static_cast<std::int32_t>(pattern.size());
        assert(0 < pat_size);

        suffixes<string_t>(pattern, suff);
        good_chars.assign(pat_size, pat_size);

        // E.g. pattern = "ABABABAB", pat_size = 8, suff = {0,2,0,4,0,6,0,8}.
        for (std::int32_t index = pat_size - 1, ind_3{}; 0 <= index; --index)
        {
            if (suff[index] != index + 1)
            {
                continue;
            }

            // Prefix == suffix.
            for (const auto delta = pat_size - 1 - index; ind_3 < delta; ++ind_3)
            {
                if (good_chars[ind_3] == pat_size)
                {
                    good_chars[ind_3] = delta;
                }
            }
        }

        // Now, good_chars={2,2,4,4,6,6,8,8}
        for (std::int32_t index{}; index < pat_size - 1; ++index)
        {
            // aaaXXXXXbbXXXXX  s=5 the suffix length
            //        index k    (pat_size - 1)
            //         ___v___

            const auto &sui = suff[index]; // index=6, su2=0
            const auto k_before_suffix = pat_size - 1 - sui; // k=7
            const auto v_dist_to_right_edge = pat_size - 1 - index; // val=1
            good_chars[k_before_suffix] = v_dist_to_right_edge;
        }

        // good_chars={2,2,4,4,6,6,8,1}
        if constexpr (::Standard::Algorithms::is_debug)
        {
            for (std::int32_t index{}; index < pat_size; ++index)
            {
                assert(0 < good_chars[index]);
            }
        }
        // AABCAACAAA, pat_size=10, good_chars={8,8,8,8,8,8,8,1,2,3}
    }

    template<class string_t>
    constexpr void boyer_moore(const string_t &pattern, std::vector<std::int32_t> &suff,
        std::map<char, std::int32_t> &last_positions, std::vector<std::int32_t> &good_chars, const string_t &text,
        std::vector<std::int32_t> &result)
    {
        const auto pat_size = static_cast<std::int32_t>(pattern.size());
        require_positive(pat_size, "pattern size");

        const auto text_size = static_cast<std::int32_t>(text.size());
        require_positive(text_size, "text size");

        bad_character_rule<string_t>(pattern, last_positions);
        good_rule<string_t>(suff, pattern, good_chars);

        result.clear();
        for (std::int32_t index{}; index <= text_size - pat_size;)
        {
            auto ind_3 = pat_size - 1;

            while (!(ind_3 < 0) && pattern[ind_3] == text[index + ind_3])
            {
                --ind_3;
            }

            if (ind_3 < 0)
            {
                result.push_back(index);
                index += good_chars[0]; // Add the period.
                continue;
            }

            const auto &c_failed = text[index + ind_3];
            const auto fin = last_positions.find(c_failed);
            const auto xxx = (fin == last_positions.end() ? 1 : -fin->second) + ind_3;
            // xxx could be < 0.

            const auto &yyy = good_chars.at(ind_3);
            const auto largest = std::max(xxx, yyy);

            assert(0 < largest);

            index += largest;
        }
    }

    // BNDM http://www-igm.univ-mlv.fr/~lecroq/string/bndm.html
    // where DAWG is directed acyclic word graph.
    template<class string_t>
    constexpr void backward_nondeterministic_dawg_matching(const string_t &pattern, std::vector<std::uint64_t> &bits,
        const string_t &text, std::vector<std::int32_t> &result)
    {
        const auto pat_size = static_cast<std::int32_t>(pattern.size());
        require_positive(pat_size, "pattern size");

        if (bit_match_pattern_max_size < pattern.size()) [[unlikely]]
        {
            throw std::runtime_error("Use pattern size <= word size");
        }

        const auto text_size = static_cast<std::int32_t>(text.size());
        require_positive(text_size, "text size");

        constexpr auto bit_count = static_cast<std::uint64_t>(1) << (sizeof(decltype(pattern[0])) << 3U);

        static_assert(0U < bit_count); // 256

        bits.assign(bit_count, 0);
        result.clear();

        {
            std::uint64_t sub = 1;

            for (auto index = pat_size - 1; 0 <= index; --index)
            {
                const auto &cha = pattern[index];
                bits.at(cha) |= sub;
                sub <<= 1U;
            }
        }

        for (std::int32_t index{}; index <= text_size - pat_size;)
        {
            auto ind_3 = pat_size - 1;
            auto last = pat_size;
            auto mask = ~static_cast<std::uint64_t>(0);

            do
            {
                const auto &cha = text[index + ind_3];
                mask &= bits.at(cha);

                if (0U < mask)
                {
                    mask <<= 1U;

                    if (ind_3 == 0)
                    {
                        result.push_back(index);
                    }
                    else
                    {
                        last = ind_3;
                    }
                }
            } while (0 <= --ind_3 && mask);

            index += last;
        }
    }

    // BOM http://www-igm.univ-mlv.fr/~lecroq/string/bom.html
    template<class string_t, class span_t>
    constexpr auto transition(const span_t &sptd, const std::vector<std::int32_t> &tempz,
        const std::vector<std::pair<std::int32_t, std::int32_t>> &bom_cells, std::int32_t pos, const char cha)
        -> std::int32_t
    {
        if (0 < pos && sptd[pos - 1] == cha)
        {
            return pos - 1;
        }

        pos = tempz[pos];

        while (!(pos < 0))
        {
            const auto &cell = bom_cells[pos];
            if (sptd[cell.first] == cha)
            {
                return cell.first;
            }

            assert(pos != cell.second);
            pos = cell.second;
        }

        return Inner::zero_pointer;
    }

    template<class string_t>
    constexpr void set_transition(std::vector<std::int32_t> &tempz,
        std::vector<std::pair<std::int32_t, std::int32_t>> &bom_cells, std::int32_t index_4, std::int32_t index_5)
    {
        assert(!(index_4 < 0) && !(index_5 < 0));
        assert(bom_cells.size() < tempz.size());

        bom_cells.emplace_back(index_5, tempz[index_4]);
        tempz[index_4] = static_cast<std::int32_t>(bom_cells.size() - 1U);
    }

    template<class string_t, class span_t>
    constexpr void oracle(const span_t &sptd, const std::int32_t pat_size, std::vector<std::int32_t> &dexes,
        std::vector<bool> &bools, std::vector<std::int32_t> &tempz,
        std::vector<std::pair<std::int32_t, std::int32_t>> &bom_cells)
    {
        dexes.at(pat_size) = pat_size + 1;

        std::int32_t index_5{};

        for (auto index = pat_size; 0 < index; --index)
        {
            auto cha = sptd[index - 1];
            auto index_4 = dexes[index];

            while (
                index_4 <= pat_size && (index_5 = transition<string_t>(sptd, tempz, bom_cells, index_4, cha)) < 0)
            {
                set_transition<string_t>(tempz, bom_cells, index_4, index - 1);
                index_4 = dexes[index_4];
            }

            dexes[index - 1] = index_4 == pat_size + 1 ? pat_size : index_5;
        }

        std::int32_t index_4{};

        while (index_4 <= pat_size)
        {
            bools[index_4] = true;
            index_4 = dexes[index_4];
        }
    }

    template<class string_t>
    constexpr void backward_oracle_matching(const string_t &pattern, std::vector<std::int32_t> &dexes,
        const string_t &text, std::vector<bool> &bools, std::vector<std::int32_t> &tempz,
        std::vector<std::pair<std::int32_t, std::int32_t>> &bom_cells, std::vector<std::int32_t> &result)
    {
        const auto pat_size = static_cast<std::int32_t>(pattern.size());
        require_positive(pat_size, "pattern size");

        const auto text_size = static_cast<std::int32_t>(text.size());
        require_positive(text_size, "text size");

        dexes.assign(pat_size + 1LL, 0);
        tempz.assign(pat_size + 1LL, Inner::zero_pointer);
        bools.assign(pat_size + 1LL, false);

        bom_cells.clear();
        bom_cells.reserve(pat_size + 1);

        result.clear();

        const std::span sptd{ pattern.data(), pattern.data() + pat_size };
        std::int32_t period{};
        oracle<string_t>(sptd, pat_size, dexes, bools, tempz, bom_cells);
        //
        std::int32_t ind_3{};

        while (ind_3 <= text_size - pat_size)
        {
            auto index = pat_size - 1;
            auto index_4 = pat_size;
            auto shift = pat_size;
            std::int32_t index_5{};

            while (!(index + ind_3 < 0) &&
                0 <= (index_5 = transition<string_t>(sptd, tempz, bom_cells, index_4, text[index + ind_3])))
            {
                index_4 = index_5;
                if (bools[index_4])
                {
                    period = shift;
                    shift = index;
                }

                --index;
            }

            if (index < 0)
            {
                result.push_back(ind_3);
                shift = period;
            }

            ind_3 += shift;
        }
    }

    namespace Inner
    {
        template<class char_t, class string_t>
        constexpr void build_slow_prevs(const string_t &pattern, const std::unordered_set<char_t> &prev_chars,
            const std::int32_t index, std::unordered_map<char_t, std::int32_t> &positions)
        {
            const auto size = static_cast<std::int32_t>(pattern.size());

            for (const auto &prc : prev_chars)
            {
                // Skip the first character for the last position.
                std::int32_t start = index == size ? 1 : 0;

                do
                {
                    // The best next position is for
                    // the longest suffix of "pattern[0..index - 1]prc"
                    // which is also a prefix of the pattern.
                    const auto len1 = index - start;
                    assert(!(len1 < 0) && len1 < size);

                    if (pattern[len1] != prc)
                    {
                        continue;
                    }

                    std::int32_t matches{};

                    while (matches < len1 && pattern[matches] == pattern[matches + start])
                    {
                        ++matches;
                    }

                    if (matches == len1)
                    {
                        positions[prc] = len1 + 1;
                        break;
                    }
                } while (++start <= index);
            }
        }
    } // namespace Inner

    template<class char_t, class string_t>
    constexpr auto automaton_pattern_matching_build_slow(const string_t &pattern)
        -> std::vector<std::unordered_map<char_t, std::int32_t>>
    {
        const auto size = require_positive(static_cast<std::int32_t>(pattern.size()), "pattern size");

        std::vector<std::unordered_map<char_t, std::int32_t>> result(size + 1);
        std::unordered_set<char_t> prev_chars;
        std::int32_t index{};

        do
        {
            auto &positions = result[index];
            Inner::build_slow_prevs<char_t, string_t>(pattern, prev_chars, index, positions);

            if (index < size)
            {
                const auto &cha = pattern[index];
                positions[cha] = index + 1;
                prev_chars.insert(cha);
            }
        } while (++index <= size);

        return result;
    }

    template<class map_t, class char_t>
    constexpr auto automaton_next_position(const map_t &automaton, const char_t &cha) -> std::int32_t
    {
        const auto fin = automaton.find(cha);
        const auto next = fin == automaton.end() ? 0 : fin->second;
        assert(!(next < 0));

        return next;
    }

    template<class char_t, class string_t>
    constexpr auto automaton_pattern_matching_build(const string_t &pattern)
        -> std::vector<std::unordered_map<char_t, std::int32_t>>
    {
        const auto size = static_cast<std::int32_t>(pattern.size());
        require_positive(size, "pattern size");

        std::vector<std::unordered_map<char_t, std::int32_t>> result(size + 1);

        for (std::int32_t index{}, max_fix_position{}; index <= size; ++index)
        {
            auto &positions = result[index];
            positions = result[max_fix_position];

            if (index < size)
            {
                const auto &cha = pattern[index];
                assert(max_fix_position < size);

                max_fix_position = automaton_next_position(result[max_fix_position], cha);
                assert(!(max_fix_position < 0) && max_fix_position < size);

                positions[cha] = index + 1;
            }
        }

        return result;
    }

    template<class map_t, class string_t>
    constexpr void automaton_pattern_matching(
        const map_t &automaton, const string_t &text, std::vector<std::int32_t> &result)
    {
        require_greater(automaton.size(), 1U, "automaton size");
        result.clear();

        const auto text_size = static_cast<std::int32_t>(text.size());
        const auto pat_size = static_cast<std::int32_t>(automaton.size() - 1U);
        assert(0 < pat_size);

        if (text_size < pat_size)
        {
            return;
        }

        for (std::int32_t index{}, position{}; index < text_size; ++index)
        {
            const auto &cha = text[index];
            const auto &cur = automaton[position];
            position = automaton_next_position(cur, cha);
            assert(!(position < 0) && position <= pat_size);

            if (pat_size == position)
            {
                result.push_back(index + 1 - pat_size);

                assert(!(result.back() < 0) && result.back() < text_size);
            }
        }
    }
} // namespace Standard::Algorithms::Strings
