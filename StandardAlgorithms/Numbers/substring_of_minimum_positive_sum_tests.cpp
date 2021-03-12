#include"substring_of_minimum_positive_sum_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"substring_of_minimum_positive_sum.h"
#include<array>
#include<functional>
#include<iostream>
#include<numeric>

namespace
{
    constexpr auto mps_measure_time = false;

    [[nodiscard]] auto to_string(const Standard::Algorithms::Numbers::first_last_sum &test) -> std::string
    {
        return std::string("begin ") + ::Standard::Algorithms::Utilities::zu_string(test.begin) + ", end " +
            ::Standard::Algorithms::Utilities::zu_string(test.end) + ", sum " +
            ::Standard::Algorithms::Utilities::zu_string(test.sum);
    }

    struct mps_test_case final
    {
        std::string name{};
        std::vector<std::int32_t> data{};
        Standard::Algorithms::Numbers::first_last_sum expected{};
    };

    [[nodiscard]] auto to_string(const mps_test_case &test) -> std::string
    {
        auto result = "name '" + test.name + "', data {";

        for (const auto &datum : test.data)
        {
            result += std::to_string(datum);
            result += ", ";
        }

        result += "}, expected ";
        result += to_string(test.expected);

        return result;
    }

    void check_result(const mps_test_case &test, const Standard::Algorithms::Numbers::first_last_sum &actual)
    {
        ::Standard::Algorithms::ert::are_equal(test.expected.sum, actual.sum, "actual sum");

        if (actual.sum < 0)
        {
            return;
        }

        ::Standard::Algorithms::ert::greater(actual.sum, 0, "actual sum");
        ::Standard::Algorithms::ert::greater(actual.end, actual.begin, "actual begin end");
        ::Standard::Algorithms::ert::greater(test.data.size() + 1U, actual.end, "actual end");

        const auto total = std::accumulate(test.data.cbegin() + static_cast<std::ptrdiff_t>(actual.begin),
            test.data.cbegin() + static_cast<std::ptrdiff_t>(actual.end), std::int64_t{});

        ::Standard::Algorithms::ert::are_equal(actual.sum, total, "total actual sum for the chosen range");
    }

    void check_simple(const Standard::Algorithms::Numbers::first_last_sum &expected,
        const Standard::Algorithms::Numbers::first_last_sum &actual)
    {
        assert(expected.sum != 0);

        ::Standard::Algorithms::ert::are_equal(expected.sum, actual.sum, "simple sum");
        ::Standard::Algorithms::ert::are_equal(expected.begin, actual.begin, "simple begin");
        ::Standard::Algorithms::ert::are_equal(expected.end, actual.end, "simple end");
    }

    void run_mps_slow(mps_test_case &test, const bool is_random)
    {
        const auto actual =
            Standard::Algorithms::Numbers::substring_of_minimum_positive_sum_slow(0, test.data, test.data.size());

        if (is_random)
        {
            ::Standard::Algorithms::ert::greater(actual.sum, 0, "random sum");
            test.expected = actual;
        }
        else
        {
            check_simple(test.expected, actual);
        }

        check_result(test, actual);
    }

    using func_t = std::function<Standard::Algorithms::Numbers::first_last_sum(const std::vector<std::int32_t> &)>;

    void run_mps_regular(const func_t &func, const mps_test_case &test, const bool is_random)
    {
        const auto actual = func(test.data);

        const auto &expected = test.expected;
        ::Standard::Algorithms::ert::are_equal(expected.sum, actual.sum, "regular sum");

        if (!is_random)
        {
            check_simple(expected, actual);
        }

        check_result(test, actual);
    }

