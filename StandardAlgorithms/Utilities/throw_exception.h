#pragma once
#include"w_stream.h"
#include<cassert>
#include<concepts>
#include<exception>
#include<stdexcept>
#include<string>
#include<utility>

namespace Standard::Algorithms
{
    // auto str = ::Standard::Algorithms::Utilities::w_stream();
    // str << "Error: " << "details.";
    // throw_exception(str);
    template<class exception_t = std::runtime_error>
    requires(std::derived_from<exception_t, std::exception>)
    [[noreturn]] inline auto throw_exception(std::ostringstream &str, const std::string &message = {}) -> std::exception
    {
        assert(str.good());

        if (!message.empty())
        {
            if (constexpr char separator = ' '; separator != message[0])
            {
                str << separator;
            }

            str << message;

            assert(str.good());
        }

        throw exception_t(str.str());
    }
} // namespace Standard::Algorithms
