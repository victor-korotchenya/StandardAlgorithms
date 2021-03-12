#pragma once
#include"../Utilities/has_openmp.h"
#include"../Utilities/require_matrix.h"
#include"../Utilities/require_utilities.h"
#include"greatest_rectangle_in_histogram.h"
#include"sort.h" // min3

namespace Standard::Algorithms::Numbers
{
    // 2D boolean matrix data[m][n], not necessary squared, is given.
    // 1) Find a max square sub-matrix having all true values.
    // 2) Find a max area sub-matrix with only true values.
    template<class point_t, class rectangle_t>
    // todo(p4): make it non-template?
    requires(sizeof(std::size_t) <= sizeof(typename point_t::distance_t) &&
        sizeof(typename point_t::distance_t) <= sizeof(typename rectangle_t::distance_t))
    struct max_sub_matrix_in_2d_matrix final
    {
        max_sub_matrix_in_2d_matrix() = delete;

        [[nodiscard]] static constexpr auto max_area(const std::vector<std::vector<bool>> &data) -> rectangle_t
        {
            const auto size = require_positive(data.size(), "size");
            const auto row_size = require_positive(data[0].size(), "data[0].size");
            require_matrix(data);

            std::vector<std::size_t> currents(row_size);
            rectangle_t result{};
            std::size_t max_area{};

            for (std::size_t row{}; row < size; ++row)
            {
                const auto &data_row = data[row];

                for (std::size_t column{}; column < row_size; ++column)
                {
                    auto &cur = currents[column];

                    cur = data_row[column] ? cur + 1ZU : 0ZU;
                }

                constexpr auto shall_compute_start_index = true;

                std::size_t start_index{};
                std::size_t start_height{};
                std::size_t start_width{};

                const auto temp_area = greatest_rectangle_in_histogram<std::size_t, std::size_t>::compute_fast(
                    currents, start_index, start_height, start_width, shall_compute_start_index);

                if (!(max_area < temp_area))
                {
                    continue;
                }

                assert(start_height <= row + 1ZU && 0U < start_width);

                max_area = temp_area;

                point_t low{ row + 1ZU - start_height, start_index };
                point_t high{ row + 1ZU, start_index + start_width };

                result = { low, high };
            }

            return result;
        }

        /* // todo(p3): fix this incorrect code.
        [[nodiscard]]  constexpr static auto MaxArea___Wrong_to_be_fixed(const std::vector<std::vector<bool>>& data) ->
        rectangle_t
        {
            const auto size = require_positive(data.size(), "size");
            const auto rowSize = require_positive(data[0].size(), "data[0].size");
            require_matrix(data);

            const auto rowSizePlusOne = rowSize + 1ZU;

            // One can go left (first) or go up (second).
            using TLeftUpPair = std::pair<rectangle_t, rectangle_t>;

            constexpr TLeftUpPair zeroPair{};

            rectangle_t result{};
            std::size_t maxArea{};

            // Index starts with 1.
            std::vector<TLeftUpPair> previousMatrix(rowSizePlusOne);
            std::vector<TLeftUpPair> currentMatrix(rowSizePlusOne);

            for (std::size_t row{}; row < size; ++row)
            {
                const auto& dataRow = data[row];

                for (std::size_t column{}; column < rowSize; ++column)
                {
                    if (!dataRow[column])
                    {
                        currentMatrix[column + 1ZU] = zeroPair;
                        continue;
                    }

                    const auto& leftRectangle = currentMatrix[column].first;
                    const auto leftHeight = leftRectangle.Height();
                    const auto leftWidth = leftRectangle.Width();

                    const auto& upRectangle = previousMatrix[column + 1ZU].second;
                    const auto upHeight = upRectangle.Height();
                    const auto upWidth = upRectangle.Width();

                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                    if ((0U == leftHeight && 0U != leftWidth)
                        || (0U != leftHeight && 0U == leftWidth))
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << "Both must be 0, but leftHeight " << leftHeight
                            << ", leftWidth " << leftWidth << ".";

                        throw_exception(str);
                    }

                    if ((0U == upHeight && 0U != upWidth)
                        || (0U != upHeight && 0U == upWidth))
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << "Both must be 0, but upHeight " << upHeight
                            << ", upWidth " << upWidth << ".";

                        throw_exception(str);
                    }
                    }

                    // Go left.
                    const auto height1 = 0U == leftHeight
                        ? upHeight + 1ZU
                        : std::min(leftHeight, upHeight + 1ZU);

                    const auto width1 = leftWidth + 1ZU;

                    const point_t lastPoint {row + 1ZU, column + 1ZU};

                    const rectangle_t bestLeft { point_t{row + 1ZU - height1, column + 1ZU - width1},
                        lastPoint};

                    if (const auto tempResult1 = height1 * width1;
                        maxArea < tempResult1)
                    {
                        maxArea = tempResult1;
                        result = bestLeft;
                    }

                    // Go up.
                    const auto height2 = upHeight + 1ZU;

                    const auto width2 = 0U == upWidth
                        ? leftWidth + 1ZU
                        : std::min(upWidth, leftWidth + 1ZU);

                    const rectangle_t bestUp{point_t{row + 1ZU - height2, column + 1ZU - width2},
                        lastPoint};

                    if (const auto tempResult2 = height2 * width2;
                        maxArea < tempResult2)
                    {
                        maxArea = tempResult2;
                        result = bestUp;
                    }

                    currentMatrix[column + 1ZU] = {bestLeft, bestUp};
                }

                std::swap(previousMatrix, currentMatrix);
            }

            return result;
        }*/

