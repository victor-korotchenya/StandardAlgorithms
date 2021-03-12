#pragma once

#include "BitArray.h"

namespace Privet::Algorithms::Numbers
{
    // The query "Has(hashCode)" returns either "possibly in set" or "definitely not in set".
    // Some call it Bloom's filter.
    class HashedBitArray final
    {
        DISABLE_ALL_CONSTRUCTORS(HashedBitArray);

        BitArray _BitArray;

        size_t _BitArraySize;
        size_t _NumberOfHashFunctions;
        static const size_t BitArray_DefaultSize = 1;

    public:

        explicit HashedBitArray(
            const size_t capacity,
            const size_t bitArraySize,
            const size_t numberOfHashFunctions);

        //TODO: p1. constructor chaining does not exist in MS VC++.

        explicit HashedBitArray(const size_t capacity);

        void Add(const size_t hashCode);

        bool Has(const size_t hashCode) const;

        inline size_t GetBitArraySize() const noexcept
        {
            return _BitArraySize;
        }

        inline size_t GetNumberOfHashFunctions() const noexcept
        {
            return _NumberOfHashFunctions;
        }

        //Returns the number of set bits.
        size_t GetBitCount() const;

    private:

        inline void Init(
            const size_t capacity,
            const size_t bitArraySize,
            const size_t numberOfHashFunctions);
    };
}