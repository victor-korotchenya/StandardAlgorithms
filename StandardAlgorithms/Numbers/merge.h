#pragma once
#include"../Utilities/is_debug.h"
#include"../Utilities/project_constants.h"
#include"sorted_arrays_median_element.h"
#include<algorithm> // is_sorted
#include<array>
#include<cassert>
#include<cstddef>
#include<cstdint>
#include<execution>
#include<future> // async
#include<span>
#include<utility> // pair

namespace Standard::Algorithms::Numbers::Inner
{
    using merge_range_t = std::pair<std::size_t, std::size_t>;

    [[nodiscard]] inline constexpr auto compute_double_limit(std::uint64_t limit) noexcept -> std::uint64_t
    {
        limit = std::max<std::uint64_t>(limit, 1);

        auto dou = (limit << 1U) | 1LLU;
        if (dou < limit)
        {
            dou |= limit;
        }

        assert(limit <= dou);

        return dou;
    }

    constexpr auto is_merge_test = true;

    constexpr std::uint64_t profitable_merge = 1LLU << // NOLINTNEXTLINE
        (is_merge_test ? 2U : 20U);

    static_assert(0U < profitable_merge);

    template<std::uint64_t min_parallel_size, std::uint64_t double_size = compute_double_limit(min_parallel_size)>
    requires(0U < min_parallel_size && min_parallel_size < double_size)
    [[nodiscard]] constexpr auto shall_run_in_parallel(
        const std::uint64_t left_size, const std::uint64_t right_size) noexcept -> std::pair<bool, bool>
    {
        if (left_size < profitable_merge && right_size < profitable_merge)
        {
            return { false, false };
        }

        if (double_size <= left_size && double_size <= right_size)
        {
            return { true, true };
        }

        const auto is_comrad = left_size < right_size;
        return { !is_comrad, is_comrad };
    }

    template<std::uint64_t min_parallel_size>
    requires(0U < min_parallel_size)
    [[nodiscard]] constexpr auto shall_run_in_parallel(const std::pair<std::size_t, std::size_t> &left_eq,
        const std::size_t left_size, const std::pair<std::size_t, std::size_t> &right_eq,
        const std::size_t right_size) noexcept -> std::pair<bool, bool>
    {
        if (const auto gut = left_eq.first <= left_eq.second && left_eq.second <= left_size &&
                right_eq.first <= right_eq.second && right_eq.second <= right_size;
            !gut) [[unlikely]]
        {// Bad inout (-.
            return { false, false };
        }

        const auto less_excess = std::min(left_eq.first, right_eq.first);
        const auto right_excess = std::min(left_size - left_eq.second, right_size - right_eq.second);

        auto res = shall_run_in_parallel<min_parallel_size>(less_excess, right_excess);
        return res;
    }

    template<std::size_t size, class future_t = std::future<void>>
    requires(0U < size)
    constexpr void wait_all(std::array<std::pair<bool, future_t>, size> &flags_handles)
    {
        std::string errors;

        for (auto &[shall_run, honey] : flags_handles)
        {
            if (!shall_run)
            {
                continue;
            }

            try
            {
                honey.wait();
            }
            catch (const std::exception &exc)
            {
                errors += "Wait error: ";
                errors += exc.what();
                errors += "\n";
            }
        }

        if (!errors.empty()) [[unlikely]]
        {
            throw std::runtime_error(errors);
        }
    }

