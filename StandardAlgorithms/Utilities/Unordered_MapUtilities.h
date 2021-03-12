#pragma once
#include <vector>
#include <unordered_map>

template <typename T>
void GetUniqueItemsAndIndexes(const std::vector<T>& data, std::unordered_map<T, std::vector<size_t>>& itemsAndIndexes)
{
    const size_t size = data.size();
    itemsAndIndexes.clear();

    for (size_t index = 0; index < size; ++index)
    {
        const T& datum = data[index];

        auto found = itemsAndIndexes.find(datum);
        if (itemsAndIndexes.end() == found)
        {
            std::vector<size_t> list(1);
            list[0] = index;

            itemsAndIndexes.insert(std::make_pair(datum, list));
        }
        else
        {
            std::vector<size_t>& list = found->second;
            list.push_back(index);
        }
    }
}