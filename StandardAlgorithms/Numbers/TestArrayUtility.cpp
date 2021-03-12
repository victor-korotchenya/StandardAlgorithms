#include "TestArrayUtility.h"
#include "../Assert.h"

using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;

void TestArrayUtility::CheckArrayIndex(
    const size_t size, const size_t index, const std::string& name)
{
    Assert::Greater(size, index, "index_" + name);
}