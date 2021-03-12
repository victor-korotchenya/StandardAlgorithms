#include <cassert>
#include <cmath>
#include <iomanip>
#include "TestNamedFunctor.h"
#include "LaunchException.h"
#include "ConsoleColor.h"
#include "../elapsed_time_ns.h"

using namespace std;
using namespace Privet::Algorithms::Colors;
using namespace Privet::Algorithms::Utilities;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    int log10up(const int v)
    {
        const auto log = v > 10 ? log10(v) : 1;
        const auto c = ceil(log);
        const auto r = static_cast<int>(c);
        assert(r > 0);
        return r;
    }
}

TestNamedFunctor::TestNamedFunctor(
    const string& name, TestNamedFunctor::VoidFunc testFunction, int total_tests)
    : TestNamedFunctor::TParent(name, testFunction),
    total_tests_log10(log10up(total_tests))
{
    assert(total_tests > 0);
}

void TestNamedFunctor::operator()(TestRunContext* testRunContext)
{
    auto has_error = false;
    exception ex;
    int64_t elapsed_ns;

    elapsed_time_ns t0;
    try
    {
        GetFunction()();

        elapsed_ns = t0.elapsed();
    }
    catch (const exception& e)
    {
        elapsed_ns = t0.elapsed();

        has_error = true;
        ex = e;
        unique_lock<mutex> addLock(testRunContext->Mutex);

        testRunContext->PrintNewLine();

        ErrorColor errorColor;
        cout << "The test '" << get_Name()
            << "' has failed:\n" << e.what() << '\n';
    }

    constexpr auto max_duration_ns = 1000 * 1000 *
#ifdef _DEBUG
        50;
#else
        5;
#endif
    if (elapsed_ns > max_duration_ns)
    {
        unique_lock<mutex> addLock(testRunContext->Mutex);

        //testRunContext->PrintNewLine();
        testRunContext->LongRuningTests++;

        cout << " Test_" << setfill('0') << setw(total_tests_log10) << testRunContext->LongRuningTests
            << " '" << get_Name() << "' took too long: "
            << elapsed_ns / 1000 << " micro-second(s).\n";
    }

    if (has_error)
    {
        throw LaunchException(ex);
    }
}