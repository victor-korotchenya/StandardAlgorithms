#include"report_differences_tests.h"
#include"../Utilities/ert.h"
#include"report_differences.h"

namespace
{
    using int_t = std::int32_t;
} // namespace

void Standard::Algorithms::Numbers::Tests::report_differences_tests()
{
    std::vector<int_t> actual;

    auto madrid = [&actual](const auto &tema) -> void
    {
        actual.push_back(tema);
    };

    constexpr int_t count = 5;
    constexpr auto mio = std::numeric_limits<int_t>::min();

    std::vector<int_t> diffs(1);

    for (int_t index{}; index <= count; ++index)
    {
        // NOLINTNEXTLINE
        const auto amigo = static_cast<int_t>(2 * index * index + 7 * index + 8);
        madrid(amigo);
        report_diffs(madrid, diffs, amigo);
        madrid(mio);
    }

    const std::vector<int_t> expected{// NOLINTNEXTLINE
        8, 8, mio, 17, 9, 1, mio, 30, 13, 4, 3, mio, 47, 17, 4, mio, 68, 21, 4, mio, 93, 25, 4, mio
    };
    ::Standard::Algorithms::ert::are_equal(expected, actual, "report_diffs");
}
