#include <string>
#include <sstream>
#include "prime_number_utility.h"
#include "HashedBitArrayUtility.h"
#include "HashedBitArray.h"
#include "../Utilities/StreamUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

HashedBitArray::HashedBitArray(
    const size_t capacity, const size_t bitArraySize, const size_t numberOfHashFunctions)
    : _BitArray(BitArray_DefaultSize)
{
    Init(capacity, bitArraySize, numberOfHashFunctions);
}

HashedBitArray::HashedBitArray(const size_t capacity)
    : _BitArray(BitArray_DefaultSize)
{
    const double errorProbability = HashedBitArrayUtility::ErrorProbability(capacity);
    const size_t bitArraySize = HashedBitArrayUtility::BitArraySize(capacity, errorProbability);
    const size_t numberOfHashFunctions =
        HashedBitArrayUtility::NumberOfHashFunctions(capacity, bitArraySize);

    Init(capacity, bitArraySize, numberOfHashFunctions);
}

void HashedBitArray::Init(
    const size_t capacity, const size_t bitArraySize, const size_t numberOfHashFunctions)
{
    HashedBitArrayUtility::CheckCapacity(capacity);
    HashedBitArrayUtility::CheckBitArraySize(bitArraySize);
    HashedBitArrayUtility::CheckNumberOfHashFunctions(
        capacity, bitArraySize, numberOfHashFunctions);

#ifdef _DEBUG
    if (!is_prime_simple(bitArraySize))
    {
        ostringstream ss;
        ss << "_DEBUG. The bitArraySize (" << bitArraySize << ") must be a prime number.";

        StreamUtilities::throw_exception(ss);
    }
#endif
    _BitArraySize = bitArraySize;
    _NumberOfHashFunctions = numberOfHashFunctions;

    _BitArray.Resize(bitArraySize);
}

void HashedBitArray::Add(const size_t hashCode)
{
    size_t sum = hashCode;
    for (size_t i = 0; i < _NumberOfHashFunctions; ++i)
    {
        const size_t bitNumber = sum % _BitArraySize;

        _BitArray.SetBit(bitNumber);

        sum += hashCode;
    }
}

bool HashedBitArray::Has(const size_t hashCode) const
{
    size_t sum = hashCode;
    for (size_t i = 0; i < _NumberOfHashFunctions; ++i)
    {
        const size_t bitNumber = sum % _BitArraySize;

        unsigned char value = _BitArray.GetBit(bitNumber);
        if (0 == value)
        {
            return false;
        }

        sum += hashCode;
    }

    return true;
}

size_t HashedBitArray::GetBitCount() const
{
    const size_t result = _BitArray.GetBitCount();

#ifdef _DEBUG
    if (result > _BitArraySize)
    {
        ostringstream ss;
        ss << "_BitArray.GetBitCount returns " << result <<
            " while there are " << _BitArraySize << " bits.";
        StreamUtilities::throw_exception(ss);
    }
#endif

    return result;
}