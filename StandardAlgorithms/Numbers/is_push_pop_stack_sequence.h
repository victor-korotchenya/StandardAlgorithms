#pragma once
#include<cstddef>
#include<stack>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given two arrays, identify whether
    // the second one is push pop sequence using the first array.
    // E.g. given {1, 2, 3}, these are okay:
    // - {1, 2, 3}
    // - {2, 1, 3}
    // - {3, 2, 1}
    // And these are not:
    // - {5,}
    // - {2, 1, 4}
    // - {3, 1, 2}
    // - {3, 2, 1, 0}.
    template<class item_t>
    [[nodiscard]] constexpr auto is_push_pop_stack_sequence(
        const std::vector<item_t> &array1, const std::vector<item_t> &array2) -> bool
    {
        const auto size = array1.size();

        if (size != array2.size())
        {
            return false;
        }

        if (size == 0U)
        {
            return true;
        }

        std::stack<item_t> sta;
        std::size_t index1{};
        std::size_t index2{};

        do
        {
            if (!sta.empty() && sta.top() == array2[index2])
            {
                sta.pop();
                ++index2;
                continue;
            }

            while (index1 < size && array1[index1] != array2[index2])
            {
                sta.push(array1[index1++]);
            }

            ++index1, ++index2;
        } while (index1 < size && index2 < size);

        while (!sta.empty() && index2 < size && sta.top() == array2[index2])
        {
            sta.pop();
            ++index2;
        }

        return index2 == size && index1 == size;
    }
} // namespace Standard::Algorithms::Numbers
