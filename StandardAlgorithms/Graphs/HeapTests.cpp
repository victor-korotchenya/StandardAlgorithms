#include <algorithm>
#include <sstream>
#include "Heap.h"
#include "HeapTests.h"
#include "PriorityQueue.h"
#include "HeapUtilities.h"
#include "../Assert.h"
#include "../Utilities/Random.h"

using namespace std;
using namespace Privet::Algorithms::Heaps;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    static void ClearAndBuildTest();

    static void CheckForEquality(
        const std::vector< int >& itemsSorted,
        const size_t size,
        Privet::Algorithms::Heaps::Heap<int>& heap,
        const std::string& extraError);

#ifdef SHALL_PRINT_TEST_RESULTS
    static void CheckForEquality2(
        const std::vector< int >& itemsSorted,
        const size_t size,
        Heap<int>& heap,
        const string& extraError);
#endif

    template <typename T >
    static inline size_t GetIndex(const T& a)
    {
        return static_cast<size_t>(a);
    }

    void CheckForEquality(
        const vector< int >& itemsSorted,
        const size_t size,
        Heap<int>& heap,
        const string& extraError)
    {
        for (int index = 0; index < size; ++index)
        {
            const int expected = itemsSorted[index];
            const int actual = heap.top();
            heap.pop();

            if (expected == actual)
            {
                continue;
            }

            ostringstream ss;
            ss << "Items mismatch at index=" << index << ": expect '" << expected
                << "', but actual '" << actual << "'.";
            if (!extraError.empty())
            {
                ss << " " << extraError;
            }

            StreamUtilities::throw_exception(ss);
        }

        const size_t count = heap.size();
        Assert::AreEqual<size_t>(0, count, "The size after removing all items.");
    }

#ifdef SHALL_PRINT_TEST_RESULTS
    void CheckForEquality2(
        const vector< int >& itemsSorted,
        const size_t size,
        Heap<int>& heap,
        const string& extraError)
    {
        for (int index = 0; index < size; ++index)
        {
            const int expected = itemsSorted[index];
            const int actual = heap.top();

            //The difference from CheckForEquality:
            heap.DeleteTop2();

            if (expected == actual)
            {
                continue;
            }

            ostringstream ss;
            ss << "Items mismatch at index=" << index << ": expect '" << expected
                << "', but actual '" << actual << "'.";
            if (!extraError.empty())
            {
                ss << " " << extraError;
            }

            StreamUtilities::throw_exception(ss);
        }

        const size_t count = heap.size();
        Assert::AreEqual<size_t>(0, count, "The size after removing all items.");
    }
#endif

    void ClearAndBuildTest()
    {
        random_device rd;

        using DataType = unsigned short;
        const DataType someItem = static_cast<DataType>(rd());

        using ClearAndBuildTest_GetIndexFunction = size_t(*)(const DataType& vertex);

        const int size = 15;
        //All items must be unique.
        PriorityQueue< DataType, ClearAndBuildTest_GetIndexFunction > heap(GetIndex, size);

        heap.push(someItem);
        heap.push(someItem + 1);
        //TDO: p2. heap.push(someItem + 2); heap.push(someItem + 2);
        heap.push(someItem + 2);
        heap.push(someItem + 3);

        vector< DataType > items;
        FillRandom(items, size, DataType(0), true);

        vector< DataType > itemsCopy = items;

        const DataType* const data = items.data();
        heap.ClearAndBuild(data, size);
        heap.Validate("ClearAndBuildTest");

        //K-th.
        const size_t zero_based_index = size >> 1;
        sort(itemsCopy.begin(), itemsCopy.end());
        Heap< DataType*, vector< DataType* >, less_equal_byPtr< DataType >  > heapTemp;

        const DataType& kthItem = HeapUtilities::KSmallestElement< DataType >(
            heap, zero_based_index, heapTemp);

        Assert::AreEqual(itemsCopy[zero_based_index], kthItem, "kthItem");
    }
}