        // The returned matrix will be a square.
        [[nodiscard]] static constexpr auto max_square(const std::vector<std::vector<bool>> &data) -> rectangle_t
        {
            const auto size = require_positive(data.size(), "size");
            const auto row_size = require_positive(data[0].size(), "data[0].size");
            require_matrix(data);

            const auto row_size_plus_one = row_size + 1ZU;

            rectangle_t result{};
            std::size_t max_side{};

            // Index starts with 1.
            std::vector<std::size_t> previous_matrix(row_size_plus_one);
            std::vector<std::size_t> current_matrix(row_size_plus_one);

            for (std::size_t row{}; row < size; ++row)
            {
                const auto &data_row = data[row];

                for (std::size_t column{}; column < row_size; ++column)
                {
                    if (!data_row[column])
                    {
                        current_matrix[column + 1ZU] = 0;
                        continue;
                    }

                    // upLeft ; up
                    // left    ;   side
                    const auto &up_left = previous_matrix[column];
                    const auto &upp = previous_matrix[column + 1ZU];
                    const auto &left = current_matrix[column];

                    const auto side = min3(up_left, upp, left) + 1ZU;
                    current_matrix[column + 1ZU] = side;

                    if (!(max_side < side))
                    {
                        continue;
                    }

                    max_side = side;

                    result = { point_t{ row + 1ZU - side, column + 1ZU - side }, point_t{ row + 1ZU, column + 1ZU } };
                }

                std::swap(previous_matrix, current_matrix);
            }

            return result;
        }

        // Time O(n*n*m).
        // The returned matrix contains the maximum available sum.
        template<std::signed_integral int_t, std::signed_integral long_int_t>
        requires(sizeof(int_t) <= sizeof(long_int_t))
        [[nodiscard]] static constexpr auto max_sum(
            const std::vector<std::vector<int_t>> &data, rectangle_t &result_rectangle) -> long_int_t
        {
            require_positive(data.size(), "size");
            require_positive(data[0].size(), "data[0].size");
            require_matrix(data);

            // The first row consists entirely from zeros.
            std::vector<std::vector<long_int_t>> sums;

            // todo(p4): Having 100*2 matrix, it could be 50 times faster to sum by rows.
            {
                constexpr bool include_first_zero_row = true;

                sum_vertical_matrices(data, sums, include_first_zero_row);
            }

            auto result = compute_with_omp<long_int_t>(sums, result_rectangle);

            return result;
        }

