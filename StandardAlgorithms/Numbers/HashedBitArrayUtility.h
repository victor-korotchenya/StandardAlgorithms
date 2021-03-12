#pragma once

namespace Privet::Algorithms::Numbers
{
    class HashedBitArrayUtility final
    {
        HashedBitArrayUtility() = delete;

    public:

        static double ErrorProbability(const size_t capacity);

        static size_t BitArraySize(const size_t capacity, const double errorProbability);

        static size_t NumberOfHashFunctions(const size_t capacity, const size_t bitArraySize);

        static void CheckCapacity(const size_t value);

        static void CheckBitArraySize(const size_t value);

        static void CheckErrorProbability(const double value);

        static void CheckNumberOfHashFunctions(
            const size_t capacity,
            const size_t bitArraySize,
            const size_t value);
    };
}