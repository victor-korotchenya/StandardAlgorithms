#pragma once
#include"../Utilities/zu_string.h"
#include<cstddef>
#include<stdexcept>
#include<string>

namespace Standard::Algorithms::Numbers
{
    struct too_deep_stack_exception final : std::runtime_error
    {
        inline explicit too_deep_stack_exception(const std::size_t deepness = {})
            : std::runtime_error(build_message(deepness))
        {
        }

private:
        [[nodiscard]] inline static constexpr auto build_message(const std::size_t deepness) -> std::string
        {
            return "The stack deepness " + ::Standard::Algorithms::Utilities::zu_string(deepness) + " is overflown.";
        }
    };
} // namespace Standard::Algorithms::Numbers
