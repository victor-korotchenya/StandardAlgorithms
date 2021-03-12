#pragma once
#include <vector>
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template <typename Number, typename Size>
    bool CheckInput(const std::vector<Number>& numbers, const Size expected_sum)
    {
        const auto numbersSize = numbers.size();
        RequirePositive(numbersSize, "There must be at least one positive number.");

        RequirePositive(expected_sum, "The expected sum must be positive.");

        auto has = false;
        for (Size i = 0; i < numbersSize; ++i)
        {
            if (numbers[i] <= 0)
            {
                std::ostringstream ss;
                ss << "The number " << numbers[i]
                    << " at index " << i
                    << " must be positive.";
                StreamUtilities::throw_exception(ss);
            }

            if (static_cast<Size>(numbers[i]) <= expected_sum)
            {
                has = true;
            }
        }

        return has;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //It is NP-complete problem.
            //If all numbers are greater than expected_sum, return 0.
            //TODO: p3. There is no check for overflows.
            template <typename Number, typename Size>
            Size FindMaximumSum(
                const std::vector<Number>& numbers,
                const Size expected_sum,
                std::vector<Size>& chosenIndexes)
            {
                if (!CheckInput<>(numbers, expected_sum))
                    return 0;

                const auto numbersSize = static_cast<Size>(numbers.size());
                std::vector<std::vector<bool>> reachables(numbersSize + 1, std::vector<bool>(expected_sum + 1));

                for (Size index = 0; index <= numbersSize; ++index)
                    //Can make 0 sum.
                    reachables[index][0] = true;

                //numbers from 0-th index up to "i" - rows, sums - columns.
                for (Size index = 0; index < numbersSize; ++index)
                {
                    for (Size sum = 1; sum <= expected_sum; ++sum)
                    {
                        reachables[index + 1][sum] = reachables[index][sum]
                            || numbers[index] <= sum && reachables[index][sum - numbers[index]];
                    }
                }

                Size result = 0;
                for (Size sum = expected_sum; 0 < sum; --sum)
                {
                    if (reachables[numbersSize][sum])
                    {
                        result = sum;
                        break;
                    }
                }
                RequirePositive(result, "FindMaximumSum must return positive value.");

                chosenIndexes.clear();
                auto rest = result;
                Size last_index = numbersSize;
                for (;;)
                {//Make a greedy choice.
                    if (numbers[last_index - 1] <= rest
                        && reachables[last_index][rest - numbers[last_index - 1]])
                    {
                        chosenIndexes.push_back(last_index - 1);
                        rest -= numbers[last_index - 1];
                        if (0 == rest)
                            break;
                    }

                    if (0 == last_index--)
                        //prevent an approximation error.
                        break;
                }

                RequirePositive(chosenIndexes.size(),
                    "There must be chosenIndexes in FindMaximumSum.");

                if (0 != rest)
                {
                    std::ostringstream ss;
                    ss << "The remainder sum (" << rest
                        << ") must be zero. result=" << result << ".";
                    StreamUtilities::throw_exception<>(ss);
                }

                return result;
            }
        }
    }
}