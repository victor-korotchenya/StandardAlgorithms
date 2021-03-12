#include"greatest_rectangle_in_histogram_tests.h"
#include"../Utilities/ert.h"
#include"greatest_rectangle_in_histogram.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    using algorithm_t = Standard::Algorithms::Numbers::greatest_rectangle_in_histogram<int_t, long_int_t>;

    constexpr auto shall_compute_start_index = true;

    constexpr void run_subtest(const std::string &prefix, const std::vector<int_t> &heights,
        const int_t &expected_start_height, const std::pair<std::size_t, std::size_t> &expected_start_index_width,
        const long_int_t &expected_area)
    {
        for (const auto is_fast : {
                 true,
                 false,
             })
        {
            const auto name = prefix + (is_fast ? "Fast " : "Slow ");

            // No constant
            // NOLINTNEXTLINE
            std::size_t startIndex{};

            // No constant
            // NOLINTNEXTLINE
            int_t startHeight{};

            // No constant
            // NOLINTNEXTLINE
            std::size_t startWidth{};

            const auto actual_area = (is_fast ? algorithm_t::compute_fast : algorithm_t::compute_slow)(
                heights, startIndex, startHeight, startWidth, shall_compute_start_index);

            ::Standard::Algorithms::ert::are_equal(expected_start_index_width.first, startIndex, name + "index");
            ::Standard::Algorithms::ert::are_equal(expected_start_height, startHeight, name + "height");
            ::Standard::Algorithms::ert::are_equal(expected_start_index_width.second, startWidth, name + "width");
            ::Standard::Algorithms::ert::are_equal(expected_area, actual_area, name + "area");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::greatest_rectangle_in_histogram_tests()
{
    {
        constexpr int_t expected_start_height = 9;
        constexpr std::size_t expected_start_index = 2;
        constexpr std::size_t expected_width = 2;
        constexpr auto expected_area =
            static_cast<long_int_t>(static_cast<long_int_t>(expected_start_height) * expected_width);

        const std::vector<int_t> heights{// NOLINTNEXTLINE
            5, 4, expected_start_height, expected_start_height + 1, 2, 7, 7
        };

        run_subtest(
            "Simple 1 ", heights, expected_start_height, { expected_start_index, expected_width }, expected_area);
    }
    {
        constexpr int_t size = 10;

        std::vector<int_t> heights;

        for (int_t index{}; index < size - 1; ++index)
        {
            heights.push_back(size - index);
        }

        heights.push_back(size);

        constexpr std::size_t expected_start_index{};
        constexpr int_t expected_start_height = 6;
        constexpr std::size_t expected_width = 5;

        constexpr auto expected_area =
            static_cast<long_int_t>(static_cast<long_int_t>(expected_start_height) * expected_width);

        run_subtest(
            "Simple 2 ", heights, expected_start_height, { expected_start_index, expected_width }, expected_area);
    }
}
