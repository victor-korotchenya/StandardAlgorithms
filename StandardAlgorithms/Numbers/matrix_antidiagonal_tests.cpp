#include"matrix_antidiagonal_tests.h"
#include"../Numbers/matrix_antidiagonal.h"
#include"../Utilities/test_utilities.h"

namespace
{
    using item_t = std::int32_t;
    using matan_t = Standard::Algorithms::Numbers::matrix_antidiagonal<item_t>;
    using length_t = typename matan_t::length_t;
    using row_column_t = std::pair<length_t, length_t>;

    template<length_t rows, length_t columns, length_t antidiagonals>
    requires(0U < rows && 0U < columns && 0U < antidiagonals)
    constexpr void test_antidiagonal_pos_impl(
        const std::string &name, const std::vector<std::vector<row_column_t>> &expecteds)
    {
        ::Standard::Algorithms::ert::are_equal(rows, expecteds.size(), name + " expecteds size");

        const matan_t matan(rows, columns);

        ::Standard::Algorithms::ert::are_equal(antidiagonals, matan.data().size(), name + " anti-diagonals");

        for (length_t row{}; const auto &sub_expecteds : expecteds)
        {
            const auto sub_name = name + " sub-expecteds at " + std::to_string(row);

            ::Standard::Algorithms::ert::are_equal(columns, sub_expecteds.size(), sub_name + " size");

            for (length_t column{}; const auto &expected : sub_expecteds)
            {
                const auto actual = matan.to_antidiagonal_pos(row, column);

                ::Standard::Algorithms::ert::are_equal(
                    expected, actual, sub_name + " to_antidiagonal_pos column " + std::to_string(column));

                ++column;
            }

            ++row;
        }
    }

    template<length_t rows, length_t columns, length_t antidiagonals>
    requires(0U < rows && 0U < columns && 0U < antidiagonals)
    constexpr void test_antidiagonal_sizes(const std::string &name, const std::vector<length_t> &expecteds)
    {
        ::Standard::Algorithms::ert::are_equal(antidiagonals, expecteds.size(), name + " expecteds size");

        const matan_t matan(rows, columns);

        ::Standard::Algorithms::ert::are_equal(antidiagonals, matan.data().size(), name + " anti-diagonals");

        for (length_t ant{}; ant < antidiagonals; ++ant)
        {
            const auto &expected = expecteds.at(ant);
            const auto actual = matan.compute_antidiagonal_size(ant);

            ::Standard::Algorithms::ert::are_equal(expected, actual, name + ", ant " + std::to_string(ant));
        }
    }

    constexpr void test_antidiagonal_pos_size()
    {
        constexpr auto antidiagonals = 10U;

        const std::vector<length_t> antidiagonal_sizes{// NOLINTNEXTLINE
            1, 2, 3, 3, 3, 3, 3, 3, 2, 1
        };

        {
            constexpr auto rows = 3U;
            constexpr auto columns = 8U;
            const std::string name = "3*8";

            // 0 1 2 3 4 5 6 7
            // 1 2 3 4 5 6 7 8
            // 2 3 4 5 6 7 8 9
            const std::vector<std::vector<row_column_t>> antidiagonal_positions{// row 0: // NOLINTNEXTLINE
                { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 0 }, { 6, 0 }, { 7, 0 } },
                // row 1: // NOLINTNEXTLINE
                { { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 }, { 8, 0 } },
                // row 2: // NOLINTNEXTLINE
                { { 2, 2 }, { 3, 2 }, { 4, 2 }, { 5, 2 }, { 6, 2 }, { 7, 2 }, { 8, 1 }, { 9, 0 } }
            };

            test_antidiagonal_pos_impl<rows, columns, antidiagonals>(name, antidiagonal_positions);

            test_antidiagonal_sizes<rows, columns, antidiagonals>(name, antidiagonal_sizes);
        }
        {
            constexpr auto rows = 8U;
            constexpr auto columns = 3U;
            const std::string name = "8*3";

            // 0 1 2
            // 1 2 3
            // 2 3 4
            // 3 4 5
            // 4 5 6
            // 5 6 7
            // 6 7 8
            // 7 8 9
            const std::vector<std::vector<row_column_t>> antidiagonal_positions{// row 0: // NOLINTNEXTLINE
                { { 0, 0 }, { 1, 0 }, { 2, 0 } },
                // row 1: // NOLINTNEXTLINE
                { { 1, 1 }, { 2, 1 }, { 3, 0 } },
                // row 2: // NOLINTNEXTLINE
                { { 2, 2 }, { 3, 1 }, { 4, 0 } },
                // row 3: // NOLINTNEXTLINE
                { { 3, 2 }, { 4, 1 }, { 5, 0 } },
                // row 4: // NOLINTNEXTLINE
                { { 4, 2 }, { 5, 1 }, { 6, 0 } },
                // row 5: // NOLINTNEXTLINE
                { { 5, 2 }, { 6, 1 }, { 7, 0 } },
                // row 6: // NOLINTNEXTLINE
                { { 6, 2 }, { 7, 1 }, { 8, 0 } },
                // row 7: // NOLINTNEXTLINE
                { { 7, 2 }, { 8, 1 }, { 9, 0 } }
            };

            test_antidiagonal_pos_impl<rows, columns, antidiagonals>(name, antidiagonal_positions);

            test_antidiagonal_sizes<rows, columns, antidiagonals>(name, antidiagonal_sizes);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::matrix_antidiagonal_tests()
{
    test_antidiagonal_pos_size();
}
