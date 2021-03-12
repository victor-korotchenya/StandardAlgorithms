#pragma once
#include <vector>

namespace Privet::Algorithms::Numbers::Tests
{
    class ArrayInversionCountTests final
    {
        ArrayInversionCountTests() = delete;

    public:

        static void Test();

    private:

        static void SimpleTest();

        static void RandomTest();

        using Number = long long int;

        using Data = std::vector< Number >;

        static void TestHelper(
            const Data& data,
            const std::string& name,
            bool hasExpected = false,
            size_t expected = 0);
    };
}