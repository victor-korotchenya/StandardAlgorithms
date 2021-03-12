#ifdef _OPENMP
#include <omp.h>
#endif

#include <iomanip>
#include <vector>
#include <limits>
#include <iostream>
#include "Arithmetic.h"
#include "bit_utilities.h"
#include "ShortestSuperSequence.h"
#include "../Utilities/StreamUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

Privet::Algorithms::ShortestSuperSequence::ShortestSuperSequence(
    const unsigned int wordCount,
    const char words[][Dimensions],
    const PrintSequenceFunction printSequence)
    : _WordCount(wordCount), _PrintSequence(printSequence)
{
    if (words)
    {
        //TODO: p4. finish it.
    }
    //_Words = (words);
}

void Privet::Algorithms::ShortestSuperSequence::Compute(
    const unsigned int wordCount,
    const char words[][Dimensions],
    const PrintSequenceFunction printSequence,
    vector< char >& smallestSuperSequence)
{
    CheckWordCountPositive(wordCount);
    smallestSuperSequence.clear();

    unsigned int letterFrequencies[Dimensions][LatinLetterCount];
    const size_t letterCountsSizeBytes = sizeof letterFrequencies;
    memset(letterFrequencies, 0, letterCountsSizeBytes);

    //B and Q are not used.
    CountLetterFrequencies(wordCount, words, letterFrequencies);

    vector< char > beginning, ending;

    //14 letters are left: {A12, C5, D5, H3, I8, K4, L4, M9, N11, O5, R3, S4, T6, V4,}
    vector< char > remaining;

    FindEdgeLetters(letterFrequencies, beginning, ending, remaining);

    int isNew = 1;
    if (0 == isNew)
    {
        //Letters can be in 2 positions.
        DuplicateData(remaining);

        ComputeMiddle(
            wordCount, words,
            printSequence,
            beginning, remaining, ending,
            smallestSuperSequence);
    }
    else
    {
        ComputeMiddleNew(
            wordCount, words,
            printSequence,
            beginning, remaining, ending,
            smallestSuperSequence);
    }
}

void Privet::Algorithms::ShortestSuperSequence::Compute27(
    const unsigned int wordCount,
    const char words[][Dimensions],
    const PrintSequenceFunction printSequence,
    vector< char >& smallestSuperSequence)
{
    cout << "Start Compute27...\n";

    CheckWordCountPositive(wordCount);
    smallestSuperSequence.clear();

    unsigned int letterFrequencies[Dimensions][LatinLetterCount];
    const size_t letterCountsSizeBytes = sizeof letterFrequencies;
    memset(letterFrequencies, 0, letterCountsSizeBytes);

    //B and Q are not used.
    CountLetterFrequencies(wordCount, words, letterFrequencies);

    vector< char > beginning, ending;

    //14 letters are left: {A12, C5, D5, H3, I8, K4, L4, M9, N11, O5, R3, S4, T6, V4,}
    vector< char > remaining;

    ending.push_back('D');
    FindEdgeLetters(letterFrequencies, beginning, ending, remaining);
    swap(remaining[2], remaining[13]);
    remaining.resize(13);

    int isNew = 1;
    if (0 == isNew)
    {
        //Letters can be in 2 positions.
        DuplicateData(remaining);

        ComputeMiddle(
            wordCount, words,
            printSequence,
            beginning, remaining, ending,
            smallestSuperSequence);
    }
    else
    {
        ComputeMiddleNew27(
            wordCount, words,
            printSequence,
            beginning, remaining, ending,
            smallestSuperSequence);
    }
}

bool Privet::Algorithms::ShortestSuperSequence::IsValidSuperSequence(
    const unsigned int wordCount,
    const char words[][Dimensions],
    const vector< char >& sequence,
    unsigned int* firstMismatchIndex)
{
    CheckWordCountPositive(wordCount);

    const size_t sequenceSize = sequence.size();

    *firstMismatchIndex = 0;

    for (unsigned int wordIndex = 0; wordIndex < wordCount; ++wordIndex)
    {
        const char* const word = words[wordIndex];

        const size_t initialIndex = 0;
        const size_t firstLetterIndex = FindFirstChar(word[0], sequence, initialIndex);
        if (sequenceSize <= firstLetterIndex)
        {
            *firstMismatchIndex = wordIndex;
            return false;
        }

        const size_t secondLetterIndex = FindFirstChar(word[1], sequence, 1 + firstLetterIndex);
        if (sequenceSize <= secondLetterIndex)
        {
            *firstMismatchIndex = wordIndex;
            return false;
        }
    }

    return true;
}

