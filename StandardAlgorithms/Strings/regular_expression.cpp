#include"regular_expression.h"
#include"../Numbers/arithmetic.h"
#include"../Utilities/project_constants.h"
#include<array>
#include<numeric>
#include<tuple>
#include<unordered_set>
#include<vector>

namespace
{
    inline constexpr auto question = Standard::Algorithms::Strings::question;

    inline constexpr auto star = Standard::Algorithms::Strings::star;

    // Time O(n).
    [[nodiscard]] constexpr auto are_stars_only(const std::string_view str) noexcept -> bool
    {
        const auto pos = str.find_first_not_of(star);
        auto stars_only = pos == std::string::npos;
        return stars_only;
    }

    // Time O(1).
    [[nodiscard]] inline constexpr auto qu_match(const char pat, const char tex) noexcept -> bool
    {
        auto qmat = pat == tex || pat == question;
        return qmat;
    }

    // There could be '?'s in the pattern. Time O(n).
    [[nodiscard]] constexpr auto without_star_match(
        const Standard::Algorithms::Strings::pattern_text_view<const std::string_view, const std::string_view>
            &pat_text) noexcept -> bool
    {
        const auto &[pattern, text] = pat_text;
        const auto size = pattern.size();

        if (size != text.size())
        {
            return false;
        }

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &pat = pattern[index];
            const auto &tex = text[index];

            if (!qu_match(pat, tex))
            {
                return false;
            }
        }

