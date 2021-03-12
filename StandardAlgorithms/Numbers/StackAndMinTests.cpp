#include <limits>
#include <vector>
#include "StackAndMin.h"
#include "StackAndMinSimple.h"
#include "../Assert.h"
#include "../Utilities/Random.h"
#include "Arithmetic.h"
#include "StackAndMinTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

void StackAndMinTests::Test()
{
    using AddFunction = Number(*)(Number const, Number const);

    StackAndMin< Number, AddFunction > stack(
        AddSigned< Number >,
        SubtractSigned< Number >);

    Assert::AreEqual< size_t >(0, stack.size(), "Size after creation.");

    StackAndMinSimple< Number > stackSimple;

    const size_t size = 20;

    //To prevent overflow.
    constexpr Number cutter = numeric_limits< Number >::max() / Number(3);

    vector< Number > numbers;
    FillRandom(numbers, size, cutter);

    for (size_t i = 0; i < size; ++i)
    {
        stack.push(numbers[i]);
        stackSimple.push(numbers[i]);

        const string name = to_string(i);
        Assert::AreEqual(numbers[i], stack.top(), "Top after push #" + name);

        Assert::AreEqual(stackSimple.Minimum(), stack.Minimum(),
            "Minimum after push #" + name);
    }

    for (size_t i = 0; i < size - 1; ++i)
    {
        stack.pop();
        stackSimple.pop();

        const string name = to_string(i);
        const auto arrayIndex = size - i - 2;
        Assert::AreEqual(numbers[arrayIndex], stack.top(), "Top after pop #" + name);

        Assert::AreEqual(stackSimple.Minimum(), stack.Minimum(),
            "Minimum after pop #" + name);
    }

    stack.pop();
    Assert::AreEqual(true, stack.empty(), "empty");

    stackSimple.pop();
}