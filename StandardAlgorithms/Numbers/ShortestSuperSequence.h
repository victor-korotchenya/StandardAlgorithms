#pragma once
#include <vector>
#include "../disable_constructors.h"
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        class ShortestSuperSequence final
        {
            static const unsigned char CapitalA = 0x41;
            static const unsigned char SmallA = 0x61;

            static const unsigned char LatinLetterCount = 26;

            static const unsigned int FirstLetterDimension = 0;
            static const unsigned int SecondLetterDimension = 1;
            static const unsigned int AllLettersDimension = 2;
            static const unsigned int Dimensions = 3;

            using PrintSequenceFunction = void(*)(const std::vector< char >& sequence);

            const unsigned int _WordCount;
            //const char* _Words[][Dimensions];

            PrintSequenceFunction _PrintSequence;

            DISABLE_ALL_CONSTRUCTORS(ShortestSuperSequence);

        public:

            ShortestSuperSequence(
                const unsigned int wordCount,
                //Must consists of only Latin letters in any case.
                const char words[][Dimensions],
                const PrintSequenceFunction printSequence);

            static void Compute(
                const unsigned int wordCount,
                const char words[][Dimensions],
                const PrintSequenceFunction printSequence,
                //The returned sequence will be in the UPPER case.
                std::vector< char >& smallestSuperSequence);

            static void Compute27(
                const unsigned int wordCount,
                const char words[][Dimensions],
                const PrintSequenceFunction printSequence,
                //The returned sequence will be in the UPPER case.
                std::vector< char >& smallestSuperSequence);

            static bool IsValidSuperSequence(
                const unsigned int wordCount,
                const char words[][Dimensions],
                const std::vector< char >& sequence,
                unsigned int* firstMismatchIndex);

        private:
            static inline void CheckWordCountPositive(const unsigned int wordCount)
            {
                if (0 == wordCount)
                {
                    throw std::runtime_error("Word count must be positive.");
                }
            }

            static void CountLetterFrequencies(
                const unsigned int wordCount,
                const char words[][Dimensions],
                unsigned int letterFrequencies[Dimensions][LatinLetterCount]);

            //If some letter never occurs as the 2nd letter,
            // it can be put to the beginning.
            //Similarly, a letter is always 2nd: put it to the end.
            static void FindEdgeLetters(
                const unsigned int letterFrequencies[Dimensions][LatinLetterCount],
                std::vector< char >& beginning,
                std::vector< char >& ending,
                std::vector< char >& remaining);

            static void DuplicateData(std::vector< char >& v);

            static void ComputeMiddle(
                const unsigned int wordCount,
                const char words[][Dimensions],
                const PrintSequenceFunction printSequence,
                const std::vector< char >& beginning,
                const std::vector< char >& remaining,
                const std::vector< char >& ending,
                std::vector< char >& smallestSuperSequence);

            static void ComputeMiddleNew(
                const unsigned int wordCount,
                const char words[][Dimensions],
                const PrintSequenceFunction printSequence,
                const std::vector< char >& beginning,
                const std::vector< char >& remaining,
                const std::vector< char >& ending,
                std::vector< char >& smallestSuperSequence);

            static void ComputeMiddleNew27(
                const unsigned int wordCount,
                const char words[][Dimensions],
                const PrintSequenceFunction printSequence,
                const std::vector< char >& beginning,
                const std::vector< char >& remaining,
                const std::vector< char >& ending,
                std::vector< char >& smallestSuperSequence);

            static inline void AttempOneValue(
                const unsigned int wordCount,
                const char words[][Dimensions],
                const PrintSequenceFunction printSequence,
                const size_t beginningSize,
                const size_t remainingSize,
                const std::vector< char >& remaining,
                const std::vector< char >& ending,
                const size_t minPopCount,
                const size_t attempt,

                size_t& currentMinSize,
                std::vector< char >& tempSequence,
                std::vector< char >& smallestSuperSequence);

            static void TryUpdateMinimum(
                const size_t attempt,
                const PrintSequenceFunction printSequence,
                size_t& currentMinSize,
                std::vector< char >& tempSequence,
                std::vector< char >& smallestSuperSequence);

            static void AppendMiddle(
                const size_t mask,
                const size_t size,
                const std::vector< char >& middle,
                std::vector< char >& sequence);

            static void AppendEnding(const std::vector< char >& ending, std::vector< char >& sequence);

            static inline void ReportProgress(
                const size_t mask,
                const size_t maxAttempts,
                const size_t attempt,
                const size_t smallestSuperSequenceSize);

            static inline char IndexToChar(unsigned int letterIndex)
            {
                const char result = static_cast<char>(CapitalA + letterIndex);
                return result;
            }

            static inline char GetCharIndex(
                const unsigned char letter,
                const unsigned char letterCount)
            {
                const unsigned char resultCapital = letter - CapitalA;

                if (resultCapital < letterCount)
                {
                    return resultCapital;
                }
                else
                {
                    const unsigned char result = letter - SmallA;

                    if (result < letterCount)
                    {
                        return result;
                    }
                }

                std::ostringstream ss;
                ss << "The letter(" << letter << ") must belong to Latin alphabet.";

#ifdef _DEBUG
                throw
#endif
                    StreamUtilities::throw_exception(ss);
            }

            static inline size_t FindFirstChar(
                const char letter,
                const std::vector< char >& sequence,
                const size_t initialIndex)
            {
                const char letterUpperCase = static_cast<char>(toupper(letter));

                const auto end = sequence.cend();
                const auto it = find(sequence.cbegin() + initialIndex, end, letterUpperCase);
                if (end == it)
                {
                    return 0 - static_cast<size_t>(1);
                }

                const size_t index = distance(sequence.begin(), it);
                return index;
            }
        };
    }
}