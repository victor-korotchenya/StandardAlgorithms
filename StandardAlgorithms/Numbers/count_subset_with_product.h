#pragma once
#include <cassert>
#include <unordered_map>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class int_t = int, class long_int_t = int64_t,
        int_t mod = 1000 * 1000 * 1000 + 7>
        int_t count_subset_with_product_they_rec(
            const std::vector<int_t>& ar,
            const int_t product,
            const int_t i,
            std::vector<std::vector<int_t>>& buf,
            const long_int_t cur_prod = 1)
    {
        assert(product > 1 && cur_prod > 0 && 0 == product % cur_prod);
        if (i < 0)
            return cur_prod == product;

        assert(i >= 0 && i < static_cast<int_t>(buf.size()) && static_cast<int_t>(buf.size()) && cur_prod > 0 && cur_prod < static_cast<int_t>(buf[0].size()));
        auto& count = buf[i][cur_prod];
        if (count < 0)
        {
            const auto prod2 = cur_prod * static_cast<long_int_t>(ar[i]),
                rem = product % prod2;
            const int_t included = rem ? 0 : count_subset_with_product_they_rec(
                ar, product, i - 1, buf, prod2);
            const auto excluded = count_subset_with_product_they_rec(
                ar, product, i - 1, buf, cur_prod);

            count = included + excluded;
            if (count >= mod)
                count -= mod;
        }
        assert(count >= 0 && count < mod);
        return count;
    }

    template <class int_t, int_t mod>
    int_t count_subset_with_product1(const std::vector<int_t>& ar)
    {
        int_t p2 = 1;
        for (const auto& a : ar)
        {
            assert(a > 0);
            if (a == 1)
            {
                p2 <<= 1;
                if (p2 >= mod)
                    p2 -= mod;
                assert(p2 >= 0 && p2 < mod);
            }
        }

        int_t count = p2 - 1;
        if (p2 < 0)
            p2 += mod;
        assert(count >= 0 && count < mod);
        return count;
    }

    template <class int_t, class long_int_t, int_t mod>
    void process_divisor(
        // Divisor - count.
        std::unordered_map<int_t, int_t>& freqs,
        std::vector<std::pair<int_t, int_t>>& edited,
        const int_t product,
        const int_t a)
    {
        edited.clear();
        for (const auto& p : freqs)
        {
            const auto divisor = static_cast<long_int_t>(a) * p.first;
            assert(divisor > 0);
            if (product % divisor)
                continue;

            // Let freqs = {{3, 1}}, a = 2 - must avoid double adding 12, 24, etc.
            // 3*2 - add {6, 1},
            // then do 6*2 to wrongfully add {12, 1}.
            assert(divisor <= product);
            edited.push_back({ static_cast<int_t>(divisor), p.second });
        }

        for (const auto& p : edited)
        {
            const auto& divisor = p.first;
            assert(divisor > 0 && !(product % divisor) && divisor <= product && p.second >= 0 && p.second < mod);
            auto& f = freqs[divisor];
            f += p.second;

            if (f >= mod)
                f -= mod;
            assert(f >= 0 && f < mod);
        }

        auto& f = freqs[a];
        if (++f >= mod)
            f -= mod;
        assert(f >= 0 && f < mod);
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Count not empty subsets of positive ints, having product>0.
            template<class int_t, class long_int_t = int64_t,
                int_t mod = 1000 * 1000 * 1000 + 7>
                int_t count_subset_with_product(const std::vector<int_t>& ar, const int_t product)
            {
                static_assert(sizeof(int_t) <= sizeof(long_int_t) && mod >= 2);

                const auto size = static_cast<int_t>(ar.size());
                RequirePositive(size, "size");
                RequirePositive(product, "product");

                if (product == 1)
                    return count_subset_with_product1<int_t, mod>(ar);

                std::unordered_map<int_t, int_t> freqs;
                std::vector<std::pair<int_t, int_t>> edited;
                int_t p2 = 1;
                for (int_t i = 0; i < size; ++i)
                {
                    const auto& a = ar[i];
                    assert(a > 0);
                    if (a == 1)
                    {
                        p2 <<= 1;
                        if (p2 >= mod)
                            p2 -= mod;
                        assert(p2 >= 0 && p2 < mod);
                        continue;
                    }

                    if (!(product % a))
                        process_divisor<int_t, long_int_t, mod>(freqs, edited, product, a);
                }

                const auto count = freqs[product] * static_cast<long_int_t>(p2) % mod;
                assert(count >= 0 && count < mod);
                return static_cast<int_t>(count);
            }

            template<class int_t = int, class long_int_t = int64_t,
                int_t mod = 1000 * 1000 * 1000 + 7>
                int_t count_subset_with_product_slow(const std::vector<int_t>& ar, const int_t product)
            {
                static_assert(sizeof(int_t) <= sizeof(long_int_t) && mod >= 2);

                const auto size = static_cast<int_t>(ar.size());
                assert(size <= 20);
                RequirePositive(size, "size");
                RequirePositive(product, "product");

                constexpr auto one = long_int_t(1);
                const auto edge_mask = one << size;
                assert(edge_mask > 0);

                int_t count = 0;
                for (long_int_t mask = 1; mask < edge_mask; ++mask)
                {
                    long_int_t p = 1;
                    for (long_int_t i = 0; i < size; ++i)
                    {
                        if (!((mask >> i) & 1))
                            continue;

                        p *= ar[i];
                        assert(ar[i] > 0 && p > 0);
                        if (product % ar[i] || p > product)
                        {
                            p = 0;
                            break;
                        }
                    }

                    count += p == product;
                    if (count >= mod)
                        count -= mod;
                    assert(count >= 0 && count < mod);
                }
                return count;
            }

            template<class int_t = int, class long_int_t = int64_t,
                int_t mod = 1000 * 1000 * 1000 + 7>
                int_t count_subset_with_product_they(const std::vector<int_t>& ar, const int_t product)
            {
                static_assert(std::is_signed_v<int_t> && std::is_signed_v<long_int_t> && sizeof(int_t) <= sizeof(long_int_t) && mod >= 2);

                const auto size = static_cast<int_t>(ar.size());
                RequirePositive(size, "size");
                RequirePositive(product, "product");

                std::vector<std::vector<int_t>> buf(size,
                    std::vector<int_t>(product + 1, -int_t(1)));

                const auto count = count_subset_with_product_they_rec(ar, product, size - 1, buf);
                assert(count >= 0 && count < mod);
                return count;
            }
        }
    }
}