#pragma once
#include "../Utilities/StreamUtilities.h"
#include "SubSet.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Generates all the subsets of a set of size n.
            template <typename Number = size_t>
            class SubSetsNumber final
            {
                SubSetsNumber() = delete;

            public:

                using SubSets = typename SubSet<Number>::TSubSets;
                using SubSequence = typename SubSet<Number>::TSubSequence;

                static SubSets AllNonEmptySubSets(
                    const Number& setSize,
                    const Number& minimumValue = 0);
            };

            template <typename Number>
            typename SubSetsNumber<Number>::SubSets
                SubSetsNumber<Number>::AllNonEmptySubSets(
                    const Number& setSize,
                    const Number& minimumValue)
            {
                const Number minSetSize = 1;
                if (setSize < minSetSize)
                {
                    std::ostringstream ss;
                    ss << "The setSize(" << setSize << ") must be at least " << minSetSize << ".";
                    StreamUtilities::throw_exception(ss);
                }

                const Number maxSetSize = 63;
                if (maxSetSize < setSize)
                {
                    std::ostringstream ss;
                    ss << "The setSize(" << setSize << ") must not exceed " << maxSetSize << ".";
                    StreamUtilities::throw_exception(ss);
                }

                SubSets result;

                for (Number tempSize = 1; tempSize <= setSize; ++tempSize)
                {
                    SubSet<Number> subSet{ SubSequence{ minimumValue + tempSize} };
                    result.insert(subSet);
                }

                return result;
            }
        }
    }
}