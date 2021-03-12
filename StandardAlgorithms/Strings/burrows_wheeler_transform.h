#pragma once
#include"../Utilities/require_utilities.h"
#include"suffix_array.h"
#include<limits>
#include<map>
#include<vector>

namespace Standard::Algorithms::Strings
{
    // Permute the characters to have many repetitions to be used in further text compression.
    // See also move_to_front_encoder.

    // Build from a suffix array in time O(n).
    template<class encoded_t, class string_t,
        // todo(p4): make suffix_array_other accept any char_t.
        class encoded_char_t = typename encoded_t::value_type, class text_char_t = typename string_t::value_type,
        encoded_char_t unique_smallest_char = std::numeric_limits<encoded_char_t>::min(),
        // Only for tests. todo(p4): make suffix_array_other accept any char_t.
        text_char_t in_min = 1>
    requires(unique_smallest_char <= static_cast<encoded_char_t>(in_min))
    [[nodiscard]] constexpr auto burrows_wheeler_transform(const string_t &c_source) -> encoded_t
    {
        throw_if_empty("source", c_source);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto iter = std::min_element(c_source.begin(), c_source.end());

            require_greater(*iter, in_min, "The min char is not good.");
        }

        auto source = c_source;
        source += in_min;
        const auto size = source.size();

        std::vector<std::int32_t> sar(size);
        suffix_array_other(source, sar);

        assert(size == sar.size());

        encoded_t result(size, encoded_char_t{});

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &suff = sar[index];
            auto &res = result[index];
            assert(0 <= suff && static_cast<std::size_t>(suff) < size);

            // Just take the previous letter.
            // todo(p4): Avoid an unnecessary ternary operator.
            res = 0 < suff ? source.at(suff - 1) : unique_smallest_char;
        }

        return result;
    }

    // Slow time O(n*n).
    template<class string_t, class char_t = typename string_t::value_type, class encoded_t = string_t,
        char_t unique_smallest_char = std::numeric_limits<char_t>::min()>
    [[nodiscard]] constexpr auto burrows_wheeler_transform_slow(const string_t &c_source) -> encoded_t
    {
        throw_if_empty("source", c_source);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto iter = std::min_element(c_source.begin(), c_source.end());

            require_greater(*iter, unique_smallest_char, "The unique smallest char is not good.");
        }

        auto source = c_source;
        source.push_back(unique_smallest_char);

        const auto size = source.size();

        encoded_t result{};
        result.reserve(size);

        std::vector<string_t> rotations(size, source);

        for (std::size_t index = 1; index < size; ++index)
        {
            auto &rota = rotations[index];
            std::rotate(rota.begin(), rota.begin() + index, rota.end());
        }

        std::sort(rotations.begin(), rotations.end());

        for ([[maybe_unused]] std::size_t index{}; const auto &rota : rotations)
        {
            assert(!rota.empty());

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(0U == index || rotations.at(index - 1U) < rota);

                ++index;
            }

            result.push_back(rota.back());
        }

        return result;
    }

    namespace Inner
    {
        // Time O(n * log(Abc size)).
        constexpr void bwt_ranks(const auto &encoded, std::vector<std::size_t> &ranks, auto &code_ranks)
        {
            for (const auto &cha : encoded)
            {
                ++(code_ranks[cha]);
            }

            for (std::size_t pre{}; const auto &[code, rank1] : code_ranks)
            {// Use a sorting map to sum the previous ranks.
                auto &cur = code_ranks.at(code);
                const auto old = cur;
                cur = pre;
                pre += old;
            }

            for (std::size_t pos{}; const auto &cha : encoded)
            {
                auto &rank1 = code_ranks[cha];
                assert(pos < ranks.size() && rank1 < ranks.size());

                ranks.at(pos) = rank1;
                ++rank1, ++pos;
            }
        }
    } // namespace Inner

    // Time O(n * log(Abc size)).
    template<class string_t, class encoded_t, class encoded_char_t = typename encoded_t::value_type,
        // Use a sorting map.
        class map_t = std::map<encoded_char_t, std::size_t>,
        encoded_char_t unique_smallest_char = std::numeric_limits<encoded_char_t>::min()>
    [[nodiscard]] constexpr auto bwt_reverse(const encoded_t &encoded) -> string_t
    {
        const auto result_size = require_greater(encoded.size(), 1ZU, "encoded size") - 1ZU;
        assert(0U < result_size);

        const auto iter = std::find(encoded.begin(), encoded.end(), unique_smallest_char);
        if (encoded.end() == iter) [[unlikely]]
        {
            auto err = "The encoded must contain the unique smallest encoded char '" +
                std::to_string(unique_smallest_char) + "'.";

            throw std::runtime_error(err);
        }

        if (const auto next_it = std::find(iter + 1, encoded.end(), unique_smallest_char); encoded.end() != next_it)
            [[unlikely]]
        {
            const auto pos_0 = iter - encoded.begin();
            const auto pos_1 = next_it - encoded.begin();

            auto err = "The encoded contains 2 smallest encoded char at " + std::to_string(pos_0) + " and also at " +
                std::to_string(pos_1) + " error.";

            throw std::runtime_error(err);
        }

        string_t result{};
        result.reserve(result_size);

        std::vector<std::size_t> ranks(result_size + 1ZU);
        {
            map_t code_ranks{};
            Inner::bwt_ranks(encoded, ranks, code_ranks);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(code_ranks.contains(unique_smallest_char));

                [[maybe_unused]] const auto &rank1 = code_ranks.at(unique_smallest_char);
                assert(1U == rank1);
            }
        }

        std::size_t pos{};

        do
        {
            assert(pos < encoded.size());

            const auto &cha = encoded.at(pos);
            result += cha;
            pos = ranks.at(pos);
        } while (result.size() < result_size);

        std::reverse(result.begin(), result.end());

        return result;
    }
} // namespace Standard::Algorithms::Strings
