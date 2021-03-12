#include"max_contiguous_sum_removing_most_m_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"max_contiguous_sum_removing_most_m.h"
#include<cstdio> // puts

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;
    using floating_t = Standard::Algorithms::floating_point_type;

    constexpr auto load_tests = false;
    constexpr long_int_t not_computed = -1;

    // NOLINTNEXTLINE
    constexpr auto value_max = load_tests ? 1'000 : 30;

    static_assert(0 < value_max);


    // todo(p2): write/use a load test lib.

    // NOLINTNEXTLINE
    [[maybe_unused]] std::atomic<std::int64_t> fast_wins;

    // NOLINTNEXTLINE
    [[maybe_unused]] std::atomic<std::int64_t> recurs_wins;

    // NOLINTNEXTLINE
    [[maybe_unused]] std::atomic<floating_t> fast_ratio_max;

    // NOLINTNEXTLINE
    [[maybe_unused]] std::atomic<floating_t> recurs_ratio_max;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, std::size_t remove_max, std::vector<int_t> &&arr, long_int_t expected) noexcept
            : base_test_case(std::move(name))
            , Remove_max(remove_max)
            , Arr(std::move(arr))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto remove_max() const noexcept -> std::size_t
        {
            return Remove_max;
        }

        [[nodiscard]] constexpr auto arr() const &noexcept -> const std::vector<int_t> &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const long_int_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Arr.size(), "array size");
            Standard::Algorithms::require_positive(Remove_max, "remove max");

            Standard::Algorithms::require_less_equal(Remove_max, Arr.size(), "remove max");

            Standard::Algorithms::require_less_equal(not_computed, Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("array", Arr, str);
            ::Standard::Algorithms::print_value("remove max", str, Remove_max);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::size_t Remove_max;
        std::vector<int_t> Arr;
        long_int_t Expected;
    };

    void gen_add_random_test(const std::int32_t attempt, std::vector<test_case> &test_cases, const std::string &prefix,
        Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd, const std::int32_t size)
    {
        Standard::Algorithms::require_positive(size, "size");

        std::vector<int_t> arr(size);
        std::generate(arr.begin(), arr.end(),
            [&rnd]
            {
                return rnd();
            });

        const auto remove_max = static_cast<std::size_t>(rnd(1, size));

        test_cases.emplace_back(prefix + std::to_string(attempt), remove_max, std::move(arr), not_computed);
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "remove not consecutive <0 numbers, 3", // NOLINTNEXTLINE
            3, // NOLINTNEXTLINE
            { 6, -100, 5, -2, -3, 10, -5, -10, -100, 152 }, // NOLINTNEXTLINE
            163 });
        // 6,     , 5, -2, -3, 10, -5,    ,     , 152 }

        test_cases.push_back({ "remove not consecutive <0 numbers", // NOLINTNEXTLINE
            2, // NOLINTNEXTLINE
            { 3, -100, 6, -1, 10, -100, 2 }, // NOLINTNEXTLINE
            20 });
        // 3,     , 6, -1, 10,     , 2 }

        test_cases.push_back({ "-1", 1, { -1 }, 0 });
        test_cases.push_back({ "0", 1, { 0 }, 0 });
        test_cases.push_back({ "1", 1, { 1 }, 1 });

        test_cases.push_back({ "2", // NOLINTNEXTLINE
            1, { 11, 234 }, 245 });

        test_cases.push_back({ "all < 0, 3", // NOLINTNEXTLINE
            3, // NOLINTNEXTLINE
            { -1, -12, -34 }, // NOLINTNEXTLINE
            0 });

        test_cases.push_back({ "all < 0, 2", // NOLINTNEXTLINE
            2, // NOLINTNEXTLINE
            { -1, -12, -34 }, // NOLINTNEXTLINE
            0 });

        test_cases.push_back({ "all < 0, 1", // NOLINTNEXTLINE
            1, // NOLINTNEXTLINE
            { -1, -12, -34 }, // NOLINTNEXTLINE
            0 });

        test_cases.push_back({ "0 <= all", // NOLINTNEXTLINE
            3, // NOLINTNEXTLINE
            { 1, 0, 34 }, // NOLINTNEXTLINE
            35 });

        test_cases.push_back({ "0 < inside", // NOLINTNEXTLINE
            1, // NOLINTNEXTLINE
            { -1, 0, 1, 34, 0, -2 }, // NOLINTNEXTLINE
            35 });

        test_cases.push_back({ "0 < inside, 6", // NOLINTNEXTLINE
            6, // NOLINTNEXTLINE
            { -1, 0, 1, 34, 0, -2 }, // NOLINTNEXTLINE
            35 });

        test_cases.push_back({ "negative ins <= remove_max", // NOLINTNEXTLINE
            2, // NOLINTNEXTLINE
            { -1, 2, -5, -17, 7, 0, -2 }, // NOLINTNEXTLINE
            9 });

        test_cases.push_back({ "remove_max < negative ins", // NOLINTNEXTLINE
            1, // NOLINTNEXTLINE
            { -1, 2, -5, -17, 7, 0, -2 }, // NOLINTNEXTLINE
            7 });

        test_cases.push_back({ "base1", // NOLINTNEXTLINE
            1, // NOLINTNEXTLINE
            { -2, 1, 3, -2, 4, -7, 20 }, // NOLINTNEXTLINE
            26 });
        //{ , 1, 3, -2, 4, , 20 }

        test_cases.push_back({ "base2", // NOLINTNEXTLINE
            2, // NOLINTNEXTLINE
            { -2, 1, 3, -2, 4, -7, 20 }, // NOLINTNEXTLINE
            28 });
        //{ , 1, 3, , 4, , 20 }

        test_cases.push_back({ "base3", // NOLINTNEXTLINE
            3, // NOLINTNEXTLINE
            { -2, 1, 3, -2, 4, -7, 20, -9 }, // NOLINTNEXTLINE
            28 });

        test_cases.push_back({ "base4", // NOLINTNEXTLINE
            4, // NOLINTNEXTLINE
            { -2, 1, 3, -2, 4, -7, 20 }, // NOLINTNEXTLINE
            28 });

        test_cases.push_back({ "base5", // NOLINTNEXTLINE
            2, // NOLINTNEXTLINE
            { 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, 4, -1, -1, -1, -1, 1, 2, -1 }, // NOLINTNEXTLINE
            5 }); // 4, , , -1, -1, 1, 2

        test_cases.push_back({ "base7", // NOLINTNEXTLINE
            7, // NOLINTNEXTLINE
            { -2, 1, 3, -2, 4, -7, 20 }, // NOLINTNEXTLINE
            28 });

        test_cases.push_back({ "base11", // NOLINTNEXTLINE
            1, // NOLINTNEXTLINE
            { -1, 1, -1, -1, 1, 1 }, // NOLINTNEXTLINE
            2 });

        test_cases.push_back({ "base12", // NOLINTNEXTLINE
            2, // NOLINTNEXTLINE
            { -1, 1, -1, -1, 1, 1 }, // NOLINTNEXTLINE
            3 });

        test_cases.push_back({ "base16", // NOLINTNEXTLINE
            6, // NOLINTNEXTLINE
            { -1, 1, -1, -1, 1, 1 }, // NOLINTNEXTLINE
            3 });

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(-value_max, value_max);

        [[maybe_unused]] constexpr auto n_max = 10'000;
        [[maybe_unused]] constexpr auto step = 10;

        if constexpr (load_tests)
        {
            {
                const std::string stat_prefix = "stat";

                for (int_t size = step; size <= n_max; size += step)
                {
                    gen_add_random_test(size, test_cases, stat_prefix, rnd, size);
                }
            }
        }

        const std::string random_prefix = "Random";

        constexpr auto max_attempts = load_tests ? 1'000 : 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size = rnd(load_tests ? n_max / step : 1, // NOLINTNEXTLINE
                load_tests ? n_max : 10);

            gen_add_random_test(att, test_cases, random_prefix, rnd, size);
        }

        if constexpr (load_tests)
        {
            std::puts(" max_contiguous_sum_removing_most_m_tests gen completed.");
        }
    }

    template<class value_t>
    constexpr void maxin(std::atomic<value_t> &ato, const value_t &candidate)
    {
        value_t rad{};

        do
        {
            rad = ato.load();

            if (!(rad < candidate))
            {
                return;
            }
        } while (!ato.compare_exchange_weak(rad, candidate, std::memory_order_release, std::memory_order_relaxed));
    }

    void run_test_case(const test_case &test)
    {
        const Standard::Algorithms::elapsed_time_ns tim0;

        const auto fast = Standard::Algorithms::Numbers::max_contiguous_sum_removing_most_m<long_int_t, int_t>(
            test.arr(), test.remove_max());

        const auto fast_elapsed = tim0.elapsed();

        if (long_int_t{} <= test.expected())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "max_contiguous_sum_removing_most_m");
        }

        {
            const auto sens =
                Standard::Algorithms::Numbers::max_contiguous_sum_removing_most_m_sensitive<long_int_t, int_t>(
                    test.arr(), test.remove_max());

            ::Standard::Algorithms::ert::are_equal(fast, sens, "max_contiguous_sum_removing_most_m_sensitive");
        }

        const Standard::Algorithms::elapsed_time_ns tim1;

        const auto recurs = Standard::Algorithms::Numbers::max_contiguous_sum_removing_most_m_rec<long_int_t, int_t>(
            test.arr(), test.remove_max());

        const auto recurs_elapsed = tim1.elapsed();

        ::Standard::Algorithms::ert::are_equal(fast, recurs, "max_contiguous_sum_removing_most_m_rec");

        if (0 < recurs_elapsed && 0 < fast_elapsed)
        {
            if constexpr (load_tests)
            {
                const auto ratio = ::Standard::Algorithms::ratio_compute(fast_elapsed, recurs_elapsed);

                constexpr floating_t one_f{ 1.0 };

                if (one_f < ratio)
                {
                    fast_wins.fetch_add(1, std::memory_order_relaxed);

                    maxin(fast_ratio_max, ratio);
                }
                else if (ratio < one_f)
                {
                    recurs_wins.fetch_add(1, std::memory_order_relaxed);

                    const auto ratio_rev = one_f / ratio;

                    maxin(recurs_ratio_max, ratio_rev);
                }

                const auto report = "n" + std::to_string(test.arr().size()) + ", de " +
                    std::to_string(test.remove_max())

                    + ", t/m " + std::to_string(ratio) + ", t " +
                    std::to_string(recurs_wins.fetch_add(0, std::memory_order_relaxed)) + ", m " +
                    std::to_string(fast_wins.fetch_add(0, std::memory_order_relaxed))

                    + "; th r " + std::to_string(recurs_ratio_max.load(std::memory_order_relaxed)) + ", m r " +
                    std::to_string(fast_ratio_max.load(std::memory_order_relaxed)) + ", t t " +
                    std::to_string(recurs_elapsed) + ", m t " + std::to_string(fast_elapsed);
                std::puts(report.data());
            }
        }

        if (constexpr auto large_size = 10U; large_size < test.arr().size())
        {
            return;
        }

        const auto slow = Standard::Algorithms::Numbers::max_contiguous_sum_removing_most_m_slow<long_int_t, int_t>(
            test.arr(), test.remove_max());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "max_contiguous_sum_removing_most_m_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::max_contiguous_sum_removing_most_m_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests<load_tests ? all_cores : single_core>(
        run_test_case, generate_test_cases);
}
