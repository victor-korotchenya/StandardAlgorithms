#pragma once
#include <string>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                class TestArrayUtility final
                {
                    TestArrayUtility() = delete;

                public:
                    static void CheckArrayIndex(const size_t size, const size_t index,
                        const std::string& name);
                };
            }
        }
    }
}