#pragma once
#include <exception>
#include <string>
#include <sstream>

struct TooDeepStackException final : std::exception
{
    TooDeepStackException(const size_t deepness = {})
        : std::exception(GetMessage(deepness).c_str())
    {
    }

private:

    static inline std::string GetMessage(const size_t deepness)
    {
        std::ostringstream ss;
        ss << "The stack deepness " << deepness << " is overflown.";
        const auto result = ss.str();
        return result;
    }
};