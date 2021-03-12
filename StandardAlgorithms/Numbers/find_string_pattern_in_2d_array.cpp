#include"find_string_pattern_in_2d_array.h"
#include"../Utilities/require_matrix.h"
#include"arithmetic.h"
#include<initializer_list>
#include<unordered_set>

namespace
{
    using distance_t = std::size_t;
    using point_t = Standard::Algorithms::Geometry::point2d<distance_t>;

    using used_points_t = std::unordered_set<point_t>;

    struct context_t final
    {
        const distance_t size{};
        const std::vector<std::string> &text;
        const std::string &pattern;
        used_points_t &used_points;
        distance_t index{};
    };

    void mark_boarders(context_t &context)
    {
        Standard::Algorithms::require_positive(context.size, "context size");

        Standard::Algorithms::require_positive(context.size + 2U, "context size + 2");

        // Assume size = 5.
        // In order to easy handle the boundaries,
        // these rows/columns will be marked as "always used": 0, 6.
        const auto last_row_column = Standard::Algorithms::require_positive(context.size + 1U, "last row and column");

        for (distance_t index{}; index <= last_row_column; ++index)
        {
            context.used_points.insert(std::initializer_list<point_t>{
                { 0U, index }, { last_row_column, index }, { index, 0U }, { index, last_row_column } });
        }
    }

    // The row and column both start from 1 in the first call method.
    [[nodiscard]] auto find_helper(context_t &context, const distance_t row, const distance_t column) -> bool
    {
        const point_t point1{ row, column };

        if (context.used_points.contains(point1))
        {
            return false;
        }

        assert(0U < row && 0U < column && context.index < context.pattern.size());

        const auto &cha = context.pattern.at(context.index);

        if (cha != context.text.at(row - 1U).at(column - 1U))
        {
            return false;
        }

        context.used_points.insert(point1);

        if (++context.index == context.size)
        {// The last symbol has been matched.
            return true;
        }

        // Try to match a neighbor.
        if (find_helper(context, row - 1U, column - 1U) ||
            // todo(p4): use a stack.
            find_helper(context, row - 1U, column) || find_helper(context, row - 1U, column + 1U) ||
            //
            find_helper(context, row, column - 1U) || find_helper(context, row, column + 1U) ||
            //
            find_helper(context, row + 1U, column - 1U) || find_helper(context, row + 1U, column) ||
            find_helper(context, row + 1U, column + 1U))
        {
            return true;
        }

        context.used_points.erase(point1);

        assert(0U < context.index);

        --context.index;

        return false;
    }

    void fill_found_points(const distance_t size, const used_points_t &used_points, const distance_t pattern_size,
        std::vector<point_t> &points)
    {
        Standard::Algorithms::require_positive(pattern_size, "pattern size");

        points.resize(pattern_size);

        distance_t count{};

        for (const auto &point1 : used_points)
        {
            constexpr distance_t zero{};
            constexpr distance_t one = 1;

            if (zero < point1.x && zero < point1.y && point1.x <= size && point1.y <= size)
            {
                points[count] = point_t{ point1.x - one, point1.y - one };

                if (++count == pattern_size)
                {
                    return;
                }
            }
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "Internal error: only " << count << " out of " << pattern_size << " points have been found.";

        Standard::Algorithms::throw_exception(str);
    }
} // namespace

// todo(p4): Fast. If all pattern chars are the same.

[[nodiscard]] auto Standard::Algorithms::Numbers::find_string_pattern_in_2d_array_slow(
    const std::vector<std::string> &text, const std::string &pattern, std::vector<point_t> &points) -> bool
{
    const auto size = require_positive(text.size(), "text size");

    const auto pattern_size = require_positive(pattern.size(), "pattern size");

    points.clear();

    if (const auto size_squared = multiply_unsigned<std::uint64_t>(size, size); size_squared < pattern_size)
    {
        return false;
    }

    {
        constexpr auto square = true;
        constexpr check_matrix_options options{ square };

        require_matrix(text, options);
    }

    const auto used_points_size_max_estimate = (size << 2U) + pattern_size;

    used_points_t used_points(used_points_size_max_estimate);

    context_t context{ size, text, pattern, used_points };

    mark_boarders(context);

    for (distance_t row = 1; row <= context.size; ++row)
    {
        for (distance_t column = 1; column <= context.size; ++column)
        {
            if (find_helper(context, row, column))
            {
                fill_found_points(size, used_points, pattern_size, points);

                return true;
            }
        }
    }

    return false;
}
