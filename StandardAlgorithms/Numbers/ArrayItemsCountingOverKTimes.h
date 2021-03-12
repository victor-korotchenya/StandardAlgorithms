#pragma once
#include <utility>
#include <vector>
#include <unordered_map>
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array of n>0 items,
            // and number k>0,
            //return indexes of major elements,
            // appearing >= k times in the array.
            //
            //e.g. n=9, k=3, there can be maximum 3 major items.
            // But n=9, k=4, there can be maximum 2 major items.
            template <typename Number>
            class ArrayItemsCountingOverKTimes final
            {
                ArrayItemsCountingOverKTimes() = delete;

            public:

                //TODO: p3. Using a std::vector instead of hash table could be faster for big "minimumCount".
                static std::vector<size_t> MajorElementsIndexesFast(
                    const std::vector<Number>& data,
                    const size_t minimumCount);

                //It can use more memory, potentially caching every item.
                static std::vector<size_t> MajorElementsIndexes_MuchMemory(
                    const std::vector<Number>& data,
                    const size_t minimumCount);

            private:

                using FirstIndex_Count = std::pair<size_t, size_t>;

                using TFrequencies = std::unordered_map<Number, FirstIndex_Count>;

                using TCandidates = std::unordered_map<Number, size_t>;

                static void CheckInput(const size_t size, const size_t minimumCount);

                static TFrequencies CalcFrequenciesSlow(
                    const std::vector<Number>& data);

                static std::vector<size_t> AddMajors(
                    const TFrequencies& frequencies,
                    const size_t minimumCount);

                static TCandidates
                    CalcCandidates(
                        const std::vector<Number>& data, const size_t resultSizeMax);

                static void DecrementSize(TCandidates& data);

                static TFrequencies CalcFrequenciesFast(
                    const std::vector<Number>& data,
                    const TCandidates& candidates);
            };

            template <typename Number>
            void ArrayItemsCountingOverKTimes<Number>::CheckInput(
                const size_t size, const size_t minimumCount)
            {
                if (size < 1)
                {
                    std::ostringstream ss;
                    ss << "Error: the data size(" << size << ") must be at least 1.";
                    StreamUtilities::throw_exception(ss);
                }

                if (minimumCount < 1)
                {
                    std::ostringstream ss;
                    ss << "Error: the minimumCount(" << minimumCount << ") must be at least 1.";
                    StreamUtilities::throw_exception(ss);
                }

                if (size < minimumCount)
                {
                    std::ostringstream ss;
                    ss << "Error: the minimumCount(" << minimumCount
                        << ") must not exceed the size(" << size << ").";
                    StreamUtilities::throw_exception(ss);
                }
            }

            template <typename Number>
            typename ArrayItemsCountingOverKTimes<Number>::TFrequencies
                ArrayItemsCountingOverKTimes<Number>::CalcFrequenciesSlow(
                    const std::vector<Number>& data)
            {
                const size_t size = data.size();

                TFrequencies result;

                for (size_t i = 0; i < size; ++i)
                {
                    typename TFrequencies::iterator found = result.find(data[i]);

                    if (result.end() != found)
                    {//Increment the count.
                        ++((*found).second.second);
                    }
                    else
                    {
                        result.insert(std::make_pair(data[i], FirstIndex_Count(i, 1)));
                    }
                }

                return result;
            }

            template <typename Number>
            std::vector<size_t> ArrayItemsCountingOverKTimes<Number>::AddMajors(
                const TFrequencies& frequencies,
                const size_t minimumCount)
            {
                std::vector<size_t> result;

                for (const auto& b : frequencies)
                {
                    if (minimumCount <= b.second.second)
                    {
                        result.push_back(b.second.first);
                    }
                }

                return result;
            }

            template <typename Number>
            typename ArrayItemsCountingOverKTimes<Number>::TCandidates
                ArrayItemsCountingOverKTimes<Number>::CalcCandidates(
                    const std::vector<Number>& data,
                    const size_t resultSizeMax)
            {
                const size_t size = data.size();

                TCandidates result;
                result.reserve(resultSizeMax);

                for (size_t i = 0; i < size; ++i)
                {
                    auto found = result.find(data[i]);
                    if (result.end() != found)
                    {
                        ++((*found).second);
                        continue;
                    }

                    const bool shallDecrementSize = resultSizeMax <= result.size();
                    if (shallDecrementSize)
                    {
                        DecrementSize(result);
                    }
                    else
                    {
                        result.insert(std::make_pair(data[i], 1));
                    }
                }

                return result;
            }

            template <typename Number>
            std::vector<size_t> ArrayItemsCountingOverKTimes<Number>::MajorElementsIndexes_MuchMemory(
                const std::vector<Number>& data,
                const size_t minimumCount)
            {
                const size_t size = data.size();
                CheckInput(size, minimumCount);

                const TFrequencies frequencies = CalcFrequenciesSlow(data);
                const std::vector<size_t> result = AddMajors(frequencies, minimumCount);
                return result;
            }

            template <typename Number>
            void ArrayItemsCountingOverKTimes<Number>::DecrementSize(
                TCandidates& data)
            {
                TCandidates temp;
                temp.reserve(data.size());

                for (const auto& c : data)
                {
                    if (1 < c.second)
                    {
                        temp.insert(std::make_pair(c.first, c.second - 1));
                    }
                }

                std::swap(temp, data);
            }

            template <typename Number>
            typename ArrayItemsCountingOverKTimes<Number>::TFrequencies
                ArrayItemsCountingOverKTimes<Number>::CalcFrequenciesFast(
                    const std::vector<Number>& data,
                    const TCandidates& candidates)
            {
                const size_t size = data.size();

                TFrequencies result;
                result.reserve(candidates.size());

                for (size_t i = 0; i < size; ++i)
                {
                    typename TCandidates::const_iterator
                        numberCountsFound = candidates.find(data[i]);

                    if (candidates.cend() != numberCountsFound)
                    {
                        typename TFrequencies::iterator frequenciesFound = result.find(data[i]);

                        if (result.end() != frequenciesFound)
                        {//Increment the count.
                            ++((*frequenciesFound).second.second);
                        }
                        else
                        {
                            result.insert(std::make_pair(data[i], FirstIndex_Count(i, 1)));
                        }
                    }
                }

                return result;
            }

            template <typename Number>
            std::vector<size_t> ArrayItemsCountingOverKTimes<Number>::MajorElementsIndexesFast(
                const std::vector<Number>& data,
                const size_t minimumCount)
            {
                const size_t size = data.size();
                CheckInput(size, minimumCount);

                const size_t resultSizeMax = size / minimumCount;
#ifdef _DEBUG
                if (resultSizeMax < 1)
                {
                    std::ostringstream ss;
                    ss << "Error: resultSizeMax(" << resultSizeMax << ") must be at least 1.";
                    StreamUtilities::throw_exception(ss);
                }
#endif
                const TCandidates candidates = CalcCandidates(data, resultSizeMax);
                const TFrequencies frequencies = CalcFrequenciesFast(data, candidates);
                const std::vector<size_t> result = AddMajors(frequencies, minimumCount);
                return result;
            }
        }
    }
}