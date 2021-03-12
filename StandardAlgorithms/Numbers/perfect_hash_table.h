#pragma once
#include"../Utilities/is_debug.h"
#include"../Utilities/random.h"
#include"../Utilities/run_until_success.h"
#include<algorithm>
#include<bit>
#include<cstddef>
#include<cstdint>
#include<stdexcept>
#include<type_traits>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given n distinct constant keys,
    // the perfect read-only static hashing table uses 2 probes/trials to search in O(1).
    template<class key_t>
    struct perfect_hash_table final
    {
        using uint_t = std::uint64_t;

        static_assert(sizeof(std::size_t) == sizeof(uint_t), "todo(p4): smaller sizes.");

        static constexpr uint_t zero{};
        static constexpr uint_t one = 1U;

        explicit constexpr perfect_hash_table(const std::vector<key_t> &data, const bool shall_shrink = false) noexcept(
            false)
            : results(compute_results(data, shall_shrink))
        {
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept
        {
            return results.first_coef.offset == zero;
        }

        [[nodiscard]] constexpr auto contains(const key_t &key, const uint_t &key_hash) const noexcept -> bool
        {
            if (is_empty())
            {
                return false;
            }

            assert(!results.buckets.empty() && zero < results.first_coef.offset);

            const auto ha_1 = carter_wegman_hash(key_hash, results.first_coef, results.first_coef.offset);

            assert(ha_1 < results.first_coef.offset &&
                ha_1 + store_last_coefficient_sentinel < results.coefficients.size());

            const auto &coef = results.coefficients[ha_1];

            if (const auto empty_slot = coef.mult == zero; empty_slot)
            {
                return false;
            }

            const auto &next_offset = results.coefficients[ha_1 + store_last_coefficient_sentinel].offset;

            assert(coef.offset < next_offset && zero < coef.mult);

            const auto mod_2 = next_offset - coef.offset;
            assert(zero < mod_2);

            auto ha_2 = carter_wegman_hash(key_hash, coef, mod_2);
            assert(ha_2 < mod_2);

            ha_2 += coef.offset;
            assert(ha_2 < results.used_bits.size());

            auto has = results.used_bits[ha_2] && results.buckets[ha_2] == key;
            return has;
        }

        template<class key_t_2 = key_t>
        [[nodiscard]] constexpr auto contains(const key_t_2 &key) const noexcept -> typename std::enable_if_t<
            std::is_nothrow_convertible_v<key_t_2, key_t> && std::is_nothrow_convertible_v<key_t, uint_t>, bool>
        {
            const auto &key_2 = static_cast<key_t>(key);
            const auto key_hash_2 = static_cast<uint_t>(key_2);

            auto has = contains(key_2, key_hash_2);
            return has;
        }

private:
        using rand_t = Standard::Algorithms::Utilities::random_t<uint_t>;

        struct coefficient_offset final
        {
            uint_t mult{};
            uint_t add{};
            uint_t offset{};
        };

        struct hash_results final
        {
            std::vector<key_t> buckets{};
            std::vector<bool> used_bits{};
            std::vector<coefficient_offset> coefficients{};
            coefficient_offset first_coef{};
        };

        struct temp_context final
        {
            const std::vector<key_t> &data;
            std::vector<std::vector<std::size_t>> indexes{};
            rand_t rnd{ one, mersenne_prime - one };
        };

        // Return 0U on failure.
        [[nodiscard]] static constexpr auto key_gen(rand_t &rnd1) noexcept -> uint_t
        {
            uint_t key{};

            const std::function<bool()> func = [&rnd1, &key]()
            {
                auto cand = rnd1();
                const auto tze = std::countr_zero(cand);
                cand >>= static_cast<uint_t>(tze);

                assert(!(tze < 0) && zero < cand && cand < mersenne_prime);

                constexpr auto large_k = static_cast<uint_t>(one << 30U);

                if (large_k < cand)
                {
                    key = cand;
                    return true;
                }

                return false;
            };

            if (::Standard::Algorithms::Utilities::run_until_success(func, "key_gen", silent_on_fail))
            {
                assert(zero < key && key < mersenne_prime);
            }

            return key;
        }

        // Universal hashing: ((a*x + b) % prime) % mod.
        // Here a > 0; mod can be not prime. Here are prime*(prime - 1) functions.
        //
        // Let n>0, y be unsigned integers, and w = 2**n = 1 << n.
        // Note that when n>0, then w >= 2; that is (w-1) > 0.
        // Now, y = (y / w)*w + (y % w) = q*w + r,
        // where q = y / w = y >> n, the highest bits,
        // r = y % w = y & (w - 1), the n lowest bits.
        // So, y = (y >> n)*w + (y & (w - 1)).
        //
        // Let's Mersenne prime be mp = (w - 1).
        // Compute everything modulo mp:
        // y = (y >> n)*(w) + (y & (w - 1))
        // y = (y >> n)*(w - 1 + 1) + (y & mp) =
        // y = (y >> n)*(0 + 1) + (y & mp) =
        // y = (y >> n) + (y & mp).
        // Thus, y modulo mp = ((y >> n) + (y & mp)) modulo mp.
        //
        // For 64-bit ints, n>=40 would be large enough so that (y >> n) were less than mp.
        // For any n>0, (y & mp) <= mp.
        // Adding 2 numbers:
        // - first strictly less than mp,
        // - and second maybe mp,
        // produces a number, which is strictly less than 2*mp.
        // Why? Because shifting, OR-ing are faster than division.
        [[nodiscard]] static constexpr auto carter_wegman_hash(
            const uint_t xxx, const coefficient_offset &coef, const uint_t mod) noexcept -> uint_t
        {
            const auto &mult = coef.mult;
            const auto &add = coef.add;

            assert(zero < mult && mult < mersenne_prime && zero != (mult & one));
            assert(!(add < zero) && add < mersenne_prime);
            assert(zero < mod && mod < mersenne_prime);

            const auto yyy = mult * xxx + add;
            auto hash = (yyy >> shift) + (yyy & mersenne_prime);
            // Here, hash < 2*mersenne_prime.

            if (!(hash < mersenne_prime))
            {
                hash -= mersenne_prime;
                assert(hash < mersenne_prime);
            }

            hash %= mod;
            return hash;
        }

        static constexpr void calc_offset(const std::vector<key_t> &data, hash_results &result) noexcept(false)
        {
            auto &offset = result.first_coef.offset;
            {// todo(p3): maybe size * 1.5 for big n?
                const auto is_even1 = (data.size() & 1U) == 0U;
                offset = static_cast<uint_t>(data.size()) + (is_even1 ? one : zero);

                assert(zero < offset);
            }

            result.coefficients.resize(offset + store_last_coefficient_sentinel);
        }

        // Return 0 if failed.
        [[nodiscard]] static constexpr auto split_return_secondary_size(
            hash_results &result, temp_context &context) noexcept(false) -> uint_t
        {
            result.first_coef.mult = key_gen(context.rnd);
            if (result.first_coef.mult == zero)
            {
                return {};
            }

            result.first_coef.add = key_gen(context.rnd);
            if (result.first_coef.add == zero)
            {
                return zero;
            }

            assert(!context.data.empty() && zero < result.first_coef.mult && zero < result.first_coef.offset &&
                context.data.size() <= result.first_coef.offset);

            context.indexes.clear();
            context.indexes.resize(result.first_coef.offset);

            // The expected size of the secondary storage = n*2; increase it some more.
            const auto max_total_size = static_cast<uint_t>(result.first_coef.offset << 2U);
            assert(result.first_coef.offset < max_total_size);

            uint_t secondary_size{};

            for (std::size_t index{}; index < context.data.size(); ++index)
            {
                const auto &raw_value = context.data[index];

                // todo(p4): random hash func.
                const auto hash =
                    carter_wegman_hash(static_cast<uint_t>(raw_value), result.first_coef, result.first_coef.offset);

                assert(hash < result.first_coef.offset);

                auto &indexes = context.indexes[hash];
                secondary_size += (static_cast<uint_t>(indexes.size()) << one) | one;

                if (max_total_size < secondary_size)
                {
                    return {};
                }

                indexes.push_back(index);
            }

            assert(zero < secondary_size);
            return secondary_size;
        }

        static constexpr void sentinel_coefficient(hash_results &result, const std::size_t offset) noexcept
        {
            assert(0U < offset && !(result.coefficients.size() < store_last_coefficient_sentinel * 2LLU));

            auto &last = result.coefficients.back();
            last.mult = last.add = zero;
            last.offset = offset;
        }

        static constexpr void block_size_one(hash_results &result, temp_context &context, const std::size_t index,
            coefficient_offset &coef, std::size_t &offset) noexcept(false)
        {
            coef.mult = coef.add = one;

            const auto &raw_value = context.data.at(index);
            result.buckets.at(offset) = raw_value; // Might throw.
            result.used_bits[offset] = true;
            ++offset;
        }

        // todo(p3): compress < (n*n);
        [[nodiscard]] static constexpr auto block_size_many(hash_results &result, temp_context &context,
            coefficient_offset &coef, std::size_t &offset, const uint_t secondary_size,
            const std::vector<std::size_t> &indexes1) noexcept(false) -> bool
        {
            const auto mod = static_cast<uint_t>(indexes1.size()) * static_cast<uint_t>(indexes1.size());

            assert(!context.data.empty() && one < indexes1.size() && indexes1.size() < secondary_size);
            assert(one < mod && mod + offset <= result.used_bits.size());

            const std::function<bool()> func = [&result, &context, &indexes1, &coef, offset, mod]()
            {
                coef.mult = key_gen(context.rnd);
                if (coef.mult == zero)
                {
                    return false;
                }

                coef.add = key_gen(context.rnd);
                if (coef.add == zero)
                {
                    return false;
                }

                {
                    auto iter = result.used_bits.begin() + offset;
                    std::fill(iter, iter + mod, false);
                }

                for (const auto &index : indexes1)
                {
                    const auto &raw_value = context.data.at(index);
                    auto hash = carter_wegman_hash(static_cast<uint_t>(raw_value), coef, mod);
                    assert(hash < mod);

                    hash += offset;
                    assert(hash < result.used_bits.size() && hash < result.buckets.size());

                    if (result.used_bits[hash])
                    {
                        if (result.buckets[hash] == raw_value) [[unlikely]]
                        {
                            throw std::invalid_argument("There is a duplicate value at index " + std::to_string(index));
                        }

                        return false;
                    }

                    result.buckets[hash] = raw_value; // Might throw.
                    result.used_bits[hash] = true;
                }

                return true;
            };

            if (!::Standard::Algorithms::Utilities::run_until_success(func, "block_size_many", silent_on_fail))
            {
                return false;
            }

            offset += mod;
            assert(offset <= secondary_size);

            return true;
        }

        [[nodiscard]] static constexpr auto compute_results_one_attempt(
            hash_results &result, temp_context &context) noexcept(false) -> bool
        {
            const auto secondary_size = split_return_secondary_size(result, context);
            if (zero == secondary_size)
            {
                return false;
            }

            assert(result.first_coef.offset <= secondary_size + one);

            result.buckets.resize(secondary_size);
            result.used_bits.resize(secondary_size);

            std::size_t offset{};

            for (std::size_t index{}; index < context.indexes.size(); ++index)
            {
                const auto &indexes1 = context.indexes[index];
                auto &coef = result.coefficients[index];
                coef.offset = offset;

                if (indexes1.empty())
                {
                    coef.mult = coef.add = zero;
                    continue;
                }

                assert(offset < secondary_size);

                if (indexes1.size() == one)
                {
                    block_size_one(result, context, indexes1[0], coef, offset);
                    continue;
                }

                if (!block_size_many(result, context, coef, offset, secondary_size, indexes1))
                {
                    return false;
                }
            }

            sentinel_coefficient(result, offset);
            return true;
        }

        [[nodiscard]] static constexpr auto compute_results(
            const std::vector<key_t> &data, const bool shall_shrink) noexcept(false) -> hash_results
        {
            hash_results result{};

            if (data.empty())
            {
                assert(zero == result.first_coef.mult && zero == result.first_coef.offset);
                return result;
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                // todo(p4): assert unique;
                auto cop = data;
                std::sort(cop.begin(), cop.end());
                cop.erase(std::unique(cop.begin(), cop.end()), cop.end());

                assert(cop.size() == data.size());
            }

            calc_offset(data, result);
            temp_context context{ data };

            const std::function<bool()> func = [&result, &context]()
            {
                return compute_results_one_attempt(result, context);
            };

            if (!::Standard::Algorithms::Utilities::run_until_success(func, "compute_results", silent_on_fail))
                [[unlikely]]
            {
                auto err = "Failed to produce a perfect hash table of size " + std::to_string(data.size());

                throw std::runtime_error(err);
            }

            assert(zero < result.first_coef.mult);

            if (shall_shrink)
            {
                result.buckets.shrink_to_fit();
                result.used_bits.shrink_to_fit();
            }

            return result;
        }

        static constexpr auto shift = 61U;

        // A large Mersenne prime M61 = 2**61 - 1 = 2305843009213693951.
        // 2,305,843,009,213,693,951 in hex: 0x1fFfFfFfFfFfFfFf.
        static constexpr uint_t mersenne_prime = (one << shift) - one;

        // To know the first and last offsets in the results.buckets.
        static constexpr uint_t store_last_coefficient_sentinel = 1;
        static constexpr auto silent_on_fail = false;

        const hash_results results;
    };
} // namespace Standard::Algorithms::Numbers
