#pragma once
#include"../Numbers/arithmetic.h"
#include"../Utilities/throw_exception.h"
#include"point.h"
#include<vector>

namespace Standard::Algorithms::Geometry
{
    template<class int_t1>
    struct segment_info final
    {
        using int_t = int_t1;

        // NOLINTNEXTLINE
        int_t slope{};

        // The y-intercept is the point where the line crosses the y axis (when x=0).
        // NOLINTNEXTLINE
        int_t intercept{};

        // NOLINTNEXTLINE
        int_t error{};

        // todo(p3): Use number_comparer in order to compare doubles with epsilon.
        [[nodiscard]] constexpr auto operator<=> (const segment_info &) const noexcept = default;
    };

    template<class int_t>
    auto operator<< (std::ostream &str, const segment_info<int_t> &seg) -> std::ostream &
    {
        str << "Slope " << seg.slope << ", intercept " << seg.intercept << ", error " << seg.error;
        return str;
    }

    template<class int_t1>
    struct segment_result final
    {
        using int_t = int_t1;

        // NOLINTNEXTLINE
        segment_info<int_t> info{};

        // First point number, zero based, from the points array.
        // NOLINTNEXTLINE
        std::size_t point1{};

        // Last point number.
        // NOLINTNEXTLINE
        std::size_t point2{};

        [[nodiscard]] constexpr auto operator<=> (const segment_result &) const noexcept = default;
    };

    template<class int_t>
    auto operator<< (std::ostream &str, const segment_result<int_t> &seg) -> std::ostream &
    {
        str << seg.info << ", points (" << seg.point1 << ", " << seg.point2 << ")";
        return str;
    }

    template<class int_t1>
    struct segmented_regression_session final
    {
        using int_t = int_t1;
        using absolute_value_function_t = int_t (*)(const int_t &);

        int_t infinity{};

        absolute_value_function_t absolute_value{};

        // These sums store 0 at [0].
        std::vector<int_t> x{};
        std::vector<int_t> y{};
        std::vector<int_t> xx{};
        std::vector<int_t> xy{};
        std::vector<int_t> yy{};

        // It does not store 0 at [0].
        std::vector<std::size_t> optimal_indexes{};

        // Stores 0 at [0].
        std::vector<int_t> optimals{};
    };

    template<class int_t>
    // todo(p4): use a ctor?
    constexpr void initialize_session(segmented_regression_session<int_t> &session, const std::size_t size)
    {
        constexpr std::size_t one = 1;

        const auto size_plus_one = Standard::Algorithms::Numbers::add_unsigned(one, size);

        session.x.resize(size_plus_one);
        session.y.resize(size_plus_one);
        session.xx.resize(size_plus_one);
        session.xy.resize(size_plus_one);
        session.yy.resize(size_plus_one);

        constexpr int_t zero{};
        session.x[0] = zero;
        session.y[0] = zero;
        session.xx[0] = zero;
        session.xy[0] = zero;
        session.yy[0] = zero;

        session.optimal_indexes.resize(size);
        session.optimals.resize(size_plus_one);
        session.optimals[0] = zero;
    }

    template<class int_t1, class long_int_t1, class point_t1,
        class absolute_value_function_t = typename segmented_regression_session<long_int_t1>::absolute_value_function_t>
    requires(sizeof(int_t1) <= sizeof(long_int_t1))
    struct segmented_regression final
    {
        using int_t = int_t1;
        using long_int_t = long_int_t1;
        using point_t = point_t1;

        segmented_regression() = delete;

        // The 'points' must be sorted in the strictly increasing order of x-coordinate.
        static constexpr auto solve(
            // E.g. std::numeric_limits< double >::infinity()
            const long_int_t &infinity,
            // E.g. std::abs
            absolute_value_function_t absolute_value_function, const std::vector<point_t> &points,
            // The "segmentCost' must be non-negative
            const long_int_t &segment_cost, std::vector<segment_result<long_int_t>> &segments) -> long_int_t
        {
            if (nullptr == absolute_value_function) [[unlikely]]
            {
                throw std::runtime_error("nullptr == absoluteValueFunction");
            }

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

            segmented_regression_session<long_int_t> session;
            session.infinity = infinity;
            session.absolute_value = absolute_value_function;
            initialize_session<long_int_t>(session, size);

            auto result = find_optimal(points, segment_cost, session);
            restore_solution(size, session, segments);

            return result;
        }

private:
        static constexpr auto find_optimal(const std::vector<point_t> &points, const long_int_t segment_cost,
            segmented_regression_session<long_int_t> &session) -> long_int_t
        {
            const auto size = points.size();
            auto &optimals = session.optimals;
            auto &optimal_indexes = session.optimal_indexes;

            int_t last = points[0].x;
            std::size_t next{};

            do
            {
                const auto &point = points[next];
                if (next != 0U)
                {
                    check_point_set_last(point, next, last);
                }

                compute_sums(point, next, session);

                const auto index_val = find_minimum(session, next);
                optimal_indexes[next] = index_val.first;
                optimals[++next] = index_val.second + segment_cost;
            } while (next < size);

            const auto &result = optimals[size];
            return result;
        }

