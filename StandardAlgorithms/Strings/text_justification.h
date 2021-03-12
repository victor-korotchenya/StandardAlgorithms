#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Strings::Inner
{
    template<class iter_t>
    constexpr void check_input(iter_t first, iter_t last, const std::size_t line_length)
    {
        if (first == last) [[unlikely]]
        {
            throw std::out_of_range("Must be at least one word.");
        }

        require_positive(line_length, "line length.");

        do
        {
            const auto &cur = *first;

            if (!(cur - 1U <= line_length - 1U)) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The word length (" << cur << ") must be in [1, " << line_length << "].";

                throw_exception<std::out_of_range>(str);
            }
        } while (++first != last);
    }

    template<class empty_space_eval_func_t>
    [[nodiscard]] constexpr auto compute_cost_slow(const std::vector<std::size_t> &word_lengths,
        const std::size_t line_length, empty_space_eval_func_t empty_space_eval)
        -> std::vector<std::pair<std::size_t, std::size_t>>
    {
        const auto size = word_lengths.size();

        // todo(p3): n*n -> n space.
        std::vector<std::vector<std::size_t>> matrix(
            size, std::vector<std::size_t>(size, std::numeric_limits<std::size_t>::max()));

        for (std::size_t finish{}; finish < size; ++finish)
        {
            auto new_len = word_lengths[finish];
            auto &matr = matrix[finish];
            matr[finish] = empty_space_eval(line_length - new_len);

            auto last = finish;

            while (++last < size)
            {
                new_len += 1ZU + word_lengths[last];
                if (line_length < new_len)
                {
                    break;
                }

                matr[last] = empty_space_eval(line_length - new_len);
            }
        }

        // The cost and the index to the best previous range
        // for a prefix from 0 to (index-1) inclusively.
        std::vector<std::pair<std::size_t, std::size_t>> cost_previous(
            size + 1ZU, std::make_pair(std::numeric_limits<std::size_t>::max(), 0ZU));

        cost_previous.at(0).first = 0ZU;

        for (std::size_t finish{}; finish < size; ++finish)
        {
            const auto &matr = matrix[finish];
            const auto &pre = cost_previous[finish].first;

            for (std::size_t las = 1U; las <= size; ++las)
            {
                if (std::numeric_limits<std::size_t>::max() == matr[las - 1U])
                {
                    continue;
                }

                const auto cost = pre // Take [0..finish] inclusively
                    + // and then add the last line as [finish+1..las].
                    matrix[finish][las - 1U];

                auto &cur = cost_previous[las];

                if (cost < cur.first)
                {
                    cur.first = cost;
                    cur.second = finish;
                }
            }
        }

        return cost_previous;
    }

    template<class pair_t>
    constexpr void backtrace_indexes_slow(const std::vector<std::pair<std::size_t, std::size_t>> &cost_previous,
        std::size_t size, std::vector<pair_t> &chosen_indexes)
    {
        chosen_indexes.clear();

        do
        {
            const auto &pre = cost_previous.at(size).second;

            chosen_indexes.emplace_back(pre, size - 1U);
            size = pre;
        } while (0U < size);

        std::reverse(chosen_indexes.begin(), chosen_indexes.end());
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    template<std::integral int_t = std::size_t,
        class func_t1 = decltype([] [[nodiscard]] (int_t siz) -> int_t
        {// Squared; could be made cubic.
            return siz * siz;
        })>
    requires(sizeof(std::int32_t) <= sizeof(int_t))
    using squared_eval_t = func_t1;

    // Text justification, pretty printing.
    // Given word lengths, each not exceeding the line max length, compute where to start new lines.
    // Idea. Let every word start a line.
    // Time O(n*n), space O(n).
    template<class pair_t, class empty_space_eval_func_t = squared_eval_t<>>
    [[nodiscard]] constexpr auto text_justification_line_word_breaking_wrapping(
        const std::vector<std::size_t> &word_lengths, const std::size_t line_length_max,
        std::vector<pair_t> &chosen_indexes, empty_space_eval_func_t empty_space_eval = {}) -> std::size_t
    {
        Inner::check_input(word_lengths.cbegin(), word_lengths.cend(), line_length_max);

        chosen_indexes.clear();

        constexpr auto minus_one = std::size_t{} - static_cast<std::size_t>(1);
        static_assert(0U < minus_one);

        const auto size = word_lengths.size();

        if (const auto can_fit_in_one_line = size <= line_length_max; can_fit_in_one_line)
        {
            for (std::size_t sum = minus_one, index{};;)
            {
                sum += 1ZU + word_lengths[index];

                if (line_length_max < sum)
                {
                    break;
                }

                if (++index == size)
                {
                    chosen_indexes.emplace_back(0ZU, size - 1ZU);
                    return empty_space_eval(line_length_max - sum);
                }
            }
        }

        assert(1U < size);

        constexpr auto inf = std::numeric_limits<std::size_t>::max() / 3U;

        std::vector<std::size_t> costs(size, inf);
        costs.back() = empty_space_eval(line_length_max - word_lengths.back());

        std::vector<std::size_t> indexes(size);
        indexes.back() = size - 1ZU;

        for (auto index = size - 2ZU;;)
        {// Each word starts a new line.
            auto &cost = costs[index];

            for (auto ind_2 = index, length = minus_one; ind_2 < size; ++ind_2)
            {// Try all possible packs.
                length += 1ZU + word_lengths[ind_2];

                if (line_length_max < length)
                {
                    break;
                }

                // todo(p4): The last line cost had better be zero.
                const std::size_t cand =
                    empty_space_eval(line_length_max - length) + (ind_2 == size - 1ZU ? 0ZU : costs[ind_2 + 1ZU]);

                if (cand < cost)
                {
                    cost = cand;
                    indexes[index] = ind_2;
                }
            }

            if (!index--)
            {
                break;
            }
        }

        for (std::size_t index{}; index < size; index = 1ZU + indexes[index])
        {
            chosen_indexes.emplace_back(index, indexes[index]);
        }

        return costs.at(0);
    }

    // Slow time, space O(n*n).
    template<class pair_t, class empty_space_eval_func_t = squared_eval_t<>>
    [[nodiscard]] constexpr auto text_justification_line_word_breaking_wrapping_slow(
        const std::vector<std::size_t> &word_lengths, const std::size_t line_length_max,
        std::vector<pair_t> &chosen_indexes, empty_space_eval_func_t empty_space_eval = {}) -> std::size_t
    {
        Inner::check_input(word_lengths.cbegin(), word_lengths.cend(), line_length_max);

        const auto size = word_lengths.size();

        const auto cost_previous =
            Inner::compute_cost_slow<empty_space_eval_func_t>(word_lengths, line_length_max, empty_space_eval);

        Inner::backtrace_indexes_slow<pair_t>(cost_previous, size, chosen_indexes);

        return cost_previous.at(size).first;
    }
} // namespace Standard::Algorithms::Strings