void Privet::Algorithms::Trees::Tests::HeapTests()
{
    ClearAndBuildTest();

#ifdef _DEBUG
    size_t size = 15;
#else
    size_t size = 64;
#endif

#ifdef SHALL_PRINT_TEST_RESULTS
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    LONGLONG t0 = 0, t2 = 0;

#ifdef _DEBUG
    const int attemptCount = 3;
#else
    const int attemptCount = 10;
#endif

#else
    const int attemptCount = 1;
#endif
    for (int attempt = 0; attempt <= attemptCount; ++attempt)
    {
        vector< int > items;
        FillRandom(items, size);

        vector< int > itemsSorted(items.cbegin(), items.cend());
        sort(itemsSorted.begin(), itemsSorted.end());

#ifdef SHALL_PRINT_TEST_RESULTS
        LARGE_INTEGER time0, time1, time2, time3, elapsedMicroseconds2, elapsedMicroseconds;
#endif

        {
            Heap<int> heap(size);
            heap.push(items[0]);
            Assert::AreEqual(items[0], heap.top(), "Top after 1st adding.");

            //The first item is already added.
            for (int index = 1; index < size; ++index)
            {
                const auto item = items[index];
                heap.push(item);
            }
            const size_t count0 = heap.size();
            Assert::AreEqual(size, count0, "The size after adding all items.");

#ifdef SHALL_PRINT_TEST_RESULTS
            QueryPerformanceCounter(&time0);
#endif

            CheckForEquality(itemsSorted, size, heap, string("The heap has just been built."));

#ifdef SHALL_PRINT_TEST_RESULTS
            QueryPerformanceCounter(&time1);
#endif
        }
        {
            Heap<int> heap(size);
            heap.push(items[0]);
            Assert::AreEqual(items[0], heap.top(), "Top after 1st adding.");

            //The first item is already added.
            for (int index = 1; index < size; ++index)
            {
                const auto item = items[index];
                heap.push(item);
            }
            const size_t count0 = heap.size();
            Assert::AreEqual(size, count0, "The size after adding all items.");

#ifdef SHALL_PRINT_TEST_RESULTS
            QueryPerformanceCounter(&time2);

            CheckForEquality2(itemsSorted, size, heap, string("The heap has just been built."));

            QueryPerformanceCounter(&time3);
#endif
        }

#ifdef SHALL_PRINT_TEST_RESULTS
        elapsedMicroseconds.QuadPart = time1.QuadPart - time0.QuadPart;
        elapsedMicroseconds.QuadPart *= 1000000;
        elapsedMicroseconds.QuadPart /= Frequency.QuadPart;

        elapsedMicroseconds2.QuadPart = time3.QuadPart - time2.QuadPart;
        elapsedMicroseconds2.QuadPart *= 1000000;
        elapsedMicroseconds2.QuadPart /= Frequency.QuadPart;

        if (attempt > 0 //To warm up.
            && elapsedMicroseconds.QuadPart > 0)
        {
            t0 += elapsedMicroseconds.QuadPart;
            t2 += elapsedMicroseconds2.QuadPart;
            const double ratio = ((double)elapsedMicroseconds2.QuadPart)
                / elapsedMicroseconds.QuadPart;

            const double ratioTotal = ((double)t2) / t0;

            cout << attempt << ": M2/M=" << ratio
                << ", M2=" << elapsedMicroseconds2.QuadPart
                << ", M=" << elapsedMicroseconds.QuadPart
                << ", size=" << "0x" << hex << size << dec
                << ", ratioTotal=" << ratioTotal
                << '\n';
        }
#endif

        size <<= 1;
    }

    //const size_t repetitionIndexesSize = 2;
    //const array<int, repetitionIndexesSize> repetitionIndexes = {3,6,};
    //for (int index3 = 0; index3 < repetitionIndexesSize; ++index3)
    //{
    //    const int repetitionIndex = repetitionIndexes[index3];
    //    heap.Add(items[repetitionIndex]);
    //}
    //
    //const size_t count1 = heap.size();
    //Assert::AreEqual(size + repetitionIndexesSize, count1, "The size after adding repetitions.");
}