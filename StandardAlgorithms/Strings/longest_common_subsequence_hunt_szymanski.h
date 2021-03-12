#pragma once
#include"../Utilities/is_debug.h"
#include"match_begin_end.h"
#include<vector>

namespace Standard::Algorithms::Strings
{
    struct match_info_t final
    {
        // Source string start index, destination .., and positive length for both.
        using source_destination_length_t = std::tuple<std::size_t, std::size_t, std::size_t>;

        // Source string start index, destination .., and previous pointer or 'null_pointer' if none.
        using source_destination_prev_t = std::tuple<std::size_t, std::size_t, std::ptrdiff_t>;

        std::vector<source_destination_length_t> matches{};

        std::vector<source_destination_prev_t> all_prevs{};

        std::vector<std::ptrdiff_t> prevs{};
    };

    // Hunt Szymanski algorithm to find LCS of 2 strings.
    // LIS, the longest increasing subsequence, seems to exploit a similar idea.
    // Good for random strings, especially when the source is fixed and the destination changes.
    // If possible, the source had better be the smallest string.
    // Given "128456371" and "0012459670",
    // the computed LCS is "124567" of size = 6.
    struct longest_common_subsequence_hunt_szymanski final
    {
        longest_common_subsequence_hunt_szymanski() = delete;

        template<bool shall_fill_matches, class string_t, class map_t>
        static constexpr auto lcs(const string_t &source, const map_t &source_key_positions,
            const string_t &destination, std::vector<std::size_t> &commons,
            // Set 'shall_fill_matches' to true to get the actual matches.
            match_info_t &match_info) -> std::size_t
        {
            assert(source_key_positions.size() <= source.size());

            commons.clear();

            if constexpr (shall_fill_matches)
            {
                match_info.matches.clear();
                match_info.all_prevs.clear();
                match_info.prevs.clear();
            }

            if (source.empty() || destination.empty())
            {
                return 0;
            }

            assert(!source_key_positions.empty());

            const auto [start, source_stop, destination_stop] =
                match_begin_end<string_t, string_t>(source, destination);

            if constexpr (shall_fill_matches)
            {
                if (0U < start)
                {
                    match_info.matches.emplace_back(0U, 0U, start);

                    validate_match(match_info.matches.back(), source.size(), destination.size());
                }
            }

            auto lcs = start;
            if (start < source_stop)
            {
                const auto pointer = lcs_impl<shall_fill_matches, string_t, map_t>(source, source_key_positions, start,
                    source_stop, destination, destination_stop, commons, match_info);

                lcs += commons.size();

                if constexpr (shall_fill_matches)
                {
                    if (!match_info.all_prevs.empty())
                    {
                        assert(0U < lcs && !(pointer < 0));

                        backtrack_matches(match_info, pointer, source.size(), destination.size());
                    }
                }
            }

            if constexpr (shall_fill_matches)
            {
                if (source_stop < source.size())
                {
                    assert(source.size() - source_stop == destination.size() - destination_stop);

                    prepend_match_in_reverse_order(
                        match_info.matches, source_stop, destination_stop, source.size() - source_stop);

                    validate_match(match_info.matches.back(), source.size(), destination.size());
                }
            }

            lcs += source.size() - source_stop;
            assert(lcs <= std::min(source.size(), destination.size()));

            return lcs;
        }

        static constexpr std::ptrdiff_t null_pointer = -static_cast<std::ptrdiff_t>(1);

private:
        using source_destination_length_t = typename match_info_t::source_destination_length_t;

        inline static constexpr auto push_all_prev(std::size_t source_index, std::size_t destination_index,
            match_info_t &match_info, std::size_t index) -> std::ptrdiff_t
        {
            const auto size = static_cast<std::ptrdiff_t>(match_info.all_prevs.size());
            const auto prev = 0U < index ? match_info.prevs[index - 1U] : null_pointer;

            match_info.all_prevs.emplace_back(source_index, destination_index, prev);

            return size;
        }

