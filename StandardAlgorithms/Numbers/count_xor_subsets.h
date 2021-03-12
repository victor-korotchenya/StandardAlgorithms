#pragma once
#include"../Utilities/require_utilities.h"
#include"arithmetic.h"
#include<array>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an array, count subsets whose items XOR is equal to x.
    //
    // Time pseudo-polynomial O(n * max_value), space O(max_value).
    template<std::unsigned_integral int_t, int_t mod>
    requires(1U < mod)
    [[nodiscard]] constexpr auto count_xor_subsets(const std::vector<int_t> &arr, const int_t &xxx) -> int_t
    {
        if (arr.empty())
        {
            return xxx == 0U ? 1U : 0U;
        }

        const auto &largest = *std::max_element(arr.cbegin(), arr.cend());
        const auto max_value = xor_max(largest);

        if (const auto impossible = max_value < xxx; impossible)
        {
            return 0U;
        }

        using v_t = std::vector<int_t>;

        std::array<v_t, 2> buffers{ v_t(max_value + 1LLU), v_t(max_value + 1LLU) };

        buffers[0][0] = 1U;

        int_t pos{};

        for (const auto &value : arr)
        {
            const auto &prevs = buffers[pos];
            auto &curs = buffers[pos ^ 1U];

            std::copy(prevs.cbegin(), prevs.cend(), curs.begin());

            for (int_t prod{}; prod <= max_value; ++prod)
            {
                const auto &prev = prevs[prod];

                // todo(p2): Will it run faster on the newest CPUs, if the next block is removed?
                if (prev == 0U)
                {
                    continue;
                }

                const auto cand = static_cast<int_t>(value ^ prod);
                assert(cand <= max_value);

                auto &count = curs[cand];
                count += prev;

                if (!(count < mod))
                {
                    count -= mod;
                }

                assert(count < mod);
            }

            pos ^= 1U;
        }

        const auto &result = buffers.at(pos).at(xxx);
        assert(result < mod);

        return result;
    }

    template<std::unsigned_integral int_t, int_t mod>
    requires(1U < mod)
    [[nodiscard]] constexpr auto count_xor_subsets_slow_still(const std::vector<int_t> &arr, const int_t &xxx) -> int_t
    {
        if (arr.empty())
        {
            return xxx == 0U ? 1U : 0U;
        }

        const auto &largest = *std::max_element(arr.cbegin(), arr.cend());
        const auto max_value = xor_max(largest);

        if (const auto is_impossible = max_value < xxx; is_impossible)
        {
            return 0U;
        }

        using vect_pair = std::vector<std::pair<int_t, bool>>;

        std::array<vect_pair, 2> buffers{ vect_pair(max_value + 1LLU), vect_pair(max_value + 1LLU) };

        buffers[0][0] = { 1U, true };

        int_t pos{};

        for (const auto &value : arr)
        {
            const auto &prevs = buffers[pos];
            auto &curs = buffers[pos ^ 1U];

            std::copy(prevs.cbegin(), prevs.cend(), curs.begin());

            for (int_t prod{}; prod <= max_value; ++prod)
            {
                const auto &prev = prevs[prod];

                if (!prev.second)
                {
                    continue;
                }

                const auto cand = static_cast<int_t>(value ^ prod);
                assert(cand <= max_value);

                auto &cur = curs.at(cand);
                cur.second = true;

                auto &count = cur.first;
                count += prev.first;

                if (!(count < mod))
                {
                    count -= mod;
                }

                assert(count < mod);
            }

            pos ^= 1U;
        }

        const auto &result = buffers.at(pos).at(xxx).first;
        assert(result < mod);

        return result;
    }

    // Slow time O(2**n * n).
    template<std::unsigned_integral int_t, int_t mod>
    requires(1U < mod)
    [[nodiscard]] constexpr auto count_xor_subsets_slow(const std::vector<int_t> &arr, const int_t &xxx) -> int_t
    {
        const auto size = arr.size();
        {
            constexpr auto greatest = 21U;

            require_greater(greatest, size, "size");
        }

        using mask_t = std::uint32_t;

        const auto edge_mask = static_cast<mask_t>(1) << size;
        assert(0U < edge_mask);

        int_t result{};

        for (mask_t mask{}; mask < edge_mask; ++mask)
        {
            int_t prod{};

            for (mask_t index{}; index < size; ++index)
            {
                if (const auto bit = (mask >> index) & 1U; bit != 0U)
                {
                    prod ^= arr[index];
                }
            }

            if (prod == xxx && ++result == mod)
            {
                result = 0U;
            }
        }

        assert(result < mod);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
