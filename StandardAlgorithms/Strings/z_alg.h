#pragma once
#include <string>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            //Given a string s, z algorithm computes z[n],
            // where z[i] is the length of the longest substring starting from s[i],
            // which is also a prefix of s.
            //
            // String matching example: pattern "aab", text "baabaa".
            // The concatenated string "aab#baabaa"
            // Z array for the above is 0100031021
            //                               aab
            // The value 3 (pattern size) in Z array means a matching.
            std::vector<int> z_array(const std::string& s);

            std::vector<int> z_array_slow(const std::string& s);

            //A border is a prefix, equal to suffix and shorter then the whole string.
            std::vector<int> string_borders_slow(const std::string& s);
            std::vector<int> string_borders(const std::string& s);

            namespace Tests
            {
                void z_array_test();
                void string_border_test();
            }
        }
    }
}