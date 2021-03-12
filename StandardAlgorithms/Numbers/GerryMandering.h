#pragma once
#include <vector>
#include <string>
#include "Arithmetic.h"

namespace Privet::Algorithms::Numbers
{
    struct Precinct final
    {
        size_t AVoters = 0, BVoters = 0;

        inline size_t Sum() const
        {
            const size_t result = AddUnsigned(AVoters, BVoters);
            return result;
        }

        friend inline std::ostream& operator <<
            (std::ostream& str, Precinct const& b)
        {
            str
                << "(" << b.AVoters << ", " << b.BVoters << ")";
            return str;
        }
    };

    class GerryMandering final
    {
        GerryMandering() = delete;

    public:
        //The first party is to win.
        static bool Partition(
            const std::vector< Precinct >& precincts,
            std::vector< size_t >& chosenIndexes);

        //TODO: p3. make it faster - use fewer false values.
        //Current running time is (size^4 * (a+b)^2).

    private:
        static void PreProcessPrecincts(
            const std::vector< Precinct >& precincts,
            size_t& votersPerPrecinct,
            size_t& totalVotersA,
            size_t& totalVotersB);

        static void ResizeMatrix(
            const size_t halfSizePlusOne,
            const size_t sizePlusOne,
            const size_t votersMaxExclusive,
            std::vector< std::vector< std::vector< std::vector< bool > > > >& possibles);

        static void CalcMatrix(
            const std::vector< Precinct >& precincts,
            const size_t votersMaxExclusive,
            std::vector< std::vector< std::vector< std::vector< bool > > > >& possibles);

        static bool Backtrack(
            const size_t votersMinInclusive,
            const size_t votersMaxExclusive,
            const std::vector <std::vector< bool > >& possible2);
    };
}