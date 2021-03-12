#pragma once
#include "OccurrencesInSortedArray.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                class OccurrencesInSortedArrayTests final
                {
                    OccurrencesInSortedArrayTests() = delete;

                public:
                    static void Test();

                private:

                    using Number = long;
                    using Data = std::vector<Number>;

                    using Pair = OccurrencesInSortedArray::Pair;

                    static Pair NotFoundPair();

                    static void StandardTest();

                    static void AllSameTest();

                    static void RandomTest();

                    static void BeforeAfter(
                        const Data& data, const std::string& name);

                    static void Helper(
                        const Data& data, const Number& value,
                        const std::string& name,
                        const Pair& expected,
                        const bool hasExpected = true);
                };
            }
        }
    }
}