#pragma once
#include"logarithm.h"
#include"permutation.h"
#include<span>

namespace Standard::Algorithms::Numbers::Inner
{
    // Values at even indexes are moved to the beginning while keeping the order.
    //
    // Let size = 8,
    // {0, 1, 2, 3, 4, 5, 6, 7} source ->
    // {0, 2, 4, 6, 1, 3, 5, 7} destination.
    // {0, 2, 4, 6,   ,   ,   ,   } destination even numbers.
    // {  ,   ,   ,   , 1, 3, 5, 7} destination odd numbers.
    //
    // {0, 4, 1, 5, 2, 6, 3, 7} permutation to go from source to destination.
    // {0,   , 1,   , 2,   , 3,   } permutation, even i, value = i/2.
    // {  , 4,  ,  5,   , 6,   , 7} permutation, odd i, value = (size - 1)/2 + (i + 1)/2 = 3 + (i + 1)/2.
    //
    //
    // Let size = 9,
    // {0, 1, 2, 3, 4, 5, 6, 7, 8} source ->
    // {0, 2, 4, 6, 8, 1, 3, 5, 7} destination.
    // {0, 2, 4, 6, 8,   ,   ,   ,   } destination even numbers.
    // {  ,   ,   ,   ,   , 1, 3, 5, 7} destination odd numbers.
    //
    // {0, 5, 1, 6, 2, 7, 3, 8, 4} permutation to go from source to destination.
    // {0,   , 1,   , 2,   , 3,   , 4} permutation, even i, value = i/2.
    // {  , 5,   , 6,   , 7,   , 8,   } permutation, odd i, value = (size - 1)/2 + (i + 1)/2 = 4 + (i + 1)/2.
    template<std::integral int_t>
    constexpr auto even_begin_odd_end_position(const int_t &index, const int_t &size) -> int_t
    {
        assert(int_t{} <= index && index < size);

        constexpr int_t one = 1;
        constexpr int_t two = 2;

        const auto is_odd = (one & index) != int_t{};

        auto value = static_cast<int_t>((is_odd ? (size - one) / two : int_t{}) + (index + one) / two);

        assert(int_t{} <= value && value < size);

        return value;
    }

    // Let size = 8, compute the inverse.
    //
    // {0, 2, 4, 6,   ,   ,   ,   } source even numbers.
    // {  ,   ,   ,   , 1, 3, 5, 7} source odd numbers.
    // {0, 2, 4, 6, 1, 3, 5, 7} source ->
    // {0, 1, 2, 3, 4, 5, 6, 7} destination.
    //
    // {0, 2, 4, 6, 1, 3, 5, 7} permutation to go from source to destination == source.
    // {0, 2, 4, 6,   ,   ,   ,   } permutation, i <= (size - 1)/2 = 3, value = 2*i.
    // {  ,   ,   ,   , 1, 3, 5, 7} permutation, i > (size - 1)/2 = 3, value = 2*i - size + ((size - 1) & 1) = 2*i - 7.
    //
    //
    // Let size = 9, compute the inverse.
    // {0, 2, 4, 6, 8,   ,   ,   ,   } source even numbers.
    // {  ,   ,   ,   ,   , 1, 3, 5, 7} source odd numbers.
    // {0, 2, 4, 6, 8, 1, 3, 5, 7} source ->
    // {0, 1, 2, 3, 4, 5, 6, 7, 8} destination.
    //
    // {0, 2, 4, 6, 8, 1, 3, 5, 7} permutation to go from source to destination == source.
    // {0, 2, 4, 6, 8,   ,   ,   ,   } permutation, i <= (size - 1)/2 = 4, value = 2*i.
    // {  ,   ,   ,   ,   , 1, 3, 5, 7} permutation, i > (size - 1)/2 = 4, value = 2*i - size + ((size - 1) & 1) = 2*i
    // - 9.
    template<std::integral int_t>
    constexpr auto even_begin_odd_end_position_inverse(const int_t &index, const int_t &size) -> int_t
    {
        assert(int_t{} <= index && index < size);

        constexpr int_t one = 1;
        constexpr int_t two = 2;

        // Can be computed once (CSE, common subexpression evaluation).
        const auto edge = static_cast<int_t>((size - one) / two);

        auto value = static_cast<int_t>((edge < index ? ((size - one) & one) - size : int_t{}) + index * two);

        assert(int_t{} <= value && value < size);

        return value;
    }

