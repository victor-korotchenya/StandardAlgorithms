#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include "Arithmetic.h"
#include "TooDeepStackException.h"

// todo: p2. test it.
class CodeGeneratorCalculator final
{
    CodeGeneratorCalculator() = delete;

public:
    //Given code words e.g. {0, 01, 011, 0111, 1111},
    //counts the number of possible strings of length n.
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
    static inline void CountCodes(
        const std::vector<std::string>& words,
        //0 will be the first item.
        std::vector< size_t >& codeSizes)
    {
        if (AreDistinctCodes(words))
        {
            CountDistinctCodes(words, codeSizes);
            return;
        }

        CountCodesWithRepetition(words, codeSizes);
    }

private:
    static inline void CountCodesWithRepetition(
        const std::vector<std::string>& words,
        //0 will be the first item.
        std::vector< size_t >& codeSizes)
    {
        const size_t wordCount = words.size();

        FormatWordSizes(words, wordCount, codeSizes, false);
        codeSizes.clear();
        codeSizes.push_back(0);

        std::vector< size_t > currentString;
        std::unordered_set<std::string> stringSet;

        try
        {
            const size_t maxLength = 1024 * 1024;
            for (size_t stringLength = 1; stringLength <= maxLength; ++stringLength)
            {
                const size_t deepnessInitial = 0;

                const size_t count = ExploreRepetitive(
                    words, wordCount,
                    currentString, stringLength,
                    deepnessInitial,
                    stringSet);
                codeSizes.push_back(count);

                stringSet.clear();
                currentString.clear();

                std::cout << stringLength << ", " << count << '\n';
            }
        }
        catch (const ArithmeticException&
#ifdef _DEBUG
            ex
#endif
            )
        {//Ignore the overflow.
#ifdef _DEBUG
            std::cout << " _DEBUG. Got an overflow: " << ex.what() << '\n';
#endif
        }
        catch (const TooDeepStackException&
#ifdef _DEBUG
            ex
#endif
            )
        {//Ignore the error.
#ifdef _DEBUG
            std::cout << " _DEBUG. Got TooDeepStackException: " << ex.what() << '\n';
#endif
        }
    }

    static bool AreDistinctCodes(const std::vector<std::string>& words)
    {//TODO: p3. How to check?
        return words.empty();
    }

    static inline void CountDistinctCodes(
        const std::vector<std::string >& words,
        //0 will be the first item.
        std::vector< size_t >& codeSizes)
    {
        const size_t wordCount = words.size();

        std::vector< size_t > wordSizes;
        FormatWordSizes(words, wordCount, wordSizes);

        codeSizes.clear();
        codeSizes.push_back(0);

        try
        {
            const size_t maxLength = 1024 * 1024;
            for (size_t stringLength = 1; stringLength <= maxLength; ++stringLength)
            {
                const size_t count = Explore(
                    wordSizes, wordCount, codeSizes,
                    stringLength);

                codeSizes.push_back(count);
            }
        }
        catch (const ArithmeticException&
#ifdef _DEBUG
            ex
#endif
            )
        {//Ignore the overflow.
#ifdef _DEBUG
            std::cout << " _DEBUG. Got an overflow: " << ex.what() << '\n';
#endif
        }
    }

    static void FormatWordSizes(
        const std::vector<std::string >& words,
        const size_t wordCount,
        std::vector< size_t >& wordSizes,
        const bool shallInsert = true)
    {
        if (0 == wordCount)
        {
            throw std::runtime_error("The 'code words' must have at least one item.");
        }

        if (shallInsert)
        {
            wordSizes.reserve(wordCount);
        }

        for (size_t i = 0; i < wordCount; ++i)
        {
            const std::string& codeWord = words[i];
            const size_t wordSize = codeWord.size();
            if (0 == wordSize)
            {
                std::ostringstream ss;
                ss << "The code word number " << i << " must be not empty string.";
                StreamUtilities::throw_exception(ss);
            }

            if (shallInsert)
            {
                wordSizes.push_back(wordSize);
            }
        }

        if (shallInsert && wordCount > 1)
        {
            //TODO: p3. There can be several codes of the same length.
            //Usually, there are few words.
            sort(wordSizes.begin(), wordSizes.end());
        }
    }

    static inline size_t Explore(
        const std::vector< size_t >& wordSizes,
        const size_t wordCount,
        const std::vector< size_t >& previousCodeSizes,
        const size_t charactersLeft)
    {
        size_t result = 0;
        for (size_t index = 0; index < wordCount; ++index)
        {
            const size_t wordSize = wordSizes[index];
            if (wordSize > charactersLeft)
            {//Since wordSizes are sorted.
                break;
            }

            result = AddUnsigned<size_t>(
                result,
                (0 == (charactersLeft - wordSize)) ? 1
                //Explore(charactersLeft - wordSize);
                : previousCodeSizes[charactersLeft - wordSize]);
        }

        return result;
    }

    static inline size_t ExploreRepetitive(
        const std::vector< std::string >& words,
        const size_t wordCount,
        std::vector< size_t >& currentString,
        const size_t charactersLeft,
        const size_t deepness,
        //To check for the uniqueness.
        std::unordered_set<std::string>& stringSet)
    {
        size_t result = 0;
        for (size_t index = 0; index < wordCount; ++index)
        {
            const std::string& word = words[index];
            const size_t wordSize = word.size();
            if (charactersLeft < wordSize)
            {
                continue;
            }

            currentString.push_back(index);

            if (charactersLeft - wordSize > 0)
            {
                const size_t deepnessMax = 1024;
                if (deepnessMax <= deepness)
                {
                    throw TooDeepStackException(deepnessMax);
                }

                const size_t count = ExploreRepetitive(
                    words, wordCount,
                    currentString, charactersLeft - wordSize,
                    deepness + 1,
                    stringSet);
                result = AddUnsigned<size_t>(result, count);
            }
            else
            {
                std::ostringstream ss;

                const size_t currentStringSize = currentString.size();
                for (size_t position = 0; position < currentStringSize; ++position)
                {
                    const auto& s = words[currentString[position]];
                    ss << s;
                }

                const auto formedWord = ss.str();
                const auto insertResult = stringSet.insert(formedWord);
                if (insertResult.second)
                {
                    result = AddUnsigned<size_t>(result, 1);
                }
                else
                {
                    std::cout << "not inserted\n";
                }
            }

            currentString.pop_back();
        }

        return result;
    }
};