    constexpr void copy_both(
#ifndef __clang__
        const auto &policy,
#endif
        const auto &arr_one, const merge_range_t &range_one, const auto &arr_two, const merge_range_t &range_two,
        auto &dest, const merge_range_t &range_dest)
    {
        assert(range_one.first <= range_one.second && range_one.second <= static_cast<std::size_t>(arr_one.size()));

        assert(range_two.first <= range_two.second && range_two.second <= static_cast<std::size_t>(arr_two.size()));

        assert(range_dest.first <= range_dest.second && range_dest.second <= static_cast<std::size_t>(dest.size()));

        assert(range_one.second - range_one.first + range_two.second - range_two.first ==
            range_dest.second - range_dest.first);

        std::copy(
#ifndef __clang__
            policy,
#endif
            arr_one.begin() + range_one.first, arr_one.begin() + range_one.second, dest.begin() + range_dest.first);

        auto dest_iter = dest.begin() + range_dest.first + range_one.second - range_one.first;

        std::copy(
#ifndef __clang__
            policy,
#endif
            arr_two.begin() + range_two.first, arr_two.begin() + range_two.second, dest_iter);
    }

    constexpr void slow_merge_util(
#ifndef __clang__
        const auto &policy,
#endif
        const auto &aaa, merge_range_t range_1, const auto &bbb, merge_range_t range_2, auto &dest,
        merge_range_t range_dest)
    {
        assert(range_1.first < range_1.second && range_1.second <= static_cast<std::size_t>(aaa.size()));

        assert(range_2.first < range_2.second && range_2.second <= static_cast<std::size_t>(bbb.size()));

        assert(range_dest.first < range_dest.second && range_dest.second <= static_cast<std::size_t>(dest.size()));

        assert(range_1.second - range_1.first + range_2.second - range_2.first == range_dest.second - range_dest.first);

        {
            const auto &front = bbb[range_2.first];
            const auto &back = aaa[range_1.second - 1ZU];

            if (!(front < back))
            {// aaa {1, 2, 9, 10, 10} + bbb {10, 20, 30, 40}
                copy_both(
#ifndef __clang__
                    policy,
#endif
                    aaa, range_1, bbb, range_2, dest, range_dest);

                return;
            }
        }
        {
            const auto &front = aaa[range_1.first];
            const auto &back = bbb[range_2.second - 1ZU];

            if (back < front)
            {// aaa {10, 20, 30, 40} + bbb {1, 2, 9}
                copy_both(
#ifndef __clang__
                    policy,
#endif
                    bbb, range_2, aaa, range_1, dest, range_dest);

                return;
            }
        }

        for (;;)
        {
            const auto &aaa_value = aaa[range_1.first];
            const auto &bbb_value = bbb[range_2.first];
            auto &dest_value = dest[range_dest.first];
            ++range_dest.first;

            const auto is_bbb_less = bbb_value < aaa_value;
            {
                const auto &val = is_bbb_less ? bbb_value : aaa_value;
                dest_value = val;
            }

            auto &range = is_bbb_less ? range_2 : range_1;
            if (++range.first == range.second)
            {
                break;
            }
        }

        std::copy(
#ifndef __clang__
            policy,
#endif
            aaa.begin() + range_1.first, aaa.begin() + range_1.second, dest.begin() + range_dest.first);

        const auto dest_iter = dest.begin() + range_dest.first + range_1.second - range_1.first;

        std::copy(
#ifndef __clang__
            policy,
#endif
            bbb.begin() + range_2.first, bbb.begin() + range_2.second, dest_iter);
    }

