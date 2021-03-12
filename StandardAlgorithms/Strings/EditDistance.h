#pragma once

#include <algorithm> // min, max
#include <vector>
#include "../Numbers/Sort.h" //Min3
#include "../Numbers/Arithmetic.h"
#include "../Utilities/ExceptionUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            template <typename T = char,
                typename TCollection = std::string,
                typename TSize = size_t>
                TSize EditDistance(
                    const TCollection& source, const TCollection& target,
                    const TSize addCost, const TSize changeCost, const TSize deleteCost)
            {
                RequirePositive(addCost, "addCost");
                RequirePositive(changeCost, "changeCost");
                RequirePositive(deleteCost, "deleteCost");

                const auto sourceSize = static_cast<TSize>(source.size()),
                    targetSize = static_cast<TSize>(target.size()),
                    minSize = std::min(sourceSize, targetSize);

                auto beginMatches = TSize(0);
                while (beginMatches < minSize
                    && source[beginMatches] == target[beginMatches])
                {
                    ++beginMatches;
                }

                if (beginMatches == sourceSize)
                {
                    const auto result1 = MultiplyUnsigned<TSize>(targetSize - sourceSize, addCost);
                    return result1;
                }
                if (beginMatches == targetSize)
                {
                    const auto result1 = MultiplyUnsigned<TSize>(sourceSize - targetSize, deleteCost);
                    return result1;
                }
                //Having a partial match.

                auto endMatches = TSize(0);
                while (endMatches < minSize
                    && source[sourceSize - endMatches - 1]
                    == target[targetSize - endMatches - 1])
                {
                    ++endMatches;
                }

                const auto sourceEndingExclusive = sourceSize - endMatches,
                    sourceSymbolsToMatch = sourceEndingExclusive - beginMatches;
                if (0 == sourceSymbolsToMatch)
                {
                    const auto delta = targetSize - sourceSize,
                        result1 = MultiplyUnsigned<TSize>(delta, addCost);
                    return result1;
                }

                const auto targetEndingExclusive = targetSize - endMatches,
                    targetSymbolsToMatch = targetEndingExclusive - beginMatches;
                if (0 == targetSymbolsToMatch)
                {
                    const auto delta = sourceSize - targetSize,
                        result1 = MultiplyUnsigned<TSize>(delta, deleteCost);
                    return result1;
                }

                const TSize tempArraySize = 1 + targetSymbolsToMatch;
                std::vector<TSize> previous(tempArraySize), current(tempArraySize);

                for (TSize i = 0, tempCost = 0;
                    i < tempArraySize;
                    ++i, tempCost += deleteCost)
                {
                    previous[i] = tempCost;
                    current[i] = tempCost;
                }

                for (TSize i = 0; i < sourceSymbolsToMatch; ++i)
                {
                    current[0] = (i + 1) * deleteCost;
                    for (TSize j = 0; j < targetSymbolsToMatch; ++j)
                    {
                        const T& ci = source[beginMatches + i],
                            & cj = target[beginMatches + j];

                        const TSize costC = ci == cj ? 0 : changeCost;
                        const TSize a = previous[j + 1] + deleteCost;
                        const TSize b = previous[j] + costC;
                        const TSize c = current[j] + addCost;
                        current[j + 1] = Privet::Algorithms::Numbers::Min3(a, b, c);
                    }
                    std::swap(previous, current);
                }

                const auto result = previous[targetSymbolsToMatch];
                return result;
            }
        }
    }
}