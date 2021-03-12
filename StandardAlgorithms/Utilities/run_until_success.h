#pragma once
#include"project_constants.h"
#include<functional>
#include<string>

namespace Standard::Algorithms::Utilities
{
    // Return false on error and when (throw_on_fail is false).
    [[nodiscard]] auto run_until_success(const std::function<bool()> &func, const std::string &extra_message,
        bool throw_on_fail = Standard::Algorithms::Utilities::throw_on_fail_default,
        std::int32_t max_attempts = Standard::Algorithms::Utilities::max_attempts) -> bool;
} // namespace Standard::Algorithms::Utilities
