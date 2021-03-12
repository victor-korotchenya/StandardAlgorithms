#pragma once
#include <vector>
#include "../disable_constructors.h"

namespace Privet::Algorithms::Numbers
{
    class BitArray final
    {
        DISABLE_ALL_CONSTRUCTORS(BitArray);

        std::vector< unsigned long long > _Data;
        unsigned char* _DataRaw;

        static const size_t _BitesPerByte = 8;
        static const size_t _WordsPerByte = 8;
        static const size_t _BitesPerWord = _BitesPerByte * _WordsPerByte;

    public:

        explicit BitArray(const size_t initialSizeBits);

        void Resize(const size_t newSizeBits);

        //Sets a bit in the bit-array.
        void SetBit(const size_t position);

        //Gets a bit from the bit-array.
        unsigned char GetBit(const size_t position) const;

        //Returns the number of the set bits.
        size_t GetBitCount() const;

    private:
        static size_t CalcWords(const size_t bitArraySize);

#ifdef _DEBUG
        void CheckPosition(const size_t position) const;
#endif
    };
}