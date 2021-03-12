#pragma once
#include"arithmetic.h"
#include"too_deep_stack_exception.h"
#include<iostream>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // todo(p2): test.
    struct code_generator_calculator final
    {
        code_generator_calculator() = delete;

        // Given code words e.g. {0, 01, 011, 0111, 1111},
        // counts the number of possible strings of length n.
        //  1, 1
        //  2, 2
        //  3, 4
        //  4, 9
        //  5, 17
        //  6, 34
        //  7, 68
        //  8, 137
        //  9, 273
        //  10, 546
        //  11, 1092
        //  12, 2185
        //  13, 4369
        //  14, 8738
        //  15, 17476
        //  16, 34953
        //  17, 69905
        //  18, 139810
        //  19, 279620
        //  20, 559241
        //  21, 1118481
        //  22, 2236962
        //  23, 4473924
        //  24, 8947849
        //  25, 17895697
        //  26, 35791394
        //  27, 71582788
        //  28, 143165577
        //  29, 286331153
        //  30, 572662306
        //  31, 1145324612
        //  32, 2290649225
        //  33, 4581298449
        //  34, 9162596898
        //  35, 18325193796
        //  36, 36650387593
        //  37, 73300775185
        //  38, 146601550370
        //  39, 293203100740
        //  40, 586406201481
        //  41, 1172812402961
        //  42, 2345624805922
        //  43, 4691249611844
        //  44, 9382499223689
        //  45, 18764998447377
        //  46, 37529996894754
        //  47, 75059993789508
        //  48, 150119987579017
        //  49, 300239975158033
        //  50, 600479950316066
        //  51, 1200959900632132
        //  52, 2401919801264265
        //  53, 4803839602528529
        //  54, 9607679205057058
        //  55, 19215358410114116
        //  56, 38430716820228233
        //  57, 76861433640456465
        //  58, 153722867280912930
        //  59, 307445734561825860
        //  60, 614891469123651721
        //  61, 1229782938247303441
        //  62, 2459565876494606882
        //  63, 4919131752989213764
        //  64, 9838263505978427529
        inline static // todo(p3): constexpr
            void
            count_codes(const std::vector<std::string> &words,
                // 0 will be the first item.
                std::vector<std::size_t> &code_sizes)
        {
            if (are_distinct_codes(words))
            {
                count_distinct_codes(words, code_sizes);
                return;
            }

            count_codes_with_repetition(words, code_sizes);
        }

private:
        inline static // todo(p3): constexpr
            void
            count_codes_with_repetition(const std::vector<std::string> &words, std::vector<std::size_t> &code_sizes)
        {
            const auto word_count = words.size();

            format_word_sizes(words, word_count, code_sizes, false);
            code_sizes.clear();
            code_sizes.push_back(0);

            std::vector<std::size_t> current_string;
            std::unordered_set<std::string> string_set;

            try
            {
                for (std::size_t string_length = 1; string_length <= max_length; ++string_length)
                {
                    constexpr std::uint32_t deepness_initial{};

                    const auto count1 = explore_repetitive(
                        words, word_count, current_string, string_length, deepness_initial, string_set);

                    code_sizes.push_back(count1);

                    string_set.clear();
                    current_string.clear();

                    std::cout << string_length << ", " << count1 << '\n';
                }
            }
            catch ([[maybe_unused]] const arithmetic_exception &exc)
            {// Ignore the overflow.
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    std::cout << " _DEBUG. Got an overflow: " << exc.what() << '\n';
                }
            }
            catch ([[maybe_unused]] const too_deep_stack_exception &exc)
            {// Ignore the error.
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    std::cout << " _DEBUG. Got too_deep_stack_exception: " << exc.what() << '\n';
                }
            }
        }

        [[nodiscard]] inline static constexpr auto are_distinct_codes(const std::vector<std::string> &words) -> bool
        {// todo(p3): How to check?
            return words.empty();
        }

        inline static constexpr void count_distinct_codes(const std::vector<std::string> &words,
            // 0 will be the first item.
            std::vector<std::size_t> &code_sizes)
        {
            const auto word_count = words.size();

            std::vector<std::size_t> word_sizes;
            format_word_sizes(words, word_count, word_sizes);

            code_sizes.assign(1, 0);

            try
            {
                for (std::size_t string_length = 1; string_length <= max_length; ++string_length)
                {
                    const auto count1 = explore(word_sizes, word_count, code_sizes, string_length);

                    code_sizes.push_back(count1);
                }
            }
            catch ([[maybe_unused]] const arithmetic_exception &exc)
            {// Ignore the overflow.
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    std::cout << " _DEBUG. Got an overflow: " << exc.what() << '\n';
                }
            }
        }

        inline static constexpr void format_word_sizes(const std::vector<std::string> &words,
            const std::size_t word_count, std::vector<std::size_t> &word_sizes, const bool shall_insert = true)
        {
            if (0U == word_count) [[unlikely]]
            {
                throw std::runtime_error("The 'code words' must have at least one item.");
            }

            if (shall_insert)
            {
                word_sizes.reserve(word_count);
            }

            for (std::size_t index{}; index < word_count; ++index)
            {
                const auto &code_word = words[index];
                const auto word_size = code_word.size();

                if (0U == word_size)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The code word number " << index << " must be not empty string.";

                    throw_exception(str);
                }

                if (shall_insert)
                {
                    word_sizes.push_back(word_size);
                }
            }

            if (shall_insert && 1U < word_count)
            {
                // todo(p3): There can be several codes of the same length.
                // Usually, there are few words.
                std::sort(word_sizes.begin(), word_sizes.end());
            }
        }

        [[nodiscard]] inline static constexpr auto explore(const std::vector<std::size_t> &word_sizes,
            const std::size_t word_count, const std::vector<std::size_t> &previous_code_sizes,
            const std::size_t characters_left) -> std::size_t
        {
            std::size_t result{};

            for (std::size_t index{}; index < word_count; ++index)
            {
                const auto word_size = word_sizes[index];

                if (characters_left < word_size)
                {// Since wordSizes are sorted.
                    break;
                }

                result = add_unsigned<std::size_t>(result,
                    (0U == (characters_left - word_size)) ? 1U
                                                          // Explore(charactersLeft - wordSize);
                                                          : previous_code_sizes[characters_left - word_size]);
            }

            return result;
        }

        [[nodiscard]] inline static constexpr auto explore_repetitive(const std::vector<std::string> &words,
            const std::size_t word_count, std::vector<std::size_t> &current_string, const std::size_t characters_left,
            const std::uint32_t deepness,
            // To check for uniqueness.
            std::unordered_set<std::string> &string_set) -> std::size_t
        {
            std::size_t result{};

            for (std::size_t index{}; index < word_count; ++index)
            {
                const auto &word = words[index];
                const auto word_size = word.size();

                if (characters_left < word_size)
                {
                    continue;
                }

                current_string.push_back(index);

                if (0 < characters_left - word_size)
                {
                    constexpr auto deepness_max = 1024U;

                    if (deepness_max <= deepness) [[unlikely]]
                    {
                        throw too_deep_stack_exception(deepness_max);
                    }

                    const auto count1 = explore_repetitive(
                        words, word_count, current_string, characters_left - word_size, deepness + 1U, string_set);

                    result = add_unsigned<std::size_t>(result, count1);
                }
                else
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();

                    const auto current_string_size = current_string.size();

                    for (std::size_t position{}; position < current_string_size; ++position)
                    {
                        const auto &word_2 = words[current_string[position]];
                        str << word_2;
                    }

                    const auto formed_word = str.str();
                    const auto insert_result = string_set.insert(formed_word);

                    if (insert_result.second)
                    {
                        result = add_unsigned<std::size_t>(result, 1);
                    }
                    else
                    {
                        std::cout << "not inserted\n";
                    }
                }

                current_string.pop_back();
            }

            return result;
        }

        static constexpr std::size_t max_length = 1U << 20U;
    };
} // namespace Standard::Algorithms::Numbers
