#include"mean_clustering_tests.h"
#include"../Utilities/definitiv_operator_feed.h"
#include"../Utilities/test_utilities.h"
#include"enumerate_choose.h"
#include"mean_clustering.h"
#include<array>

namespace // todo(p4): point ops - move out.
{
    // todo(p3): Compare classes with approx members.
    // todo(p4): using floating_t = Standard::Algorithms::floating_point_type;
    using floating_t = double;
    using point_t = std::pair<floating_t, floating_t>;

    constexpr void point_add(point_t &dest, const point_t &to_add) noexcept
    {
        dest.first += to_add.first;
        dest.second += to_add.second;
    }

    constexpr void point_div(point_t &dest, const floating_t &to_divide)
    {
        assert(floating_t{} != to_divide);

        dest.first /= to_divide;
        dest.second /= to_divide;
    }

    [[nodiscard]] constexpr auto point_eval_variance(const point_t &uno, const point_t &momento) noexcept -> floating_t
    {
        const auto xxx = uno.first - momento.first;
        const auto yyy = uno.second - momento.second;
        auto squared = xxx * xxx + yyy * yyy;

        return squared;
    }
} // namespace

namespace // Test data.
{
    constexpr floating_t half_x = 0.5;
    constexpr floating_t mid_x = 10;
    constexpr floating_t largest_y = 20;

    constexpr auto cluster_count = 3U;

    constexpr floating_t min_variance = 4;
    constexpr floating_t max_variance = 426.9000000000000341061;
    static_assert(min_variance < max_variance);

    [[nodiscard]] constexpr auto build_points() -> std::vector<point_t>
    {
        return std::vector<point_t>{
            { -mid_x, -largest_y }, // 0-th cluster.
            { 0, 0 }, // 1
            { 0, 1 }, // 1
            { 0, largest_y }, // 2
            { 1, 0 }, // 1
            { 1, 1 }, // 1
            { 1, largest_y }, // 2
            { 2, largest_y } // 2
        };
    }

    [[nodiscard]] constexpr auto cluster_centers_sample_0() -> std::vector<point_t>
    {
        return std::vector<point_t>{
            { -mid_x, -largest_y }, // 0-th cluster.
            { half_x, half_x }, // 1
            { 1, largest_y } // 2
        };
    }

    [[nodiscard]] constexpr auto cluster_centers_sample_bad() -> std::vector<point_t>
    {
        return std::vector<point_t>{// NOLINTNEXTLINE
            { -8.0 / 5.0, -18.0 / 5.0 }, // NOLINTNEXTLINE
            { 0, largest_y }, // NOLINTNEXTLINE
            { 3.0 / 2.0, largest_y }
        };
    }

    [[nodiscard]] constexpr auto cluster_centers_sample_bad_again() -> std::vector<point_t>
    {
        return std::vector<point_t>{// NOLINTNEXTLINE
            { -8.0 / 5.0, -18.0 / 5.0 }, // NOLINTNEXTLINE
            { half_x, largest_y }, // NOLINTNEXTLINE
            { 2, largest_y }
        };
    }
} // namespace

namespace
{
    using random_t1 = Standard::Algorithms::Utilities::definitiv_operator_feed<std::size_t>;

    enum class [[nodiscard]] mean_clustering_kind
    {
        naive,
        furthest
    };

    [[nodiscard]] constexpr auto to_string(const mean_clustering_kind &kind) -> const char *
    {
        switch (kind)
        {
        case mean_clustering_kind::naive:
            return "naive";
        case mean_clustering_kind::furthest:
            return "furthest";
        default:
            assert(0);
            std::unreachable();
            return "unknown mean clustering kind";
        }
    }

    [[nodiscard]] constexpr auto run_mean(const mean_clustering_kind &kind, const std::string &name,
        const std::vector<point_t> &points, random_t1 &rnd) -> std::pair<floating_t, std::vector<point_t>>
    {
        assert(!name.empty());

        constexpr auto total_iterations = 1U;
        constexpr auto iteration_max_attempts = 10U;

        constexpr floating_t inf = 1E50;
        constexpr floating_t minus_inf = -inf;
        static_assert(minus_inf < floating_t{} && floating_t{} < inf && minus_inf < inf);

        {
            static_assert(std::numeric_limits<floating_t>::min() < std::numeric_limits<floating_t>::max());

            static_assert(floating_t{} < std::numeric_limits<floating_t>::min(), "Ops, floating minimum is positive.");
        }

        using random_t_ref = random_t1 &;

        using point_add_t = void (*)(point_t &, const point_t &);
        using point_div_t = void (*)(point_t &, const floating_t &);
        using eval_variance_t = floating_t (*)(const point_t &, const point_t &);

        using context_t = Standard::Algorithms::Numbers::mean_clustering_context<point_add_t, point_t, point_div_t,
            random_t_ref, eval_variance_t, floating_t>;

        context_t context{ .cluster_count = cluster_count,
            .points = points,
            .total_iterations = total_iterations,
            .iteration_max_attempts = iteration_max_attempts,
            .minus_inf = minus_inf,
            .inf = inf,
            .point_add = &point_add,
            .point_div = &point_div,
            .eval_variance = &point_eval_variance,
            .rnd = rnd };

        auto actual_variance = inf;

        switch (kind)
        {
        case mean_clustering_kind::naive:
            actual_variance = Standard::Algorithms::Numbers::naive_mean_clustering(context);
            break;
        case mean_clustering_kind::furthest:
            actual_variance = Standard::Algorithms::Numbers::furthest_mean_clustering(context);
            break;
        default:
            assert(0);
            std::unreachable();
            break;
        }

        auto &centers = context.cluster_centers;
        ::Standard::Algorithms::ert::are_equal(cluster_count, centers.size(), name + " the cluster centers size");

        // Easy compare.
        std::sort(centers.begin(), centers.end());

        return { actual_variance, std::move(centers) };
    }

