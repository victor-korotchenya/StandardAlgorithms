#include"launch_exception.h"

Standard::Algorithms::Utilities::launch_exception::launch_exception(const std::string &message)
    : std::runtime_error(message)
{
}

Standard::Algorithms::Utilities::launch_exception::launch_exception(const char *const message)
    : std::runtime_error(message)
{
}

Standard::Algorithms::Utilities::launch_exception::launch_exception(const std::exception &other)
    : std::runtime_error(other.what())
{
}
