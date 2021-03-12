#pragma once
//#include <functional> // less
//#include <algorithm> //stable_partition
#include <vector>

#ifdef _DEBUG
//#include "Arithmetic.h"
#endif

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array:
            // {1, 5, -7, 10, 0, -2, -8, 0, 4}, it must become:
            // {-7, -2, -8, 0, 0, 1, 5, 10, 4}.
            //
            // That is:
            // A) all negative numbers are before 0 if any.
            // B) 0-s if any.
            // C) positive numbers if any.
            //
            //Note that the relative order must be maintained.
            template <typename TNumber, typename TLess = std::less< TNumber > >
            class NumberSplitter final
            {
                NumberSplitter() = delete;

            public:

                static void Slow(
                    std::vector< TNumber >& data,
                    const TLess& lessArg = TLess());

                //TODO: p3. It is incorrect - can someone do it using O(n) time and O(1) space?
                static void Fast(std::vector< TNumber >& data,
                    const TLess& lessArg = TLess());

                static void SplitByPivotUnstable(std::vector< TNumber >& data,
                    const TNumber& pivot,
                    const TLess& lessArg = TLess());
            };

            template <typename TNumber, typename TLess >
            void NumberSplitter< TNumber, TLess >::
                Slow(std::vector< TNumber >& data, const TLess& lessArg)
            {
#ifdef _DEBUG
                //        AssertSigned< TNumber >("Split");
#endif
                const size_t size = data.size();
                if (size <= 1)
                {//Already sorted.
                    return;
                }

                const TNumber zero = TNumber(0);
                std::vector<TNumber> negatives, zeros, positives;

                for (size_t i = 0; i < size; ++i)
                {
                    const TNumber& datum = data[i];

                    if (lessArg(datum, zero))
                    {
                        negatives.push_back(datum);
                    }
                    else if (lessArg(zero, datum))
                    {
                        positives.push_back(datum);
                    }
                    else
                    {
                        zeros.push_back(datum);
                    }
                }

                std::vector< TNumber > result;
                result.insert(result.end(), negatives.cbegin(), negatives.cend());
                result.insert(result.end(), zeros.cbegin(), zeros.cend());
                result.insert(result.end(), positives.cbegin(), positives.cend());

                swap(data, result);
            }

            template <typename TNumber, typename TLess >
            void NumberSplitter< TNumber, TLess >::
                Fast(std::vector< TNumber >& data, const TLess& lessArg)
            {
#ifdef _DEBUG
                //        AssertSigned< TNumber >("Split");
#endif
        // This does not handle zeros:
        //
        //stable_partition(data.begin(), data.end(),
        //  //Lambda.
        //
        //  //[&] capture-by-reference
        //  // to solve: Error C3493 'lessArg' cannot be implicitly captured because no
        //  // default capture mode has been specified.
        //  [&](const TNumber &a)
        //  {
        //    return lessArg(a, TNumber(0));
        //  });

        //TODO: p3. It is incorrect and unstable.
        //SplitByPivotUnstable(data, TNumber(0), lessArg);

        //Workaround.
                Slow(data, lessArg);
            }

            template <typename TNumber, typename TLess >
            void NumberSplitter< TNumber, TLess >::
                SplitByPivotUnstable(
                    std::vector< TNumber >& data,
                    const TNumber& pivot,
                    const TLess& lessArg)
            {
#ifdef _DEBUG
                //        AssertSigned< TNumber >("Split");
#endif
        //Using signed numbers allows one fewer comparison.
                using TSize = long long int;

                const TSize size = TSize(data.size());
                if (size <= 1)
                {//Already sorted.
                    return;
                }

                //TODO: p1. Prove the correctness.

                //It must hold:
                // 0 <= negativeIndex <= zeroIndex <= positiveIndex < size.
                TSize negativeIndex = 0, currentOrZeroIndex = 0, positiveIndex = size - 1;

                while (currentOrZeroIndex <= positiveIndex)
                {
                    if (lessArg(pivot, data[currentOrZeroIndex]))
                    {
                        swap(data[currentOrZeroIndex], data[positiveIndex]);

                        //Using signed numbers allows one fewer comparison.
                        //if (0 == positiveIndex)
                        //{
                        //  break;
                        //}

                        --positiveIndex;
                    }
                    else
                    {
                        if (lessArg(data[currentOrZeroIndex], pivot))
                        {
                            swap(data[negativeIndex], data[currentOrZeroIndex]);

                            ++negativeIndex;
                        }

                        ++currentOrZeroIndex;
                    }
                }
            }
        }
    }
}