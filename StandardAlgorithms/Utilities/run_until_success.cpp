#include"run_until_success.h"
#include<stdexcept>

[[nodiscard]] auto Standard::Algorithms::Utilities::run_until_success(const std::function<bool()> &func,
    const std::string &extra_message, bool throw_on_fail, std::int32_t max_attempts) -> bool
{
    for (std::int32_t attempt{};;)
    {
        if (func())
        {
            return true;
        }

        if (!(++attempt < max_attempts))
        {
            if (throw_on_fail)
            {
                throw std::runtime_error(
                    extra_message + " failed after " + std::to_string(max_attempts) + " attempts.");
            }

            return false;
        }
    }
}
