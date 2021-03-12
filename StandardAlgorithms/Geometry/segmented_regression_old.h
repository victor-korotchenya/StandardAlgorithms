#pragma once
#include"segmented_regression.h"

namespace Standard::Algorithms::Geometry
{
    template<class int_t1, class long_int_t1, class point_t1>
    requires(sizeof(int_t1) <= sizeof(long_int_t1))
    struct segmented_regression_old final
    {
        using int_t = int_t1;
        using long_int_t = long_int_t1;
        using point_t = point_t1;

        segmented_regression_old() = delete;

        // The 'points' must be sorted in the strictly increasing order of x-coordinate.
        [[nodiscard]] static constexpr auto solve(
            // E.g. std::numeric_limits< double >::infinity()
            const long_int_t &infinity, const std::vector<point_t> &points,
            // The "segmentCost' must be non-negative
            const long_int_t segment_cost, std::vector<segment_result<long_int_t>> &segments) -> long_int_t
        {
            const auto size = points.size();
            if (constexpr auto size_min = 2U; size < size_min) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "There must be at least " << size_min << " points, now having " << size << ".";
                throw_exception(str);
            }

            constexpr long_int_t zero{};
            if (segment_cost < zero) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The segment cost (" << segment_cost << ") must be non-negative.";
                throw_exception(str);
            }

            check_points(points);

            segmented_regression_session<long_int_t> session;
            initialize_session<long_int_t>(session, size);

            std::vector<std::vector<segment_info<long_int_t>>> infos(size, std::vector<segment_info<long_int_t>>(size));

            compute_coefficients(infinity, points, infos);

            auto result = find_optimal(size, segment_cost, infos, session);

            restore_solution(size, infos, session, segments);

            return result;
        }

private:
        static constexpr void check_points(const std::vector<point_t> &points)
        {
            const auto size = points.size();

            auto last = points.at(0).x;
            constexpr std::size_t skip_first = 1;

            for (auto index = skip_first; index < size; ++index)
            {
                const auto &point = points[index];

                if (points[index].x <= last) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Point (" << point << ") at [" << index
                        << "] must have x-coordinate, greater than previous value " << last << ".";

                    throw_exception(str);
                }

                last = points[index].x;
            }
        }

        static constexpr void compute_coefficients(const long_int_t &infinity, const std::vector<point_t> &points,
            std::vector<std::vector<segment_info<long_int_t>>> &infos)
        {
            constexpr std::size_t one = 1;

            const auto size = points.size();
            const auto size_plus_one = Standard::Algorithms::Numbers::add_unsigned(one, size);

            std::vector<long_int_t> total_sum_x(size_plus_one);
            std::vector<long_int_t> total_sum_y(size_plus_one);
            std::vector<long_int_t> total_sum_xx(size_plus_one);
            std::vector<long_int_t> total_sum_xy(size_plus_one);

            constexpr long_int_t zero{};
            total_sum_x[0] = zero;
            total_sum_y[0] = zero;
            total_sum_xy[0] = zero;
            total_sum_xx[0] = zero;

            for (std::size_t index{}; index < size; ++index)
            {
                const auto &point = points[index];
                const auto foll = index + 1ZU;
                total_sum_x[foll] = total_sum_x[index] + point.x;
                total_sum_y[foll] = total_sum_y[index] + point.y;

                total_sum_xx[foll] = total_sum_xx[index] + point.x * point.x;
                total_sum_xy[foll] = total_sum_xy[index] + point.x * point.y;

                auto &info = infos[index];

                for (std::size_t ind2{}; ind2 <= index; ++ind2)
                {
                    // Use "long_int_t" type because it is to be cast to long_int_t.
                    const auto segment_length = static_cast<long_int_t>(foll - ind2);

                    const auto sum_x = static_cast<long_int_t>(total_sum_x[foll] - total_sum_x[ind2]);
                    const auto sum_y = static_cast<long_int_t>(total_sum_y[foll] - total_sum_y[ind2]);

                    const auto sum_xx = static_cast<long_int_t>(total_sum_xx[foll] - total_sum_xx[ind2]);
                    const auto sum_xy = static_cast<long_int_t>(total_sum_xy[foll] - total_sum_xy[ind2]);

                    const auto numerator = static_cast<long_int_t>(segment_length * sum_xy - sum_x * sum_y);

                    if (zero == numerator)
                    {
                        info[ind2].slope = zero;
                    }
                    else
                    {
                        const auto denominator = static_cast<long_int_t>(segment_length * sum_xx - sum_x * sum_x);

                        info[ind2].slope =
                            zero == denominator ? infinity : static_cast<long_int_t>(numerator / denominator);
                    }

                    info[ind2].intercept = static_cast<long_int_t>((sum_y - info[ind2].slope * sum_x) / segment_length);

                    auto error_temp = zero;

                    for (auto ind3 = ind2; ind3 <= index; ++ind3)
                    {
                        const auto &point_k = points[ind3];

                        const auto tmp =
                            static_cast<long_int_t>(point_k.y - info[ind2].slope * point_k.x - info[ind2].intercept);

                        error_temp += static_cast<long_int_t>(tmp * tmp);
                    }

                    info[ind2].error = error_temp;
                }
            }
        }

        [[nodiscard]] static constexpr auto find_optimal(const std::size_t size, const long_int_t segment_cost,
            const std::vector<std::vector<segment_info<long_int_t>>> &infos,
            segmented_regression_session<long_int_t> &session) -> long_int_t
        {
            auto &optimals = session.optimals;
            auto &optimal_indexes = session.optimal_indexes;

            optimals[0] = long_int_t{};

            std::size_t index{};

            do
            {
                const auto &info = infos[index];

                std::size_t best_index{};
                auto min_value = info[0].error;
                std::size_t ind2{};

                while (++ind2 <= index)
                {
                    const auto value = static_cast<long_int_t>(info[ind2].error + optimals[ind2]);
                    if (value < min_value)
                    {
                        best_index = ind2;
                        min_value = value;
                    }
                }

                optimal_indexes[index] = best_index;
                optimals[++index] = static_cast<long_int_t>(min_value + segment_cost);
            } while (index < size);

            const auto &result = optimals[size];
            return result;
        }

        static constexpr void restore_solution(const std::size_t size,
            const std::vector<std::vector<segment_info<long_int_t>>> &infos,
            const segmented_regression_session<long_int_t> &session, std::vector<segment_result<long_int_t>> &segments)
        {
            segments.clear();

            auto next = size - 1ZU;
            auto current = session.optimal_indexes.at(next);

            for (;;)
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    if (0U == next) [[unlikely]]
                    {
                        throw std::runtime_error("0 == next");
                    }

                    if (next <= current) [[unlikely]]
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << "next(" << next << ") <= current(" << current << ").";
                        throw_exception(str);
                    }
                }

                const auto &info = infos[next][current];

                // todo(p4): Clang bug?                 segments.emplace_back({ info.slope, info.intercept, info.error},
                // current, next);
                segments.push_back(segment_result<long_int_t>{
                    segment_info<long_int_t>{ info.slope, info.intercept, info.error }, current, next });

                if (0U == current)
                {
                    std::reverse(segments.begin(), segments.end());
                    return;
                }

                next = current - 1ZU;
                current = session.optimal_indexes[next];
            }
        }
    };
} // namespace Standard::Algorithms::Geometry
