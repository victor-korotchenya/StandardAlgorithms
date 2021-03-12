#pragma once

#include <vector>
#include <stack>

namespace Privet::Algorithms::Numbers
{
    //Given two arrays, identify whether
    //the second one is push pop sequence using the first array.
    //E.g. given {1, 2, 3}, these are OK:
    // - {1, 2, 3}
    // - {2, 1, 3}
    // - {3, 2, 1}
    // And these are not:
    // - {5,}
    // - {2, 1, 4}
    // - {3, 1, 2}
    // - {3, 2, 1, 0}.
    template <typename T>
    bool IsPushPopStackSequence(
        const std::vector<T>& array1,
        const std::vector<T>& array2)
    {
        const size_t size = array1.size();
        if (size != array2.size())
        {
            return false;
        }

        if (array1.empty())
        {//Both empty.
            return true;
        }

        std::stack <T> stack1;
        size_t index1 = 0;
        size_t index2 = 0;
        do
        {
            if (!stack1.empty() &&
                stack1.top() == array2[index2])
            {
                stack1.pop();
                ++index2;
                continue;
            }

            while (index1 < size &&
                array1[index1] != array2[index2])
                stack1.push(array1[index1++]);

            ++index1, ++index2;
        } while (index1 < size && index2 < size);

        while (!stack1.empty() &&
            index2 < size &&
            stack1.top() == array2[index2])
        {
            stack1.pop();
            ++index2;
        }

        return index2 == size && index1 == size;
    }
}