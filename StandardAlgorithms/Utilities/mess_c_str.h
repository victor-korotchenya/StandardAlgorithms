#pragma once
#include<cstring>
#include<string>

namespace Standard::Algorithms
{
    // An empty std::string dot c_str() might crash.
    // Can return null pointer.
    [[nodiscard]] [[gnu::const]] inline constexpr auto mess_c_str(const auto &message)
    {
        const auto *const constant_mess = message.empty() ? nullptr : message.c_str();
        return constant_mess;
    }

    [[nodiscard]] [[gnu::pure]] constexpr auto mess_c_str(auto &message) // todo(p3): del copy-paste.
    {
        auto *const mess = message.empty() ? nullptr : message.c_str();
        return mess;
    }
} // namespace Standard::Algorithms
