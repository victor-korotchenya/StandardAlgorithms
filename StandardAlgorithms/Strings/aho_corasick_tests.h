#pragma once
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            namespace Tests
            {
                void aho_corasick_tests();

                template<typename key_t, typename value_t>
                void pairs_leave_value_and_sort(const std::vector<std::pair<key_t, value_t>>& source,
                    std::vector<value_t>& actual)
                {
                    actual.clear();

                    std::transform(source.begin(), source.end(), back_inserter(actual),
                        [](const std::pair<key_t, value_t>& p) -> auto { return p.second; });

                    sort(actual.begin(), actual.end());
                }
            }
        }
    }
}