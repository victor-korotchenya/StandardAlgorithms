#include"heap_common.h"
#include"../Utilities/create_or_terminate.h"
#include<iostream>
#include<stdexcept>

namespace
{
    [[nodiscard]] constexpr auto build_prefixes_danger() noexcept(false)
    {
        std::array<std::string, ::Standard::Algorithms::Utilities::max_logn_algorithm_depth + 1> result{};

        const std::string level_sep = "| ";
        std::size_t index{};

        for (auto &cur : result)
        {
            const auto &prev = result.at(index);
            if (index++ != 0U)
            {
                cur = prev + level_sep;
            }
        }

        return result;
    }
} // namespace

namespace Standard::Algorithms::Heaps
{
    const std::array<std::string, ::Standard::Algorithms::Utilities::max_logn_algorithm_depth + 1> &prefix_cache =
        create_or_terminate(&build_prefixes_danger);
} // namespace Standard::Algorithms::Heaps
