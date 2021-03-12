#pragma once
// "mean_clustering.h"
#include"cluster_size_validate.h"
#include<algorithm>
#include<cassert>
#include<limits>
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    template<class point_add_t, class point_t, class point_div_t, class random_t, class eval_variance_t,
        std::floating_point variance_t>
    struct mean_clustering_context final
    {
        std::size_t cluster_count{};
        const std::vector<point_t> &points;

        std::size_t total_iterations = 1;
        std::size_t iteration_max_attempts = 1;

        variance_t minus_inf = std::numeric_limits<variance_t>::min();
        variance_t inf = std::numeric_limits<variance_t>::max();

        static_assert(std::numeric_limits<variance_t>::min() < std::numeric_limits<variance_t>::max());

        point_add_t point_add{};
        point_div_t point_div{};
        eval_variance_t eval_variance{};
        random_t rnd{};

        // Output of size cluster_count.
        std::vector<point_t> cluster_centers{};

        // Temporary of size cluster_count.
        std::vector<std::size_t> center_counts{};
        std::vector<point_t> temp_centros{};

        // Temporary of either cluster_count or points size.
        std::vector<std::size_t> center_ids{};

        // Temporary of points size.
        std::vector<bool> takens{};
    };
} // namespace Standard::Algorithms::Numbers

