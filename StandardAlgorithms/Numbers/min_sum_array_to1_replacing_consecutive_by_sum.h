#pragma once
#include"../Utilities/check_size.h"
#include"../Utilities/project_constants.h"
#include"../Utilities/require_utilities.h"
#include"prefix_sum.h"
#include<cstdio> // puts
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::signed_integral long_int_t>
    [[nodiscard]] constexpr auto min_sum_array_to1_slow_rec(const std::vector<long_int_t> &arr, const long_int_t takes)
        -> long_int_t
    {
        constexpr auto inf = static_cast<long_int_t>(std::numeric_limits<long_int_t>::max() / 2);
        constexpr auto minus_inf = -inf;

        static_assert(minus_inf < long_int_t{} && long_int_t{} < inf);

        const auto size = static_cast<long_int_t>(arr.size());
        assert(2 <= takes && takes <= size);

        auto result = inf;

        std::vector<long_int_t> buffers(size - takes + 1);

        for (long_int_t start{}; start <= size - takes; ++start)
        {
            // todo(p4): avoid extra copies?
            std::copy(arr.cbegin(), arr.cbegin() + start + 1, buffers.begin());

            for (long_int_t mid = 1; mid < takes; ++mid)
            {
                buffers[start] += arr[start + mid];
            }

            auto sum = buffers[start];
            if (takes < size)
            {
                for (auto index = start + 1; index <= size - takes; ++index)
                {// todo(p4): std::copy
                    buffers[index] = arr[index + takes - 1];
                }

                auto part = min_sum_array_to1_slow_rec<long_int_t>(buffers, takes);

                assert(minus_inf < sum && sum < inf);
                sum += part;
            }

            assert(minus_inf < sum && sum < inf);

            result = std::min(result, sum);
        }

        assert(minus_inf < result && result < inf);

        return result;
    }

    template<std::signed_integral long_int_t>
    constexpr void min_sum_ar_process_interval(const long_int_t takes, const std::vector<long_int_t> &prefix_sums,
        long_int_t start, std::vector<std::vector<long_int_t>> &buffers, const bool is_indivisible,
        const long_int_t stop_incl, const bool is_log)
    {
        constexpr auto inf = static_cast<long_int_t>(std::numeric_limits<long_int_t>::max() / 2);

        constexpr auto minus_inf = -inf;

        static_assert(minus_inf < long_int_t{} && long_int_t{} < inf);

        assert(2 <= takes && long_int_t{} <= start && start < stop_incl && start + takes - 1 <= stop_incl);

        assert(static_cast<std::uint64_t>(stop_incl) < buffers.size());

        auto &best = buffers[start][stop_incl];
        best = inf;

        for (auto mid = start; mid < stop_incl;
             // Merge only those intervals that can be merged.
             // E.g. {a} and {b, c, d, e} cannot be merged directly when "takes" is 3.
             // So, just adding +1 is wrong.
             mid += static_cast<long_int_t>(takes - 1))
        {
            const auto &first = buffers[start][mid];
            const auto &second = buffers[mid + 1][stop_incl];

            const auto cand = static_cast<long_int_t>(first + second);
            assert(std::max({ first, second, cand }) < inf);
            assert(minus_inf < std::min({ first, second, cand }));

            // todo(p3): think negative sometimes e.g. the sum might get sort of negative infinity - check elsewhere.

            if (is_log)
            {
                if (const auto is_first_merge = takes - 1 == stop_incl - start; !is_first_merge)
                {
                    const auto msg = "   partial [" + std::to_string(start) + ", " + std::to_string(mid) + "] [" +
                        std::to_string(mid + 1) + ", " + std::to_string(stop_incl) + "] = " + std::to_string(first) +
                        " + " + std::to_string(second) + " = " + std::to_string(cand) + "; better " +
                        std::to_string(cand < best);

                    std::puts(msg.c_str());
                }
            }

            best = std::min(best, cand);
        }

        assert(minus_inf < best && best < inf);

        if (is_indivisible)
        {
            if (is_log)
            {
                static const auto *const empty_line = "";

                std::puts(empty_line);
            }

            return;
        }

        const auto add = calculate_substring_sum<long_int_t>(
            static_cast<std::size_t>(start), prefix_sums, static_cast<std::size_t>(stop_incl + 1));

        assert(minus_inf < add && add < inf);

        if (is_log)
        {
            const auto msg = "  best [" + std::to_string(start) + "][" + std::to_string(stop_incl) +
                "] = " + std::to_string(best) + " + " + std::to_string(add) + " = " + std::to_string(best + add);

            std::puts(msg.c_str());
        }

        best += add;

        assert(minus_inf < best && best < inf);
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given n>0 integers, do until 1 number remains:
    // choose takes>1 consecutive numbers and replace them by their sum.
    // Return min sum, or -1 if impossible.
    // Sample: arr {3, 5, 1, 2, 6}, takes 3; min answer 25.
    // - Merge first three: {3+5+1, 2, 6} = {9, 2, 6} = {9+2+6} = {17}. Sum: (3+5+1) + (9+2+6) = 9 + 17 = 26.
    // - Merge middle three: {3, 5+1+2, 6} = {3, 8, 6} = {3+8+6} = {17}. Sum = (5+1+2) + (3+8+6) = 8 + 17 = 25.
    // - Merge last three: {3, 5, 1+2+6} = {3, 5, 9} = {3+5+9} = {17}. Sum = (1+2+6) + (3+5+9) = 9 + 17 = 26.
    // Time O(n*n*takes), space O(n*n).
    // todo(p4): do overflow checks.
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto min_sum_array_to1_replacing_consecutive_by_sum(
        const std::vector<int_t> &arr, const long_int_t &takes) -> long_int_t
    {
        const auto size = Standard::Algorithms::Utilities::check_size<long_int_t>("size", arr.size());
        require_positive(size, "size");

        constexpr long_int_t one = 1;

        require_greater(takes, one, "takes");

        if (size == one)
        {
            return arr[0];
        }

        if (const auto rem = (size - one) % (takes - one); rem != long_int_t{})
        {
            return -one;
        }

        assert(takes <= size);

        std::vector<long_int_t> prefix_sums;
        prefix_sums.reserve(size + one);
        build_prefix_sum(arr.cbegin(), arr.cend(), prefix_sums);

        std::vector<std::vector<long_int_t>> buffers(size, std::vector<long_int_t>(size, long_int_t{}));

        auto is_log = false;
        if (is_log)
        {
            // This function name, or func name.
            const auto *const func_name = static_cast<const char *>(__FUNCTION__);

            const auto msg =
                std::string(func_name) + "(size " + std::to_string(size) + ", takes " + std::to_string(takes) + ")";

            std::puts(msg.c_str());
        }

        for (auto interval_length = takes; interval_length <= size; ++interval_length)
        {
            const auto is_indivisible = ((interval_length - one) % (takes - one)) != long_int_t{};

            if (is_log)
            {
                const auto msg = " interval length " + std::to_string(interval_length) + ", is divisible " +
                    std::to_string(!is_indivisible);

                std::puts(msg.c_str());
            }

            for (long_int_t start{}; start <= size - interval_length; ++start)
            {
                const auto stop_incl = static_cast<long_int_t>(start + interval_length - one);

                Inner::min_sum_ar_process_interval<long_int_t>(
                    takes, prefix_sums, start, buffers, is_indivisible, stop_incl, is_log);
            }
        }

        constexpr auto inf = static_cast<long_int_t>(std::numeric_limits<long_int_t>::max() / 2);

        constexpr auto minus_inf = -inf;

        static_assert(minus_inf < long_int_t{} && long_int_t{} < inf);

        const auto &result = buffers.front().back();
        assert(minus_inf < result && result < inf);

        return result;
    }

    // Slow time O(n!).
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto min_sum_array_to1_replacing_consecutive_by_sum_slow(
        const std::vector<int_t> &arr, const long_int_t &takes) -> long_int_t
    {
        require_less_equal(
            arr.size(), static_cast<std::size_t>(Standard::Algorithms::Utilities::factorial_max_slow), "size");

        const auto size = Standard::Algorithms::Utilities::check_size<int_t>("size", arr.size());
        require_positive(size, "size");

        constexpr long_int_t one = 1;

        require_greater(takes, one, "takes");

        if (size == one)
        {
            return arr[0];
        }

        if (const auto rem = (size - one) % (takes - one); rem != long_int_t{})
        {
            return -one;
        }

        assert(takes <= size);

        const std::vector<long_int_t> buffers(arr.cbegin(), arr.cend());

        auto result = Inner::min_sum_array_to1_slow_rec<long_int_t>(buffers, takes);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
