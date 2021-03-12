#pragma once
#include <string>
#include <mutex>
#include <iostream>

namespace Privet::Algorithms::Tests
{
    struct TestRunContext final
    {
        size_t LongRuningTests = 0;

        //The mutex must be used for printing onto cout.
        std::mutex Mutex;

        bool ShallPrintLine = false;

        inline void PrintNewLine()
        {
            if (ShallPrintLine)
            {
                std::cout << '\n';
            }
            else
            {
                ShallPrintLine = true;
            }
        }
    };
}