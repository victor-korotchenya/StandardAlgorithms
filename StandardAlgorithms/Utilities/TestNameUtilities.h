#pragma once
#include <stdexcept>
#include <string>
#include <vector>
#include "StreamUtilities.h"
#include "ExceptionUtilities.h"
#include "HasFunctionGetName.h"
#include "IteratorUtilities.h"

//Usage:
//TestNameUtilities::RequireUniqueNames(testCases, "testCases");
class TestNameUtilities final
{
    TestNameUtilities() = delete;

    template <typename Item>
    static const std::string& get_Name(const Item& item)
    {
        return item.get_Name();
    }

public:
    //Returns true only if there is a repetition
    // or the 'data' is empty.
    template <typename Item,
        typename GetNameFunction = const std::string& (*)(const Item&)>
        static bool RequireUniqueNames(const std::vector< Item >& data, const std::string& extraErrorMessage,
            const bool throwOnException = true, GetNameFunction getNameFunction = nullptr)
    {
        if (extraErrorMessage.empty())
            ThrowNullException("extraErrorMessage");

        if (data.empty())
        {
            if (throwOnException)
            {
                std::ostringstream ss;
                ss << "The input must be not empty.";

                StreamUtilities::throw_exception(ss, extraErrorMessage);
            }

            return true;
        }

        using Iterator = typename std::vector< Item >::const_iterator;
        GetNameFunction ptr = getNameFunction;
        if (nullptr == ptr)
            ptr = &get_Name<Item>;

        size_t firstRepeatIndex;

        const auto result = IteratorUtilities<std::string, Iterator, GetNameFunction>::FindFirstRepetition(data.cbegin(), data.cend(),
            firstRepeatIndex, extraErrorMessage, throwOnException, ptr);

        return result;
    }
};