    constexpr void run_mean_test(const mean_clustering_kind &kind, const std::vector<point_t> &points,
        const std::vector<std::size_t> &ideas, const std::vector<point_t> &expected_centers,
        const floating_t &expected_variance)
    {
        const auto name = to_string(kind) + std::string(" mean clustering");

        Standard::Algorithms::Utilities::definitiv_operator_feed rnd(ideas);
        const auto [actual_variance, actual_centers] = run_mean(kind, name, points, rnd);

        ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_variance, actual_variance, name + " variance");

        ::Standard::Algorithms::ert::are_equal_with_epsilon(expected_centers, actual_centers, name + " cluster centers");
    }

    [[nodiscard]] constexpr auto good_test_0() -> bool
    {
        const auto points = build_points();
        const auto expected_centers = cluster_centers_sample_0();

        run_mean_test(
            mean_clustering_kind::naive, points, std::vector<std::size_t>{ 0, 1, 2 }, expected_centers, min_variance);

        return true;
    }

    [[nodiscard]] constexpr auto bad_test() -> bool
    {
        const auto points = build_points();
        const auto expected_centers = cluster_centers_sample_bad();

        run_mean_test(mean_clustering_kind::naive, points,
            std::vector<std::size_t>{// NOLINTNEXTLINE
                1, 3, 6 },
            expected_centers, max_variance);

        return true;
    }

    [[nodiscard]] constexpr auto bad_test_again() -> bool
    {
        const auto points = build_points();
        const auto expected_centers = cluster_centers_sample_bad_again();

        run_mean_test(mean_clustering_kind::naive, points,
            std::vector<std::size_t>{// NOLINTNEXTLINE
                1, 3, 7 },
            expected_centers, max_variance);

        return true;
    }

    constexpr void run_testo(const mean_clustering_kind &kind, const std::vector<point_t> &points,
        const std::vector<std::size_t> &ideas, const auto &all_variances, const auto &all_centers,
        const std::size_t step)
    {
        const auto name = to_string(kind) + std::string(" mean clustering");
        Standard::Algorithms::Utilities::definitiv_operator_feed rnd(ideas);

        const auto [actual_variance, actual_centers] = run_mean(kind, name, points, rnd);

        if (const auto iter = std::find_if(all_variances.begin(), all_variances.end(),
                [actual_variance = actual_variance](const auto &vari) -> bool
                {
                    auto are_close =
                        ::Standard::Algorithms::ert::are_epsilon_equal(std::make_pair(vari, actual_variance));
                    return are_close;
                });
            all_variances.end() == iter) [[unlikely]]
        {
            auto err = to_string(kind) + std::string(" unexpected step ") +
                ::Standard::Algorithms::Utilities::zu_string(step) + ", actual variance " +
                std::to_string(actual_variance);

            throw std::runtime_error(err);
        }

        if (const auto iter = std::find_if(all_centers.begin(), all_centers.end(),
                [ptr_actual_centers = &actual_centers](const auto &centers) -> bool
                {
                    auto are_close = *ptr_actual_centers == centers;
                    return are_close;
                });
            all_centers.end() == iter) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << to_string(kind);
            ::Standard::Algorithms::print(" unexpected actual centers", actual_centers, str);
            str << ", step " << step;

            throw std::runtime_error(str.str());
        }
    }

    [[nodiscard]] constexpr auto enumerate_testos_naive() -> bool
    {
        constexpr const std::array all_variances{ min_variance, max_variance };

        const std::array all_centers{ cluster_centers_sample_0(), cluster_centers_sample_bad(),
            cluster_centers_sample_bad_again() };

        const auto points = build_points(); // NOLINTNEXTLINE
        ::Standard::Algorithms::ert::are_equal(8U, points.size(), "points size");
        constexpr auto choose_3_of_8 = 56U;

        Standard::Algorithms::Numbers::enumerate_choose choo(cluster_count, points.size());

        std::size_t step{};

        do
        {
            run_testo(mean_clustering_kind::naive, points, choo.current(), all_variances, all_centers, step);

            Standard::Algorithms::require_greater(choose_3_of_8, step, "choose step");
            ++step;
        } while (choo.next());

        ::Standard::Algorithms::ert::are_equal(choose_3_of_8, step, "final step");

        return true;
    }

    [[nodiscard]] constexpr auto enumerate_testos_furthest() -> bool
    {
        const auto points = build_points();
        constexpr const std::array all_variances{ min_variance };
        const std::array all_centers{ cluster_centers_sample_0() };

        std::vector<std::size_t> ideas(1);

        for (std::size_t start_ide{}; start_ide < points.size(); ++start_ide)
        {
            ideas.at(0) = start_ide;

            run_testo(mean_clustering_kind::furthest, points, ideas, all_variances, all_centers, start_ide);
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::mean_clustering_tests()
{
    static_assert(good_test_0());
    static_assert(bad_test(), "Some choices might be very bad, 106 times worse.");
    static_assert(bad_test_again(), "very bad again.");
    static_assert(enumerate_testos_naive());
    static_assert(enumerate_testos_furthest());

    // todo(p4): Add a test with point repetitions to have empty clusters.
}
