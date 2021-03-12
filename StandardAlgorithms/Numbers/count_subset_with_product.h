#pragma once
#include"../Utilities/require_utilities.h"
#include<unordered_map>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::signed_integral long_int_t, std::signed_integral int_t, int_t mod>
    requires(sizeof(int_t) <= sizeof(long_int_t) && 2 <= mod)
    [[nodiscard]] constexpr auto count_subset_with_product_other_rec(const int_t &product,
        const std::vector<int_t> &arr, const int_t &index, std::vector<std::vector<int_t>> &buffer,
        const long_int_t &cur_prod = 1) -> int_t
    {
        assert(0 < product && 0 < cur_prod && 0 == product % cur_prod);

        if (index < 0)
        {
            return cur_prod == product ? 1 : 0;
        }

        assert(0 <= index && index < static_cast<long_int_t>(buffer.size()) &&
            cur_prod < static_cast<long_int_t>(buffer[0].size()));

        auto &count = buffer[index][cur_prod];
        if (count < 0)
        {
            const auto excluded =
                count_subset_with_product_other_rec<long_int_t, int_t, mod>(product, arr, index - 1, buffer, cur_prod);

            const auto prod_2 = static_cast<long_int_t>(cur_prod * arr[index]);
            assert(cur_prod <= prod_2 && arr[index] <= prod_2);

            const auto rem = product % prod_2;

            const auto included = 0 < rem
                ? int_t{}
                : count_subset_with_product_other_rec<long_int_t, int_t, mod>(product, arr, index - 1, buffer, prod_2);

            count = included + excluded;

            if (!(count < mod))
            {
                count -= mod;
            }
        }

        assert(!(count < 0) && count < mod);

        return count;
    }

    template<std::integral int_t, int_t mod>
    requires(2 <= mod)
    [[nodiscard]] constexpr auto count_subset_with_product1(const std::vector<int_t> &arr) -> int_t
    {
        int_t pow2 = 1;

        for (const auto &number : arr)
        {
            assert(int_t{} < number);

            if (number != 1)
            {
                continue;
            }

            pow2 <<= 1U;

            if (!(pow2 < mod))
            {
                pow2 -= mod;
            }

            assert(int_t{} <= pow2 && pow2 < mod);
        }

        // Remove the empty set.
        if (pow2 == int_t{})
        {
            pow2 += mod - 1;
        }
        else
        {
            --pow2;
        }

        assert(int_t{} <= pow2 && pow2 < mod);

        return pow2;
    }

    template<std::signed_integral long_int_t, std::signed_integral int_t, int_t mod>
    requires(sizeof(int_t) <= sizeof(long_int_t) && 2 <= mod)
    constexpr void process_divisor(
        // Divisor - count.
        std::unordered_map<int_t, int_t> &freqs, const int_t &product, std::vector<std::pair<int_t, int_t>> &edited,
        const int_t &number)
    {
        edited.clear();

        for (const auto &freq : freqs)
        {
            const auto divisor = static_cast<long_int_t>(number) * freq.first;
            assert(0 < divisor);

            if (product % divisor != 0)
            {
                continue;
            }

            // Let freqs = {{3, 1}}, number = 2 - must avoid double adding 12, 24, etc.
            // 3*2 - add {6, 1},
            // then do 6*2 to wrongfully add {12, 1}.
            assert(divisor <= product);

            edited.push_back({ static_cast<int_t>(divisor), freq.second });
        }

        for (const auto &par : edited)
        {
            const auto &divisor = par.first;
            const auto &cnt = par.second;

            assert(0 < divisor && 0 == product % divisor && divisor <= product && 0 <= cnt && cnt < mod);

            auto &count = freqs[divisor];
            count += cnt;

            if (!(count < mod))
            {
                count -= mod;
            }

            assert(0 <= count && count < mod);
        }

        auto &counts = freqs[number];

        if (!(++counts < mod))
        {
            counts -= mod;
        }

        assert(0 <= counts && counts < mod);
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Count not empty subsets of positive integers, having product>0.
    template<std::signed_integral long_int_t, std::signed_integral int_t, int_t mod>
    requires(sizeof(int_t) <= sizeof(long_int_t) && 2 <= mod)
    [[nodiscard]] constexpr auto count_subset_with_product(const std::vector<int_t> &arr, const int_t &product) -> int_t
    {
        require_positive(static_cast<int_t>(arr.size()), "size");
        require_positive(product, "product");

        if (product == 1)
        {
            return Inner::count_subset_with_product1<int_t, mod>(arr);
        }

        std::unordered_map<int_t, int_t> freqs;
        std::vector<std::pair<int_t, int_t>> edited;
        int_t pow2 = 1;

        for (const auto &number : arr)
        {
            assert(0 < number);

            if (number == 1)
            {
                pow2 <<= 1U;

                if (!(pow2 < mod))
                {
                    pow2 -= mod;
                }

                assert(0 <= pow2 && pow2 < mod);

                continue;
            }

            if (0 == product % number)
            {
                Inner::process_divisor<long_int_t, int_t, mod>(freqs, product, edited, number);
            }
        }

        auto count = static_cast<int_t>(freqs[product] * static_cast<long_int_t>(pow2) % mod);

        assert(0 <= count && count < mod);

        return count;
    }

    template<std::signed_integral long_int_t, std::signed_integral int_t, int_t mod>
    requires(sizeof(int_t) <= sizeof(long_int_t) && 2 <= mod)
    [[nodiscard]] constexpr auto count_subset_with_product_slow(const std::vector<int_t> &arr, const int_t &product)
        -> int_t
    {
        const auto size = require_positive(static_cast<int_t>(arr.size()), "size");

        {
            constexpr auto greatest = 21;
            require_greater(greatest, size, "size too large");
        }

        require_positive(product, "product");

        constexpr long_int_t one = 1;

        const auto edge_mask = require_positive(one << size, "edge mask");

        int_t count{};

        for (long_int_t mask = 1; mask < edge_mask; ++mask)
        {
            long_int_t temp_prod = 1;

            for (long_int_t index{}; index < size; ++index)
            {
                if (((mask >> index) & 1) == 0)
                {
                    continue;
                }

                temp_prod *= arr[index];
                assert(0 < arr[index] && 0 < temp_prod);

                if (product < temp_prod || 0 != product % arr[index])
                {
                    temp_prod = 0;
                    break;
                }
            }

            count += temp_prod == product ? 1 : 0;

            if (!(count < mod))
            {
                count -= mod;
            }

            assert(0 <= count && count < mod);
        }

        return count;
    }

    template<std::signed_integral long_int_t, std::signed_integral int_t, int_t mod>
    requires(sizeof(int_t) <= sizeof(long_int_t) && 2 <= mod)
    [[nodiscard]] constexpr auto count_subset_with_product_other(const std::vector<int_t> &arr, const int_t &product)
        -> int_t
    {
        const auto size = require_positive(static_cast<int_t>(arr.size()), "size");
        require_positive(product, "product");

        std::vector<std::vector<int_t>> buffer(size, std::vector<int_t>(product + 1LL, -static_cast<int_t>(1)));

        const auto count =
            Inner::count_subset_with_product_other_rec<long_int_t, int_t, mod>(product, arr, size - 1, buffer);

        assert(0 <= count && count < mod);

        return count;
    }
} // namespace Standard::Algorithms::Numbers
