#pragma once
#include"../Utilities/require_utilities.h"
#include"weighted_interval.h"
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<class weight_t, class weighted_segment_t>
    struct weight_schedule_slow_context final
    {
        const std::vector<weighted_segment_t> &segments;

        weight_t max_weight{};
        std::vector<std::size_t> best_positions{};

        weight_t current_weight{};
        std::vector<std::size_t> current_positions{};

        std::size_t posa{};
    };

    // The intervals may touch/cross maximum in one point which must be an edge.
    // Each interval is assumed to be valid i.e. Start <= Stop.
    template<class segment_t>
    [[nodiscard]] constexpr auto are_compatible(const segment_t &odno, const segment_t &dwa) noexcept -> bool
    {
        // [[assume(!(odno.Stop < odno.Start) && !(dwa.Stop < dwa.Start))]];
        assert(!(odno.Stop < odno.Start) && !(dwa.Stop < dwa.Start));

        auto compatible = !(odno.Start < dwa.Stop) || !(dwa.Start < odno.Stop);
        return compatible;
    }

    template<class weight_t, class weighted_segment_t>
    [[nodiscard]] constexpr auto is_compatible_slow(
        const weight_schedule_slow_context<weight_t, weighted_segment_t> &context) -> bool
    {
        assert(!context.current_positions.empty());

        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto iter =
                std::find(context.current_positions.cbegin(), context.current_positions.cend(), context.posa);

            assert(iter == context.current_positions.cend());
        }

        const auto &cur_segment = context.segments.at(context.posa);

        // todo(p5): can be made faster: check only the last one + maintain validity.
        auto all_compatible = std::all_of(context.current_positions.begin(), context.current_positions.end(),
            [&context, &cur_segment](const auto &pos)
            {
                const auto &prev_segment = context.segments.at(pos);

                auto compati = are_compatible<weighted_segment_t>(prev_segment, cur_segment);

                assert(compati == are_compatible(cur_segment, prev_segment));

                return compati;
            });

        return all_compatible;
    }

    template<class weight_t, class weighted_segment_t>
    constexpr void try_maximize(weight_schedule_slow_context<weight_t, weighted_segment_t> &context)
    {
        assert(weight_t{} < context.current_weight && !context.current_positions.empty());

        if (!(context.max_weight < context.current_weight))
        {
            return;
        }

        context.max_weight = context.current_weight;
        context.best_positions.resize(context.current_positions.size());

        std::copy(context.current_positions.cbegin(), context.current_positions.cend(), context.best_positions.begin());
    }

    template<class weight_t, class weighted_segment_t>
    constexpr void recurse_slow(weight_schedule_slow_context<weight_t, weighted_segment_t> &context)
    {
        assert(weight_t{} < context.current_weight && 0U < context.posa);

        --context.posa;

        if (is_compatible_slow<weight_t, weighted_segment_t>(context))
        {// Include the current segment.
            const auto &segment = context.segments.at(context.posa);

            context.current_positions.push_back(context.posa);
            context.current_weight += segment.weight;

            assert(weight_t{} < segment.weight && !(context.current_weight < segment.weight));

            try_maximize<weight_t, weighted_segment_t>(context);

            if (0U < context.posa)
            {
                recurse_slow<weight_t, weighted_segment_t>(context);
            }

            assert(!context.current_positions.empty() && context.current_positions.back() == context.posa &&
                !(context.current_weight < segment.weight));

            context.current_positions.pop_back();
            context.current_weight -= segment.weight;
        }

        if (0U < context.posa)
        {// Exclude the current segment.
            recurse_slow<weight_t, weighted_segment_t>(context);
        }

        ++context.posa;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given weighted intervals, find a compatible subsequence of maximum weight.
    // Compatible means the current interval's stop is
    // equal to or less than the next interval's start.
    // A compatible example with start and stop times only:
    // {{1, 1}, {3, 8}, {10, 10}, {10, 20}, {20, 20}, {20, 20}, {20, 20}, {20, 512}, {512, 513}}.
    // An interval satisfies (start <= stop) and (0 < weight).
    template<class distance, class weight_t, class weighted_segment_t = weighted_interval<distance, weight_t>>
    struct weighted_interval_scheduling final
    {
        weighted_interval_scheduling() = delete;

        static constexpr std::size_t first_segment_index = 1;

        // todo(p4): There is no check for overflows.
        [[nodiscard]] static constexpr auto find_min_weight_schedule(
            // The segments are editable.
            std::vector<weighted_segment_t> &segments, std::vector<std::size_t> &chosen_segment_indexes) -> weight_t
        {
            validate(segments);

            {
                const auto sortir = [] [[nodiscard]] (
                                        const weighted_segment_t &one, const weighted_segment_t &two) noexcept
                {
                    auto result = one.Stop < two.Stop || (one.Stop == two.Stop && one.Start < two.Start) ||
                        (one.Stop == two.Stop && one.Start == two.Start && one.weight < two.weight) ||
                        // Not required strictly speaking,
                        // but stably sorted data is easier to debug.
                        (one.Stop == two.Stop && one.Start == two.Start && one.weight == two.weight && one.Id < two.Id);

                    return result;
                };

                std::sort(segments.begin(), segments.end(), sortir);
            }

            const auto largest_non_overlapping_smaller_index = compute_largest_non_overlapping_smaller_index(segments);

            const auto min_weights_matrix = compute_matrix(segments, largest_non_overlapping_smaller_index);

            assert(1LLU + segments.size() == min_weights_matrix.size());

            back_track_solution(
                segments, largest_non_overlapping_smaller_index, min_weights_matrix, chosen_segment_indexes);

            // Elements may be anywhere - sort the returned indexes.
            std::sort(chosen_segment_indexes.begin(), chosen_segment_indexes.end());

            const auto &result = require_positive(min_weights_matrix.back(), "resulting weight");

            return result;
        }

        // Slow time O(2**n * n*n).
        [[nodiscard]] static constexpr auto find_min_weight_schedule_slow(
            const std::vector<weighted_segment_t> &segments, std::vector<std::size_t> &chosen_segment_indexes)
            -> weight_t
        {
            const auto size = segments.size();
            {
                constexpr auto min_size = 1U;
                constexpr auto max_size = 20U;

                require_between(min_size, size, max_size, "segments size");
            }

            validate(segments);

            // A position is the index in the segments vector.
            // Id is segment.Id which might differ from the position.
            // Use positions for simper code; then will have to remap positions to Id's.
            Inner::weight_schedule_slow_context<weight_t, weighted_segment_t> context{ segments };

            for (std::size_t pos{}; pos < size; ++pos)
            {
                const auto &segment = segments[pos];

                context.current_weight = segment.weight;
                context.posa = pos;
                context.current_positions.assign(1, pos);

                try_maximize<weight_t, weighted_segment_t>(context);

                if (0U < pos)
                {
                    Inner::recurse_slow(context);
                }

                assert(context.current_weight == segment.weight);
                assert(context.current_positions.size() == 1U && context.current_positions[0] == pos);
            }

            require_positive(context.best_positions.size(), "slow best positions size");

            chosen_segment_indexes.resize(context.best_positions.size());

            for (std::size_t index{}; index < context.best_positions.size(); ++index)
            {// Remap positions to Id's.
                const auto &pos = context.best_positions[index];
                auto &ide = chosen_segment_indexes[index];
                ide = segments.at(pos).Id;

                assert(pos < size && ide < size);
            }

            auto &res = require_positive(context.max_weight, "slow result max weight");

            return std::move(res);
        }

private:
        static constexpr void validate(const std::vector<weighted_segment_t> &segments)
        {
            throw_if_empty("segments", segments);

            const auto segments_size = segments.size();

            for (std::size_t index{}; index < segments_size; ++index)
            {
                const auto &segment = segments[index];

                if (segment.Id != index) [[unlikely]]
                {
                    auto err = "Segment.Id " + std::to_string(segment.Id) + " must have been " + std::to_string(index) +
                        " index.";

                    throw std::runtime_error(err);
                }

                segment.validate();
            }
        }

        // To distinguish between 0-th segment and no segment:
        // A) Indexes in "minWeightsMatrix" start with first_segment_index (1).
        // B) 0 means there is no smaller value.
        // Time O(n*n).
        [[nodiscard]] static constexpr auto compute_largest_non_overlapping_smaller_index(
            const std::vector<weighted_segment_t> &segments) -> std::vector<std::size_t>
        {
            const auto segments_size = segments.size();

            std::vector<std::size_t> largest_non_overlapping_smaller_index(segments_size);

            for (auto current = first_segment_index; current < segments_size; ++current)
            {
                const auto &cur_start = segments[current].Start;
                auto previous = current - first_segment_index;

                do
                {// todo(p3): faster for many repetitions in O(log(n)), not in O(n).
                    const auto &prev_segm = segments[previous];

                    if (const auto is_compati = !(cur_start < prev_segm.Stop); is_compati)
                    {
                        largest_non_overlapping_smaller_index[current] = previous + first_segment_index;
                        break;
                    }
                } while (0U < previous--);
            }

            return largest_non_overlapping_smaller_index;
        }

        [[nodiscard]] static constexpr auto compute_matrix(const std::vector<weighted_segment_t> &segments,
            const std::vector<std::size_t> &largest_non_overlapping_smaller_index) -> std::vector<weight_t>
        {
            const auto segments_size = segments.size();

            // 0-th element will have 0 value.
            std::vector<weight_t> min_weights_matrix;
            min_weights_matrix.reserve(segments_size + 1U);
            min_weights_matrix.resize(1U);

            weight_t maxi{};
            std::size_t index{};

            do
            {
                const auto &smaller_index = largest_non_overlapping_smaller_index[index];

                assert(smaller_index <= index); // Since 1U has been added.

                const auto &prev_wei = min_weights_matrix.at(smaller_index);
                const auto &cur_segm = segments[index];

                const auto cand = static_cast<weight_t>(prev_wei + cur_segm.weight);
                assert(prev_wei < cand);

                maxi = std::max(maxi, cand);

                min_weights_matrix.push_back(maxi);
            } while (++index < segments_size);

            assert(segments_size + 1LLU == min_weights_matrix.size());

            return min_weights_matrix;
        }

        static constexpr void back_track_solution(const std::vector<weighted_segment_t> &segments,
            const std::vector<std::size_t> &largest_non_overlapping_smaller_index,
            const std::vector<weight_t> &min_weights_matrix, std::vector<std::size_t> &chosen_segment_indexes)
        {
            chosen_segment_indexes.clear();

            const auto segments_size = require_positive(segments.size(), "segments size");

            auto remaining_weight = min_weights_matrix.at(segments_size);
            require_positive(remaining_weight, "maximum weight");

            auto index = segments_size - 1U;

            do
            {
                const auto &skip_current_weight = min_weights_matrix.at(index);

                const auto &smaller_index = largest_non_overlapping_smaller_index.at(index);
                assert(smaller_index <= index); // Since 1U has been added.

                const auto &prev_wei = min_weights_matrix.at(smaller_index);
                const auto &segment = segments[index];
                const auto select_current_weight = static_cast<weight_t>(prev_wei + segment.weight);
                assert(prev_wei < select_current_weight);

                if (select_current_weight < skip_current_weight)
                {
                    continue;
                }

                chosen_segment_indexes.push_back(segment.Id);

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    if (remaining_weight < segment.weight) [[unlikely]]
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << "Remaining weight " << remaining_weight << " < " << segment.weight << " segments["
                            << index << "] weight.";

                        throw std::runtime_error(str.str());
                    }

                    remaining_weight -= segment.weight;
                }

                index = smaller_index;
            } while (0U < index--);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (weight_t{} != remaining_weight) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Non-zero last remaining weight " << remaining_weight << ".";

                    throw std::runtime_error(str.str());
                }
            }
        }
    };
} // namespace Standard::Algorithms::Numbers
