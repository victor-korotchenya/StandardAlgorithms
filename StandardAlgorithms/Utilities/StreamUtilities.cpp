#include <limits>
#include "StreamUtilities.h"

using namespace std;

//TODO: p3. call it on stream, not inside classes?
int StreamUtilities::GetMaxDoublePrecision() noexcept
{
    const auto result = numeric_limits<long double>::digits10 + 1;
    return result;
}

runtime_error StreamUtilities::throw_exception(ostringstream& str, const string& message)
{
    return throw_exception<runtime_error>(str, message);
}

inline void StreamUtilities::AppendMessageIfNotEmpty(
    ostringstream& ss, const string& message, const char separator)
{
    if (!message.empty())
    {
        AppendMessageIfNotEmpty(ss, message.c_str(), separator);
    }
}

void StreamUtilities::AppendMessageIfNotEmpty(
    ostringstream& ss, const char* const message, const char separator)
{
    if (HasMessage(message))
    {
        if (separator != message[0])
        {
            ss << separator;
        }

        ss << message;
    }
}

inline bool StreamUtilities::HasMessage(const char* message)
{
    const bool result = nullptr != message && 0 != message[0];
    return result;
}