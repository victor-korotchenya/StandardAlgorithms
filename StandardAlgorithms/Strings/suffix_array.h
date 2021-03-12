#pragma once
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            /*Suffix array is a sorted array of all suffixes of a string. sa[i] is the starting index of the i-th smallest suffix.
             The whole word index is 0.
          Suffixes of "banana":
            0    banana
            1    anana
            2    nana
            3    ana
            4    na
            5    a

          suffix array of "banana" is { 5,3,1,0,4,2 }:
            5 a
            3 ana
            1 anana
            0 banana
            4 na
            2 nana  */
            std::vector<int> suffix_array(const std::string& s);

            std::vector<int> suffix_array_slow(const std::string& s);

            namespace Tests
            {
                void suffix_array_test();
            }
        }
    }
}