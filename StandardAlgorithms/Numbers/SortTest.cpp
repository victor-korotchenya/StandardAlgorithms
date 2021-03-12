#include <algorithm>
#include <limits>
#include <string>
#include <vector>
#include "Arithmetic.h"
#include "../Utilities/Random.h"
#include "../Utilities/VectorUtilities.h"
#include "../Assert.h"
#include "BinarySearch.h"
#include "InterpolationSearch.h"
#include "../Graphs/Heap.h"
#include "Sort.h"
#include "SortTest.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Heaps;
using namespace Privet::Algorithms;

namespace
{
    template <typename T>
    void CheckArrayIsSorted(const vector<T>& data, const string& name)
    {
        const auto size = data.size();
        Assert::Greater(size, 0u, "data.size()");

        VectorUtilities::RequireArrayIsSorted(data, name);

        const T expected = T(1);
        Assert::AreEqual(expected, data[0], "data[0]");

        const T expectedLast = T(size);
        Assert::AreEqual(expectedLast, data[size - 1], "data[" + to_string(size) + "]");
    }

    int IntComparer(const int* a, const int* b)
    {
        const auto result = compare(*a, *b);
        return result;
    }

    void BinaryAndInterpolationSearchTests()
    {
        constexpr auto length = 20;

        vector<int> dataVector(length);
        iota(dataVector.begin(), dataVector.end(), 0);

        const auto data = dataVector.data(), endInclusive = data + length - 1;

        using NumberAndString = pair<int, string >;
        using NaSTestCase = pair< NumberAndString, int* >;

        vector<NaSTestCase> testCases;

        testCases.push_back(NaSTestCase(
            NumberAndString(length, "Not-existing element."), static_cast<int*>(nullptr)));

        testCases.push_back(NaSTestCase(
            NumberAndString(int(0), "First element."), const_cast<int*>(data)));

        int* middle = const_cast<int*>(data) + (length >> 1);
        testCases.push_back(NaSTestCase(
            NumberAndString(*middle, "Middle element."), middle));

        int* beforeMiddle = middle - 1;
        testCases.push_back(NaSTestCase(
            NumberAndString(*beforeMiddle, "Before middle element."), beforeMiddle));

        int* afterMiddle = middle + 1;
        testCases.push_back(NaSTestCase(
            NumberAndString(*afterMiddle, "Before middle element."), afterMiddle));

        testCases.push_back(NaSTestCase(
            NumberAndString(length - 1, "Last element."), const_cast<int*>(endInclusive)));

        using SearchFunction = int* (*)
            (const int* const begin, const int* const endInclusive1, const int& value);

        using FunctionAndNamePair = pair< SearchFunction, string >;

        const string binarySearchName = "BinarySearch";
        const string interpolationSearchName = "InterpolationSearch";

        FunctionAndNamePair functionNamePairs[] = {
            make_pair(binary_search_inclusive<int, int>, binarySearchName),
            make_pair(InterpolationSearch::Search<int>, interpolationSearchName)
        };

        const string separator = " ";
        const int searchFunctionsCount = sizeof(functionNamePairs) / sizeof(FunctionAndNamePair);

        const auto itEnd = testCases.cend();

        for (auto it = testCases.cbegin(); itEnd != it; ++it)
        {
            const NaSTestCase& testCase = *it;
            const NumberAndString& numberAndString = testCase.first;
            const int number = numberAndString.first;
            const int* expected = testCase.second;

            for (int funcId = 0; funcId < searchFunctionsCount; ++funcId)
            {
                const FunctionAndNamePair functionAndName = functionNamePairs[funcId];

                string message = numberAndString.second;
                message.append(separator);
                message.append(functionAndName.second);

                int* actual = functionAndName.first(data, endInclusive, number);
                Assert::AreEqualByPtr(expected, actual, message);
            }
        }
    }

    void TestInsertionSort()
    {
        const int length = 17;
        vector< int > dataVector;
        VectorUtilities::FillReverse(length, dataVector);
        ShuffleArray(dataVector);

        int* data = dataVector.data();
        InsertionSort<int>(data, length);
        CheckArrayIsSorted(dataVector, string(__FUNCTION__));
    }

