#pragma once
#include <vector>
#include <unordered_set>
#include <string>
#include <sstream>
#include "StreamUtilities.h"

template <typename Item,
    typename Iterator = typename std::vector<Item>::const_iterator,
    typename GetValue = const Item& (*)(const Item& item),
    typename Hasher = std::hash<Item>>
    class IteratorUtilities final
{
    IteratorUtilities() = delete;

public:
    //Returns true only if there is a repetition.
    static bool FindFirstRepetition(const Iterator beginInclusive, const Iterator endExclusive, size_t& firstRepeatIndex, const std::string& extraErrorMessage = "", const bool throwOnException = false,
        GetValue getValue = [](const Item& a) -> const Item& { return a; })
    {
        std::unordered_set< Item, Hasher> uniqueValues;

        for (Iterator it = beginInclusive; endExclusive != it; ++it)
        {
            const auto item = getValue(*it);

            const auto inserted = uniqueValues.insert(item);
            if (!inserted.second)
            {
                firstRepeatIndex = it - beginInclusive;
                if (throwOnException)
                {
                    std::ostringstream ss;
                    ss << "The item '" << item
                        << "' at index=" << firstRepeatIndex << " is repeating.";

                    StreamUtilities::throw_exception(ss, extraErrorMessage);
                }

                return true;
            }
        }

        return false;
    }
};