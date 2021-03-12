#include <string>
#include <vector>
#include <sstream>
#include "XorQueue.h"
#include "XorQueueTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using Item = struct _ITEM final
    {
        int Value;
        struct _ITEM* Next;
    };
}

void Privet::Algorithms::Trees::Tests::XorQueueTests()
{
    const int count = 5;

    vector< Item > items;
    items.resize(count);

    for (int x = 0; x < count; ++x)
    {
        items[x].Value = x;
        items[x].Next = nullptr;
    }

    Item* data = items.data();

    XorQueue<Item> list;

    for (int x = 0; x < count; ++x)
    {
        Item* newItem = data + x;
        list.Enqueue(newItem);

        Assert::AreEqual<int>(x, newItem->Value, "Before adding in XorQueueTests.");
    }

    for (int x = 0; x < count; ++x)
    {
        Item* retrievedItem = list.Dequeue();

        ostringstream ss;
        ss << x;

        string message(ss.str());

        Assert::IsNotNullPtr(retrievedItem, message);
        Assert::AreEqual<int>(x, retrievedItem->Value, "After deleting in XorQueueTests.");
    }
}