#pragma once
#include <algorithm>
#include <utility>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/VectorUtilities.h"
#include "../Graphs/BinaryIndexedTree.h"
#include "StandardOperations.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array of positive integers,
            //select strictly increasing items with maximum total sum.
            //Return the sum, and the chosen item indexes sorted.
            //
            //For example, given { 3, 2, 5 }, the sum is 8, indexes are { 0, 2 }.
            template <typename Number = unsigned int,
                //To avoid overflows.
                typename LongNumber = unsigned long long int,
                typename Size = size_t>
                class MaxSumIncreasingSequence final
            {
                MaxSumIncreasingSequence() = delete;

                static_assert(sizeof(Number) <= sizeof(LongNumber),
                    "LongNumber must not be smaller than Number.");

                using BinaryIndexedTree = Privet::Algorithms::Trees::BinaryIndexedTree
                    <LongNumber, LongNumber(*)(const LongNumber&, const LongNumber&)>;

                using value_index_pair_t = std::pair<Number, Size>;

            public:

                //Time O(n * n).
                static std::pair<LongNumber, std::vector<Size>>
                    compute_slow(const std::vector<Number>& values)
                {
                    ThrowIfEmpty(values, "values");
                    RequireAllPositive<Number>(values, "values");

                    const auto size = values.size();

                    std::vector<LongNumber> sums(size + 1);
                    std::vector<Size> previous_indexes(size);

                    LongNumber best_sum = sums[1] = values[0];
                    Size best_index{};

                    for (Size ending = 1; ending < size; ++ending)
                    {
                        sums[ending + 1] = values[ending];

                        Size beginning = 0;
                        do
                        {
                            if (values[beginning] < values[ending])
                            {
                                const auto candidate = sums[beginning + 1] + values[ending];
                                if (sums[ending + 1] < candidate)
                                {
                                    sums[ending + 1] = candidate;
                                    previous_indexes[ending] = beginning;
                                }
                            }
                        } while (++beginning < ending);

                        if (best_sum < sums[ending + 1])
                        {
                            best_sum = sums[ending + 1];
                            best_index = ending;
                        }
                    }

                    LongNumber remainder = best_sum;
                    std::vector<Size> chosen_indexes;
                    for (;;)
                    {
                        chosen_indexes.push_back(best_index);
                        if (remainder < values[best_index])
                        {
                            std::ostringstream ss;
                            ss << "Error: best_sum_slow_(" << remainder
                                << ") < values[" << best_index << "] = " << values[best_index] << ".";
                            StreamUtilities::throw_exception(ss);
                        }

                        remainder -= values[best_index];
                        best_index = previous_indexes[best_index];

                        if (0 == remainder)
                        {
                            break;
                        }
                    }

                    std::reverse(chosen_indexes.begin(), chosen_indexes.end());

                    return{ best_sum, chosen_indexes };
                }

                //Return the pair(max sum, chosen indexes).
                //Time O(n * log(n)).
                static std::pair<LongNumber, std::vector<Size>>
                    compute_fast(const std::vector<Number>& values)
                {
                    ThrowIfEmpty(values, "values");
                    RequireAllPositive<Number>(values, "values");

                    const auto increasing_value_positions = to_increasing_value_positions(values);

                    const auto size = values.size();
                    BinaryIndexedTree indexedTree(size
                        //avoid extra checks.
                        + 3, Max<LongNumber>, Max<LongNumber>);

                    const auto result = compute_fast_max_sum(
                        values, increasing_value_positions, indexedTree);
                    return result;
                }

            private:
                static std::vector<Size> to_increasing_value_positions(
                    const std::vector<Number>& values)
                {
                    const auto value_index_pairs = to_value_index(values);

                    const auto size = values.size();
                    std::vector<Size> result(size);

                    for (Size i = 0, increasingIndex =
                        //The tree index starts from 1.
                        1; i < size; ++i)
                    {
                        const auto& pair1 = value_index_pairs[i];
                        result[pair1.second] = increasingIndex;

                        const auto& pair2 = value_index_pairs[i + 1];
                        const auto are_different = pair1.first != pair2.first;
                        increasingIndex += are_different;
                    }

                    return result;
                }

                static std::vector<value_index_pair_t>
                    to_value_index(const std::vector<Number>& values)
                {
                    const auto size = values.size();

                    //The last entry will be (0, 0).
                    std::vector<value_index_pair_t> result(size + 1);

                    for (auto i = 0; i < size; ++i)
                    {
                        result[i] = std::make_pair(values[i], i);
                    }

                    std::sort(result.begin(), result.end() - 1);

                    return result;
                }

                static std::pair<LongNumber, std::vector<Size>> compute_fast_max_sum(
                    const std::vector<Number>& values,
                    const std::vector<Size>& increasing_value_positions,
                    BinaryIndexedTree& indexedTree)
                {
                    const auto size = values.size();
                    std::vector<Size> chosen_indexes;
                    std::vector<LongNumber> previous_values(size);

                    LongNumber max_sum{};
                    Size best_index{};
                    for (Size i = 0; i < size; ++i)
                    {
                        const auto& smallest_position = increasing_value_positions[i];
                        const auto previous_value = indexedTree.get(smallest_position);

                        const auto total_sum = previous_values[i] = previous_value + values[i];
                        indexedTree.set(smallest_position + 1, total_sum);

                        if (max_sum < total_sum)
                        {
                            max_sum = total_sum;
                            best_index = i;
                        }
                    }

                    auto remainder = max_sum;
                    for (;;)
                    {
                        chosen_indexes.push_back(best_index);
                        if (remainder < values[best_index])
                        {
                            std::ostringstream ss;
                            ss << "Error: best_sum_fast_(" << remainder
                                << ") < values[" << best_index << "] = " << values[best_index] << ".";
                            StreamUtilities::throw_exception(ss);
                        }

                        remainder -= values[best_index];
                        if (0 == remainder)
                        {
                            break;
                        }

                        while (previous_values[--best_index] != remainder)
                        {
                        }
                    }

                    std::reverse(chosen_indexes.begin(), chosen_indexes.end());

                    return{ max_sum,chosen_indexes };
                }
            };
        }
    }
}