    void run_mps(mps_test_case &test, const bool is_random = false)
    {
        const char *name = "substring_of_minimum_positive_sum_slow";

        try
        {
            const Standard::Algorithms::elapsed_time_ns tim0;

            run_mps_slow(test, is_random);

            [[maybe_unused]] const auto slow_elapsed = tim0.elapsed();
            const Standard::Algorithms::elapsed_time_ns tim1;

            name = "substring_of_minimum_positive_sum_better";

            run_mps_regular(&Standard::Algorithms::Numbers::substring_of_minimum_positive_sum_better, test, is_random);

            [[maybe_unused]] const auto better_elapsed = tim1.elapsed();
            const Standard::Algorithms::elapsed_time_ns tim2;

            name = "substring_of_minimum_positive_sum_better2";

            run_mps_regular(&Standard::Algorithms::Numbers::substring_of_minimum_positive_sum_better2, test, is_random);

            [[maybe_unused]] const auto better_elapsed2 = tim2.elapsed();
            const Standard::Algorithms::elapsed_time_ns tim3;

            name = "substring_of_minimum_positive_sum";

            run_mps_regular(&Standard::Algorithms::Numbers::substring_of_minimum_positive_sum, test, is_random);

            if constexpr (mps_measure_time)
            {
                const auto fast_elapsed = tim3.elapsed();

                const auto better_ratio = ::Standard::Algorithms::ratio_compute(better_elapsed, slow_elapsed);

                const auto better_ratio2 = ::Standard::Algorithms::ratio_compute(better_elapsed2, better_elapsed);

                const auto ratio = ::Standard::Algorithms::ratio_compute(fast_elapsed, better_elapsed2);

                std::cout << " mps " << test.data.size() << " size, slow " << slow_elapsed << "; bet " << better_elapsed
                          << ", slow/bet " << better_ratio << "; b2 " << better_elapsed2 << ", bet/b2 " << better_ratio2
                          << "; fast " << fast_elapsed << " ns, b2/fast " << ratio << "\r\n";
            }
        }
        catch (const std::exception &exc)
        {
            assert(name != nullptr);

            std::string error(exc.what());
            error += " in ";
            error += name;
            error += "\n test: ";
            error += to_string(test);

            throw std::runtime_error(error);
        }
    }

    void simple_mps()
    {
        std::array tests{ mps_test_case{ "empty" },
            mps_test_case{ "no positive", // NOLINTNEXTLINE
                { 0, -100 } },
            mps_test_case{ "one number", // NOLINTNEXTLINE
                { 600 }, { 0, 1, 600 } },
            mps_test_case{ "all positive", // NOLINTNEXTLINE
                { 600, 700, 50, 1'000 }, { 2, 3, 50 } },
            mps_test_case{ "several positive large negative", // NOLINTNEXTLINE
                { 600, 700, 50, -1'000 }, { 2, 3, 50 } },
            mps_test_case{ "several positive small negative", // NOLINTNEXTLINE
                { -10, 600, 50, 700 }, { 2, 3, 50 } },
            mps_test_case{ "several positive small negative near", // NOLINTNEXTLINE
                { 600, -10, 50, 700 }, { 1, 3, 40 } },
            mps_test_case{ "several positives plus several negatives", // NOLINTNEXTLINE
                { 200, -20, 10, 50, -32, -1'200 }, { 1, 5, 8 } },
            mps_test_case{ "all 3 numbers", // NOLINTNEXTLINE
                { -10, 40, -5 }, { 0, 3, 25 } },
            mps_test_case{ "all 6 numbers", // NOLINTNEXTLINE
                { -10, 600, 50, 700, -1'270, -65 }, { 0, 6, 5 } } };

        for (auto &test : tests)
        {
            run_mps(test);
        }
    }

    template<class vector_t>
    void append_random(vector_t &vec, const std::uint32_t size)
    {
        Standard::Algorithms::require_positive(size, "append size");

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        for (std::uint32_t index{}; index < size; ++index)
        {
            vec.push_back(rnd());
        }
    }

    void random_mps()
    {
        constexpr auto some_positive = std::numeric_limits<std::int32_t>::max();

        mps_test_case test{ "random_mps", { some_positive } };

        constexpr auto thou = mps_measure_time ? 10'000U : (Standard::Algorithms::is_debug ? 50U : 1'000U);

        append_random(test.data, thou - 1U);
        run_mps(test, true);

        if constexpr (mps_measure_time)
        {
            constexpr auto size = 500;

            for (std::int32_t index{}; index < size; ++index)
            {
                append_random(test.data, thou);
                run_mps(test, true);
            }
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::substring_of_minimum_positive_sum_tests()
{
    simple_mps();
    random_mps();
}
