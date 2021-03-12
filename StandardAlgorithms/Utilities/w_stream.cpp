#include"w_stream.h"
#include"localize_stream.h"

[[nodiscard]] auto Standard::Algorithms::Utilities::w_stream() -> std::ostringstream
{
    std::ostringstream s_str;
    localize_stream(s_str);
    return s_str;
}

void Standard::Algorithms::Utilities::reset(std::ostringstream &str)
{
    str.str({});
    str.clear();
}
