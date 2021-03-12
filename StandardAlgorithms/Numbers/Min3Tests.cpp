#include <algorithm>
#include <string>
#include <random>
#include <stdexcept>
#include "Sort.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

void Min3Tests()
{
    random_device rd;

    using Number = short;

    const Number a = Number(rd());
    const Number b = Number(rd());
    const Number c = Number(rd());

    const Number minAB = min(a, b);
    const Number minABC = min(minAB, c);

    const Number actual = Min3(a, b, c);

    if (minABC != actual)
    {
        const string message = "a=" + to_string(a)
            + ", b=" + to_string(b)
            + ", c=" + to_string(c);

        throw runtime_error(message.c_str());
    }
}