    void TestHeapSort()
    {
        const int length = 16;
        using DataType = unsigned char;
        vector< DataType > dataVector;

        VectorUtilities::FillReverse(length, dataVector);
        ShuffleArray(dataVector);

        vector< DataType > dataVector2 = dataVector;
        DataType* data = dataVector2.data();

        //To sort in ascending order, max heap must be used.
        Heap< DataType, vector< DataType >, greater_equal< DataType > > heap;
        heap.HeapSort(data, length);

        CheckArrayIsSorted(dataVector2, string(__FUNCTION__));
    }

    void TestSort_N(const int length,
        void(*function1)(int*))
    {
        vector< int > dataVector;
        VectorUtilities::FillReverse(length, dataVector);

        int* data = dataVector.data();
        function1(data);
        CheckArrayIsSorted(dataVector, string(__FUNCTION__) + "_");

        //Already sorted - check again.
        function1(data);
        CheckArrayIsSorted(dataVector, string(__FUNCTION__) + "__");

        const int attemptMax = 10;
        for (int attempt = 0; attempt < attemptMax; ++attempt)
        {
            ShuffleArray(dataVector);

            function1(data);
            CheckArrayIsSorted(dataVector,
                string(__FUNCTION__) + "_" + to_string(attempt));
        }
    }

    template<typename Item,
        typename digit_t,
        typename get_size_t,
        typename get_item_t,
        typename Collection = vector<Item>,
        typename TSize = unsigned>
        void Test_radix_sort_helper(
            const string& name,
            const Collection& dataOriginal,
            const Collection& expected,
            get_size_t get_size,
            get_item_t get_item)
    {
        Assert::AreEqual(dataOriginal.size(), expected.size(),
            "size for " + name);
        VectorUtilities::RequireArrayIsSorted(expected, name);

        auto actual = dataOriginal;

        radix_sort<Collection, digit_t, TSize,
            get_size_t, get_item_t>
            (actual, get_size, get_item);

        Assert::AreEqual(expected, actual, "expected_" + name);
    }

    void Test_radix_sort_string()
    {
        const auto name = string(__FUNCTION__);

        using Item = string;
        using digit_t = unsigned char;
        using Collection = vector<Item>;
        using TSize = unsigned;

        const Collection dataOriginal{ "BBC", "X", "ok", "AAA",
            "", "AAA", "ABCD", "AA\xFF", "", "AAA" };
        const Collection expected{ "","",
            "AAA","AAA","AAA", "AA\xFF", "ABCD", "BBC", "X", "ok" };

        auto get_size = [](const Item& item) -> size_t
        {
            return item.size();
        };

        auto get_item = [](const Item& item, const TSize index) -> digit_t
        {
            return item[index];
        };

        Test_radix_sort_helper<Item, digit_t,
            decltype(get_size), decltype(get_item)>(name, dataOriginal, expected,
                get_size, get_item);
    }

    void Test_radix_sort_int()
    {
        const auto name = string(__FUNCTION__);

        using Item = unsigned;
        using digit_t = unsigned char;
        using Collection = vector<Item>;
        using TSize = unsigned;

        const Collection dataOriginal{ 3457, 0,
            numeric_limits<Item>::max(),
            numeric_limits<Item>::min(),
            798,679,57809,981238546,
            0xFfFfFfFf, 6780, 571234354 };

        const Collection expected{ 0, numeric_limits<Item>::min(),
            679, 798, 3457, 6780, 57809,
            571234354,
            981238546,
            0xFfFfFfFf,
            numeric_limits<Item>::max() };

        auto get_size = [](const Item&) -> Item
        {
            return sizeof(Item);
        };

        auto get_item = [](const Item& item, const TSize index) -> digit_t
        {
            const auto shift = (3 - index) << 3;
            const auto t = digit_t(0xFF & (item >> shift));
            return t;
        };

        Test_radix_sort_helper<Item, digit_t,
            decltype(get_size), decltype(get_item)>(name, dataOriginal, expected,
                get_size, get_item);
    }
}

