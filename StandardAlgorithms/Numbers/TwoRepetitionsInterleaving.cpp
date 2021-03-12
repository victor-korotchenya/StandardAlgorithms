#include "Arithmetic.h"
#include "TwoRepetitionsInterleaving.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

bool TwoRepetitionsInterleaving::FindInterleaving(
    const vector< bool >& sourceSignal,
    const vector< bool >& sequence1,
    const vector< bool >& sequence2,
    vector< size_t >& chosenIndexes1)
{
    const size_t sourceSize = sourceSignal.size();
    if (0 == sourceSize)
    {
        throw exception("There must be not empty input signal string.");
    }

    const size_t size1 = sequence1.size();
    if (0 == size1)
    {
        throw exception("There must be not empty sequence1.");
    }

    const size_t size2 = sequence2.size();
    if (0 == size2)
    {
        throw exception("There must be not empty sequence2.");
    }

    chosenIndexes1.clear();

    const bool hasFirstSymbolMatch = sourceSignal[0] == sequence1[0]
        || sourceSignal[0] == sequence2[0];
    if (!hasFirstSymbolMatch)
    {//Quick fail.
        return false;
    }

    const auto sourceSizePlusOne = AddUnsigned<size_t>(1, sourceSize);

    //possibleInterleavings[i][j] means there is an interleaving
    // of sizes (sequence1 ^ x).substr(0, i)
    // and (sequence2 ^ y).substr(0, j).
    //It has 0 at first row and column.
    vector<vector<bool>> possibleInterleavings(sourceSizePlusOne, vector<bool>(sourceSizePlusOne));

    FillMatrix(sourceSignal, sequence1, sequence2, possibleInterleavings);

    const auto result = TryFindSolution(
        sourceSignal, sequence1, possibleInterleavings, chosenIndexes1);
    return result;
}

void TwoRepetitionsInterleaving::FillMatrix(
    const vector< bool >& sourceSignal,
    const vector< bool >& sequence1,
    const vector< bool >& sequence2,
    vector< vector< bool > >& possibleInterleavings)
{
    const auto sourceSize = sourceSignal.size(),
        size1 = sequence1.size(),
        size2 = sequence2.size();

    possibleInterleavings[0][1] = true;
    possibleInterleavings[1][0] = true;

    for (size_t k = 0; k < sourceSize; ++k)
    {
        for (size_t i = 0; i <= k; ++i)
        {
            const size_t j = k - i;

            //Propagate the truth
            const bool hasSequence1 = possibleInterleavings[i][j + 1]
                && sourceSignal[k] == sequence1[i % size1];

            if (hasSequence1 ||
                possibleInterleavings[i + 1][j] && sourceSignal[k] == sequence2[j % size2])
            {
                possibleInterleavings[i + 1][j + 1] = true;
            }
        }
    }
}

bool TwoRepetitionsInterleaving::TryFindSolution(
    const vector< bool >& sourceSignal,
    const vector< bool >& sequence1,
    const vector< vector< bool > >& possibleInterleavings,
    vector < size_t >& chosenIndexes1)
{
    const size_t sourceSizePlusOne = 1 + sourceSignal.size();

    for (size_t i = 1; i < sourceSizePlusOne; ++i)
    {
        const size_t j = sourceSizePlusOne - i;
        if (possibleInterleavings[i][j])
        {
            Backtrack(sourceSignal, sequence1, i, chosenIndexes1);
            return true;
        }
    }

    return false;
}

void TwoRepetitionsInterleaving::Backtrack(
    const vector< bool >& sourceSignal,
    const vector< bool >& sequence1,
    size_t i,
    vector< size_t >& chosenIndexes1)
{
#ifdef _DEBUG
    const size_t sourceSize = sourceSignal.size();
#endif
    const size_t size1 = sequence1.size();

    chosenIndexes1.reserve(i);

    size_t sourceIndex = 0;
    size_t index1 = 0;
    for (;;)
    {
        const bool symbol = sequence1[index1];
        while (symbol != sourceSignal[sourceIndex])
        {
            ++sourceIndex;
#ifdef _DEBUG
            if (sourceSize == sourceIndex)
            {
                throw exception("sourceSize == sourceIndex");
            }
#endif
        }

        chosenIndexes1.push_back(sourceIndex);

        if (0 == --i)
        {
            break;
        }

        ++sourceIndex;

        index1 = (1 + index1) % size1;
    }
}