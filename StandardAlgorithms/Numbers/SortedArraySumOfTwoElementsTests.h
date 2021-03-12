#pragma once
#include "SortedArraySumOfTwoElements.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                class SortedArraySumOfTwoElementsTests final
                {
                    SortedArraySumOfTwoElementsTests() = delete;

                public:
                    static void Test();

                private:
                    using Number = long long int;
                    using Pair = SortedArraySumOfTwoElements::Pair;
                };
            }
        }
    }
}