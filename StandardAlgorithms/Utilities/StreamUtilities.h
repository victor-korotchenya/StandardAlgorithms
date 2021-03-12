#pragma once
#include <algorithm>
#include <cassert>
// #include <format> todo: p2. #include <format>
#include <exception>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <type_traits>
#include <utility>

class StreamUtilities final
{
    StreamUtilities() = delete;

public:
    //For unknown reasons it is not working.
    //E.g. 24.1 can be printed with 16 digits before the actual number.
    // Or 100 is printed as 000000FC53CEF4A8100.
    //
    //template <class TStream >
    //inline static TStream& SetMaxPrecision(
    //    TStream& stream, size_t precision = 0)
    //{
    //    if (0 == precision)
    //    {
    //        precision = GetMaxDoublePrecision();
    //    }

    //    stream << setprecision(precision);
    //    return stream;
    //}

    static int GetMaxDoublePrecision() noexcept;

    //Usage example:
    //
    //StreamUtilities::AppendMessageIfNotEmpty(ss, message);
    static void AppendMessageIfNotEmpty(
        std::ostringstream& ss, const std::string& message, const char separator = ' ');

    static void AppendMessageIfNotEmpty(
        std::ostringstream& ss, const char* const message, const char separator = ' ');

    static bool HasMessage(const char* message);

    //Usage example:
    //
    //std::ostringstream ss;
    //ss << "Error: " <<  << ".";
    //StreamUtilities::throw_exception(ss);
    template<class E = std::runtime_error>
    static inline E throw_exception(std::ostringstream& str, const std::string& message = "")
    {
        AppendMessageIfNotEmpty(str, message);
        const std::string s = str.str();
        throw E(s.c_str());
    }

    static std::runtime_error throw_exception(std::ostringstream& str, const std::string& message = "");
};