        // Slow time O(n*n*m*m*m). Space O(1).
        template<std::signed_integral int_t, std::signed_integral long_int_t>
        requires(sizeof(int_t) <= sizeof(long_int_t))
        [[nodiscard]] static constexpr auto max_sum_slow(
            const std::vector<std::vector<int_t>> &data, rectangle_t &result_rectangle) -> long_int_t
        {
            const auto size = require_positive(data.size(), "size");
            const auto row_size = require_positive(data[0].size(), "data[0].size");
            require_matrix(data);

            result_rectangle = {};
            long_int_t result{};

            for (std::size_t start_column{}; start_column < row_size; ++start_column)
            {
                for (auto stop_column = start_column; stop_column < row_size; ++stop_column)
                {
                    for (std::size_t start_row{}; start_row < size; ++start_row)
                    {
                        long_int_t temp{};

                        for (auto stop_row = start_row; stop_row < size; ++stop_row)
                        {
                            for (auto col = start_column; col <= stop_column; ++col)
                            {
                                temp += data[stop_row][col];
                            }

                            if (!(result < temp))
                            {
                                continue;
                            }

                            result = temp;

                            result_rectangle = { point_t{ start_row, start_column },
                                point_t{ stop_row + 1ZU, stop_column + 1ZU } };
                        }
                    }
                }
            }

            return result;
        }

private:
        // Sum the numbers in vertical bars from (index, 0) to (index, j),
        // and place the result to sums[index][j].
        // The 'source' must have the same sub-matrix sizes.
        template<class int_t, class long_int_t>
        requires(sizeof(int_t) <= sizeof(long_int_t))
        static constexpr void sum_vertical_matrices(const std::vector<std::vector<int_t>> &source,
            std::vector<std::vector<long_int_t>> &sums, const bool include_first_zero_row = false)
        {
            const auto size = require_positive(source.size(), "size");
            const auto row_size = require_positive(source[0].size(), "source[0]size");

            const auto extra_one = include_first_zero_row ? 1ZU : 0ZU;
            sums.resize(size + extra_one);

            if (include_first_zero_row)
            {
                sums.at(0).assign(row_size, long_int_t{});
            }

            for (std::size_t row{}; row < size; ++row)
            {
                const auto current_index = row + extra_one;

                auto &sums_row = sums.at(current_index);
                sums_row.assign(row_size, long_int_t{});

                const auto is_first_row = 0U == row;

                const auto previous_index = is_first_row ? 0ZU : current_index - 1ZU;
                const auto &previou_sums_row = sums[previous_index];
                const auto &source_row = source[row];

                for (std::size_t column{}; column < row_size; ++column)
                {
                    sums_row[column] = is_first_row
                        ? source_row[column]
                        : static_cast<long_int_t>(source_row[column] + previou_sums_row[column]);
                }
            }
        }

        template<std::signed_integral long_int_t>
        [[nodiscard]] static auto compute_with_omp(
            const std::vector<std::vector<long_int_t>> &sums, rectangle_t &result_rectangle) -> long_int_t
        {
            // OMP in MSVC cannot handle unsigned integers, phew.
            using omp_signed_number_t = std::int64_t;

            const auto size = require_positive(static_cast<omp_signed_number_t>(sums.size())
                    // In the sums, the first row has zeros only.
                    - 1,
                "size");

            const auto row_size = require_positive(static_cast<omp_signed_number_t>(sums[0].size()), "sums[0].size");

            long_int_t result{};

            omp_signed_number_t xx1{};
            omp_signed_number_t xx2{};
            omp_signed_number_t yy1{};
            omp_signed_number_t yy2{};

            // Sharing "resultRectangle" or having a template type in OMP gives wrong results sometimes!
            // Solution: just use simple numbers as std::int32_t, std::int64_t.
#pragma omp parallel for default(none) shared(result, size, row_size, sums, xx1, xx2, yy1, yy2)
            for (omp_signed_number_t row_begin = 0; row_begin < size; ++row_begin)
            {
                const auto &previous_row = sums[row_begin];

                for (auto row_end = row_begin + 1; row_end <= size; ++row_end)
                {
                    const auto &end_row = sums[row_end];

                    long_int_t current_sum{};
                    omp_signed_number_t min_column{};

                    for (omp_signed_number_t column{}; column < row_size; ++column)
                    {
                        current_sum = static_cast<long_int_t>(current_sum + end_row[column] - previous_row[column]);

                        if (current_sum <= 0)
                        {// If got negative sum, then nullify it.
                            current_sum = 0;
                            min_column = column + 1;
                            continue;
                        }

                        if (result < current_sum)
                        {
#pragma omp critical
                            {
                                if (result < current_sum)
                                {
                                    // Do not uncomment!
                                    //
                                    // resultRectangle = {
                                    // point_t{static_cast<std::size_t>(xx1), static_cast<std::size_t>(yy1)},
                                    // point_t{static_cast<std::size_t>(xx2), static_cast<std::size_t>(yy2)}};
                                    //
                                    // #pragma omp flush(resultRectangle) - this is wrong.

                                    xx1 = row_begin;
                                    xx2 = row_end;

                                    yy1 = min_column;
                                    yy2 = column + 1;

                                    result = current_sum;
                                }
                            }
                        }
                    }
                }
            }

            assert(xx1 <= xx2 && yy1 <= yy2);

            result_rectangle = { point_t{ static_cast<std::size_t>(xx1), static_cast<std::size_t>(yy1) },
                point_t{ static_cast<std::size_t>(xx2), static_cast<std::size_t>(yy2) } };

            return result;
        }
    };
} // namespace Standard::Algorithms::Numbers
