#include"span_tests.h"
#include"../Utilities/ert.h"
#include"span.h"

namespace
{
    using int_t = std::int32_t;
} // namespace

void Standard::Algorithms::Numbers::Tests::span_tests()
{
    const std::vector<int_t> source{// NOLINTNEXTLINE
        9, 5, 3, 5, 8, 8, 1
    };
    const std::span spa = source;

    const std::vector<std::size_t> expected{// NOLINTNEXTLINE
        0, 0, 0, 2, 3, 4, 0
    };
    ::Standard::Algorithms::ert::are_equal(source.size(), expected.size(), "expected span size");

    std::vector<std::size_t> actual;
    span_calculator::calc_span_simple(spa, actual);
    ::Standard::Algorithms::ert::are_equal(expected, actual, "calc_span_simple");

    actual.clear();
    span_calculator::calc_span_fast(spa, actual);
    ::Standard::Algorithms::ert::are_equal(expected, actual, "calc_span_fast");
}
