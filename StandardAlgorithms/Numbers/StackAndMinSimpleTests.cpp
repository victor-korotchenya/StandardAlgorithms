#include "StackAndMinSimple.h"
#include "../Assert.h"
#include "StackAndMinSimpleTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;

void StackAndMinSimpleTests::Test()
{
    StackAndMinSimple< Number > stack;
    Assert::AreEqual< size_t >(0, stack.size(), "Size after creation.");

    const Number numbers[] = { 259, 150, 201, 140, };
    const Number minimums[] = { numbers[0], numbers[1], numbers[1], numbers[3], };
    const size_t numbersSize = sizeof(numbers) / sizeof(Number);

    Number minValue = numbers[0];
    for (size_t i = 0; i < numbersSize; ++i)
    {
        stack.push(numbers[i]);

        const string name = to_string(i);
        Assert::AreEqual(numbers[i], stack.top(), "Top after push #" + name);

        if (numbers[i] < minValue)
        {
            minValue = numbers[i];
        }

        Assert::AreEqual(minValue, stack.Minimum(), "Minimum after push #" + name);
    }

    for (size_t i = 0; i < numbersSize - 1; ++i)
    {
        stack.pop();

        const string name = to_string(i);
        const auto arrayIndex = numbersSize - i - 2;
        Assert::AreEqual(numbers[arrayIndex], stack.top(), "Top after pop #" + name);

        Assert::AreEqual(minimums[arrayIndex], stack.Minimum(), "Minimum after pop #" + name);
    }

    stack.pop();
    Assert::AreEqual(true, stack.empty(), "empty");
}