void Privet::Algorithms::Numbers::Tests::sort_tests()
{
    {
        constexpr auto si = 1000u;
        vector<long long unsigned> a;
        FillRandom(a, si);

        auto b = a;
        sort(b.begin(), b.end());

        auto c = a, d = a;
        radix_sort_unsigned(c.data(), si, d.data());
        Assert::AreEqual(b, c, "radix_sort_unsigned");
    }
    {
        const vector<pair<char, char>> expected{
            {1, 0},{1, 1},
            {2, 1}, {2, 0},
            {3,2}, {3,1}, {3,3},
        },
        a{
            // Stable sort test
            {3,2}, {3,1}, {3,3},
            {1, 0}, {2, 1},
            {1, 1}, {2, 0},
        };

        {
            auto b = a;
            stable_sort(b.begin(), b.end(), [](const auto& x, const auto& y) -> bool
                {
                    return x.first < y.first;
                });
            Assert::AreEqual(expected, b, "stable_sort");
        }

        const auto to_index = [](const auto& v) -> unsigned
        {
            return static_cast<unsigned>(v->first);
        };

        auto c = a, d = a;

        counting_sort<unsigned, 256u//, decltype(to_index)
        >(c.begin(), c.end(), d.begin(), to_index);
        Assert::AreEqual(expected, c, "counting_sort");
    }

    BinaryAndInterpolationSearchTests();

    TestSort_N(3, &FastSort3<int>);
    TestSort_N(4, &FastSort4<int>);
    TestSort_N(5, &FastSort5<int>);
    TestInsertionSort();
    TestHeapSort();
    Test_radix_sort_string();
    Test_radix_sort_int();

#ifdef _WIN32z
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    const int maxAttempts = 2
        //The first attempt is ignored to warm up the CPU.
        + 1;
#else
    const int maxAttempts = 1;
#endif

    const int length = 100;

    for (int attempt = 0; attempt < maxAttempts; ++attempt)
    {
#ifdef _WIN32z
        LARGE_INTEGER time0, time1, time2, time3, elapsedMicroseconds0, elapsedMicroseconds1;
#endif
        {
            vector< int > data;
            VectorUtilities::FillReverse(length, data);

#ifdef _WIN32z
            QueryPerformanceCounter(&time0);
#endif
            SelectionSort(data.data(), length, //sizeof(int),
                &IntComparer, false);
#ifdef _WIN32z
            QueryPerformanceCounter(&time1);
#endif

            CheckArrayIsSorted(data, "SelectionSort");
        }

        {
            vector< int > data;
            VectorUtilities::FillReverse(length, data);

#ifdef _WIN32z
            QueryPerformanceCounter(&time2);
#endif
            selection_sort_slow(data.begin(), data.end());
#ifdef _WIN32z
            QueryPerformanceCounter(&time3);
#endif

            CheckArrayIsSorted(data, "selection_sort_slow");
        }

#ifdef _WIN32z
        elapsedMicroseconds0.QuadPart = time1.QuadPart - time0.QuadPart;
        elapsedMicroseconds0.QuadPart *= 1000000;
        elapsedMicroseconds0.QuadPart /= Frequency.QuadPart;

        elapsedMicroseconds1.QuadPart = time3.QuadPart - time2.QuadPart;
        elapsedMicroseconds1.QuadPart *= 1000000;
        elapsedMicroseconds1.QuadPart /= Frequency.QuadPart;

        if (attempt)
        {
            const double ratio = static_cast<double>(elapsedMicroseconds1.QuadPart)
                / static_cast<double>(elapsedMicroseconds0.QuadPart);

            if (ratio) {}//Pacify compilter.
#ifdef SHALL_PRINT_TEST_RESULTS
            cout << attempt << ": ratio=" << ratio
                << ", C=" << elapsedMicroseconds0.QuadPart
                << ", C++=" << elapsedMicroseconds1.QuadPart
                << '\n';
#endif
        }
#endif
    }
}