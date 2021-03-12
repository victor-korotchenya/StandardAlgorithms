#include "Span.h"
#include "SpanTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms;
using namespace Privet::Algorithms::Numbers;

void Privet::Algorithms::Numbers::Tests::SpanTests::Test()
{
    using Number = int;

    const Number data[] = { 9, 5, 3, 5, 8, 1 };
    const size_t dataSize = sizeof(data) / sizeof(Number);

    const size_t span[] = { 0, 0, 0, 2, 3, 0 };
    const size_t spanSize = sizeof(span) / sizeof(size_t);
    Assert::AreEqual(dataSize, spanSize, "dataSize");

    vector< size_t > actual;

    SpanCalculator::CalcSpanSimple(data, dataSize, actual);
    Assert::AreEqual<size_t>(span, dataSize, actual, "CalcSpanSimple");

    SpanCalculator::CalcSpanFast(data, dataSize, actual);
    Assert::AreEqual<size_t>(span, dataSize, actual, "CalcSpanFast");
}