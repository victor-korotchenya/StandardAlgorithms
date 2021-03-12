#include <sstream>
#include "prime_number_utility.h"
#include "HashedBitArrayUtility.h"
#include "../Utilities/StreamUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

double HashedBitArrayUtility::ErrorProbability(const size_t capacity)
{
    CheckCapacity(capacity);

    const double capacityCast = double(capacity);
    const double result = 1.0 / capacityCast;
#ifdef _DEBUG
    CheckErrorProbability(result);
#endif
    return result;
}

size_t HashedBitArrayUtility::BitArraySize(
    const size_t capacity, const double errorProbability)
{
    CheckCapacity(capacity);
    CheckErrorProbability(errorProbability);

    const double ratio = (-2.082) * log(errorProbability);

    const double capacityBits = capacity * ratio;
    const size_t capacityBitsCast = static_cast<size_t>(capacityBits);

    if (capacityBitsCast < capacity)
    {
        ostringstream ss;
        ss << "Got an overflow when calculating array size for capacity=" << capacity
            << ", error probability=" << errorProbability << ".";
        StreamUtilities::throw_exception(ss);
    }

    const size_t result = calc_prime(capacityBitsCast);
    if (result < capacityBitsCast)
    {
        ostringstream ss;
        ss << "Error in calc_prime when calculating array size for capacity=" << capacity
            << ", error probability=" << errorProbability << ".";
        StreamUtilities::throw_exception(ss);
    }

    CheckBitArraySize(result);

    return result;
}

size_t HashedBitArrayUtility::NumberOfHashFunctions(
    const size_t capacity, const size_t bitArraySize)
{
    CheckCapacity(capacity);
    CheckBitArraySize(bitArraySize);

    const double l2 = log(2.0);
    const double resultRaw = (l2 * bitArraySize) / double(capacity);

    const size_t result = static_cast<size_t>(resultRaw);

    CheckNumberOfHashFunctions(capacity, bitArraySize, result);

    return result;
}

void HashedBitArrayUtility::CheckCapacity(const size_t value)
{
    const size_t minValue = 4;
    if (value < minValue)
    {
        ostringstream ss;
        ss << "The capacity (" << value << ") must be at least " << minValue << ".";
        StreamUtilities::throw_exception(ss);
    }
}

void HashedBitArrayUtility::CheckBitArraySize(const size_t value)
{
    const size_t minValue = 4;
    if (value < minValue)
    {
        ostringstream ss;
        ss << "The Bit Array Size (" << value << ") must be at least " << minValue << ".";
        StreamUtilities::throw_exception(ss);
    }
}

void HashedBitArrayUtility::CheckErrorProbability(const double value)
{
    if (value > 0 && value < 1)
    {
        return;
    }

    ostringstream ss;
    ss << "The error probability (" << value <<
        ") must belong to the range (0, 1) exclusively.";

    StreamUtilities::throw_exception(ss);
}

void HashedBitArrayUtility::CheckNumberOfHashFunctions(
    const size_t capacity,
    const size_t bitArraySize,
    const size_t value)
{
    const size_t minValue = 3;
    if (value < minValue)
    {
        ostringstream ss;
        ss << "The number of hash functions (" << value <<
            ") must be at least " << minValue
            << ", capacity=" << capacity
            << ", bitArraySize=" << bitArraySize << ".";
        StreamUtilities::throw_exception(ss);
    }

    if (capacity <= value)
    {
        ostringstream ss;
        ss << "The number of hash functions (" << value <<
            ") must be less than capacity (" << capacity << ")"
            << ", bitArraySize=" << bitArraySize << ".";
        StreamUtilities::throw_exception(ss);
    }
}