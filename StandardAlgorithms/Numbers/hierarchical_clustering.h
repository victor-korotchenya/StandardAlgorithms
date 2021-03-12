#pragma once
#include"../Utilities/zu_string.h"
#include"cluster_size_validate.h"
#include<algorithm>
#include<cassert>
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    // Find the two closest buddies, and place them in one cluster.
    template<class eval_distance_t, class distance_t>
    constexpr void run_hierarchical_clustering_n3_once(eval_distance_t eval_distance, const distance_t &inf,
        std::vector<std::size_t> &clusters, const std::size_t point_count)
    {
        std::size_t pos0{};
        std::size_t pos1 = 1;
        auto best = inf;

        for (std::size_t index{}; index + 1U < point_count; ++index)
        {
            const auto &cl0 = clusters[index];

            for (std::size_t ind_2 = index + 1U; ind_2 < point_count; ++ind_2)
            {
                const auto &cl1 = clusters[ind_2];
                if (cl0 == cl1)
                {
                    continue;
                }

                const auto cand = eval_distance(index, ind_2);
                assert(cand < inf);

                if (!(cand < best))
                {
                    continue;
                }

                best = cand;
                pos0 = index;
                pos1 = ind_2;
            }
        }

        const auto &clu_0 = clusters[pos0];
        const auto &clu_1 = clusters[pos1];

        const auto min_cl = std::min(clu_0, clu_1);
        const auto max_cl = std::max(clu_0, clu_1);

        assert(min_cl < max_cl && max_cl < point_count && best < inf);

        for (std::size_t index{}; index < point_count; ++index)
        {
            auto &clu = clusters[index];

            if (clu == max_cl)
            {// Join the two closest clusters into one.
                clu = min_cl;
            }
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given {0, 1, 0, 3, 3},
    // return {0, 1, 0, 2, 2}
    // where all the compressed, normalized numbers are consecutive from 0 to some max.
    inline constexpr void remap_used_clusters(
        std::vector<std::size_t> &temp, std::size_t cluster_count, std::vector<std::size_t> &clusters)
    {
        assert(&temp != &clusters);

        const auto size = clusters.size();
        cluster_size_validate(cluster_count, size);
        temp.assign(size, 0U);

        for (const auto &clus : clusters)
        {
            assert(clus < size);

            temp.at(clus) = 1U;
        }

        if (temp.at(0) == 0U) [[unlikely]]
        {
            throw std::invalid_argument("The 0-th cluster must have been used.");
        }

        temp.at(0) = 0U;

        for (std::size_t index{}; index < size - 1U; ++index)
        {
            const auto &prev = temp[index];
            auto &cur = temp[index + 1U];
            cur += prev;
        }

        assert(temp.back() == cluster_count - 1U);

        for (std::size_t index{}; index < size; ++index)
        {
            auto &clo = clusters[index];
            assert(clo < size);

            const auto &mapped = temp.at(clo);
            assert(mapped < cluster_count);

            clo = mapped;
        }
    }

    // Slow time O(n**3).
    template<class eval_distance_t, class distance_t>
    constexpr void hierarchical_clustering_simple(const std::size_t cluster_count, eval_distance_t eval_distance,
        std::vector<std::size_t> &temp, const distance_t &inf, std::vector<std::size_t> &clusters,
        const std::size_t point_count)
    {
        cluster_size_validate(cluster_count, point_count);

        // todo(p5): maybe some points are 0-distanced?
        clusters.resize(point_count);
        std::iota(clusters.begin(), clusters.end(), std::size_t{});

        if (point_count == cluster_count)
        {
            return;
        }

        auto cur_size = point_count;

        do
        {
            Inner::run_hierarchical_clustering_n3_once(eval_distance, inf, clusters, point_count);
        } while (cluster_count < --cur_size);

        remap_used_clusters(temp, cluster_count, clusters);
    }
} // namespace Standard::Algorithms::Numbers
