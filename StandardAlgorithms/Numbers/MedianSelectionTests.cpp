#include <iostream>
#include <vector>
#include <algorithm>
#include "../Utilities/Random.h"
#include "dutch_flag_split.h"
#include "MedianSelection.h"
#include "../Utilities/PrintUtilities.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    template<class value_t>
    pair<size_t, size_t> split_with_numbers(vector<value_t>& ar, const value_t& value)
    {
        const auto p = dutch_flag_split_into3_less_equal_greater_3way_partition(ar.begin(), ar.end(), value);

        auto start = ar.begin();
        const auto res = make_pair(p.first - start, p.second - start);
        return res;
    }

    using CompareIntFunction = int(*)(const void* a, const void* b);

    int CompareInt(const void* pa, const void* pb)
    {
        const int a = *static_cast<const int*>(pa);
        const int b = *static_cast<const int*>(pb);

        const int result = a - b;
        return result;
    }

    using SelectOrderStatisticFunction = int* (*)(
        int* data, const int dataSize,
        const CompareIntFunction comparer,
        const int statisticOrder);

    void TestHelper(
        int* data,
        const int dataSize,
        const SelectOrderStatisticFunction function1,
        const char functionIndex,
        char error[],
        const int delta,
        const int order)
    {
        const int* pItem = function1(data, dataSize, &CompareInt, order);
        const auto actualValue = pItem ? *pItem : INT_MIN;

        error[0] = char(functionIndex + '0');

        const int expectedValue = delta + order + 1;
        Assert::AreEqual(expectedValue, actualValue, error);
    }

    void FillArray(vector< int >& dataVector, const int dataSize, const int delta)
    {
        dataVector.resize(dataSize);

        for (int j = 0; j < dataSize; ++j)
        {
            dataVector[j] = delta + dataSize - j;
        }
    }

    void TestMedianSelectionByDataSize(
        vector< int >& dataVector,
        const int dataSize,
        random_device& rd,
        const SelectOrderStatisticFunction function1,
        const char functionIndex)
    {
        const int delta = 170;
        FillArray(dataVector, dataSize, delta);

        int* data = dataVector.data();

        const int orderMin = 0;
        char errorMin[] = "0_valueMin";
        TestHelper(
            data,
            dataSize,
            function1,
            functionIndex,
            errorMin,
            delta,
            orderMin);

        ShuffleArray(dataVector);
        const int orderMiddle = (dataSize - 1) >> 1;
        char errorMiddle[] = "0_valueMiddle";
        TestHelper(
            data,
            dataSize,
            function1,
            functionIndex,
            errorMiddle,
            delta,
            orderMiddle);

        ShuffleArray(dataVector);
        const int orderMax = dataSize - 1;
        char errorMax[] = "0_valueMax";
        TestHelper(
            data,
            dataSize,
            function1,
            functionIndex,
            errorMax,
            delta,
            orderMax);

        if (dataSize > 2)
        {
            const int randomOrder = rd() % dataSize;

            ShuffleArray(dataVector);
            char errorRandom[] = "0_valueRandom";
            TestHelper(
                data,
                dataSize,
                function1,
                functionIndex,
                errorRandom,
                delta,
                randomOrder);
        }
    }

    //This is to be used by tests only.
    template <typename TItem,
        //int (*compar)(const void*,const void*)
        typename TComparer,
        typename TInteger >
        TItem* QuickSort_SelectionOrderStatistic(
            TItem* data, const TInteger dataSize,
            const TComparer comparer,
            const TInteger statisticOrder)
    {
        if (dataSize <= 0)
        {
            ostringstream ss;
            ss << "The data size (" << dataSize << ") must positive.";
            StreamUtilities::throw_exception(ss);
        }

        if (statisticOrder < 0)
        {
            ostringstream ss;
            ss << "The statistic order (" << statisticOrder << ") must be 0 or positive.";
            StreamUtilities::throw_exception(ss);
        }

        if (dataSize <= statisticOrder)
        {
            ostringstream ss;
            ss << "The statistic order (" << statisticOrder <<
                ") must be smaller than data size (" << dataSize << ").";
            StreamUtilities::throw_exception(ss);
        }

        if (nullptr == comparer)
        {
            throw exception("The comparer must be not null.");
        }

        qsort(data, dataSize, sizeof(TItem), comparer);

        TItem* result = data + statisticOrder;
        return result;
    }

    template <typename TItem,
        typename TComparer,
        typename TInteger>
        TItem* SelectOrderStatistic_Test(
            TItem* data, const TInteger dataSize,
            const TComparer comparer,
            const TInteger statisticOrder)
    {
        // todo: del Pacify the compiler.
        if (comparer) {}

        TItem* result = MedianSelection<TItem, TInteger>::SelectOrderStatistic(
            data, dataSize, statisticOrder);
        return result;
    }

    void MedianSelectionTests_Performance()
    {
        vector< int > dataVector;

        const int dataSize = 20// *1024// *1024
            ;
        const int delta = 170;

        FillArray(dataVector, dataSize, delta);

        const int medianStatisticOrder = dataSize >> 1;

        //LARGE_INTEGER totalTime0, totalTime1;
        //totalTime0.QuadPart = 0;
        //totalTime1.QuadPart = 0;

        //LARGE_INTEGER Frequency;
        //QueryPerformanceFrequency(&Frequency);

        const int attemptLength = 10;
        for (int attempt = 0; attempt < attemptLength; ++attempt)
        {
            ShuffleArray(dataVector);

            //LARGE_INTEGER t0, t1, t2, elapsedMicroseconds0, elapsedMicroseconds1;

            vector< int > dataVector0(dataVector);
            vector< int > dataVector1(dataVector);

            int* data0 = dataVector0.data();
            int result0;
            int result1;

            // QueryPerformanceCounter(&t0);

            nth_element(
                dataVector1.begin(),
                dataVector1.begin() + medianStatisticOrder,
                dataVector1.end());
            result0 = dataVector1[medianStatisticOrder];

            //QueryPerformanceCounter(&t1);

            int* r1 = MedianSelection<int, int>::SelectOrderStatistic(
                data0, dataSize, medianStatisticOrder);
            result1 = *r1;

            // QueryPerformanceCounter(&t2);

            if (result0 != result1)
            {
                sort(dataVector.begin(), dataVector.end());
                const int resultSorted = dataVector[medianStatisticOrder];

                ostringstream ss;
                ss << result0 << "=result0 != result1=" << result1
                    << " in MedianSelectionTests_Performance."
                    << " sorted result=" << resultSorted << ".";

                StreamUtilities::throw_exception(ss);
            }

            //elapsedMicroseconds0.QuadPart = t1.QuadPart - t0.QuadPart;
            //elapsedMicroseconds0.QuadPart *= 1000000;
            //elapsedMicroseconds0.QuadPart /= Frequency.QuadPart;

            //elapsedMicroseconds1.QuadPart = t2.QuadPart - t1.QuadPart;
            //elapsedMicroseconds1.QuadPart *= 1000000;
            //elapsedMicroseconds1.QuadPart /= Frequency.QuadPart;

            //totalTime0.QuadPart += elapsedMicroseconds0.QuadPart;
            //totalTime1.QuadPart += elapsedMicroseconds1.QuadPart;

            int a = 1;
            if (2 == a)
            {
                //const double ratio = elapsedMicroseconds1.QuadPart ?
                //  static_cast<double>(elapsedMicroseconds0.QuadPart) / elapsedMicroseconds1.QuadPart : 0;

                //const double ratioTotal = totalTime1.QuadPart ?
                //  static_cast<double>(totalTime0.QuadPart) / totalTime1.QuadPart : 0;

                //cout << attempt << ": M0/M1=" << ratio
                //  << ", M0=" << elapsedMicroseconds0.QuadPart
                //  << ", M1=" << elapsedMicroseconds1.QuadPart
                //  << ", ratioTotal=" << ratioTotal
                //  << '\n';
            }
        }
    }

    void dutch_flag_split_into3_tests()
    {
        using value_t = short;
        const value_t value = 4;
        vector<value_t> vec{ 8,1,value,2,value,5,value, }; // 1,2 4,4,4 5,8
        const auto actual = split_with_numbers(vec, value);
        const auto expected = make_pair<size_t, size_t>(2u, 5u);
        Assert::AreEqual(expected, actual, "dutch_flag_split_into3_less_equal_greater_3way_partition");
    }

    void dutch_flag_split_into3_tests_random()
    {
        SizeRandom sr;
        const auto size = sr(1, 30);

        using value_t = uint16_t;
        vector<value_t> vec(size);
        FillRandom<value_t>(vec, size, {}, true);
        auto copy = vec;

        value_t value;
        if (sr(0, 1))
        {// existing value.
            value = vec[vec.size() >> 1];
        }
        else
        {
            for (;;) // not existing value.
            {
                value = static_cast<value_t>(sr(0u, numeric_limits<value_t>::max()));
                const auto it = find(vec.begin(), vec.end(), value);
                if (it == vec.end())
                    break;
            }
        }

        const auto actual = split_with_numbers(vec, value);

        sort(copy.begin(), copy.end());
        size_t eq = 0, gre = copy.size();
        for (size_t i = 0u; i < copy.size(); ++i)
        {
            if (copy[i] < value)
                eq = i + size_t(1);
            else if (copy[i] > value)
            {
                gre = i;
                break;
            }
        }

        const auto expected = make_pair(eq, gre);
        if (expected == actual)
            return;

        stringstream ss;
        ::Print("dutch_flag_split_into3_less_equal_greater_3way_partition random sorted vec", copy, ss);

        const auto error = ss.str();
        Assert::AreEqual(expected, actual, error);
    }
}

void Privet::Algorithms::Numbers::Tests::MedianSelectionTests()
{
    SelectOrderStatisticFunction functions[] = {
      SelectOrderStatistic_Test<int, CompareIntFunction, int>,
      QuickSort_SelectionOrderStatistic<int, CompareIntFunction, int>
    };

    const int functionIndexMax = sizeof(functions) / sizeof(SelectOrderStatisticFunction);

    vector< int > dataVector;
    const int dataSizeMax = 23;
    dataVector.resize(dataSizeMax);

    random_device rd;

    for (int dataSize = dataSizeMax; dataSize > 0; --dataSize)
        //for (int dataSize = 1; dataSize < dataSizeMax; ++dataSize)
    {
        for (char functionIndex = 0; functionIndex < functionIndexMax; ++functionIndex)
        {
            TestMedianSelectionByDataSize(
                dataVector, dataSize,
                rd,
                functions[functionIndex], functionIndex);
        }
    }

    MedianSelectionTests_Performance();
    dutch_flag_split_into3_tests();
    dutch_flag_split_into3_tests_random();
}