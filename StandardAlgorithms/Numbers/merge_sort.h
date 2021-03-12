#pragma once
#include"merge.h"
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    constexpr auto profitable_merge_sort = 1LLU << // NOLINTNEXTLINE
        (is_merge_test ? 2U : 19U);

    static_assert(0U < profitable_merge_sort);

    constexpr void merge_sort_slow(
#ifndef __clang__
        const auto &policy,
#endif
        auto &arr, auto &buffer)
    {
        const auto size = static_cast<std::size_t>(arr.size());

        assert(1ZU < size && size == buffer.size());
        // [[assume(1ZU < size && size == buffer.size())]];

        for (std::uint64_t half = 1; half < size; half <<= 1U)
        {
            for (std::uint64_t lenin{};; lenin += half * 2U)
            {
                const auto med = lenin + half;

                if (size <= med)
                {
                    break;
                }

                // Whole stripes, and a partial stripe e.g. 2+1; or 4 + 1, 4 + 2, 4 + 3.
                const auto endd = std::min(lenin + half * 2U, size);

                slow_merge_util(
#ifndef __clang__
                    policy,
#endif
                    arr, std::make_pair(lenin, med), arr, std::make_pair(med, endd), buffer,
                    std::make_pair(lenin, endd));

                std::copy(
#ifndef __clang__
                    policy,
#endif
                    buffer.begin() + lenin, buffer.begin() + endd, arr.begin() + lenin);
            }
        }
    }

    constexpr void merge_sort_rec(auto arr, auto buffer // std::span needs no reference "&".
#if _DEBUG
        ,
        std::uint32_t depth = {}
#endif
    )
    {
#if _DEBUG
        {
            ++depth;

            assert(0U < depth && depth <= ::Standard::Algorithms::Utilities::stack_max_size);
        }
#endif

        const auto size = arr.size();

        assert(1ZU < size && buffer.size() == size);
        // [[assume(1ZU < size && buffer.size() == size)]];

        const auto med = size >> 1U;

        if (1ZU < med)
        {
            auto uno = arr.subspan(0U, med);
            auto duo = buffer.subspan(0U, med);

            merge_sort_rec(uno, duo
#if _DEBUG
                ,
                depth
#endif
            );
        }

        if (2ZU < size)
        {
            auto uno = arr.subspan(med);
            auto duo = buffer.subspan(med);

            merge_sort_rec(uno, duo
#if _DEBUG
                ,
                depth
#endif
            );
        }

        {
            auto uno = arr.subspan(0ZU, med);
            auto duo = arr.subspan(med);

            slow_merge_util(
#ifndef __clang__
                std::execution::seq,
#endif
                uno, std::make_pair(0ZU, uno.size()), duo, std::make_pair(0ZU, duo.size()), buffer,
                std::make_pair(0ZU, size));

            std::copy(buffer.begin(), buffer.end(), arr.begin());
        }
    }

    template<
#ifndef __clang__
        class policy_t,
