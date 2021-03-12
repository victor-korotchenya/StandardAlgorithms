#include <sstream>
#include "GerryMandering.h"
#include "../Utilities/StreamUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

bool GerryMandering::Partition(
    const vector< Precinct >& precincts,
    vector< size_t >& chosenIndexes)
{
    const size_t size = precincts.size();
    const size_t one = 1;

    size_t votersPerPrecinct = 0, totalVotersA = 0, totalVotersB = 0;

    PreProcessPrecincts(precincts, votersPerPrecinct, totalVotersA, totalVotersB);

    chosenIndexes.clear();

    //Quick test.
    //10 vs 9 - cannot win in both.
    //10 == a1 + a2; 9 = b1 + b2;
    //e.g. 5 + 5 => both b1, b2 <= 4 => the sum can be maximum 8.
    //e.g. 6 + 4 => b1 <= 5, b2 <= 3 => the sum can be maximum 8.
    if (totalVotersA - one <= totalVotersB)
    {
        return false;
    }
    //TODO: p3. impl other special (quick to check) cases.

    const size_t halfSize = size >> 1;
    const size_t halfSizePlusOne = halfSize + one;
    const size_t sizePlusOne = AddUnsigned(one, size);

    const auto votersMaxExclusive = MultiplyUnsigned(size, votersPerPrecinct);
    const size_t votersMinInclusive = (votersMaxExclusive >> 2) + one;

    //c - consider precincts from 0 to c,
    //d - # of precincts, chosen for Region 1,
    //e - there are at least e voters for A in Region 1,
    //f - there are at least f voters for A in Region 2.
    vector< vector< vector< vector< bool > > > > possibles;

    ResizeMatrix(halfSizePlusOne, sizePlusOne, votersMaxExclusive, possibles);

    CalcMatrix(precincts, votersMaxExclusive, possibles);

    const bool result = Backtrack(
        votersMinInclusive, votersMaxExclusive,
        possibles[size][halfSize]);

    return result;
}

void GerryMandering::PreProcessPrecincts(
    const vector< Precinct >& precincts,
    size_t& votersPerPrecinct,
    size_t& totalVotersA,
    size_t& totalVotersB)
{
    const size_t size = precincts.size();
    if (size < 2)
    {
        ostringstream ss;
        ss << "The number of precincts (" << size << ") must be at least 2.";
        StreamUtilities::throw_exception(ss);
    }

    const size_t one = 1;
    const bool isOddSize = one == (size & one);
    if (isOddSize)
    {
        ostringstream ss;
        ss << "The number of precincts (" << size << ") must be even.";
        StreamUtilities::throw_exception(ss);
    }

    votersPerPrecinct = precincts[0].AVoters + precincts[0].BVoters;

    totalVotersA = 0;
    totalVotersB = 0;

    for (size_t i = 0; i < size; ++i)
    {
        const Precinct& precinct = precincts[i];

        const size_t sum = precinct.Sum();
        if (votersPerPrecinct != sum)
        {
            ostringstream ss;
            ss << "The number of voters (" << sum << ") per precinct at [" << i
                << "] must be " << votersPerPrecinct <<
                ", the same for all of them.";
            StreamUtilities::throw_exception(ss);
        }

        totalVotersA = AddUnsigned(totalVotersA, precinct.AVoters);
        totalVotersB = AddUnsigned(totalVotersB, precinct.BVoters);
    }
}

void GerryMandering::ResizeMatrix(
    const size_t halfSizePlusOne,
    const size_t sizePlusOne,
    const size_t votersMaxExclusive,
    vector< vector< vector< vector< bool > > > >& possibles)
{
    possibles.resize(sizePlusOne);
    for (size_t c = 0; c < sizePlusOne; ++c)
    {
        vector< vector< vector< bool > > >& possible1 = possibles[c];

        possible1.resize(halfSizePlusOne);
        for (size_t d = 0; d < halfSizePlusOne; ++d)
        {
            vector< vector< bool > >& possible2 = possible1[d];

            possible2.resize(votersMaxExclusive);
            for (size_t e = 0; e < votersMaxExclusive; ++e)
            {
                possible2[e].resize(votersMaxExclusive);
            }
        }
    }
}

void GerryMandering::CalcMatrix(
    const vector< Precinct >& precincts,
    const size_t votersMaxExclusive,
    vector< vector< vector< vector< bool > > > >& possibles)
{
    const size_t size = precincts.size();
    const size_t halfSize = size >> 1;

    possibles[0][0][0][0] = true;

    for (size_t c = 0; c < size; ++c)
    {
        const size_t aVoters = precincts[c].AVoters;

        for (size_t d = 1; d <= halfSize; ++d)
        {
            for (size_t e = 0; e < votersMaxExclusive; ++e)
            {
                const bool hasEVoters = aVoters <= e;

                const size_t fInitialValue = hasEVoters ? 0 : aVoters;

                for (size_t f = fInitialValue; f < votersMaxExclusive; ++f)
                {
                    const bool isOk =
                        hasEVoters && possibles[c][d - 1][e - aVoters][f]
                        || aVoters <= f && possibles[c][d][e][f - aVoters];
                    if (isOk)
                    {
                        possibles[c + 1][d][e][f] = true;
                    }
                }
            }
        }
    }
}

bool GerryMandering::Backtrack(
    const size_t votersMinInclusive,
    const size_t votersMaxExclusive,
    const vector <vector< bool > >& possible2)
{
    for (size_t e = votersMinInclusive; e < votersMaxExclusive; ++e)
    {
        const vector< bool >& possible3 = possible2[e];

        for (size_t f = votersMinInclusive; f < votersMaxExclusive; ++f)
        {
            if (possible3[f])
            {
                //TODO: p3. Fill the "chosenIndexes".
                return true;
            }
        }
    }

    return false;
}