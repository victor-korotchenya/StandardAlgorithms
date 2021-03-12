#pragma once
#include <limits>
#include <ostream>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>
#include "WordPosition.h"
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Strings
{
    template<typename TSize = size_t,
        //Must be unsigned.
        typename TWeight = long long,
        typename TDictionary = std::unordered_set<std::string>>
        struct WordRecognizer final
    {
        WordRecognizer() = delete;

        static_assert(std::is_signed<TWeight>::value,
            "The TWeight must be unsigned as it can be negative.");
        using WordPosition = WordPosition<TSize>;
        using CostAndPositions = std::pair<TWeight, std::vector<WordPosition>>;

        //Given a text and a dictionary of words,
        //insert separators to maximize the total weight:
        // - If a word belongs to the dictionary, the cube of the word length is added.
        // - Else that is subtracted.
        //Return the total weight and words positions(offset and length).
        //
        //The running time is O(|text| * |longest word| * DictionarySearchTime)
        //There is no check for arithmetic overflow.
        static CostAndPositions Recognize(
            const std::string& text,
            const TDictionary& words,
            //If zero, then the "words" will be scanned to find the longest word.
            TSize maxLengthOfWord = {});

        //The cube is being used to favor the longer words.
        static constexpr inline TWeight EvaluateWeight(const TWeight size)
        {
            return size * size * size;
        }

    private:
        using TMatrices = std::pair<std::vector<TWeight>, std::vector<WordPosition>>;

        static TMatrices ComputeBestWeightsAndPositions(
            const std::string& text,
            const TDictionary& words,
            const TSize maxLengthOfWord);

        static void TrySuffix(
            const std::string& text, const TDictionary& words,
#ifdef _DEBUG
            const TSize maxLengthOfWord,
#endif
            const TWeight initialSequenceWeight,
            const TSize subLength, const TSize offset,
            TWeight& bestWeight,
            WordPosition& bestPosition);

        static std::pair<TWeight, WordPosition> BestPrefix(
            const std::string& text,
            const TDictionary& words,
            const TSize maxLengthOfWord,
            const std::vector<TWeight>& weights,
            const TSize subLength);

        static TWeight EvaluateWord(
            const TDictionary& words, const std::string& word);

        static CostAndPositions BacktraceResult(
            const size_t textSize, const TMatrices& matrices);

        static TSize WordMaxLength(const TDictionary& words);
    };

    template <typename TSize, typename TWeight, typename TDictionary>
    typename WordRecognizer<TSize, TWeight, TDictionary>::CostAndPositions
        WordRecognizer<TSize, TWeight, TDictionary>::Recognize(
            const std::string& text,
            const TDictionary& words,
            TSize maxLengthOfWord)
    {
        ThrowIfEmpty(text, "text");
        ThrowIfEmpty(words, "words");

        if (0 == maxLengthOfWord)
        {
            maxLengthOfWord = WordMaxLength(words);
        }
#ifdef _DEBUG
        else
        {
            const auto actualMaxLength = WordMaxLength(words);
            if (actualMaxLength != maxLengthOfWord)
            {
                std::ostringstream ss;
                ss << "Error: actualMaxLength (" << actualMaxLength
                    << ") != maxLengthOfWord(" << maxLengthOfWord << ").";
                StreamUtilities::throw_exception(ss);
            }
        }
#endif
        const auto matrices = ComputeBestWeightsAndPositions(
            text, words, maxLengthOfWord);
        const auto result = BacktraceResult(text.size(), matrices);
        return result;
    }

    template <typename TSize, typename TWeight, typename TDictionary>
    typename WordRecognizer<TSize, TWeight, TDictionary>::TMatrices
        WordRecognizer<TSize, TWeight, TDictionary>::ComputeBestWeightsAndPositions(
            const std::string& text,
            const TDictionary& words,
            const TSize maxLengthOfWord)
    {
        const auto textSize = text.size();

        std::vector<TWeight> weights(textSize + 1);
        std::vector<WordPosition> positions(textSize);

        for (TSize subLength = 0; subLength < textSize; ++subLength)
        {
            const auto best = BestPrefix(
                text, words, maxLengthOfWord, weights, subLength);

            weights[subLength + 1ll] = best.first;
            positions[subLength] = best.second;
        }

        return{ weights, positions };
    }

    template <typename TSize, typename TWeight, typename TDictionary>
    std::pair<TWeight, typename WordRecognizer<TSize, TWeight, TDictionary>::WordPosition>
        WordRecognizer<TSize, TWeight, TDictionary>::BestPrefix(
            const std::string& text,
            const TDictionary& words,
            const TSize maxLengthOfWord,
            const std::vector<TWeight>& weights,
            const TSize subLength)
    {
        TWeight bestWeight = std::numeric_limits<TWeight>::min();

        //The first assignment must overwrite it.
        WordPosition bestPosition{};

        //Skip too long words.
        const TSize initialOffset = subLength < maxLengthOfWord
            ? 0
            : subLength - maxLengthOfWord + 1;

        //Now go by "suffix": "the initial sequence weight" + "suffix weight".
        for (TSize offset = initialOffset; offset <= subLength; ++offset)
        {
            const auto initialSequenceWeight = weights[offset];

            TrySuffix(
                text, words,
#ifdef _DEBUG
                maxLengthOfWord,
#endif
                initialSequenceWeight,
                subLength, offset,
                bestWeight, bestPosition);
        }

#ifdef _DEBUG
        if (std::numeric_limits<TWeight>::min() == bestWeight)
        {
            throw std::runtime_error("Min Weight == bestWeight");
        }
#endif
        return{ bestWeight,bestPosition };
    }

    template <typename TSize, typename TWeight, typename TDictionary>
    void WordRecognizer<TSize, TWeight, TDictionary>::TrySuffix(
        const std::string& text, const TDictionary& words,
#ifdef _DEBUG
        const TSize maxLengthOfWord,
#endif
        const TWeight initialSequenceWeight,
        const TSize subLength, const TSize offset,
        TWeight& bestWeight, WordPosition& bestPosition)
    {
        const TSize wordLength = subLength + 1 - offset;
#ifdef _DEBUG
        if (maxLengthOfWord < wordLength)
        {
            std::ostringstream ss;
            ss << "maxLengthOfWord(" << maxLengthOfWord
                << ") < wordLength(" << wordLength << ")"
                << ", offset=" << offset
                << ", subLength=" << subLength
                << ".";
            StreamUtilities::throw_exception(ss);
        }
#endif
        //TODO: p3. Do not create a new string.
        const auto suffix = text.substr(offset, wordLength);
        const auto wordWeight = EvaluateWord(words, suffix);

        const TWeight currentWeight = initialSequenceWeight + wordWeight;
        if (bestWeight < currentWeight)
        {
            bestWeight = currentWeight;
            bestPosition = { offset, wordLength };
        }
    }

    template <typename TSize, typename TWeight, typename TDictionary>
    typename WordRecognizer<TSize, TWeight, TDictionary>::CostAndPositions
        WordRecognizer<TSize, TWeight, TDictionary>::BacktraceResult(
            const size_t textSize, const TMatrices& matrices)
    {
        const std::vector<TWeight>& weights = matrices.first;
        const std::vector<WordPosition>& positions = matrices.second;

        const TWeight cost = weights[textSize];

        std::vector<WordPosition> resultPositions;

        for (auto index = textSize - 1;;)
        {
            resultPositions.push_back(positions[index]);
            if (0 == positions[index].get_Offset())
            {
                break;
            }

            const auto lengthToSubtract = positions[index].get_Length();
#ifdef _DEBUG
            if (index < lengthToSubtract)
            {
                std::ostringstream ss;
                ss << "index(" << index << ") < " << lengthToSubtract
                    << " == positions[" << index << "].get_Length().";
                StreamUtilities::throw_exception(ss);
            }
#endif
            index -= lengthToSubtract;
        };

        std::reverse(resultPositions.begin(), resultPositions.end());

        return{ cost, resultPositions };
    }

    template <typename TSize, typename TWeight, typename TDictionary>
    TWeight WordRecognizer<TSize, TWeight, TDictionary>::EvaluateWord(
        const TDictionary& words, const std::string& word)
    {
        const auto size = word.size();
        const auto weight = EvaluateWeight(static_cast<TWeight>(size));

        const auto found = words.find(word);

        const auto result = words.end() == found
            ? -weight : weight;
        return result;
    }

    template <typename TSize, typename TWeight, typename TDictionary>
    TSize WordRecognizer<TSize, TWeight, TDictionary>::WordMaxLength(
        const TDictionary& words)
    {
        size_t resultLong = 0;

        for (const std::string& word : words)
        {
            const size_t wordLength = word.size();
#ifdef _DEBUG
            if (0 == wordLength)
            {
                throw std::runtime_error("0 == wordLength");
            }
#endif
            if (resultLong < wordLength)
            {
                resultLong = wordLength;
            }
        }

        const auto result = static_cast<TSize>(resultLong);
        const auto resultTemp = static_cast<size_t>(result);
        if (result != resultTemp)
        {
            std::ostringstream ss;
            ss << "Too long words are not supported: "
                << resultLong << " as TSize is " << result
                << ". Consider replacing TSize.";
            StreamUtilities::throw_exception(ss);
        }

        return result;
    }
}