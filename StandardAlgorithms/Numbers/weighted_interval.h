#pragma once
#include"../Utilities/w_stream.h"
#include<compare>
#include<cstddef>
#include<stdexcept>

namespace Standard::Algorithms::Numbers
{
    template<class distance_t, class weight_t, class id_t = std::size_t>
    struct weighted_interval final
    {
        [[nodiscard]] constexpr auto operator<=> (const weighted_interval &) const noexcept = default;

        constexpr void validate() const
        {
            if (Stop < Start) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The " << Id << " interval start " << Start << " cannot exceed stop " << Stop << ".";

                throw std::runtime_error(str.str());
            }

            if (constexpr weight_t zero{}; weight <= zero) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The " << Id << " interval [" << Start << ", " << Stop << "] weight (" << weight
                    << ") must be positive.";

                throw std::runtime_error(str.str());
            }
        }

        // NOLINTNEXTLINE
        distance_t Start{};

        // NOLINTNEXTLINE
        distance_t Stop{};

        // NOLINTNEXTLINE
        weight_t weight{};

        // NOLINTNEXTLINE
        id_t Id{};
    };

    template<class distance_t, class weight_t, class id_t>
    auto operator<< (std::ostream &str, const weighted_interval<distance_t, weight_t, id_t> &interval) -> std::ostream &
    {
        str << "Start " << interval.Start << ", Stop " << interval.Stop << ", weight " << interval.weight << ", Id "
            << interval.Id;

        return str;
    }

    /* // todo(p3): del?
        // Here the "Start" is implicit - it is the previous Stop.
        template<class distance_t, class weight_t>
        struct FinishWeightInterval final
        {
            // NOLINTNEXTLINE
            distance_t Finish{};

            // NOLINTNEXTLINE
            weight_t weight{};

            [[nodiscard]] constexpr auto operator<=> (const FinishWeightInterval&) const noexcept = default;
        };

        //The [7, 7] interval has 1 item, [5,7] - 3 items.
        template<class Interval>
        [[nodiscard]] constexpr auto items_in_interval(const Interval& interval) noexcept
        {
            auto result = interval.Stop - interval.Start + 1;
            return result;
        }

        template<class Interval>
        [[nodiscard]] constexpr auto is_too_wide_interval(const Interval& interval) noexcept -> bool
        {
            const auto count = items_in_interval(interval);
            auto result = count < interval.weight;
            return result;
        }

        template<class Interval>
        [[nodiscard]] constexpr auto shall_include_all_interval_items(const Interval& interval) noexcept -> bool
        {
            const auto count = items_in_interval(interval);
            auto result = count == interval.weight;
            return result;
        }

        template<class distance_t, class weight_t, class Interval>
        [[nodiscard]] constexpr auto are_enough_remaining_intervals(const distance_t& rightMostPosition,
                const weight_t& maxWeight,
                const weight_t& currentWeightSum,
                const Interval& interval) noexcept -> bool
        {
            const auto canDo = rightMostPosition - interval.Stop;
            const auto mustBe = maxWeight - currentWeightSum;
            auto result = mustBe <= canDo;
            return result;
        }

        // Split the [Stop] intervals (a.k.a. regions or segments)
        // into [Start, Stop].
        // It is assumed there are no duplicates in the input.
        // Weights must not decrease.
        // Invalid intervals - return nullopt.
        template<class distance_t, class weight_t,
            class TInputIterator,
            class TInput = FinishWeightInterval<distance_t, weight_t>,
            class TOutput = weighted_interval<distance_t, weight_t>>
        [[nodiscard]]  constexpr auto to_full_intervals(TInputIterator inputBegin,
            TInputIterator inputEnd,
            // The beginning of all intervals.
            const distance_t &startValue = {},
            const bool shallJoinZeroWeightIntervals = true)
        -> std::optional<std::vector<TOutput>>
        {
            std::vector<TOutput> intervals;
            TInput previous{};

            for (auto hasPrevious = false; inputBegin != inputEnd; ++inputBegin)
            {
                const auto& current = *inputBegin;

                if (hasPrevious)
                {
                    assert(!intervals.empty());

                    if (previous.Stop == current.Stop
                        || current.weight < previous.weight)
                    {
                        return std::nullopt;
                    }

                    // Here: prev.Stop < curr.Stop
                    // prev.weight <= curr.weight

                    if (shallJoinZeroWeightIntervals && weight_t{} == current.weight)
                    {// Join with the previous.
                        intervals[0] = TOutput{startValue, current.Stop, current.weight};
                    }
                    else
                    {
                        const auto delta = static_cast<weight_t>(current.weight - previous.weight);

                        auto& lastInterval = intervals.back();

                        if (const auto shallMerge = shallJoinZeroWeightIntervals &&
                            weight_t{} == delta && weight_t{} == lastInterval.weight;
                            shallMerge)
                        {
                            lastInterval = TOutput{lastInterval.Start, current.Stop, delta};
                        }
                        else
                        {
                            // todo(p4): emplace_back()
                            intervals.push_back(TOutput{previous.Stop + 1, current.Stop, delta});
                        }
                    }
                }
                else
                {// The first interval.
                    hasPrevious = true;

                    // todo(p4): emplace_back()
                    intervals.push_back(TOutput{startValue, current.Stop, current.weight});
                }

                previous = current;
            }

            return intervals;
        }
        */
} // namespace Standard::Algorithms::Numbers
