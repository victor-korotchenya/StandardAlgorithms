#pragma once
#include "ArraySumOfThreeElements.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                class ArraySumOfThreeElementsTests final
                {
                    ArraySumOfThreeElementsTests() = delete;

                public:
                    static void Test();

                private:
                    using Number = long long int;

                    using Tuple = ArraySumOfThreeElements::Tuple;

                    static const size_t _MinSize = 3;

                    static Number GetRandomSum(
                        const std::vector<Number>& data);
                };
            }
        }
    }
}