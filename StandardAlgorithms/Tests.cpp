#include "Geometry/add_tests.h"
#include "Graphs/add_tests.h"
#include "Numbers/add_tests.h"
#include "Strings/add_tests.h"
#include "Tests.h"

using namespace std;
using namespace Privet::Algorithms;

vector<pair<string, test_function>> Privet::Algorithms::GetTests()
{
    vector<pair<string, test_function>> tests;

    const string longTimeRunningPrefix = "$";

    Privet::Algorithms::Geometry::Tests::add_tests(tests, longTimeRunningPrefix);
    Privet::Algorithms::Graphs::Tests::add_tests(tests, longTimeRunningPrefix);
    Privet::Algorithms::Numbers::Tests::add_tests(tests, longTimeRunningPrefix);
    Privet::Algorithms::Strings::Tests::add_tests(tests, longTimeRunningPrefix);

    return tests;
}