        template<bool shall_fill_matches>
        static constexpr void process_list(const std::size_t min_source_destination_size,
            const std::vector<std::size_t> &list, const std::size_t start,
            // matched indexes from the source.
            std::vector<std::size_t> &commons, const std::size_t source_stop, match_info_t &match_info,
            std::ptrdiff_t &pointer, const std::size_t destination_index)
        {
            assert(!list.empty());

            auto bound = commons.size();

            for (std::ptrdiff_t ind_2 = static_cast<std::ptrdiff_t>(list.size()) - 1; 0 <= ind_2; --ind_2)
            {
                const auto &source_index = list[ind_2];
                if (source_index < start)
                {
                    break; // Already compared.
                }

                if (!(source_index < source_stop) || // The ending has already been matched.
                    (0 < ind_2 && 0U < bound &&
                        // Source = 'accc', destination = 'ac'.
                        // The last 'c' at index 3 in the source can be skipped
                        // because there is a smaller index match at index 1
                        // after 'a', the current bound.
                        commons[bound - 1U] < list[ind_2 - 1]))
                {
                    continue;
                }

                {
                    auto iter = commons.cbegin();
                    bound = std::lower_bound(iter, iter + static_cast<std::ptrdiff_t>(bound), source_index) - iter;
                }

                if (bound == commons.size())
                {
                    assert(commons.size() < min_source_destination_size);
                    commons.push_back(source_index);

                    if constexpr (shall_fill_matches)
                    {
                        pointer = push_all_prev(source_index, destination_index, match_info, bound);
                        match_info.prevs.push_back(pointer);
                    }

                    continue;
                }

                assert(bound < commons.size());

                commons[bound] = source_index;

                if constexpr (shall_fill_matches)
                {
                    match_info.prevs[bound] = push_all_prev(source_index, destination_index, match_info, bound);
                }
            }
        }

        template<bool shall_fill_matches, class string_t, class map_t>
        static constexpr auto lcs_impl(const string_t &source, const map_t &source_key_positions,
            const std::size_t start, const std::size_t source_stop, const string_t &destination,
            const std::size_t destination_stop, std::vector<std::size_t> &commons, match_info_t &match_info)
        {
            assert(!source_key_positions.empty());

            auto pointer = null_pointer;

            for (auto destination_index = start; destination_index < destination_stop; ++destination_index)
            {
                const auto &cha = destination[destination_index];
                const auto iter = source_key_positions.find(cha);
                if (iter == source_key_positions.end())
                {
                    continue;
                }

                const auto &list = iter->second;
                assert(!list.empty());

                process_list<shall_fill_matches>(std::min(source.size(), destination.size()), list, start, commons,
                    source_stop, match_info, pointer, destination_index);
            }

            return pointer;
        }

        inline static constexpr void backtrack_matches(match_info_t &match_info, std::ptrdiff_t pointer,
            const std::size_t source_size, const std::size_t destination_size)
        {
            auto &matches = match_info.matches;
            const auto initial_count = matches.size();
            do
            {
                assert(!(pointer < 0) && static_cast<std::size_t>(pointer) < match_info.all_prevs.size());

                const auto &item = match_info.all_prevs[pointer];

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    if (initial_count < matches.size())
                    {
                        const auto &last = matches.back();
                        assert(std::get<0>(item) < std::get<0>(last) && std::get<1>(item) < std::get<1>(last));
                    }
                }

                prepend_match_in_reverse_order(matches, std::get<0>(item), std::get<1>(item), 1);

                validate_match(matches.back(), source_size, destination_size);

                pointer = std::get<2>(item);
            } while (!(pointer < 0));

            std::reverse(matches.begin() + static_cast<std::ptrdiff_t>(initial_count), matches.end());
        }

        inline static constexpr void prepend_match_in_reverse_order(std::vector<source_destination_length_t> &matches,
            const std::size_t source_start, const std::size_t destination_start, const std::size_t length)
        {
            assert(0U < length && 0U < source_start + length && 0U < destination_start + length);

            if (!matches.empty())
            {
                auto &last = matches.back();
                const auto is_append =
                    std::get<0>(last) == source_start + length && std::get<1>(last) == destination_start + length;

                if (is_append)
                {
                    last = { source_start, destination_start, length + std::get<2>(last) };
                    return;
                }
            }

            matches.emplace_back(source_start, destination_start, length);
        }

        inline static constexpr void validate_match(const source_destination_length_t &match,
            const std::size_t source_length, const std::size_t destination_length)
        {
            assert(0U < source_length && 0U < destination_length);

            [[maybe_unused]] const auto &source_start = std::get<0>(match);
            [[maybe_unused]] const auto &destination_start = std::get<1>(match);
            [[maybe_unused]] const auto &length = std::get<2>(match);

            assert(0U < length && 0U < source_start + length && 0U < destination_start + length);
        }
    };
} // namespace Standard::Algorithms::Strings
