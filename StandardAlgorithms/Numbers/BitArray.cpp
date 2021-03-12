#include <exception>
#include <string>
#include <sstream>
#include "Arithmetic.h"
#include "BitArray.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

BitArray::BitArray(const size_t initialSizeBits)
    : _DataRaw(nullptr)
{
    static_assert(_WordsPerByte == sizeof(unsigned long long),
        "The size of unsigned long long must be _WordsPerByte bytes.");

    Resize(initialSizeBits > 0 ? initialSizeBits : 1);
}

void BitArray::Resize(const size_t newSizeBits)
{
    if (0 == newSizeBits)
    {
        throw exception("The new size of the Bit Storage must be positive.");
    }

    const size_t words = CalcWords(newSizeBits);

    _Data.resize(words, 0);
    _DataRaw = reinterpret_cast<unsigned char*>(_Data.data());
}

//void AssignAllBits(const unsigned char byteValue = 0);
//
//void BitArray::AssignAllBits(const unsigned char byteValue)
//{
//  const unsigned long long newValue = ConvertByDuplicating(byteValue);
//
//  _Data.assign(_Data.size(), newValue);
//}

size_t BitArray::CalcWords(const size_t bitArraySize)
{
    const size_t bitsDraft = PadByAlignment(bitArraySize, _BitesPerWord);
    const size_t words = bitsDraft / _BitesPerWord;

#ifdef _DEBUG
    if (0 == words)
    {
        throw exception("0 == words in BitArray::CalcWords.");
    }
#endif
    return words;
}

void BitArray::SetBit(const size_t position)
{
#ifdef _DEBUG
    CheckPosition(position);
#endif

    //0..7 =>  byte = 0 and bit = 0..7
    //8..15 => byte = 1 and bit = 0..7

    const size_t positionByte = position >> 3;
    const unsigned char positionBit = static_cast<unsigned char>(position & 7);

    static const unsigned char one = 1;
    const unsigned char oneShifted = one << positionBit;
    const unsigned char mask = ~oneShifted;

    const unsigned char oldByte = _DataRaw[positionByte];
    const unsigned char newByte = (oldByte & mask) | oneShifted;
    _DataRaw[positionByte] = newByte;
}

unsigned char BitArray::GetBit(const size_t position) const
{
#ifdef _DEBUG
    CheckPosition(position);
#endif
    const size_t positionByte = position >> 3;
    const unsigned char positionBit = static_cast<unsigned char>(position & 7);

    static const unsigned char one = 1;

    const unsigned char oldByte = _DataRaw[positionByte];
    const unsigned char result = (oldByte >> positionBit) & one;
    return result;
}

size_t BitArray::GetBitCount() const
{
    const size_t words = _Data.size();

    size_t result = 0;
    for (size_t index = 0; index < words; ++index)
    {
        const size_t cnt = PopCount(_Data[index]);
        result += cnt;
    }

#ifdef _DEBUG
    if (result > words * _BitesPerWord)
    {
        ostringstream ss;
        ss << "GetBitCount returns " << result <<
            " while there are " << (words * _BitesPerWord) << " bits.";
        StreamUtilities::throw_exception(ss);
    }
#endif

    return result;
}

#ifdef _DEBUG
void BitArray::CheckPosition(const size_t position) const
{
    const size_t sizeBits = _Data.size() * _BitesPerWord;
    if (sizeBits <= position)
    {
        ostringstream ss;
        ss << "sizeBits(" << sizeBits << ") <= position(" << position << ") in CheckPisition.";
        StreamUtilities::throw_exception(ss);
    }
}
#endif