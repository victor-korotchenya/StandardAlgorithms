#pragma once
#include"../Utilities/require_utilities.h"
#include<optional>
#include<queue>

namespace Standard::Algorithms::Numbers::Inner
{
    template<class string_t>
    [[nodiscard]] constexpr auto skip_same(const string_t &source, const string_t &ordered)
        -> std::pair<std::int32_t, std::int32_t>
    {
        auto size = static_cast<std::int32_t>(source.size());

        assert(source.size() == ordered.size() && 0 < size);

        std::int32_t start{};

        while (start < size && source[start] == ordered[start])
        {
            ++start;
        }

        while (start < size && source[size - 1] == ordered[size - 1])
        {
            --size;
        }

        return { start, size };
    }

    template<class map_t, class queue_t>
    [[nodiscard]] constexpr auto mcp_run_once(const std::int32_t stop, map_t &str_infos, queue_t &que,
        std::size_t &steps) -> std::optional<std::pair<std::int32_t, std::size_t>>
    {
        constexpr std::int32_t one = 1;

        using dist_isdest_t = std::pair<std::int32_t, bool>;

        const dist_isdest_t info{ que.front().second.first + one, que.front().second.second };

        assert(0 < info.first);

        const auto str = std::move(que.front().first);
        que.pop();

        for (auto len = one; len < stop; ++len)
        {
            const auto remain_length = stop - len;

            for (std::int32_t from{}; from <= remain_length; ++from)
            {
                // aBcd -> Bacd, aBcd (skip), acBd, acdB.
                const auto cut = str.substr(from, len); // todo(p3): std::string_view
                const auto rest = str.substr(0, from) + str.substr(from + len);

                for (std::int32_t tod = from == 0 ? one : 0; tod <= remain_length; ++tod)
                {
                    auto cand = rest.substr(0, tod) + cut + rest.substr(tod);

                    auto &info2 = str_infos[cand];

                    if (0 == info2.first)
                    {
                        info2 = info;
                        que.emplace(std::move(cand), info);
                        ++steps;
                    }
                    else if (info.second != info2.second)
                    {
                        return std::make_pair(info.first + info2.first -
                                // Initially added two 1s as non-zeros.
                                2,
                            steps);
                    }
                }
            }
        }

        return std::nullopt;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    constexpr auto mcp_sort_perm_max_size = 20U;

    // Count min number of cut-paste operations to sort a permutation using two-wave meeting, or meet in the middle, or
    // bidirectional search. In a cut-paste operation, a contiguous substring can be cut off, and pasted into the
    // remaining string. For example, source is 54321 ; step 0 54 32 1 ; step 0 32 54 1 ; step 1 ; move 32 to the front.
    // 32541 ; step 1
    // 3 25 41 ; step 1
    // 3 41 25 ; step 2 ; move 25 to the back.
    // 34125 ; step 2
    // 34 12 5 ; step 2
    // 12 34 5 ; step 3 ; move 34 to the middle between 12 and 5.
    // 12345 ; step 3. Done in 3 steps.

    // todo(p3): min_cut_paste_to_sort_permutation

    template<class map_t, // std::unordered_map<std::string, std::pair<std::int32_t, bool>>
        class string_t>
    [[nodiscard]] constexpr auto min_cut_paste_to_sort_permutation_slow_still(const string_t &source)
        // Cost, run steps (fewer is better)
        -> std::pair<std::int32_t, std::size_t>
    {
        require_less_equal(source.size(), mcp_sort_perm_max_size, "source size");

        constexpr std::int32_t one = 1;

        const auto size = static_cast<std::int32_t>(source.size());
        if (size <= one)
        {
            return {};
        }

        auto ordered = source;
        // todo(p3): Counting sort could suit here.
        std::sort(ordered.begin(), ordered.end());

        auto [start, stop] = Inner::skip_same<string_t>(source, ordered);

        if (start == stop)
        {
            return {};
        }

        if (start + 2 == stop)
        {
            return { 1, 0 };
        }

        [[maybe_unused]] constexpr std::int32_t min_size = 3;

        assert(min_size <= stop);

        map_t str_infos{};

        using dist_isdest_t = std::pair<std::int32_t, bool>;
        using queue_t = std::queue<std::pair<string_t, dist_isdest_t>>;

        queue_t que{};

        if (dist_isdest_t src_pair{ one, false }; // Use 1 as non-zero value.
            0 == start && size == stop)
        {
            str_infos[source] = src_pair;
            que.emplace(source, src_pair);
        }
        else
        {
            stop -= start;
            assert(min_size <= stop);

            auto sub = source.substr(start, stop);
            ordered = ordered.substr(start, stop);

            str_infos[sub] = src_pair;
            que.emplace(std::move(sub), src_pair);
        }

        {
            dist_isdest_t targ_pair{ one, true };
            str_infos[ordered] = targ_pair;

            que.emplace(std::move(ordered), targ_pair);
        }

        std::size_t steps = 2;

        do
        {
            const auto opt = Inner::mcp_run_once<map_t, queue_t>(stop, str_infos, que, steps);

            if (opt.has_value())
            {
                return opt.value();
            }
        } while (0U < que.size());

        auto err = "min_cut_paste_to_sort_permutation_slow_still inner error. Source '" + source + "'.";

        throw std::runtime_error(err);
    }

    // Slow time O(n! * n**4), space O(n!).
    template<class map_t, // std::unordered_map<std::string, std::int32_t>>
        class string_t>
    [[nodiscard]] constexpr auto min_cut_paste_to_sort_permutation_slow(const string_t &source)
        -> std::pair<std::int32_t, std::size_t>
    {
        require_less_equal(source.size(), mcp_sort_perm_max_size, "source size");

        constexpr std::int32_t one = 1;

        const auto size = static_cast<std::int32_t>(source.size());
        if (size <= one)
        {
            return {};
        }

        auto ordered = source;
        // todo(p3): Counting sort could suit well here.
        std::sort(ordered.begin(), ordered.end());

        if (source == ordered)
        {
            return {};
        }

        map_t str_distance{};
        std::queue<std::pair<string_t, std::int32_t>> que;

        str_distance[source] = one;
        que.emplace(source, one);

        str_distance[ordered] = -one;
        // string_t buf(size, 0); // it is 2.5 times slower

        do
        {
            const auto dist1 = que.front().second + one;
            assert(0 < dist1);

            const auto str = std::move(que.front().first);
            que.pop();

            for (auto len = one; len < size; ++len)
            {
                const auto remain_length = size - len;

                for (std::int32_t from{}; from <= remain_length; ++from)
                {
                    // aBcd -> Bacd, aBcd (skip), acBd, acdB.
                    const auto cut = str.substr(from, len); // todo(p3): std::string_view
                    const auto rest = str.substr(0, from) + str.substr(from + len);

                    for (auto tod = from == 0 ? one : 0; tod <= remain_length; ++tod)
                    {
                        auto buf = rest.substr(0, tod) + cut + rest.substr(tod);
                        // std::copy(rest.cbegin(), rest.cbegin() + tod, buf.begin());
                        // std::copy(str.cbegin() + from, str.cbegin() + from + len, buf.begin() + tod);
                        ////std::copy(cut.cbegin(), cut.cend(), buf.begin() + tod);
                        // std::copy(rest.cbegin() + tod, rest.cend(), buf.begin() + tod + len);

                        auto &cur_dist = str_distance[buf];
                        if (0 == cur_dist)
                        {
                            cur_dist = dist1;
                            que.emplace(std::move(buf), dist1);
                        }
                        else if (cur_dist < 0)
                        {
                            assert(ordered == buf);

                            return { dist1 - one, str_distance.size() };
                        }
                    }
                }
            }
        } while (0U < que.size());

        auto err = "min_cut_paste_to_sort_permutation_slow inner error. Source '" + source + "'.";

        throw std::runtime_error(err);
    }
} // namespace Standard::Algorithms::Numbers
