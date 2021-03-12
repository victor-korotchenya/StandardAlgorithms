#include <iostream>
#include "Utilities/ConsoleColor.h"
#include "Tests.h"

using namespace std;
using namespace Privet::Algorithms::Colors;
using namespace Privet::Algorithms;

int main()
{
    cout << "__cplusplus " << (__cplusplus) << "\n";
    try
    {
        const bool isDebuggerAttached =
#ifdef _WIN32z
            // todo: p1. fix isDebuggerAttached.
            IsDebuggerPresent() ? 1 : 0;
#else
            0;
#endif

        //To force printing 1000 as 1,000.
        cout.imbue(locale(""));

        const size_t cpuCount = isDebuggerAttached ? 1 : 0;

        const auto result = RunTests(isDebuggerAttached, cpuCount);
        return result;
    }
    catch (const exception& ex)
    {
        ErrorColor errorColor;
        cout << "Error: " << ex.what() << '\n';
        return 1;
    }
    catch (...)
    {
        ErrorColor errorColor;
        cout << "Unknown error.\n";
        return 2;
    }
}