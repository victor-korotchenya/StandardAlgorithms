#include"prefix_sum_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"prefix_sum.h"
#include<iostream>

namespace // Small test of build_prefix_sum
{
    using int_t = std::int16_t;
    using long_int_t = std::int32_t;

    static_assert(sizeof(int_t) <= sizeof(long_int_t));

    [[nodiscard]] auto consteval slow_test(const std::vector<int_t> &source, const std::vector<long_int_t> &expected)
        -> bool
    {
        if (expected.empty())
        {
            return false;
        }

        std::vector<long_int_t> result_sums;

        Standard::Algorithms::Numbers::build_prefix_sum(source.cbegin(), source.cend(), result_sums);

        if (expected != result_sums)
        {
            return false;
        }

        result_sums.assign(expected.size(), long_int_t{});

        std::inclusive_scan(source.cbegin(), source.cend(), result_sums.begin() + 1);

        auto gut = expected == result_sums;
        return gut;
    }

    static_assert(slow_test({}, { 0 }));
    static_assert(!slow_test({}, {}));
    static_assert(!slow_test({ 0 }, {}));
    static_assert(!slow_test( // NOLINTNEXTLINE
        { 10 }, { 0, 20 }));
    static_assert(slow_test( // NOLINTNEXTLINE
        { 10 }, { 0, 10 }));
    static_assert(slow_test( // NOLINTNEXTLINE
        { 10 }, { 0, 10 }));
    static_assert(slow_test( // NOLINTNEXTLINE
        { 10, 20 }, { 0, 10, 30 }));
    static_assert(slow_test( // NOLINTNEXTLINE
        { 10, 20, 50 }, { 0, 10, 30, 80 }));
    static_assert(slow_test( // NOLINTNEXTLINE
        { 10, 20, 50, -30 }, { 0, 10, 30, 80, 50 }));
    static_assert(slow_test( // NOLINTNEXTLINE
        { 10, 20, 50, -30, 40 }, { 0, 10, 30, 80, 50, 90 }));
} // namespace

namespace
{
    constexpr auto shall_print_prefix_sum_report = false;

    constexpr int_t max_value = 1000;
    constexpr int_t min_value = -max_value;
    static_assert(min_value < max_value);

    constexpr auto profitable_prefix_sum = Standard::Algorithms::Numbers::Inner::profitable_prefix_sum;

    constexpr auto min_size = 0U;
    constexpr auto pes_shift = 4U;
    constexpr auto max_size = profitable_prefix_sum << pes_shift;
    static_assert(min_size < max_size);

    void prefix_sum_test_once(
        std::vector<long_int_t> &slow_buffer, const std::vector<int_t> &source, std::vector<long_int_t> &par_buffer)
    {
        const Standard::Algorithms::elapsed_time_ns tim;
        Standard::Algorithms::Numbers::build_prefix_sum(source.cbegin(), source.cend(), slow_buffer);
        [[maybe_unused]] const auto elapsed = tim.elapsed();

        Standard::Algorithms::throw_if_empty("build_prefix_sum result", slow_buffer);

        const Standard::Algorithms::elapsed_time_ns par_tim;
        {
            const std::span source_spa = source;

            Standard::Algorithms::Numbers::parallel_prefix_sum(source_spa, par_buffer);
        }
        [[maybe_unused]] const auto par_elapsed = par_tim.elapsed();

        ::Standard::Algorithms::ert::are_equal(slow_buffer, par_buffer, "parallel_prefix_sum");

        if constexpr (shall_print_prefix_sum_report)
        {
            const auto ration = ::Standard::Algorithms::ratio_compute(par_elapsed, elapsed);

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << " size " << source.size();
            str << ", build_prefix_sum " << elapsed;
            str << ", parallel_prefix_sum " << par_elapsed;
            str << ", par/seq " << ration << "\n";

            std::cout << str.str();
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::prefix_sum_tests()
{
    using largest_t = std::int64_t;

    static_assert(sizeof(long_int_t) <= sizeof(largest_t));

    Standard::Algorithms::Utilities::random_t<largest_t> rnd(min_value, max_value);

    std::vector<long_int_t> temp(max_size + 3);
    std::vector<long_int_t> temp_2(max_size + 2);

    constexpr auto max_attemps = 1;

    for (std::int32_t attempt{}; attempt < max_attemps; ++attempt)
    {
        const auto source = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

        try
        {
            prefix_sum_test_once(temp, source, temp_2);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Source" << source << ", error: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }
}
