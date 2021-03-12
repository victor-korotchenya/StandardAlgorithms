#pragma once
#include<algorithm> // copy
#include<cassert>
#include<cstddef>
#include<cstdint>
#include<execution>
#include<future> // async
#include<numeric> // inclusive_scan
#include<span>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    constexpr auto is_prefix_sum_test = true;

    constexpr std::uint64_t profitable_prefix_sum = 1LLU << // NOLINTNEXTLINE
        (is_prefix_sum_test ? 2U : 22U);

    static_assert(0U < profitable_prefix_sum);
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given numbers, allow computing a continuous range (substring) sum in O(1) after O(n) pre-calculation time.
    // long_int_t must be large enough not to overflow.
    // Time O(n).
    // For example, { 10, 20, 50, -30, 40 } will have
    // the sums { 0, 10, 30, 80, 50, 90 }.
    template<class iter_t, class iter_t_2, class long_int_t>
    requires(sizeof(decltype(*std::declval<iter_t>())) == sizeof(decltype(*std::declval<iter_t_2>())) &&
        sizeof(decltype(*std::declval<iter_t>())) <= sizeof(long_int_t))
    constexpr void build_prefix_sum(iter_t first, iter_t_2 last, std::vector<long_int_t> &result_sums)
    {
        result_sums.clear();

        for (long_int_t sum{};;)
        {
            result_sums.push_back(sum);

            if (first == last)
            {
                return;
            }

            const auto &value = *first;
            sum += static_cast<long_int_t>(value);
            ++first;
        }
    }

    // Time O(1).
    template<class int_t>
    [[nodiscard]] constexpr auto calculate_substring_sum(
        const std::size_t first_pos, const std::vector<int_t> &sums, const std::size_t last_pos) -> int_t
    {
        assert(first_pos <= last_pos && last_pos < sums.size());

        const auto &first = sums.at(first_pos);
        const auto &last = sums.at(last_pos);
        auto sum = static_cast<int_t>(last - first);
        return sum;
    }

    // todo(tormoz): It is 3 times slower than the sequential version on 250M items.
    // todo(Cuda idem my): Then, try to use a +-scan in a quick sort partitioning - will it be faster than std::sort?
    template<
#ifndef __clang__
        class policy_t = std::execution::parallel_unsequenced_policy,
    // todo(p4): review when Clang is ready for parallelism.
#endif
        class int_t, class long_int_t, std::size_t extent = std::dynamic_extent,
        std::size_t long_extent = std::dynamic_extent>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    void parallel_prefix_sum(const std::span<int_t, extent> source, std::span<long_int_t, long_extent> result_sums
#ifndef __clang__
        ,
        const policy_t &policy = std::execution::par_unseq
#endif
    )
    {
        const auto size = source.size();
        if (size + 1LLU != result_sums.size()) [[unlikely]]
        {
            throw std::invalid_argument("result_sums size is wrong");
        }

        // Given 64 numbers A[64] = { 010, 020, 030, .. , 640 },
        // prepend 0: B = { 000 } + A = { 000, 010, 020, 030, .. , 640 }.
        result_sums[0] = {};

        std::copy(
#ifndef __clang__
            policy,
#endif
            source.begin(), source.end(), result_sums.begin() + 1);

        // Sum previous 01: B[i] += B[i-01];   B[02] += B[01]; B[04] += B[03]; B[06] += B[05]; .. ; B[64] += B[63].
        // Sum previous 02: B[i] += B[i-02];   B[04] += B[02]; B[08] += B[06]; B[12] += B[10]; .. ; B[64] += B[62].
        // Sum previous 04: B[i] += B[i-04];   B[08] += B[04]; B[16] += B[12]; B[24] += B[20]; .. ; B[64] += B[60].
        // Sum previous 08: B[i] += B[i-08];   B[16] += B[08]; B[32] += B[24]; B[48] += B[40]; B[64] += B[56].
        // Sum previous 16: B[i] += B[i-16];   B[32] += B[16]; B[64] += B[48].
        // Sum previous 32: B[i] += B[i-32];   B[64] += B[32].
        // Sum previous 64: stop, nothing to add.
        std::uint64_t prev_count = 1;

        for (; prev_count * 2U <= size; prev_count *= 2U)
        {
            const auto approx_count = size / 2U / prev_count;

            if (const auto is_sequential = approx_count < Inner::profitable_prefix_sum; is_sequential)
            {
                for (auto pos = prev_count * 2U; pos <= size; pos += prev_count * 2U)
                {
                    result_sums[pos] += result_sums[pos - prev_count];
                }

                continue;
            }

#pragma omp parallel for default(none) shared(size, prev_count, result_sums)
            for (auto pos = prev_count * 2U; pos <= size; pos += prev_count * 2U)
            {
                result_sums[pos] += result_sums[pos - prev_count];
            }
        }

        // Sum previous 32: B[i] += B[i-32];   too large index.
        // Sum previous 16: B[i] += B[i-16];   B[48] += B[32].
        // Sum previous 08: B[i] += B[i-08];   B[24] += B[16]; B[40] += B[32]; B[56] += B[48].
        // Sum previous 04: B[i] += B[i-04];   B[12] += B[08]; B[20] += B[16]; B[28] += B[24]; .. ; B[60] += B[56].
        // Sum previous 02: B[i] += B[i-02];   B[06] += B[04]; B[10] += B[08]; B[14] += B[12]; .. ; B[62] += B[60].
        // Sum previous 01: B[i] += B[i-01];   B[03] += B[02]; B[05] += B[04]; B[07] += B[06]; .. ; B[63] += B[62].
        assert(0U < prev_count);

        while (0U < (prev_count /= 2U))
        {
            const auto initial_pos = prev_count * 3U;

            if (const auto approx_count = size < initial_pos ? 0U : (size - initial_pos) / 2U / prev_count;
                approx_count < Inner::profitable_prefix_sum)
            {
                for (auto pos = initial_pos; pos <= size; pos += prev_count * 2U)
                {
                    result_sums[pos] += result_sums[pos - prev_count];
                }

                continue;
            }

#pragma omp parallel for default(none) shared(size, prev_count, initial_pos, result_sums)
            for (auto pos = initial_pos; pos <= size; pos += prev_count * 2U)
            {
                result_sums[pos] += result_sums[pos - prev_count];
            }
        }
    }

    // todo(tormoz): Make faster.
    template<
#ifndef __clang__
        class policy_t = std::execution::parallel_unsequenced_policy,
    // todo(p4): review when Clang is ready for parallelism.
#endif
        class int_t, class long_int_t, std::size_t extent = std::dynamic_extent,
        std::size_t long_extent = std::dynamic_extent>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    constexpr void parallel_prefix_sum(const std::span<int_t, extent> source, std::vector<long_int_t> &result_sums
#ifndef __clang__
        ,
        const policy_t &policy = std::execution::par_unseq
#endif
    )
    {
        // todo(p4): review when std::vector::assign is parallel.
        result_sums.resize(source.size() + 1ZU);

        std::span result_sums_spa = result_sums;

        parallel_prefix_sum<
#ifndef __clang__
            policy_t,
#endif
            int_t, long_int_t, extent>(source, result_sums_spa
#ifndef __clang__
            ,
            policy
#endif
        );
    }
} // namespace Standard::Algorithms::Numbers
