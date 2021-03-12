#include <algorithm>
#include <chrono>
#include <iostream>
#include <functional>
#include "Utilities/ConsoleColor.h"
#include "Utilities/TestNameUtilities.h"
#include "Utilities/task_runner.h"
#include "Utilities/TestRunContext.h"
#include "Utilities/TestNamedFunctor.h"
#include "Tests.h"

using namespace std::chrono;
using namespace std;
using namespace Privet::Algorithms::Colors;
using namespace Privet::Algorithms::Utilities;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    template <typename TFunctor,
        typename TheFunction = typename TFunctor::TheFunction,
        typename Pair = pair<string, TheFunction>>
        vector<TheFunction> ToFunctors(const vector<Pair>& withNames)
    {
        vector<TheFunction> result(withNames.size());

        const auto total_tests = static_cast<int>(withNames.size());

        transform(withNames.cbegin(), withNames.cend(), result.begin(),
            [total_tests](const Pair& p)
            {
                return TFunctor(p.first, p.second, total_tests);
            });

        return result;
    }

    template <typename Pair = pair<string, void(*)(void)>,
        typename SecondType = typename Pair::second_type>
        vector<SecondType> ToSecondType(const vector<Pair>& values)
    {
        vector<SecondType> result(values.size());

        transform(values.cbegin(), values.cend(), result.begin(),
            [](const Pair& p)
            {
                return p.second;
            });

        return result;
    }

    template<typename NamePtr>
    void RequireUniqueFunctionPointers(const vector<NamePtr>& testPairs)
    {
        const auto functionPointers = ToSecondType(testPairs);

        size_t firstRepeatIndex = 0;

        if (IteratorUtilities<test_function>::FindFirstRepetition(functionPointers.cbegin(), functionPointers.cend(), firstRepeatIndex, __FUNCTION__))
        {
            ostringstream ss;
            ss << "FunctionPointers are repeating at index=" << firstRepeatIndex;
            if (firstRepeatIndex < testPairs.size())
                ss << ", name='" << testPairs[firstRepeatIndex].first << "'";

            ss << ".";

            StreamUtilities::throw_exception(ss);
        }
    }

    vector<pair<string, test_function>> GetTestsPrepared(const bool isDebuggerAttached = false)
    {
        auto result = GetTests();
        // todo: fix - use a map. TestNameUtilities::RequireUniqueNames(result, "testPairs");
        RequireUniqueFunctionPointers(result);

        if (!isDebuggerAttached)
        {
            using p_t = pair<string, test_function>;

            sort(result.begin(), result.end(),
                [](const p_t& a, const p_t& b)
                {
                    return a.first < b.first;
                });
        }

        return result;
    }

    void PrintErrorsSafe(const vector<pair<string, test_function>>& testPairs, const task_runner<>::result_vector_t& errors)
    {
        try
        {
            const auto itEnd = errors.end();
            for (auto it = errors.begin(); itEnd != it; ++it)
            {
                const auto& t = *it;
                if (RunResultUtilities::HasPrintableError(get<1>(t)))
                {
                    const auto& err = get<2>(t);

                    ErrorColor errorColor;
                    cout << "The test '" << testPairs[get<0>(t)].first
                        << "' has failed, error:\n" << err << '\n';
                }
            }
        }
        catch (const exception& ex)
        {
            ErrorColor errorColor;
            cout << "Error in " + string(__FUNCTION__) + ": " << ex.what() << '\n';
        }
    }

    high_resolution_clock::time_point RunTestsSafe(const bool isDebuggerAttached, const size_t cpuCount, size_t& testCount, size_t& errorCount, bool& hasUnknownError)
    {
        TestRunContext runContext;
        auto hasEndTime = false;

        high_resolution_clock::time_point timeStop;
        try
        {
            const auto testPairs = GetTestsPrepared(isDebuggerAttached);
            testCount = testPairs.size();

            const auto testFunctors = ToFunctors<TestNamedFunctor, TestNamedFunctor::TheFunction, pair<string, test_function>>(testPairs);

            const auto errors = task_runner<TestNamedFunctor::TheFunction, TestRunContext*>::
                execute(nullptr, cpuCount, hasUnknownError, testFunctors, &runContext);

            errorCount = errors.size();
            timeStop = high_resolution_clock::now();
            hasEndTime = true;

            if (0 < errorCount)
                PrintErrorsSafe(testPairs, errors);
        }
        catch (const exception& e)
        {
            if (!hasEndTime)
                timeStop = high_resolution_clock::now();

            ErrorColor errorColor;
            cout << "Exit the tests run abnormally. Error: " << e.what() << '\n';
        }

        return timeStop;
    }
}

int Privet::Algorithms::RunTests(const bool isDebuggerAttached, const size_t cpuCount)
{
    auto result = 0;
    const auto timeStart = high_resolution_clock::now();

    size_t testCount = 0, errorCount = 0;
    auto hasUnknownError = false;

    const auto timeStop = RunTestsSafe(isDebuggerAttached, cpuCount, testCount, errorCount, hasUnknownError);

    const auto elapsedTime = duration_cast<microseconds>(timeStop - timeStart).count();

    if (!hasUnknownError && 0 == errorCount)
    {
        SuccessColor successColor;
        cout << "All " << testCount << " tests are successful.\n";
    }
    else
    {
        ErrorColor errorColor;
        cout << errorCount << " of " << testCount << " tests failed.\n";

        if (hasUnknownError)
            cout << "At least one Unknown error has occurred!\n";
    }

    cout << " Elapsed time " << elapsedTime << " micro-seconds.\n";

    return result;
}