#include "../Utilities/Random.h"
#include "HashedBitArray.h"
#include "HashedBitArrayTests.h"
#include "../Utilities/StreamUtilities.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    void PropertiesTest(const HashedBitArray& hashedBitArray,
        const size_t expectedBitArraySize,
        const size_t expectedNumberOfHashFunctions)
    {
        const size_t actualBitArraySize = hashedBitArray.GetBitArraySize();
        Assert::AreEqual(expectedBitArraySize, actualBitArraySize, "BitArraySize");

        const size_t actualNumberOfHashFunctions = hashedBitArray.GetNumberOfHashFunctions();
        Assert::AreEqual(expectedNumberOfHashFunctions, actualNumberOfHashFunctions, "NumberOfHashFunctions");
    }

    void AddValues(const vector< size_t >& hashes, HashedBitArray& hashedBitArray)
    {
        const size_t hashesCount = hashes.size();

        for (size_t index = 0; index < hashesCount; ++index)
        {
            const auto value = hashes[index];
            hashedBitArray.Add(value);
            if (!hashedBitArray.Has(value))
            {
                ostringstream ss;
                ss << "Right after adding the hash[" << index <<
                    "], HashedBitArray must have a value, but it does not.";
                StreamUtilities::throw_exception(ss);
            }
        }
    }

    void CheckValues(const vector< size_t >& hashes, const HashedBitArray& hashedBitArray)
    {
        const size_t hashesCount = hashes.size();

        for (size_t index = 0; index < hashesCount; ++index)
        {
            const auto value = hashes[index];
            if (!hashedBitArray.Has(value))
            {
                ostringstream ss;
                ss << "HashedBitArray must have a value, but it does not at index=" <<
                    index << ".";
                StreamUtilities::throw_exception(ss);
            }
        }
    }

    void BitCountTest(const HashedBitArray& hashedBitArray, const size_t expectedBitArraySize)
    {
        const size_t bitCount = hashedBitArray.GetBitCount();
        if (bitCount > expectedBitArraySize)
        {
            ostringstream ss;
            ss << "bitCount(" << bitCount << ") > expectedBits(" << expectedBitArraySize << ").";
            StreamUtilities::throw_exception(ss);
        }
    }
}

void HashedBitArrayTests::Test()
{
    const size_t capacity = 345;

    const size_t capacityPlusExtra = capacity + 1;
    const size_t expectedBitArraySize = 4211;
    const size_t expectedNumberOfHashFunctions = 8;

    HashedBitArray hashedBitArray(capacityPlusExtra);

    PropertiesTest(hashedBitArray, expectedBitArraySize, expectedNumberOfHashFunctions);

    vector <size_t> hashes;
    FillRandom(hashes, capacity);

    AddValues(hashes, hashedBitArray);
    CheckValues(hashes, hashedBitArray);

    BitCountTest(hashedBitArray, expectedBitArraySize);
}