        static constexpr void restore_solution(const std::size_t size,
            const segmented_regression_session<long_int_t> &session, std::vector<segment_result<long_int_t>> &segments)
        {
            segments.clear();

            const auto &optimal_indexes = session.optimal_indexes;
            auto next = size - 1U;
            auto current = optimal_indexes.at(next);

            for (segment_info<long_int_t> segment_info{};;)
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    if (0U == next) [[unlikely]]
                    {
                        throw std::runtime_error("0 == next");
                    }

                    if (next <= current)
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << "next(" << next << ") <= current(" << current << ").";
                        throw_exception(str);
                    }
                }

                compute_segment(session, current, segment_info, next);

                // todo(p4): Clang bug? segments.emplace_back(segmentInfo, current, next);
                segments.push_back({ segment_info, current, next });

                if (0U == current)
                {
                    std::reverse(segments.begin(), segments.end());
                    return;
                }

                next = current - 1U;
                current = optimal_indexes[next];
            }
        }

        static constexpr auto find_minimum(const segmented_regression_session<long_int_t> &session,
            const std::size_t next) -> std::pair<std::size_t, long_int_t>
        {
            const auto &optimals = session.optimals;

            segment_info<long_int_t> segment_info{};
            compute_segment(session, 0U, segment_info, next);

            std::size_t best_index{};
            auto min_value = segment_info.error;

            std::size_t current{};

            while (++current <= next)
            {
                if (optimals[current] < min_value) // Since error is non-negative, it should be faster.
                {// todo(p2): check with profiler whether "if" makes sense.
                    compute_segment(session, current, segment_info, next);

                    const auto value = static_cast<long_int_t>(segment_info.error + optimals[current]);
                    if (value < min_value)
                    {
                        best_index = current;
                        min_value = value;
                    }
                }
            }

            return { best_index, min_value };
        }

        inline static constexpr void check_point_set_last(const point_t &point, const std::size_t index, int_t &last)
        {
            if (point.x <= last) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Point (" << point << ") at [" << index
                    << "] must have x-coordinate, greater than previous value " << last << ".";

                throw_exception(str);
            }

            last = point.x;
        }

        inline static constexpr void compute_sums(
            const point_t &point, const std::size_t index, segmented_regression_session<long_int_t> &session)
        {
            const long_int_t &xxx = point.x;
            const long_int_t &yyy = point.y;

            const auto foll = index + 1U;

            session.x[foll] = static_cast<long_int_t>(session.x[index] + xxx);
            session.y[foll] = static_cast<long_int_t>(session.y[index] + yyy);

            session.xx[foll] = static_cast<long_int_t>(session.xx[index] + xxx * xxx);
            session.xy[foll] = static_cast<long_int_t>(session.xy[index] + xxx * yyy);
            session.yy[foll] = static_cast<long_int_t>(session.yy[index] + yyy * yyy);
        }

        inline static constexpr void compute_segment(const segmented_regression_session<long_int_t> &session,
            const std::size_t current, segment_info<long_int_t> &segment_info, const std::size_t next)
        {
            constexpr long_int_t zero{};

            const auto foll = next + 1U;

            // Sums from (current+1) to (next+1) inclusive.
            const auto sum_x = static_cast<long_int_t>(session.x[foll] - session.x[current]);
            const auto sum_y = static_cast<long_int_t>(session.y[foll] - session.y[current]);

            const auto sum_xx = static_cast<long_int_t>(session.xx[foll] - session.xx[current]);
            const auto sum_xy = static_cast<long_int_t>(session.xy[foll] - session.xy[current]);
            const auto sum_yy = static_cast<long_int_t>(session.yy[foll] - session.yy[current]);

            const auto segment_length = static_cast<long_int_t>(foll - current);
            const auto numerator = static_cast<long_int_t>(segment_length * sum_xy - sum_x * sum_y);

            long_int_t slope;
            if (zero == numerator)
            {
                slope = zero;
            }
            else
            {
                const auto denominator = static_cast<long_int_t>(segment_length * sum_xx - sum_x * sum_x);

                slope = zero == denominator ? session.infinity : static_cast<long_int_t>(numerator / denominator);
            }

            segment_info.slope = slope;

            const auto intercept_nominator = static_cast<long_int_t>(sum_y - slope * sum_x);
            const auto intercept = static_cast<long_int_t>(intercept_nominator / segment_length);

            segment_info.intercept = intercept;

            // For all k=[current..next]
            // Take the sum of squares of these for k-th point at (X[k], Y[k]):
            //   slope[next, current] * X[k] + intercept[next, current] - Y[k].

            // The expanded sum (A == slope, B== intercept) is:
            //   A*A*X*X + B*B + Y*Y
            //      + 2AXB - 2AXY - 2BY.
            //
            //  These sums are required:
            //  X*X, Y*Y, X, X*Y, Y

            constexpr long_int_t two{ 2 };

            const auto raw_error = static_cast<long_int_t>(slope * slope * (sum_xx) + intercept * intercept_nominator +
                sum_yy + two * (slope * (intercept * sum_x - sum_xy) - intercept * sum_y));

            // Sometimes the error can be negative due to rounding.
            segment_info.error = session.absolute_value(raw_error);
        }
    };
} // namespace Standard::Algorithms::Geometry
