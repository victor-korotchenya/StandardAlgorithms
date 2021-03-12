#pragma once

#include <sstream>
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template <typename Distance, typename TWeight>
            struct WeightedInterval
            {
                Distance Start;
                Distance Stop;
                TWeight Weight;

                WeightedInterval(
                    const Distance& start = {},
                    const Distance& finish = {},
                    const TWeight& weight = {})
                    : Start(start), Stop(finish), Weight(weight)
                {
                }

                virtual ~WeightedInterval() = default;

                WeightedInterval(const WeightedInterval&) = default;

                virtual WeightedInterval& operator =(const WeightedInterval& a) noexcept(false)
#if (1 == 2)
                    noexcept(false) = default;//It does not work.
#else
                {
                    //Problem - had to implement explicitly. TODO: can use default?
                    Start = a.Start;
                    Stop = a.Stop;
                    Weight = a.Weight;

                    return *this;
                }
#endif

                WeightedInterval(WeightedInterval&&) = default;

                virtual WeightedInterval& operator =(WeightedInterval&& a) noexcept(false)
#if (1 == 2)
                    noexcept(false) = default;//It does not work.
#else
                {
                    return this->operator =(static_cast<const WeightedInterval&>(a));
                }
#endif

                virtual bool operator <(const WeightedInterval& b) const
                {
                    auto result = Start < b.Start
                        || Start == b.Start && (Stop < b.Stop
                            || Stop == b.Stop && Weight < b.Weight);
                    return result;
                }

                virtual void Validate() const
                {
                    if (Stop < Start)
                    {
                        std::ostringstream ss;
                        ss << "Stop (" << Stop
                            << ") is smaller than Start (" << Start
                            << "), Weight=" << Weight
                            << ".";
                        StreamUtilities::throw_exception(ss);
                    }
                }
            };

            //Here the "Start" is implicit - it is the previous Stop.
            template <typename Distance = int,
                typename TWeight = int>
                struct FinishWeightInterval final
            {
                Distance Finish;
                TWeight Weight;

                FinishWeightInterval(Distance finish = {}, TWeight weight = {})
                    : Finish(finish), Weight(weight)
                {
                }

                bool operator <(const FinishWeightInterval& b) const
                {
                    auto result = Finish < b.Finish
                        || Finish == b.Finish && Weight < b.Weight;
                    return result;
                }
            };

            //The [7, 7] interval has 1 item, [5,7] - 3 items.
            template<typename Interval>
            inline auto items_in_interval(const Interval& interval)
            {
                const auto result = interval.Stop - interval.Start + 1;
                return result;
            }

            template<typename Interval>
            inline bool is_too_wide_interval(const Interval& interval)
            {
                const auto canDo = items_in_interval(interval);
                const auto result = canDo < interval.Weight;
                return result;
            }

            template<typename Interval>
            inline bool shall_include_all_interval_items(const Interval& interval)
            {
                const auto canDo = items_in_interval(interval);
                const auto result = canDo == interval.Weight;
                return result;
            }

            template<typename Distance,
                typename TWeight,
                typename Interval>
                inline bool is_enough_remaining_intervals(
                    const Distance& rightMostPosition,
                    const TWeight& maxWeight,
                    const TWeight& currentWeightSum,
                    const Interval& interval)
            {
                const auto canDo = rightMostPosition - interval.Stop;
                const auto mustBe = maxWeight - currentWeightSum;
                const auto result = mustBe <= canDo;
                return result;
            }
        }
    }
}