namespace Standard::Algorithms::Numbers::Inner
{
    inline constexpr void check_variance_or_distance(
        [[maybe_unused]] const auto &context, [[maybe_unused]] const auto &variance_or_distance) noexcept
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(context.minus_inf < variance_or_distance && variance_or_distance < context.inf);
        }
    }

    constexpr void init_mean_clustering_context(auto &context)
    {
        assert(context.minus_inf < context.inf);

        cluster_size_validate(context.cluster_count, context.points.size());

        if (0U == context.total_iterations) [[unlikely]]
        {
            throw std::invalid_argument("The total number of iterations must be positive.");
        }

        if (0U == context.iteration_max_attempts) [[unlikely]]
        {
            throw std::invalid_argument("The iteration max attempts must be positive.");
        }

        assert(0U < context.cluster_count && !context.points.empty());
        assert(&context.points != &context.cluster_centers && &context.points != &context.temp_centros);

        context.cluster_centers.resize(context.cluster_count);
        context.center_counts.reserve(context.cluster_count);
        context.temp_centros.reserve(context.cluster_count);

        context.center_ids.reserve(context.points.size());
        context.takens.reserve(context.points.size());
    }

    constexpr void try_add_ide(auto &context, const std::size_t ide)
    {
        assert(ide < context.points.size() && context.takens.size() == context.points.size());

        if (context.takens[ide])
        {
            return;
        }

        context.takens[ide] = true;
        context.center_ids.push_back(ide);
    }

    constexpr void naive_random_points_as_clusters(auto &context)
    {
        cluster_size_validate(context.cluster_count, context.points.size());
        assert(context.cluster_count < context.points.size());

        context.takens.assign(context.points.size(), false);
        context.center_ids.clear();

        while (context.center_ids.size() < context.cluster_count)
        {
            const auto ide = context.rnd();
            try_add_ide(context, ide);
        }
    }

    [[nodiscard]] constexpr auto point_min_cluster_distance(auto &context, const std::size_t ide)
    {
        const auto points_size = context.points.size();

        assert(ide < points_size && points_size == context.takens.size());
        assert(!context.takens[ide] && !context.center_ids.empty());

        const auto &point = context.points.at(ide);
        auto min_distance = context.inf;

        for (const auto &center_id : context.center_ids)
        {
            assert(center_id < points_size && context.takens[center_id]);

            const auto &center = context.points.at(center_id);
            const auto cand = context.eval_variance(point, center);
            check_variance_or_distance(context, cand);

            if (!(cand < min_distance))
            {
                continue;
            }

            min_distance = cand;
        }

        check_variance_or_distance(context, min_distance);
        return min_distance;
    }

    template<std::floating_point variance_t>
    constexpr void furthest_random_points_as_clusters(auto &context)
    {// Min max idea.
        const auto points_size = context.points.size();
        cluster_size_validate(context.cluster_count, points_size);

        assert(1U < context.cluster_count && context.cluster_count < points_size);

        context.takens.assign(points_size, false);
        context.center_ids.clear();
        {
            const auto ide = context.rnd();
            try_add_ide(context, ide);

            assert(1U == context.center_ids.size());
        }

        while (context.center_ids.size() < context.cluster_count)
        {
            auto ide = points_size;
            auto max_distance = context.minus_inf;

            for (std::size_t index{}; index < points_size; ++index)
            {
                if (context.takens[index])
                {
                    continue;
                }

                const auto dist = point_min_cluster_distance(context, index);
                check_variance_or_distance(context, dist);

                if (!(max_distance < dist))
                {
                    continue;
                }

                ide = index;
                max_distance = dist;
            }

            assert(ide < points_size && !context.takens[ide]);
            check_variance_or_distance(context, max_distance);

            context.takens[ide] = true;
            context.center_ids.push_back(ide);
        }
    }

    template<std::floating_point variance_t>
    [[nodiscard]] constexpr auto try_find_closer_centers(auto &context) -> std::pair<variance_t, bool>
    {
        assert(1U < context.cluster_count && context.cluster_count < context.points.size());
        assert(context.cluster_count == context.temp_centros.size());
        assert(context.points.size() == context.center_ids.size());

        context.center_counts.assign(context.cluster_count, 0ZU);

        variance_t sum_variance{};
        bool is_changed{};

        for (std::size_t ide{}; ide < context.points.size(); ++ide)
        {
            const auto &point1 = context.points.at(ide);
            auto best_center = std::numeric_limits<std::size_t>::max();
            auto pt_min_dist = context.inf;

            for (std::size_t index{}; index < context.temp_centros.size(); ++index)
            {
                const auto &cen_point = context.temp_centros.at(index);
                const auto cand = context.eval_variance(cen_point, point1);
                check_variance_or_distance(context, cand);

                if (!(cand < pt_min_dist))
                {
                    continue;
                }

                pt_min_dist = cand;
                best_center = index;
            }

            assert(best_center < context.temp_centros.size());
            check_variance_or_distance(context, pt_min_dist);

            ++context.center_counts.at(best_center);

            sum_variance += pt_min_dist;
            check_variance_or_distance(context, sum_variance);

            if (auto &center_id = context.center_ids.at(ide); best_center != center_id)
            {
                center_id = best_center;
                is_changed = true;
            }
        }

        check_variance_or_distance(context, sum_variance);
        return { sum_variance, is_changed };
    }

    template<class point_t, std::floating_point variance_t>
    constexpr void compute_means(auto &context)
    {
        assert(!context.points.empty() && context.points.size() == context.center_ids.size());

        auto &temp_centros = context.temp_centros;
        assert(!temp_centros.empty() && temp_centros.size() < context.points.size());

        temp_centros.assign(temp_centros.size(), point_t{});
        context.center_counts.assign(temp_centros.size(), 0ZU);

        for (std::size_t ide{}; ide < context.points.size(); ++ide)
        {
            const auto &point1 = context.points.at(ide);
            const auto &center_id = context.center_ids.at(ide);
            ++(context.center_counts.at(center_id));

            auto &pt_center = temp_centros.at(center_id);
            context.point_add(pt_center, point1);
        }

        for (std::size_t center_id{}; auto &point1 : temp_centros)
        {
            if (const auto &center_size = context.center_counts.at(center_id); 0U < center_size)
            {
                const auto size_f = static_cast<variance_t>(center_size);
                assert(variance_t{} < size_f);

                context.point_div(point1, size_f);
            }

            ++center_id;
        }
    }

    template<class point_t, std::floating_point variance_t, class context_t, class init_func_t>
    [[nodiscard]] constexpr auto clustering_iteration(context_t &context, init_func_t init_func) -> variance_t
    {
        const auto points_size = context.points.size();

        assert(1U < context.cluster_count && context.cluster_count < points_size);
        init_func(context);
        assert(context.cluster_count == context.center_ids.size());

        context.temp_centros.clear();

        for (const auto &ide : context.center_ids)
        {
            assert(ide < points_size);

            const auto &point1 = context.points.at(ide);
            context.temp_centros.push_back(point1);
        }

        // Force changing all centers.
        context.center_ids.assign(points_size, points_size);

        auto left_attempts = context.iteration_max_attempts;
        assert(0U < left_attempts);

        for (auto min_variance = context.inf;;)
        {
            const auto [sum_variance, is_changed] = try_find_closer_centers<variance_t>(context);

            check_variance_or_distance(context, sum_variance);

            // Note. The variance sum should not increase.
            // assert(sum_variance <= min_variance);
            min_variance = sum_variance;

            if (0U == --left_attempts || // todo(p4): If the variance decreases by 1E-9 percent, stop?
                !is_changed)
            {
                return min_variance;
            }

            compute_means<point_t, variance_t>(context);
        }
    }

    template<class point_t, std::floating_point variance_t, class context_t,
        class init_func_t = void (*)(context_t &context)>
    constexpr auto mean_clustering_impl(context_t &context, init_func_t init_func) -> variance_t
    {
        init_mean_clustering_context(context);
        assert(0U < context.total_iterations);

        // todo(p5): maybe some points are 0-varianced?
        if (context.points.size() == context.cluster_count)
        {
            std::copy(context.points.begin(), context.points.end(), context.cluster_centers.begin());
            return {};
        }

        auto min_variance = context.inf;

        for (std::size_t itera{}; itera < context.total_iterations; ++itera)
        {
            auto cand = clustering_iteration<point_t, variance_t>(context, init_func);

            check_variance_or_distance(context, cand);
            assert(context.cluster_count == context.temp_centros.size());
            // todo(p4): del empty clusters.

            if (!(cand < min_variance))
            {
                continue;
            }

            min_variance = cand;
            std::swap(context.temp_centros, context.cluster_centers);
        }

        check_variance_or_distance(context, min_variance);
        return min_variance;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    template<class point_add_t, class point_t, class point_div_t, class random_t, class eval_variance_t,
        std::floating_point variance_t>
    constexpr auto naive_mean_clustering(
        mean_clustering_context<point_add_t, point_t, point_div_t, random_t, eval_variance_t, variance_t> &context)
        -> variance_t
    {
        auto min_variance =
            Inner::mean_clustering_impl<point_t, variance_t>(context, &Inner::naive_random_points_as_clusters);

        return min_variance;
    }

    template<class point_add_t, class point_t, class point_div_t, class random_t, class eval_variance_t,
        std::floating_point variance_t>
    constexpr auto furthest_mean_clustering(
        mean_clustering_context<point_add_t, point_t, point_div_t, random_t, eval_variance_t, variance_t> &context)
        -> variance_t
    {
        auto min_distance = Inner::mean_clustering_impl<point_t, variance_t>(
            context, &Inner::furthest_random_points_as_clusters<variance_t>);

        return min_distance;
    }
} // namespace Standard::Algorithms::Numbers