void Privet::Algorithms::ShortestSuperSequence::CountLetterFrequencies(
    const unsigned int wordCount,
    const char words[][Dimensions],
    unsigned int letterFrequencies[Dimensions][LatinLetterCount])
{
    for (unsigned int wordIndex = 0; wordIndex < wordCount; ++wordIndex)
    {
        const char* const word = words[wordIndex];

        for (unsigned int letterIndex = 0; letterIndex < 2; ++letterIndex)
        {
            const char index = GetCharIndex(word[letterIndex], LatinLetterCount);
            letterFrequencies[letterIndex][index]++;
        }
    }

    for (unsigned int letter = 0; letter < LatinLetterCount; ++letter)
    {
        letterFrequencies[AllLettersDimension][letter] =
            letterFrequencies[FirstLetterDimension][letter]
            + letterFrequencies[SecondLetterDimension][letter];
    }
}

void Privet::Algorithms::ShortestSuperSequence::FindEdgeLetters(
    const unsigned int letterFrequencies[Dimensions][LatinLetterCount],
    vector< char >& beginning,
    vector< char >& ending,
    vector< char >& remaining)
{
    for (unsigned int letter = 0; letter < LatinLetterCount; ++letter)
    {
        if (0 == letterFrequencies[AllLettersDimension][letter])
        {//Letter never happens - ignore it.
            continue;
        }

        if (0 == letterFrequencies[FirstLetterDimension][letter])
        {
            ending.push_back(IndexToChar(letter));
        }
        else if (0 == letterFrequencies[SecondLetterDimension][letter])
        {
            beginning.push_back(IndexToChar(letter));
        }
        else
        {
            remaining.push_back(IndexToChar(letter));
        }
    }
}

void Privet::Algorithms::ShortestSuperSequence::DuplicateData(vector< char >& v)
{
    const size_t size = v.size();
    if (size)
    {
        v.reserve(size << 1);
        for (size_t j = 0; j < size; ++j)
        {
            v.push_back(v[j]);
        }
    }
}

void Privet::Algorithms::ShortestSuperSequence::ComputeMiddle(
    const unsigned int wordCount,
    const char words[][Dimensions],
    const PrintSequenceFunction printSequence,
    const vector< char >& beginning,
    const vector< char >& remaining,
    const vector< char >& ending,
    vector< char >& smallestSuperSequence)
{
    const size_t remainingSize = remaining.size();

    //Each letter must be there at least once.
    const size_t minPopCount = remainingSize >> 1;
    const size_t maxAttempts = size_t(1) << remainingSize;
    size_t currentMinSize = 0 - size_t(1);

    const size_t beginningSize = beginning.size();

    //To re-use resize later after swap.
    smallestSuperSequence = beginning;

    const int limit = 256;
    const bool isSmallChunk = maxAttempts < limit;
    const int maxThreads = isSmallChunk ? 1 : limit;

    const size_t chunkSize = isSmallChunk ? maxAttempts : maxAttempts / static_cast<unsigned int>(maxThreads);

#pragma omp parallel for default(none) shared(currentMinSize, smallestSuperSequence, words, beginning, remaining, ending, cout)
    for (int chunkIndex = 0; chunkIndex < maxThreads; ++chunkIndex)
    {
        //Each thread uses its own temp memory.
        vector< char > tempSequence = beginning;

        const size_t attemptStart = isSmallChunk ? maxAttempts - 1
            : chunkIndex * chunkSize + chunkSize - 1;

        const size_t attemptStopRaw = isSmallChunk ? 0 : (chunkIndex * chunkSize);

        const size_t attemptStop = 0 == attemptStopRaw ? 1 : attemptStopRaw;

        for (size_t attempt = attemptStart; attemptStop <= attempt; --attempt)
        {
            const size_t mask = 0xFFFFFF;
            if (0 == (attempt & mask))
            {
                ReportProgress(mask, maxAttempts, attempt, currentMinSize);
            }

            AttempOneValue(
                wordCount,
                words,
                printSequence,
                beginningSize,
                remainingSize,
                remaining,
                ending,
                minPopCount,
                attempt,

                currentMinSize,
                tempSequence,
                smallestSuperSequence);
        }
    }
}

