#include"quick_select_kth_smallest_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"quick_select_kth_smallest.h"

namespace
{
    using value_t = std::int64_t;
} // namespace

void Standard::Algorithms::Numbers::Tests::quick_select_kth_smallest_tests()
{
    const std::vector<value_t> sorted_source{ 0, 1, 2, 2, 2,
        // 1.0 float as std::int32_t, 1.0 double
        // NOLINTNEXTLINE
        5, 1'065'353'216, 4'607'182'418'800'017'408 };
    auto temp = sorted_source;

    for (std::size_t rank{}; rank < sorted_source.size(); ++rank)
    {
        temp.resize(sorted_source.size());

        std::copy(sorted_source.cbegin(), sorted_source.cend(), temp.begin());
        Standard::Algorithms::Utilities::shuffle_data(temp);

        const auto ind = quick_select_kth_smallest(temp, rank);

        ::Standard::Algorithms::ert::are_equal(
            temp.at(ind), sorted_source.at(rank), "quick_select_kth_smallest " + std::to_string(rank));
    }
}
