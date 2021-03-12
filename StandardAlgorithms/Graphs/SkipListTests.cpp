#define _USE_MATH_DEFINES
#include <cmath>
#include "SkipListTests.h"
#include "../Assert.h"
#include "SkipList.h"

using namespace std;
using namespace Privet::Algorithms::Random;
using namespace Privet::Algorithms;

namespace
{
    static long long SkipListTests_Count = 0;

    bool SkipListTests_VisitFunction(const int& key, const double& value)
    {
        SkipListTests_Count += key;

        //Pacify the compiler.
        if (value) {}

        return true;
    }

    void OneRun()
    {
        const int minKey = (INT_MIN);
        const int maxKey = (INT_MAX);
        const double probability = 0.5;

        SkipList<int, double, 7> list(minKey, maxKey, probability);
        Assert::AreEqual<size_t>(0, list.length(), "Size after creation.");

        const int key0 = 1;
        const int key1 = 101;
        const int key2 = 10378;
        const double value1 = 204;

        const auto n0 = list.Find(static_cast<int>(value1));
        Assert::IsNullPtr(n0, "Find data before inserts.");
        Assert::AreEqual<size_t>(0, list.length(), "Size before inserts.");

        list.Add(key0, 2);
        list.Add(key1, value1);
        list.Add(key2, value1 + 0.01);
        const size_t sizeAfterInserts = 3;
        Assert::AreEqual<size_t>(sizeAfterInserts, list.length(), "Size after inserts.");

        const auto n = list.Find(static_cast<int>(value1));
        Assert::IsNullPtr(n, "Find not existing data.");
        Assert::AreEqual<size_t>(sizeAfterInserts, list.length(), "Size after 1st find.");

        SkipListTests_Count = 0;
        list.Visit(SkipListTests_VisitFunction);
        Assert::AreEqual<long long>(key0 + key1 + key2, SkipListTests_Count, "Visit_012.");

        const auto f = list.Find(key1);
        Assert::AreEqualByPtr(&value1, f, "Find existing data.");
        Assert::AreEqual<size_t>(sizeAfterInserts, list.length(), "Size after 2nd find.");

        list.Delete(key1);
        const size_t sizeAfterDelete = sizeAfterInserts - 1;
        Assert::AreEqual<size_t>(sizeAfterDelete, list.length(), "Size after delete.");

        const auto alreadyDeleted = list.Find(key1);
        Assert::IsNullPtr(alreadyDeleted, "Find the deleted data.");
        Assert::AreEqual<size_t>(sizeAfterDelete, list.length(), "Size after last find.");

        SkipListTests_Count = 0;
        list.Visit(SkipListTests_VisitFunction);
        Assert::AreEqual<long long>(key0 + key2, SkipListTests_Count, "Visit_02.");

        list.Add(minKey, 6543221);
        Assert::AreEqual<size_t>(sizeAfterDelete + 1, list.length(), "Size after adding -8.");

        list.Add(maxKey, 86742);
        Assert::AreEqual<size_t>(sizeAfterDelete + 2, list.length(), "Size after adding +8.");

        SkipListTests_Count = 0;
        list.Visit(SkipListTests_VisitFunction);
        Assert::AreEqual<long long>(
            key0 + key2 + minKey + maxKey, SkipListTests_Count, "Visit_02 +-8.");

        list.Delete(key0);
        list.Delete(key1);
        list.Delete(key2);

        list.Clear();

        random_device device;
        for (int i = 0; i < 10; ++i)
        {
            list.Delete(i);

            const int randomKey = device();
            const double sameValueWontCauseAddProblems = (M_PI);
            list.Add(randomKey, sameValueWontCauseAddProblems);
        }

        list.Clear();

        SkipListTests_Count = 0;
        list.Visit(SkipListTests_VisitFunction);
        Assert::AreEqual<long long>(0, SkipListTests_Count, "Visit after clear.");
        Assert::AreEqual<size_t>(0, list.length(), "length after clear.");
    }
}

void Privet::Algorithms::Trees::Tests::SkipListTests()
{
    const int length = 2;
    for (int i = 0; i < length; i++)
    {
        try
        {
            OneRun();
        }
        catch (const exception& ex)
        {
            ostringstream ss;
            ss << "Attempt=" << i << ". " << ex.what();
            StreamUtilities::throw_exception(ss);
        }
    }
}