#pragma once

#include <string>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            //Given a string, count...
            std::vector<int> count_distinct_substrings_of_length_slow(
                const std::string& str,
                const std::vector<int>& lengths);

            std::vector<int> count_distinct_substrings_of_length(
                const std::string& str,
                const std::vector<int>& lengths);

            namespace Tests
            {
                void count_distinct_substrings_of_length_tests();
            }
        }
    }
}