    template<
#ifndef __clang__
        class policy_t,
#endif
        class a_t, class b_t, class dest_t>
    constexpr void merge_util_parallel(
#ifndef __clang__
        const policy_t &policy,
#endif
        const a_t aaa, const b_t bbb, dest_t dest, std::uint32_t depth = {})
    {
        if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
        {
            throw std::runtime_error("Too deep stack in merge_util_parallel.");
        }

        assert(!aaa.empty() && !bbb.empty() && aaa.size() + bbb.size() == dest.size());

        {
            const auto &front = bbb[0];
            const auto &back = aaa.back();

            if (!(front < back))
            {// aaa {1, 2, 9, 10, 10} + bbb {10, 20, 30, 40}
                copy_both(
#ifndef __clang__
                    policy,
#endif
                    aaa, std::make_pair(0ZU, aaa.size()), bbb, std::make_pair(0ZU, bbb.size()), dest,
                    std::make_pair(0ZU, dest.size()));

                return;
            }
        }
        {
            const auto &front = aaa[0];
            const auto &back = bbb.back();

            if (back < front)
            {// aaa {10, 20, 30, 40} + bbb {1, 2, 9}
                copy_both(
#ifndef __clang__
                    policy,
#endif
                    bbb, std::make_pair(0ZU, bbb.size()), aaa, std::make_pair(0ZU, aaa.size()), dest,
                    std::make_pair(0ZU, dest.size()));

                return;
            }
        }

        const auto [aaa_eq, bbb_eq] = sorted_arrays_median_element(aaa, bbb);

        const auto [shall_run_less, shall_run_gre] =
            Inner::shall_run_in_parallel<profitable_merge>(aaa_eq, aaa.size(), bbb_eq, bbb.size());

        using future_t = std::future<void>;
        std::array<std::pair<bool, future_t>, 2> flags_handles{};

        if (shall_run_less)
        {
            a_t aaa_sub = aaa.subspan(0ZU, aaa_eq.first);
            b_t bbb_sub = bbb.subspan(0ZU, bbb_eq.first);
            dest_t dest_sub = dest.subspan(0ZU, aaa_eq.first + bbb_eq.first);

            auto &flag_handle = flags_handles[0];
            flag_handle.first = true;

            flag_handle.second = std::async(std::launch::async,
                merge_util_parallel<
#ifndef __clang__
                    policy_t,
#endif
                    a_t, b_t, dest_t>,
#ifndef __clang__
                policy,
#endif
                aaa_sub, bbb_sub, dest_sub, depth);
        }

        const auto dest_skip_gre = aaa_eq.second + bbb_eq.second;

        if (shall_run_gre)
        {
            a_t aaa_sub = aaa.subspan(aaa_eq.second, aaa.size() - aaa_eq.second);
            b_t bbb_sub = bbb.subspan(bbb_eq.second, bbb.size() - bbb_eq.second);
            dest_t dest_sub = dest.subspan(dest_skip_gre, dest.size() - dest_skip_gre);

            auto &flag_handle = flags_handles[1];
            flag_handle.first = true;

            flag_handle.second = std::async(std::launch::async,
                merge_util_parallel<
#ifndef __clang__
                    policy_t,
#endif
                    a_t, b_t, dest_t>,
#ifndef __clang__
                policy,
#endif
                aaa_sub, bbb_sub, dest_sub, depth);
        }

        if (shall_run_less)
        {
        }
        else if (0ZU < aaa_eq.first && 0ZU < bbb_eq.first)
        {// Small sizes.
            slow_merge_util(
#ifndef __clang__
                policy,
#endif
                aaa, std::make_pair(0ZU, aaa_eq.first), bbb, std::make_pair(0ZU, bbb_eq.first), dest,
                std::make_pair(0ZU, aaa_eq.first + bbb_eq.first));
        }
        else if (0ZU < aaa_eq.first)
        {
            std::copy(
#ifndef __clang__
                policy,
#endif
                aaa.begin(), aaa.begin() + aaa_eq.first, dest.begin());
        }
        else if (0ZU < bbb_eq.first)
        {
            std::copy(
#ifndef __clang__
                policy,
#endif
                bbb.begin(), bbb.begin() + bbb_eq.first, dest.begin());
        }

        if (shall_run_gre)
        {
        }
        else if (aaa_eq.second < aaa.size() && bbb_eq.second < bbb.size())
        {// Small sizes.
            slow_merge_util(
#ifndef __clang__
                policy,
#endif
                aaa, std::make_pair(aaa_eq.second, aaa.size()), bbb, std::make_pair(bbb_eq.second, bbb.size()), dest,
                std::make_pair(dest_skip_gre, dest.size()));
        }
        else if (aaa_eq.second < aaa.size())
        {
            std::copy(
#ifndef __clang__
                policy,
#endif
                aaa.begin() + aaa_eq.second, aaa.end(), dest.begin() + dest_skip_gre);
        }
        else if (bbb_eq.second < bbb.size())
        {
            std::copy(
#ifndef __clang__
                policy,
#endif
                bbb.begin() + bbb_eq.second, bbb.end(), dest.begin() + dest_skip_gre);
        }

        // {<a} + {<b};   {==a} + {==b};   {>a} + {>b}

        // Copy equal
        if (aaa_eq.first < aaa_eq.second)
        {
            std::copy(
#ifndef __clang__
                policy,
#endif
                aaa.begin() + aaa_eq.first, aaa.begin() + aaa_eq.second, dest.begin() + aaa_eq.first + bbb_eq.first);
        }

        if (bbb_eq.first < bbb_eq.second)
        {
            std::copy(
#ifndef __clang__
                policy,
#endif
                bbb.begin() + bbb_eq.first, bbb.begin() + bbb_eq.second, dest.begin() + aaa_eq.second + bbb_eq.first);
        }

        // todo(p4): Optimize: when there are not many equal items. Try step = 1, then step=2, .., 2**x, .., 2, 1.

        // todo(p2): What if it throws up before?
        wait_all<2>(flags_handles);
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Stable merge.
    // todo(p3): It often runs faster than std::merge - why?
    constexpr void merge(const auto &aaa, const auto &bbb, auto &dest)
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(aaa.begin(), aaa.end()) && std::is_sorted(bbb.begin(), bbb.end()));
        }

        const auto size_a = static_cast<std::size_t>(aaa.size());
        const auto size_b = static_cast<std::size_t>(bbb.size());
        dest.resize(size_a + size_b);

        if (0 == size_a)
        {
            std::copy(bbb.begin(), bbb.end(), dest.begin());
            return;
        }

        if (0 == size_b)
        {
            std::copy(aaa.begin(), aaa.end(), dest.begin());
            return;
        }

        Inner::slow_merge_util(
#ifndef __clang__
            std::execution::seq,
#endif
            aaa, std::make_pair(0ZU, size_a), bbb, std::make_pair(0ZU, size_b), dest,
            std::make_pair(0ZU, size_a + size_b));

        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(dest.begin(), dest.end()));
        }
    }

    // 1M items, 10K..100K profitable_merge - almost no guaranteed profit.
    // 10M items, 100K profitable_merge - often 5 times faster than merge.