#endif
        class arr_t, class buffer_t>
    void parallel_merge_sort_uti(
#ifndef __clang__
        const policy_t &policy,
#endif
        arr_t arr, buffer_t buffer
#if _DEBUG
        ,
        std::uint32_t depth = {}
#endif
    )
    {
#if _DEBUG
        {
            ++depth;

            assert(0U < depth && depth <= ::Standard::Algorithms::Utilities::stack_max_size);
        }
#endif

        const auto size = arr.size();

        assert(1ZU < size && buffer.size() == size);
        // [[assume(1ZU < size && buffer.size() == size)]];

        const auto med = size >> 1U;
        const auto [shall_run_left, shall_run_right] =
            Inner::shall_run_in_parallel<profitable_merge_sort>(med, size - med);

        using future_t = std::future<void>;
        std::array<std::pair<bool, future_t>, 2> flags_handles{};

        if (shall_run_left)
        {
            auto uno = arr.subspan(0U, med);
            auto duo = buffer.subspan(0U, med);

            auto &flag_handle = flags_handles[0];
            flag_handle.first = true;

            flag_handle.second = std::async(std::launch::async,
                parallel_merge_sort_uti<
#ifndef __clang__
                    policy_t,
#endif
                    arr_t, buffer_t>,
#ifndef __clang__
                policy,
#endif
                uno, duo
#if _DEBUG
                ,
                depth
#endif
            );
        }

        if (shall_run_right)
        {
            auto uno = arr.subspan(med);
            auto duo = buffer.subspan(med);

            auto &flag_handle = flags_handles[1];
            flag_handle.first = true;

            flag_handle.second = std::async(std::launch::async,
                parallel_merge_sort_uti<
#ifndef __clang__
                    policy_t,
#endif
                    arr_t, buffer_t>,
#ifndef __clang__
                policy,
#endif
                uno, duo
#if _DEBUG
                ,
                depth
#endif
            );
        }

        if (!shall_run_left && 1ZU < med)
        {
            auto uno = arr.subspan(0U, med);
            auto duo = buffer.subspan(0U, med);

            merge_sort_slow(
#ifndef __clang__
                policy,
#endif
                uno, duo);
        }

        if (!shall_run_right && 2ZU < size)
        {
            auto uno = arr.subspan(med);
            auto duo = buffer.subspan(med);

            merge_sort_slow(
#ifndef __clang__
                policy,
#endif
                uno, duo);
        }

        // todo(p2): What if it throws up before?
        wait_all<2>(flags_handles);

        {
            auto uno = arr.subspan(0ZU, med);
            auto duo = arr.subspan(med);

            merge_util_parallel
#ifndef __clang__
                <policy_t>
#endif
                (
#ifndef __clang__
                    policy,
#endif
                    uno, duo, buffer
#if _DEBUG
                    ,
                    depth
#endif
                );

            std::copy(
#ifndef __clang__
                policy,
#endif
                buffer.begin(), buffer.end(), arr.begin());
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Merge sort is stable.
    constexpr void merge_sort(auto &arr, auto &buffer)
    {
        const auto size = static_cast<std::size_t>(arr.size());
        if (size <= 1ZU)
        {
            return;
        }

        buffer.resize(size);
        Inner::merge_sort_slow(
#ifndef __clang__
            std::execution::seq,
#endif
            arr, buffer);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(arr.begin(), arr.end()));
        }
    }

    constexpr void merge_sort(auto &arr)
    {
        const auto size = static_cast<std::size_t>(arr.size());

        if (size <= 1ZU)
        {
            return;
        }

        using raw = decltype(arr[0]);
        using typ = std::remove_cvref_t<raw>;

        std::vector<typ> buffer(size);
        merge_sort(arr, buffer);
    }

    constexpr void merge_sort_recursive(auto &arr, auto &buffer)
    {
        const auto size = static_cast<std::size_t>(arr.size());

        if (size <= 1ZU)
        {
            return;
        }

        buffer.resize(size);

        std::span arr_span = arr;
        std::span buffer_span = buffer;

        Inner::merge_sort_rec(arr_span, buffer_span);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(arr.begin(), arr.end()));
        }
    }

    constexpr void merge_sort_recursive(auto &arr)
    {
        const auto size = static_cast<std::size_t>(arr.size());

        if (size <= 1ZU)
        {
            return;
        }

        using raw = decltype(arr[0]);
        using typ = std::remove_cvref_t<raw>;

        std::vector<typ> buffer(size);
        merge_sort_recursive(arr, buffer);
    }

    // Small sizes (e.g. n <= 32) can be insert sorted which should be faster. todo(p4): Ensure that it is so.
    // Idea 1. Recursively split the input into 2 parts, wait until both are completed, call parallel merge of both.
    // Idea 2. Sort n/2 parts of size 2; then sort n/4 parts of size 4; ..;
    // finally parallel merge the last two parts of sizes {n/2, n - n/2}.
    // Anyway, the number of the running tasks should not be too large.
#ifndef __clang__
    template<class policy_t =
                 std::execution::parallel_unsequenced_policy> // todo(p4): review when Clang is ready for parallelism.
#endif
    void parallel_merge_sort(auto &arr, auto &buffer
#ifndef __clang__
        ,
        const policy_t &policy = std::execution::par_unseq
#endif
    )
    {
        const auto size = static_cast<std::size_t>(arr.size());

        if (size <= 1ZU)
        {
            return;
        }

        buffer.resize(size);

        std::span arr_span = arr;
        std::span buffer_span = buffer;

        Inner::parallel_merge_sort_uti
#ifndef __clang__
            <policy_t>
#endif
            (
#ifndef __clang__
                policy,
#endif
                arr_span, buffer_span);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(std::is_sorted(arr.begin(), arr.end()));
        }
    }

    // 4M items - about 1.7 times faster than std::sort.
    // 8M - 2.4.
    // 128M - 2.8 times faster, not very scalable.
#ifndef __clang__
    template<class policy_t =
                 std::execution::parallel_unsequenced_policy> // todo(p4): review when Clang is ready for parallelism.
#endif
    void parallel_merge_sort(auto &arr
#ifndef __clang__
        ,
        const policy_t &policy = std::execution::par_unseq
#endif
    )
    {
        const auto size = static_cast<std::size_t>(arr.size());

        if (size <= 1ZU)
        {
            return;
        }

        using raw = decltype(arr[0]);
        using typ = std::remove_cvref_t<raw>;

        std::vector<typ> buffer(size);

        parallel_merge_sort
#ifndef __clang__
            <policy_t>
#endif
            (arr, buffer
#ifndef __clang__
                ,
                policy
#endif
            );
    }
} // namespace Standard::Algorithms::Numbers
