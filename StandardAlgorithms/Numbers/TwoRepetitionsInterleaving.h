#pragma once
#include <vector>

namespace Privet::Algorithms::Numbers
{
    class TwoRepetitionsInterleaving final
    {
        TwoRepetitionsInterleaving() = delete;

    public:

        static bool FindInterleaving(
            //The source signal.
            const std::vector< bool >& sourceSignal,
            //The first sequence to repeat.
            const std::vector< bool >& sequence1,
            //The second one.
            const std::vector< bool >& sequence2,
            //The "chosenIndexes2" can be inferred from "chosenIndexes1"
            // provided the returned value is true.
            std::vector< size_t >& chosenIndexes1);

    private:

        static void FillMatrix(
            const std::vector< bool >& sourceSignal,
            const std::vector< bool >& sequence1,
            const std::vector< bool >& sequence2,
            std::vector< std::vector< bool > >& possibleInterleavings);

        static bool TryFindSolution(
            const std::vector< bool >& sourceSignal,
            const std::vector< bool >& sequence1,
            const std::vector< std::vector< bool > >& possibleInterleavings,
            std::vector< size_t >& chosenIndexes1);

        //Match the first "i" symbols from sequence1.
        static void Backtrack(
            const std::vector< bool >& sourceSignal,
            const std::vector< bool >& sequence1,
            size_t i,
            std::vector< size_t >& chosenIndexes1);
    };
}