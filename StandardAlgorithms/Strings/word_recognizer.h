#pragma once
#include"../Utilities/check_size.h"
#include"../Utilities/require_utilities.h"
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Strings
{
    template<std::unsigned_integral int_t = std::size_t, std::signed_integral weight_t = std::int64_t,
        class set_t = std::unordered_set<std::string>, class offset_length_t = std::pair<int_t, int_t>,
        class cost_and_positions = std::pair<weight_t, std::vector<offset_length_t>>>
    struct word_recognizer final
    {
        word_recognizer() = delete;

        // Given a text and a dictionary of words,
        // insert separators to maximize the total weight:
        // - If a word belongs to the dictionary, the cube of the word length is added.
        // - Else that is subtracted.
        // Return the total weight and words positions(offset and length).
        //
        // The running time is O(|text| * |longest word| * DictionarySearchTime)
        // There is no check for arithmetic overflow.
        [[nodiscard]] static constexpr auto recognize(const std::string &text, const set_t &words,
            // If zero, then the "words" will be scanned to find the longest word.
            int_t max_length_of_word = {}) -> cost_and_positions
        {
            throw_if_empty("text", text);
            throw_if_empty("words", words);

            if (int_t{} == max_length_of_word)
            {
                max_length_of_word = word_max_length(words);
            }
            else
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    const auto actual_max_length = word_max_length(words);

                    if (actual_max_length != max_length_of_word) [[unlikely]]
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << "Error: actual max length (" << actual_max_length << ") != max length of word("
                            << max_length_of_word << ").";

                        throw_exception(str);
                    }
                }
            }

            const auto matrices = compute_best_weights_and_positions(text, words, max_length_of_word);

            auto result = backtrace_result(text.size(), matrices);
            return result;
        }

        // The length cube is being used to favor the longer words.
        [[nodiscard]] inline static constexpr auto evaluate_weight(const weight_t &size) noexcept(
            noexcept(size *size *size)) -> weight_t
        {
            return size * size * size;
        }

private:
        using t_matrices = std::pair<std::vector<weight_t>, std::vector<offset_length_t>>;

        [[nodiscard]] static constexpr auto compute_best_weights_and_positions(
            const std::string &text, const set_t &words, const int_t &max_length_of_word) -> t_matrices
        {
            const auto text_size = text.size();

            std::vector<weight_t> weights(text_size + 1LL);
            std::vector<offset_length_t> positions(text_size);

            for (int_t sub_length{}; sub_length < text_size; ++sub_length)
            {
                const auto best = best_prefix(text, words, max_length_of_word, weights, sub_length);

                weights[sub_length + 1LL] = best.first;
                positions[sub_length] = best.second;
            }

            return { std::move(weights), std::move(positions) };
        }

        static constexpr void try_suffix(const std::string &text, const set_t &words,
#ifdef _DEBUG
            const int_t max_length_of_word,
#endif
            const weight_t initial_sequence_weight, const int_t sub_length, const int_t offset, weight_t &best_weight,
            offset_length_t &best_position)
        {
            const auto word_length = static_cast<int_t>(sub_length + 1LL - offset);

#if _DEBUG
            if (max_length_of_word < word_length) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "max length of word " << max_length_of_word << " < word length " << word_length << ", offset "
                    << offset << ", sub length " << sub_length << ".";

                throw_exception(str);
            }
#endif

            const auto suffix = text.substr(offset, word_length); // todo(p3): std::string_view
            const auto word_weight = evaluate_word(words, suffix);

            const auto current_weight = static_cast<weight_t>(initial_sequence_weight + word_weight);

            if (best_weight < current_weight)
            {
                best_weight = current_weight;
                best_position = { offset, word_length };
            }
        }

        [[nodiscard]] static constexpr auto best_prefix(const std::string &text, const set_t &words,
            const int_t max_length_of_word, const std::vector<weight_t> &weights, const int_t sub_length)
            -> std::pair<weight_t, offset_length_t>
        {
            auto best_weight = std::numeric_limits<weight_t>::min();

            // The first assignment must overwrite it.
            offset_length_t best_position{};

            // Skip too long words.
            const auto initial_offset =
                sub_length < max_length_of_word ? int_t{} : static_cast<int_t>(sub_length - max_length_of_word + 1LL);

            // Now go by "suffix": "the initial sequence weight" + "suffix weight".
            for (auto offset = initial_offset; offset <= sub_length; ++offset)
            {
                const auto &initial_sequence_weight = weights[offset];

                try_suffix(text, words,
#ifdef _DEBUG
                    max_length_of_word,
#endif
                    initial_sequence_weight, sub_length, offset, best_weight, best_position);
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (std::numeric_limits<weight_t>::min() == best_weight) [[unlikely]]
                {
                    throw std::runtime_error("Min weight == best weight");
                }
            }

            return { best_weight, best_position };
        }

        [[nodiscard]] static constexpr auto evaluate_word(const set_t &words, const std::string &word) -> weight_t
        {
            const auto size = word.size();
            const auto weight = evaluate_weight(static_cast<weight_t>(size));
            const auto found = words.find(word);
            auto result = words.end() == found ? -weight : weight;
            return result;
        }

        [[nodiscard]] static constexpr auto backtrace_result(const std::size_t text_size, const t_matrices &matrices)
            -> cost_and_positions
        {
            assert(0U < text_size);

            const auto &weights = matrices.first;
            const auto &positions = matrices.second;
            const auto &cost = weights[text_size];

            std::vector<offset_length_t> result_positions;

            for (auto index = text_size - 1ZU;;)
            {
                result_positions.push_back(positions[index]);
                if (0 == positions[index].first)
                {
                    break;
                }

                const auto length_to_subtract = positions[index].second;

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    if (index < length_to_subtract)
                    {
                        auto str = ::Standard::Algorithms::Utilities::w_stream();
                        str << "index(" << index << ") < " << length_to_subtract << " == positions[" << index
                            << "].Length.";
                        throw_exception(str);
                    }
                }

                index -= length_to_subtract;
            }

            std::reverse(result_positions.begin(), result_positions.end());

            return { cost, result_positions };
        }

        [[nodiscard]] static constexpr auto word_max_length(const set_t &words) -> int_t
        {
            std::size_t result_long{};

            for (const std::string &word : words)
            {
                const auto word_length = word.size();
                assert(0U < word_length);

                if (result_long < word_length)
                {
                    result_long = word_length;
                }
            }

            auto result = Standard::Algorithms::Utilities::check_size<int_t>("Too long word size", result_long);

            return result;
        }
    };
} // namespace Standard::Algorithms::Strings
