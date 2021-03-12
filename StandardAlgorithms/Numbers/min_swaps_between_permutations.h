#pragma once
#include"../Utilities/remove_duplicates.h"
#include<queue>
#include<unordered_map>

namespace Standard::Algorithms::Numbers::Inner
{
    template<class string_t>
    [[nodiscard]] constexpr auto check_permutation_string_size(
        const string_t &source, const std::size_t max_size, const string_t &destination) -> std::int32_t
    {
        require_equal(
            source.size(), "Source and destination permutations must have the same size.", destination.size());

        {
            constexpr auto min_size = 2U;

            require_less_equal(min_size, source.size(), "size");
        }

        require_less_equal(source.size(), max_size, "size");

        auto src = source;
        std::sort(src.begin(), src.end());

        auto dest = destination;
        std::sort(dest.begin(), dest.end());

        require_equal(src, "Source and destination permutations must consist of the same items.", dest);

        ::Standard::Algorithms::remove_duplicates(src);

        require_equal(src, "The source should have been a permutation of the unique items.", dest);

        auto size = static_cast<std::int32_t>(source.size());

        return size;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Count min swaps of two consecutive elements
    // to turn a permutation into another one
    // using 2 waves meeting, or meet in the middle, or bidirectional search.
    // Time O(n*n), space O(n).
    template<class string_t>
    [[nodiscard]] constexpr auto min_swaps_between_permutations(const string_t &source, const string_t &destination)
        -> std::int64_t
    {
        if (source == destination)
        {
            return {};
        }

        constexpr auto max_size = static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max() - 2);

        // todo(p5): It can be omitted or made linear for a small ABC.
        auto size = Inner::check_permutation_string_size<string_t>(source, max_size, destination);

        while (source[size - 1] == destination[size - 1])
        {
            assert(2 <= size);
            --size;
        }

        assert(0 < size);

        std::int32_t start{};

        while (source[start] == destination[start])
        {
            assert(start < size - 1);
            ++start;
        }

        auto cop = source;
        std::int64_t swaps{};

        do
        {
            const auto &dest_cha = destination[start];

            if (dest_cha == cop[start])
            {
                continue;
            }

            auto index = start + 1;

            while (dest_cha != cop[index])
            {
                assert(index < size - 1);
                ++index;
            }

            swaps += index - start;
            assert(0 < swaps);

            do
            {
                std::swap(cop[index], cop[index - 1]);
            } while (start < --index);
        } while (++start < size);

        assert(cop == destination && 0 < swaps);

        return swaps;
    }

    // Approach from both sides should be faster.
    template<class string_t>
    [[nodiscard]] constexpr auto min_swaps_between_permutations_slow_still(
        const string_t &source, const string_t &destination) -> std::pair<std::int32_t, std::size_t>
    {
        if (source == destination)
        {
            return {};
        }

        constexpr auto max_size = 20U;

        const auto size = Inner::check_permutation_string_size<string_t>(source, max_size, destination);

        {
            auto size1 = size;

            while (source[size1 - 1] == destination[size1 - 1])
            {
                assert(2 <= size1);
                --size1;
            }

            assert(0 < size1);

            std::int32_t start{};

            while (source[start] == destination[start])
            {
                assert(start < size1 - 1);
                ++start;
            }

            if (0 < start || size1 != size)
            {
                const auto src = source.substr(start, size1 - start); // todo(p3): std::string_view
                const auto dest = destination.substr(start, size1 - start);

                auto res = min_swaps_between_permutations_slow_still<string_t>(src, dest);

                assert(0 < res.first && 0 < res.second);

                return res;
            }
        }

        // Use (distance + 1) to differentiate from 0.
        using dist1_isdestination_t = std::pair<std::int32_t, bool>;

        constexpr std::int32_t initial_distance = 1;

        std::unordered_map<string_t, dist1_isdestination_t> mao;
        mao[source] = { initial_distance, false };
        mao[destination] = { initial_distance, true };

        std::queue<std::pair<string_t, dist1_isdestination_t>> que;
        que.emplace(source, dist1_isdestination_t{ initial_distance, false });
        que.emplace(destination, dist1_isdestination_t{ initial_distance, true });

        do
        {
            auto str = std::move(que.front().first);
            const auto dist1 = que.front().second.first + 1;
            const auto isdestination = que.front().second.second;
            assert(initial_distance < dist1);
            que.pop();

            for (std::int32_t index{}; index < size - 1; ++index)
            {
                std::swap(str[index], str[index + 1]);

                auto &[dist2, isdestination2] = mao[str];

                if (0 < dist2)
                {
                    if (isdestination != isdestination2)
                    {
                        auto swaps = dist1 + dist2 - 2 * initial_distance;
                        assert(0 < swaps);

                        return { swaps, mao.size() };
                    }
                }
                else
                {
                    dist2 = dist1;
                    isdestination2 = isdestination;
                    que.emplace(str, dist1_isdestination_t{ dist1, isdestination });
                }

                std::swap(str[index], str[index + 1]);
            }
        } while (!que.empty());

        throw std::runtime_error("min_swaps_between_permutations_slow_still inner error.");
    }

    // Slow time O(n!*n*n), space O(n!).
    template<class string_t>
    [[nodiscard]] constexpr auto min_swaps_between_permutations_slow(
        const string_t &source, const string_t &destination) -> std::pair<std::int32_t, std::size_t>
    {
        if (source == destination)
        {
            return {};
        }

        constexpr auto max_size = 20U;

        const auto size = Inner::check_permutation_string_size<string_t>(source, max_size, destination);

        constexpr std::int32_t initial_distance{};

        std::unordered_map<string_t, std::int32_t> str_distance;
        str_distance[source] = initial_distance;

        std::queue<std::pair<string_t, std::int32_t>> que;
        que.emplace(source, initial_distance);

        do
        {
            auto str = std::move(que.front().first);
            const auto dist1 = que.front().second + 1;
            assert(initial_distance < dist1);
            que.pop();

            for (std::int32_t index{}; index < size - 1; ++index)
            {
                std::swap(str[index], str[index + 1]);

                auto &dist = str_distance[str];

                if (0 == dist)
                {
                    if (destination == str)
                    {
                        return { dist1, str_distance.size() };
                    }

                    dist = dist1;
                    que.emplace(str, dist1);
                }

                std::swap(str[index], str[index + 1]);
            }
        } while (!que.empty());

        throw std::runtime_error("min_swaps_between_permutations_slow inner error.");
    }
} // namespace Standard::Algorithms::Numbers