#ifndef __clang__
    template<class policy_t =
                 std::execution::parallel_unsequenced_policy> // todo(p4): review when Clang is ready for parallelism.
#endif
    void parallel_merge(const auto &aaa, const auto &bbb, auto &dest
#ifndef __clang__
        ,
        const policy_t &policy = std::execution::par_unseq
#endif
    )
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(aaa.begin(), aaa.end()) && std::is_sorted(bbb.begin(), bbb.end()));
        }

        const auto size_a = static_cast<std::size_t>(aaa.size());
        const auto size_b = static_cast<std::size_t>(bbb.size());
        dest.resize(size_a + size_b);

        if (0 == size_a)
        {
            std::copy(
#ifndef __clang__
                policy,
#endif
                bbb.begin(), bbb.end(), dest.begin());

            return;
        }

        if (0 == size_b)
        {
            std::copy(
#ifndef __clang__
                policy,
#endif
                aaa.begin(), aaa.end(), dest.begin());

            return;
        }

        const std::span aaa_span = aaa;
        const std::span bbb_span = bbb;
        std::span dest_span = dest;

        Inner::merge_util_parallel
#ifndef __clang__
            <policy_t>
#endif
            (
#ifndef __clang__
                policy,
#endif
                aaa_span, bbb_span, dest_span);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(dest.begin(), dest.end()));
        }
    }
} // namespace Standard::Algorithms::Numbers