void Privet::Algorithms::ShortestSuperSequence::ComputeMiddleNew(
    const unsigned int wordCount,
    const char words[][Dimensions],
    const PrintSequenceFunction printSequence,
    const vector< char >& beginning,
    const vector< char >& remaining,
    const vector< char >& ending,
    vector< char >& smallestSuperSequence)
{
    const size_t remainingSize = remaining.size();

    //Each letter must be there at least once.
    const size_t minPopCount = remainingSize;

    //TODO: p4. There is a problem with it right now.
    const size_t maxAttempts = 0 - size_t(1);
    size_t currentMinSize = 0 - size_t(1);

    const size_t beginningSize = beginning.size();

    //To re-use resize later after swap.
    smallestSuperSequence = beginning;

    vector< char > tempSequence = beginning;
    for (size_t digitCount = remainingSize << 1; minPopCount <= digitCount; --digitCount)
    {
        const unsigned long long one = 1;
        const unsigned long long initialValue = (one << digitCount) - one;
        unsigned long long value = initialValue;

        bool found = false;
        unsigned long long attempt = 0;
        do
        {
            ++attempt;
            const size_t mask = 0xFFFFFFF;
            if (0 == (attempt & mask))
            {
                ReportProgress(value, maxAttempts, attempt, currentMinSize);
            }

            //Copy beginning.
            tempSequence.resize(beginningSize);

            AppendMiddle(value, remainingSize, remaining, tempSequence);
            AppendEnding(ending, tempSequence);

            unsigned int firstMismatchIndex = 0;

            if (IsValidSuperSequence(
                wordCount, words, tempSequence, &firstMismatchIndex))
            {
                found = true;

                TryUpdateMinimum(
                    attempt, printSequence,
                    currentMinSize,
                    tempSequence, smallestSuperSequence);

                break;
            }

            value = next_greater_same_pop_count(value);
        } while (initialValue != value);

        if (!found)
        {
            cout << "Not found for digitCount=" << digitCount << ".\n";
            break;
        }
    }
}

void Privet::Algorithms::ShortestSuperSequence::ComputeMiddleNew27(
    const unsigned int wordCount,
    const char words[][Dimensions],
    const PrintSequenceFunction printSequence,
    const vector< char >& beginning,
    const vector< char >& remaining,
    const vector< char >& ending,
    vector< char >& smallestSuperSequence)
{
    const size_t remainingSize = remaining.size();

    //Each letter must be there at least once.
    const size_t minPopCount = remainingSize;

    //TODO: p4. There is a problem with it right now.
    const size_t maxAttempts = 0 - size_t(1);
    size_t currentMinSize = 0 - size_t(1);

    const size_t beginningSize = beginning.size();

    //To re-use resize later after swap.
    smallestSuperSequence = beginning;

    vector< char > tempSequence = beginning;
    //for (size_t digitCount = remainingSize << 1; minPopCount <= digitCount; --digitCount)
    for (size_t digitCount = 16; minPopCount <= digitCount; --digitCount)
    {
        const unsigned long long one = 1;
        const unsigned long long initialValue = (one << digitCount) - one;
        unsigned long long value = initialValue;

        bool found = false;
        unsigned long long attempt = 0;
        do
        {
            ++attempt;
            const size_t mask = 0xFFFFFFF;
            if (0 == (attempt & mask))
            {
                ReportProgress(value, maxAttempts, attempt, currentMinSize);
            }

            //Copy beginning.
            tempSequence.resize(beginningSize);

            AppendMiddle(value, remainingSize, remaining, tempSequence);
            AppendEnding(ending, tempSequence);

            unsigned int firstMismatchIndex = 0;

            //char* s1 = "FGPUWCAMNORVHIKLMSTADNEJXYZ";
            //tempSequence.clear();
            //auto s1len = strlen(s1);
            //for (int zx = 0; zx < s1len; ++zx)
            //{
            //  tempSequence.push_back(s1[zx]);
            //}

            //FGPUWCANMORVHIKLSTACNDEJXYZ
            const auto s1 = "FGPUWCANMORVHIKLSTACNDEJXYZ";
            tempSequence.clear();
            auto s1len = strlen(s1);
            for (int zx = 0; zx < s1len; ++zx)
            {
                tempSequence.push_back(s1[zx]);
            }

            if (IsValidSuperSequence(
                wordCount, words, tempSequence, &firstMismatchIndex))
            {
                found = true;

                TryUpdateMinimum(
                    attempt, printSequence,
                    currentMinSize,
                    tempSequence, smallestSuperSequence);

                break;
            }

            value = next_greater_same_pop_count(value);
        } while (initialValue != value);

        if (!found)
        {
            cout << "Not found for digitCount=" << digitCount << ".\n";
            break;
        }
    }
}

