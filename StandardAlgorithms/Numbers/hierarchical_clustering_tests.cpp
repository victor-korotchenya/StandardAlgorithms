#include"hierarchical_clustering_tests.h"
#include"../Utilities/test_utilities.h"
#include"hierarchical_clustering.h"

namespace
{
    using int_t = std::int16_t;
    using point_t = std::pair<int_t, int_t>;
    using long_int_t = std::int32_t;

    [[nodiscard]] constexpr auto hier_cl_tests() -> bool
    {
        constexpr int_t mid = 10;
        constexpr int_t largest = 20;

        const std::vector<point_t> points{
            { -mid, -largest }, // 0
            { 0, 0 }, // 1
            { 0, 1 }, // 1
            { 0, largest }, // 2
            { 1, 0 }, // 1
            { 1, 1 }, // 1
            { 1, largest }, // 2
            { 2, largest } // 2
        };

        constexpr auto cluster_count = 3U;
        Standard::Algorithms::require_greater(points.size(), cluster_count, "cluster count");

        const std::vector<std::size_t> expected_clusters{ 0, 1, 1, 2, 1, 1, 2, 2 };

        {
            const auto &maxi = *std::max_element(expected_clusters.begin(), expected_clusters.end());

            ::Standard::Algorithms::ert::are_equal(cluster_count - 1U, maxi, "max expected cluster");
        }

        const auto eval_distance = [&points](const std::size_t &index, const std::size_t &ind_2) -> long_int_t
        {
            const auto &pt1 = points[index];
            const auto &pt_2 = points[ind_2];

            const auto xxx = static_cast<long_int_t>(pt1.first) - pt_2.first;
            const auto yyy = static_cast<long_int_t>(pt1.second) - pt_2.second;

            auto dist = xxx * xxx + yyy * yyy;
            return dist;
        };

        const auto inf = std::numeric_limits<long_int_t>::max();

        std::vector<std::size_t> clusters;
        std::vector<std::size_t> temp;

        Standard::Algorithms::Numbers::hierarchical_clustering_simple(
            cluster_count, eval_distance, temp, inf, clusters, points.size());

        ::Standard::Algorithms::ert::are_equal(expected_clusters, clusters, "hierarchical_clustering_simple clusters");

        return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::hierarchical_clustering_tests()
{
    static_assert(hier_cl_tests());
}