    template<class int_t, std::size_t value_extent, class func_t,
        // Here 0 goes to 0; start with initial_index.
        int_t initial_index = 1>
    constexpr void permute_by_func(std::span<int_t, value_extent> arr, std::vector<bool> &buffer, func_t &&func)
    {
        const auto size = static_cast<int_t>(arr.size());
        assert(int_t{} < size);

        buffer.assign(size, false);

        for (auto index = initial_index; index < size; ++index)
        {
            for (auto pre = index;;)
            {
                if (buffer[pre])
                {
                    break;
                }

                buffer[pre] = true;

                const auto curr = func(pre, size);
                assert(int_t{} <= curr && curr < size);

                std::swap(arr[index], arr[curr]);

                pre = curr;
            }
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // 011101, count=6, return 101110.
    // 101110, count=6, return 011101.
    // It is an involution: reverse_bits(reverse_bits(x, b), b) == x.
    [[nodiscard]] inline constexpr auto reverse_bits(
        const std::uint64_t value, const std::unsigned_integral auto bit_count) noexcept -> std::uint64_t
    {// todo(p4): A faster lookup table.
        {
            [[maybe_unused]] constexpr auto max_bits = sizeof(std::uint64_t) << 3U;

            assert(0U < bit_count && bit_count <= max_bits);
        }

        {
            [[maybe_unused]] const auto max_bit = static_cast<std::uint32_t>(std::bit_width(value));

            assert(max_bit <= bit_count);
        }

        std::uint64_t reversed{};

        for (std::uint32_t ind{}; ind < bit_count; ++ind)
        {
            const auto bit1 = (value >> ind) & 1LLU;
            const auto rev_ind = bit_count - 1U - ind;
            reversed |= bit1 << rev_ind;
        }

        return reversed;
    }

    template<class item_t, std::size_t value_extent>
    constexpr void bit_reverse_permute(std::span<item_t, value_extent> arr)
    {
        static_assert(sizeof(std::size_t) <= sizeof(std::uint64_t));

        const auto size = arr.size();
        const auto log_2 = static_cast<std::uint32_t>(log_base_2_up(size));

        for (std::size_t ind = 1; ind < size; ++ind)
        {
            const auto rev_ind = reverse_bits(ind, log_2);
            if (rev_ind < ind)
            {
                std::swap(arr[rev_ind], arr[ind]);
            }
        }
    }

    // The 0-based permutation will store negative values after the call.
    template<std::signed_integral int_t, std::size_t perm_extent, class value_t, std::size_t value_extent>
    constexpr void apply_permutation(
        std::span<int_t, perm_extent> perm, const int_t &size, std::span<value_t, value_extent> arr)
    {
        assert(int_t{} < size && static_cast<std::uint64_t>(size) <= std::min(arr.size(), perm.size()));

        assert(!perm.empty() && int_t{} <= perm[0] && int_t{} <= perm.back());

        {
            [[maybe_unused]] constexpr auto is_zero_start = true;

            assert(is_permutation_slow2(perm, is_zero_start));
        }

        for (int_t index{}; index < size; ++index)
        {
            for (auto pre = index;;)
            {
                const auto curr = perm[pre];

                if (curr < int_t{})
                {
                    break;
                }

                std::swap(arr[index], arr[curr]);

                perm[pre] -= size;
                assert(perm[pre] < int_t{});

                pre = curr;
            }
        }
    }

    template<class perm_int_t, std::size_t perm_extent, // perm_int_t might be 'const std::int32_t' - a class in needed.
        class int_t, class value_t, std::size_t value_extent>
    requires(std::is_signed_v<perm_int_t> == std::is_signed_v<int_t>)
    constexpr void apply_permutation2(const std::span<perm_int_t, perm_extent> perm, const int_t size,
        std::span<value_t, value_extent> arr, std::vector<bool> &buffer)
    {
        assert(static_cast<std::uint64_t>(size) <= std::min(arr.size(), perm.size()));

        assert(!perm.empty() && int_t{} < size && perm_int_t{} <= perm[0] && perm_int_t{} <= perm.back());

        {
            [[maybe_unused]] constexpr auto is_zero_start = true;

            assert(is_permutation_slow2(perm, is_zero_start));
        }

        buffer.assign(size, false);

        for (int_t index{}; index < size; ++index)
        {
            for (auto pre = index;;)
            {
                if (buffer[pre])
                {
                    break;
                }

                buffer[pre] = true;

                const auto &curr = perm[pre];
                assert(int_t{} <= curr && curr < size);

                std::swap(arr[index], arr[curr]);
                pre = curr;
            }
        }
    }

    // size = 8,
    // {0, 1, 2, 3, 4, 5, 6, 7} source ->
    // {0, 2, 4, 6, 1, 3, 5, 7} destination.
    //
    // (0), (1, 2, 4), (3, 6, 5), (7) - cycles.
    //
    //
    // size = 16,
    // {00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15} source ->
    // {00, 02, 04, 06, 08, 10, 12, 14, 01, 03, 05, 07, 09, 11, 13, 15} destination.
    //
    // (0), (1, 2, 4, 8), (3, 6, 12, 9), (5, 10), (7, 14, 13, 11), (15) - cycles.
    // todo(p3): O(1) space, O(size) time? It might be needed for FFT, where size is a power of 2.
    template<class int_t, std::size_t value_extent>
    constexpr void even_begin_odd_end_permute(std::span<int_t, value_extent> arr, std::vector<bool> &buffer)
    {
        Inner::permute_by_func<int_t, value_extent>(arr, buffer, Inner::even_begin_odd_end_position<int_t>);
    }

    template<class int_t, std::size_t value_extent>
    constexpr void even_begin_odd_end_permute_inverse(std::span<int_t, value_extent> arr, std::vector<bool> &buffer)
    {
        Inner::permute_by_func<int_t, value_extent>(arr, buffer, Inner::even_begin_odd_end_position_inverse<int_t>);
    }
} // namespace Standard::Algorithms::Numbers