void Privet::Algorithms::ShortestSuperSequence::AppendEnding(
    const vector< char >& ending,
    vector< char >& sequence)
{
    if (ending.empty())
    {
        return;
    }

    const size_t newSize = sequence.size() + ending.size();
    sequence.reserve(newSize);

    sequence.insert(sequence.end(), ending.begin(), ending.end());
}

void Privet::Algorithms::ShortestSuperSequence::AppendMiddle(
    const size_t initialMask,
    const size_t size,
    const vector< char >& middle,
    vector< char >& sequence)
{
    size_t mask = initialMask;
    size_t position = 0;

    while (0 != mask)
    {
        if (1 & mask)
        {
            const size_t index = position % size;
            sequence.push_back(middle[index]);
        }

        ++position;
        mask = mask >> 1;
    }
}

void Privet::Algorithms::ShortestSuperSequence::TryUpdateMinimum(
    const size_t attempt,
    const PrintSequenceFunction printSequence,
    size_t& currentMinSize,
    vector< char >& tempSequence,
    vector< char >& smallestSuperSequence)
{
    if (tempSequence.size() < currentMinSize)
    {
#pragma omp critical (SSS_CriticalSectionName)
        {
            if (tempSequence.size() < currentMinSize)
            {
                currentMinSize = tempSequence.size();
                swap(smallestSuperSequence, tempSequence);

                cout << "attempt=" << attempt << ": ";
                printSequence(smallestSuperSequence);
            }
        }
    }
}

void Privet::Algorithms::ShortestSuperSequence::AttempOneValue(
    const unsigned int wordCount,
    const char words[][Dimensions],
    const PrintSequenceFunction printSequence,
    const size_t beginningSize,
    const size_t remainingSize,
    const vector< char >& remaining,
    const vector< char >& ending,
    const size_t minPopCount,
    const size_t attempt,

    size_t& currentMinSize,
    vector< char >& tempSequence,
    vector< char >& smallestSuperSequence)
{
    const auto popCount = PopCount(attempt);
    if (currentMinSize <= popCount || popCount < minPopCount)
    {//It makes sense to try smaller minimum - 33% speedup, from 72 to 47 sec.
      //OMP - time drops from 47 sec to 12 sec, 75% speedup.
        return;
    }

    //Copy beginning.
    tempSequence.resize(beginningSize);

    AppendMiddle(attempt, remainingSize, remaining, tempSequence);
    AppendEnding(ending, tempSequence);

    unsigned int firstMismatchIndex = 0;

    if (IsValidSuperSequence(
        wordCount, words, tempSequence, &firstMismatchIndex))
    {
        TryUpdateMinimum(
            attempt, printSequence,
            currentMinSize,
            tempSequence, smallestSuperSequence);
    }
}

void Privet::Algorithms::ShortestSuperSequence::ReportProgress(
    const size_t mask,
    const size_t maxAttempts,
    const size_t attempt,
    const size_t smallestSuperSequenceSize)
{
    const double ratioRaw = attempt / static_cast<double>(maxAttempts);
    const double ratio = 1.0 == ratioRaw ?
        (1 - numeric_limits<double>::epsilon()) : ratioRaw;

    const auto prec = StreamUtilities::GetMaxDoublePrecision();

    cout << "  Attempt=" << attempt
        << ", smallestSize=" << smallestSuperSequenceSize
        << ", mask=" << mask
        << ", ratio left=" << setprecision(prec) << ratio
        << ".\n";
}