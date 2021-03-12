#pragma once

#include <functional>
#include <limits>
#include <utility>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/StreamUtilities.h"

namespace
{
    //TODO: p3. move it to .CC file.
    template<typename pair_t>
    void check_input(const std::vector<size_t>& word_lengths,
        const size_t line_length)
    {
        const auto size = word_lengths.size();
        RequirePositive(size, "words.size.");
        RequirePositive(line_length, "line_length.");

        for (const auto& len : word_lengths)
        {
            if (!(len - 1 <= line_length - 1))
            {
                std::ostringstream ss;
                ss << "The word length (" << len << ") must be in [1, " << line_length << "].";
                StreamUtilities::throw_exception<std::out_of_range>(ss);
            }
        }
    }

    template <typename pair_t, typename empty_space_eval_func_t>
    std::vector<std::pair<size_t, size_t>> compute_cost_slow(
        const std::vector<size_t>& word_lengths,
        const size_t line_length,
        empty_space_eval_func_t empty_space_eval)
    {
        const auto size = word_lengths.size();

        //todo: p3. n*n -> n space.
        std::vector<std::vector<size_t>> matrix(size,
            std::vector<size_t>(size, std::numeric_limits<size_t>::max()));

        for (size_t fi = 0; fi < size; ++fi)
        {
            auto new_len = word_lengths[fi];
            matrix[fi][fi] = empty_space_eval(line_length - new_len);
            for (size_t la = fi + 1; la < size; ++la)
            {
                new_len += 1 + word_lengths[la];
                if (line_length < new_len) break;
                matrix[fi][la] = empty_space_eval(line_length - new_len);
            }
        }

        //The cost and the index to the best previous range
        // for a prefix from 0 to (i-1) inclusively.
        std::vector<std::pair<size_t, size_t>> cost_previous(size + 1,
            std::make_pair(std::numeric_limits<size_t>::max(), 0));

        cost_previous[0].first = 0;

        for (int fi = 1; fi <= size; fi++)
        {
            for (int la = 1; la <= size; la++)
            {
                if (std::numeric_limits<size_t>::max() != matrix[fi - 1][la - 1])
                {
                    const auto cost = cost_previous[fi - 1].first
                        //Take [0..fi-1] inclusively
                        +
                        // and then add the last line as [fi..la].
                        matrix[fi - 1][la - 1];
                    if (cost < cost_previous[la].first)
                    {
                        cost_previous[la].first = cost;
                        cost_previous[la].second = fi - 1;
                    }
                }
            }
        }

        return cost_previous;
    }

    template <typename pair_t>
    void backtrace_indexes_slow(const std::vector<std::pair<size_t, size_t>>& cost_previous,
        size_t size,
        std::vector<pair_t>& chosen_indexes)
    {
        chosen_indexes.clear();
        for (;;)
        {
            chosen_indexes.push_back(pair_t(cost_previous[size].second, size - 1));
            size = cost_previous[size].second;
            if (0 == size)
                break;
        }

        std::reverse(chosen_indexes.begin(), chosen_indexes.end());
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            // Let every word can start a line.
            // Time O(n*n), space O(n).
            template<typename pair_t// = std::pair<size_t, size_t>
                , typename empty_space_eval_func_t = std::function<size_t(size_t)>>
                size_t text_justification_line_word_breaking_wrapping(
                    const std::vector<size_t>& word_lengths,
                    const size_t line_length_max,
                    std::vector<pair_t>& chosen_indexes,
                    empty_space_eval_func_t empty_space_eval = [](size_t siz) -> auto {
                return //siz*
                    siz * siz; })
            {
                check_input<pair_t>(word_lengths, line_length_max);

                constexpr auto minus_one = size_t(0) - size_t(1);
                const auto size = word_lengths.size();

                if (size <= line_length_max)
                {// All fit in 1 line?
                    for (size_t sum = minus_one, i = 0;;)
                    {
                        sum += 1 + word_lengths[i];
                        if (sum > line_length_max)
                            break;

                        if (++i == size)
                        {
                            chosen_indexes.emplace_back(0, size - 1);
                            return empty_space_eval(line_length_max - sum);
                        }
                    }
                }

                constexpr auto inf = std::numeric_limits<size_t>::max() / 3;

                std::vector<size_t> costs(size, inf);
                costs.back() = empty_space_eval(line_length_max - word_lengths.back());

                std::vector<size_t> indexes(size);
                indexes.back() = size - 1;

                for (auto i = size - 2;;)
                {// Each word starts a new line.
                    for (auto j = i, length = minus_one; j < size; ++j)
                    {// Pack as many words as possible.
                        length += 1 + word_lengths[j];

                        if (length > line_length_max)
                            break;

                        // todo: p4. The last line cost had better be zero.
                        const size_t cand = empty_space_eval(line_length_max - length) +
                            (j == size - 1 ? 0 : costs[j + 1]);

                        if (costs[i] > cand)
                        {
                            costs[i] = cand;
                            indexes[i] = j;
                        }
                    }

                    if (!i--)
                        break;
                }

                chosen_indexes.clear();

                for (size_t i = 0; i < size; i = 1 + indexes[i])
                    chosen_indexes.emplace_back(i, indexes[i]);

                return costs.front();
            }

                // Slow time/space O(n*n).
                template<typename pair_t// = std::pair<size_t, size_t>
                    , typename empty_space_eval_func_t = std::function<size_t(size_t)>>
                    size_t text_justification_line_word_breaking_wrapping_slow(
                        const std::vector<size_t>& word_lengths,
                        const size_t line_length,
                        std::vector<pair_t>& chosen_indexes,
                        empty_space_eval_func_t empty_space_eval = [](size_t siz) -> auto {
                    return //siz*
                        siz * siz; })
                {
                    check_input<pair_t>(word_lengths, line_length);
                    const auto size = word_lengths.size();

                    const auto cost_previous = compute_cost_slow<pair_t, empty_space_eval_func_t>(
                        word_lengths, line_length, empty_space_eval);

                    backtrace_indexes_slow<pair_t>(cost_previous, size, chosen_indexes);
                    return cost_previous[size].first;
                }
        }
    }
}