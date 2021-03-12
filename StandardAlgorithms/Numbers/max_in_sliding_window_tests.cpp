#include"max_in_sliding_window_tests.h"
#include"../Utilities/ert.h"
#include"max_in_sliding_window.h"

void Standard::Algorithms::Numbers::Tests::max_in_sliding_window_tests()
{
    using data_type = std::int16_t;

    constexpr std::size_t width = 3;

    const std::vector<data_type> data{// NOLINTNEXTLINE
        9, 5, 4, 7, 6, 9, 8, 4, 5, 7
    };

    require_less_equal(width, data.size(), "width");

    const std::vector<data_type> expected{// NOLINTNEXTLINE
        9, 7, 7, 9, 9, 9, 8, 7
    };

    ::Standard::Algorithms::ert::are_equal(expected.size(), data.size() - width + 1U, "expectedSize");

    std::vector<data_type> actual;
    max_in_sliding_window(data, width, actual);

    ::Standard::Algorithms::ert::are_equal(expected, actual, "max_in_sliding_window");
}