        return true;
    }

    // Total time for a pattern O(n).
    [[nodiscard]] constexpr auto quest_count_has_star(const std::string_view pattern, std::size_t &index) noexcept
        -> std::pair<std::size_t, bool>
    {
        const auto size = pattern.size();
        assert(index < size && qu_match(pattern[index], star));

        std::size_t quest_count{};
        auto has_star = false;

        for (;;)
        {
            if (const auto &cha = pattern[index]; question == cha)
            {
                ++quest_count;
            }
            else if (star == cha)
            {
                has_star = true;
            }

            if (!(++index < pattern.size()))
            {
                break;
            }

            if (const auto &cha = pattern[index]; !qu_match(cha, star))
            {
                break;
            }
        }

        assert(0U < quest_count || has_star);

        return { quest_count, has_star };
    }

    // Given "A?*?***?B??", create 2 sections {"A???", "B??"}.
    // Time O(n).
    //
    // todo(p4): If 2 or more reduced stars, move the questions if any backward in the last section.
    // E.g. "A?*?***?B?**?C" makes sections {"A???", "B??", "C"}, but should be {"A???", "B", "C??"}.
    [[nodiscard]] constexpr auto min_length_begstar_endstar(
        const std::string_view pattern, std::vector<std::string> &sections) -> std::tuple<std::size_t, bool, bool>
    {
        sections.clear();

        if (pattern.empty())
        {
            return {};
        }

        std::size_t min_size{};
        // todo(p4): std::size_t star_sections{}; ?
        bool begings{};
        bool ends{};

        sections.emplace_back();

        for (std::size_t index{}; index < pattern.size();)
        {
            if (const auto &cha = pattern[index]; !qu_match(cha, star))
            {
                sections.back() += cha, ++index, ++min_size;
                continue;
            }

            // "?*?***?" -> "???*"
            auto [quest_count, has_star] = quest_count_has_star(pattern, index);
            min_size += quest_count;

            if (1ZU == sections.size() && sections.back().empty())
            {
                begings = 0ZU == quest_count && has_star;
            }

            while (0U < quest_count--)
            {
                sections.back() += question;
            }

            // todo(p4): star_sections += has_star ? 1 : 0; ?

            if (has_star && !sections.back().empty())
            {
                sections.emplace_back();
            }

            if (index == pattern.size())
            {
                ends = has_star;
            }
        }

        if (sections.back().empty())
        {
            sections.pop_back();
        }

        return { min_size, begings, ends };
    }

    // Time O(1).
    [[nodiscard]] constexpr auto one_less_size(const std::string_view str) noexcept -> std::string_view
    {
        assert(!str.empty());
        return { str.cbegin(), str.size() - 1U };
    }

    // Go from the end.
    // todo(p4): remove recursion.
    constexpr void is_wildcard_match_slow_rec(
        const std::string_view pattern, std::vector<std::vector<std::int8_t>> &bufs, const std::string_view text
#if _DEBUG
        ,
        std::int32_t depth = {}
#endif
    )
    {
#if _DEBUG
        if (!(++depth < ::Standard::Algorithms::Utilities::stack_max_size)) [[unlikely]]
        {
            throw std::runtime_error("Too deep stack in is_wildcard_match_slow_rec");
        }
#endif

        auto &val = bufs[pattern.size()][text.size()];
        assert(val < 0 && !pattern.empty());

        if (text.empty())
        {
            val = are_stars_only(pattern) ? 1 : 0;
            return;
        }

        if (const auto &patb = pattern.back(); star != patb)
        {
            if (!qu_match(patb, text.back()))
            {
                val = 0;
                return;
            }

            const auto &val2 = bufs[pattern.size() - 1U][text.size() - 1U];
            if (val2 < 0)
            {
                is_wildcard_match_slow_rec(one_less_size(pattern), bufs, one_less_size(text)
#if _DEBUG
                                                                             ,
                    depth
#endif
                );
            }

            val = val2;
            assert(!(val < 0));

            return;
        }

        {
            const auto &skip = bufs[pattern.size() - 1U][text.size()];
            if (skip < 0)
            {
                is_wildcard_match_slow_rec(one_less_size(pattern), bufs, text
#if _DEBUG
                    ,
                    depth
#endif
                );
            }

            if (0 < skip)
            {
                val = 1;
                return;
            }
        }

        const auto &take = bufs[pattern.size()][text.size() - 1U];
        if (take < 0)
        {
            is_wildcard_match_slow_rec(pattern, bufs, one_less_size(text)
#if _DEBUG
                                                          ,
                depth
#endif
            );
        }

        val = 0 < take ? 1 : 0;
    }

    // NFA, FSA, NFSA, NDFSA.
    // NFM, FSM, NFSM, NDFSM.
    // Time O(n).
    [[nodiscard]] constexpr auto build_nfa(const std::string_view pattern)
        -> std::vector<std::vector<std::pair<std::size_t, char>>>
    {
        assert(!pattern.empty());

        std::vector<std::vector<std::pair<std::size_t, char>>> automaton;
        automaton.reserve(pattern.size() + 1U);
        automaton.emplace_back();

        for (std::size_t index{}; index < pattern.size();)
        {
            if (const auto &cha = pattern[index]; !qu_match(cha, star))
            {
                auto &bac = automaton.back();

                assert(bac.empty() || (1U == bac.size() && star == bac[0].second));

                bac.emplace_back(automaton.size(), cha);
                automaton.emplace_back();
                ++index;
                continue;
            }

            // "?*?**?" -> "???*"
            auto [quest_count, has_star] = quest_count_has_star(pattern, index);

            while (0U < quest_count--)
            {
                auto &bac = automaton.back();
                assert(bac.empty());

                bac.emplace_back(automaton.size(), question);
                automaton.emplace_back();
            }

            if (has_star) // Remain in the current position.
            {
                auto &bac = automaton.back();
                assert(bac.empty());

                bac.emplace_back(automaton.size() - 1U, star);
            }
        }

        assert(!automaton.empty());

        if constexpr (::Standard::Algorithms::is_debug)
        {
            [[maybe_unused]] const auto max_positions_size =
                std::accumulate(automaton.cbegin(), automaton.cend(), std::size_t{},
                    [](const std::size_t prev_max, const auto &positions) noexcept
                    {
                        return std::max<std::size_t>(prev_max, positions.size());
                    });

            assert(max_positions_size <= 2ZU);
        }

        return automaton;
    }

    // Return # of steps which can be used in debugging.
    // Time O(|pattern|).
    auto accept_cycle(const std::vector<std::vector<std::pair<std::size_t, char>>> &automaton,
        const std::unordered_set<std::size_t> &prevs, const char cha, std::unordered_set<std::size_t> &curs)
        -> std::size_t
    {
        assert(!automaton.empty() && !prevs.empty() && prevs.size() <= automaton.size() && &prevs != &curs);

        curs.clear();

        [[maybe_unused]] std::size_t steps{};

        if constexpr (::Standard::Algorithms::is_debug)
        {
            steps = prevs.size();
        }

        for (const auto &pre : prevs)
        {
            assert(pre < automaton.size());

            const auto &positions = automaton[pre];

            // The last position might be empty.
            assert(!positions.empty() || pre + 1U == automaton.size());
            assert(positions.size() <= 2ZU);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                steps += positions.size();
            }

            for (const auto &[tod, matcha] : positions)
            {
                if (star == matcha)
                {// Stay.
                    assert(pre == tod);

                    curs.insert(pre);
                    continue;
                }

                assert(pre + 1 == tod && tod < automaton.size());

                if (const auto move_to_next_position = qu_match(matcha, cha); move_to_next_position)
                {
                    curs.insert(tod);
                }
            }
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            return steps;
        }

        return {};
    }

    [[nodiscard]] auto accept_nfa(
        const std::vector<std::vector<std::pair<std::size_t, char>>> &automaton, const std::string_view text) -> bool
    {
        assert(!automaton.empty() && !text.empty());

        // std::unordered_set hides duplicates as various positions can lead to a position.
        std::array<std::unordered_set<std::size_t>, 2> sostoyans{};
        sostoyans[0].insert(0ZU);

        [[maybe_unused]] std::uint64_t total_steps{};

        for (std::size_t index{}, flag{};;)
        {
            const auto &prevs = sostoyans.at(flag);
            auto &curs = sostoyans.at(flag ^ 1U);

            const auto &cha = text[index];

            [[maybe_unused]] const auto steps = accept_cycle(automaton, prevs, cha, curs);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                total_steps += steps;
            }

            if (curs.empty())
            {
                return false;
            }

            if (++index == text.size())
            {
                auto has = curs.contains(automaton.size() - 1ZU);
                return has;
            }

            flag ^= 1U;
        }
    }

    // Time O(n).
    [[nodiscard]] constexpr auto match_side(
        const std::string &section, const bool is_beginning, std::string_view &text, std::size_t &min_left_size) -> bool
    {
        assert(!section.empty() && section.size() <= min_left_size && section.size() <= text.size());

        {
            const std::size_t init_pos = is_beginning ? 0ZU : text.size() - section.size();

            const std::string_view pat_view(section);
            const std::string_view text_view = text.substr(init_pos, section.size()); // substr may throw.

            if (!without_star_match({ pat_view, text_view }))
            {
                return false;
            }
        }

        text = text.substr(is_beginning ? section.size() : 0ZU, text.size() - section.size());

        min_left_size -= section.size();

        assert(min_left_size <= text.size());

        return true;
    }

    // Total time O(|pattern| * |text|).
    [[nodiscard]] constexpr auto section_match_naive(
        const std::string_view pat_view, const std::size_t min_left_size, std::string_view &text) -> bool
    {
        assert(0ZU < min_left_size);

        for (;;)
        {
            if (text.size() < min_left_size)
            {
                return false;
            }

            if (const std::string_view text_view = text.substr(0ZU, pat_view.size());
                without_star_match({ pat_view, text_view }))
            {
                return true;
            }

            text = text.substr(1ZU);
        }
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Strings::is_wildcard_match_naive(
    const pattern_text_view<const std::string_view, const std::string_view> &pat_text) -> bool
{
    const auto &pattern = pat_text.pattern;

    if (auto first1 = pattern.find_first_of(star); std::string::npos == first1) // Can be cached.
    {// Including an empty pattern.
        return without_star_match(pat_text);
    }

    std::vector<std::string> sections;
    auto [min_left_size, begstar, endstar] = min_length_begstar_endstar(pattern, sections); // Can be cached.

    {
        const auto only_stars = 0U == min_left_size;
        assert(sections.empty() == only_stars);

        if (only_stars)
        {
            return true;
        }
    }

    auto text = pat_text.text;
    if (text.size() < min_left_size)
    {
        return false;
    }

    if (const auto &section = sections.at(0); !begstar && !match_side(section, true, text, min_left_size))
    {
        return false;
    }

    if (const auto &section = sections.back(); !endstar && !match_side(section, false, text, min_left_size))
    {
        return false;
    }

    const std::size_t left_sects = sections.size() - (endstar ? 0ZU : 1ZU);
    std::size_t sect_ind = begstar ? 0 : 1;

    assert(sect_ind <= left_sects);
    [[assume(sect_ind <= left_sects)]];

    for (; sect_ind < left_sects; ++sect_ind)
    {
        const auto &sect = sections[sect_ind];
        const std::string_view pat_view(sect);
        assert(!sect.empty() && sect.size() <= min_left_size);

        if (!section_match_naive(pat_view, min_left_size, text))
        {
            return false;
        }

        assert(!text.empty() && sect.size() <= text.size());

        text = text.substr(sect.size());
        min_left_size -= sect.size();
    }

    return true;
}

[[nodiscard]] auto Standard::Algorithms::Strings::is_wildcard_match_slow_still(
    const pattern_text_view<const std::string_view, const std::string_view> &pat_text) -> bool
{
    const auto &[pattern, text] = pat_text;

    if (pattern.empty())
    {
        return text.empty();
    }

    if (are_stars_only(pattern)) // It can be cached.
    {
        return true;
    }

    if (text.empty()) // At least 1 symbol unmatched.
    {
        return false;
    }

    const auto automaton = build_nfa(pattern); // It can also be cached.
    auto result = accept_nfa(automaton, text);
    return result;
}

[[nodiscard]] auto Standard::Algorithms::Strings::is_wildcard_match_slow(
    const pattern_text_view<const std::string_view, const std::string_view> &pat_text) -> bool
{
    const auto &pattern = pat_text.pattern;
    const auto &text = pat_text.text;

    if (pattern.empty())
    {
        return text.empty();
    }

    // todo(p4): 2 bits instead of 8 are enough.
    std::vector<std::vector<std::int8_t>> bufs(pattern.size() + 1U, std::vector<std::int8_t>(text.size() + 1U, -1));

    {
        auto &buf = bufs.at(0);
        buf.assign(buf.size(), 0);
        buf.at(0) = 1;
    }

    const auto &result = bufs.back().back();
    if (result < 0)
    {
        is_wildcard_match_slow_rec(pattern, bufs, text);
    }

    assert(!(result < 0));

    return 0 < result;
}
