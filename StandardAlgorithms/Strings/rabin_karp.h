#pragma once
#include"../Numbers/number_utilities.h" // modular_power, make_non_negative

namespace Standard::Algorithms::Strings::Inner
{
    template<class string_t>
    [[nodiscard]] constexpr auto are_equal_naive(
        const string_t &text, std::size_t text_start_index, const string_t &pattern) -> bool
    {
        const auto pat_size = pattern.size();
        std::size_t pat_ind{};

        do
        {
            assert(pat_ind < pat_size && text_start_index < text.size());

            if (!(pattern[pat_ind] == text[text_start_index]))
            {
                return false;
            }
        } while (++text_start_index, ++pat_ind < pat_size);

        return true;
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    // String matching using rolling hashing. Slow time O(m*n).
    // Rabin-Karp can be used in two-dimensional pattern matching.
    template<class string_t, class hash_func_t,
        // todo(p3): Signed type incorrectly subtracts numbers modulo mod - can be fixed.
        std::signed_integral hash_t, class result_t, bool only_first = false>
    constexpr void rabin_karp(const string_t &pattern, const hash_t &bas, const string_t &text, const hash_t &mod,
        hash_func_t &hash_func, result_t &result)
    {
        assert(hash_t{} < bas && hash_t{} < mod && bas != mod);

        result.clear();

        const std::size_t pat_size = pattern.size();
        if (!pat_size)
        {
            result.push_back(0ZU);
            return;
        }

        const std::size_t text_size = text.size();
        if (text_size <= pat_size)
        {
            if (pattern == text)
            {
                result.push_back(0ZU);
            }

            return;
        }

        // pat 3, text 10, i = [0, 7]. Last check {7, 8, 9}.
        const auto max_index = text_size - pat_size;
        assert(0U < max_index);

        const hash_t pat_hash = hash_func(bas, pattern.cbegin(), pattern.cend(), mod);
        const hash_t penult_pow = Standard::Algorithms::Numbers::modular_power(bas, pat_size - 1U, mod);

        assert(!(pat_hash < hash_t{}) && pat_hash < mod && !(penult_pow < hash_t{}) && penult_pow < mod);

        hash_t text_hash = hash_func(bas, text.cbegin(), text.cbegin() + pat_size, mod);

        for (std::size_t text_start_index{};;)
        {
            assert(!(pat_hash < hash_t{}) && pat_hash < mod);
            assert(!(text_hash < hash_t{}) && text_hash < mod);

            if (pat_hash == text_hash)
            {
                if (Inner::are_equal_naive<string_t>(text, text_start_index, pattern))
                {
                    result.push_back(text_start_index);

                    if constexpr (only_first)
                    {
                        return;
                    }
                }
            }

            if (max_index < ++text_start_index)
            {
                return;
            }

            // h0 = bas*bas*a0 + bas*a1 + a2
            // h1 = bas*bas*a1 + bas*a2 + a3
            // h1 = (h0 - bas*bas*a0)*bas + a3
            const auto &low_coef = text[text_start_index - 1U];
            const auto &high_coef = text.at(text_start_index - 1U + pat_size);
            text_hash = ((text_hash - penult_pow * low_coef) % mod * bas + high_coef) % mod;

            Standard::Algorithms::Numbers::make_non_negative<hash_t, true>(
                text_hash, mod); // todo(p3): can be omitted by: +mod.
        }
    }
} // namespace Standard::Algorithms::Strings
