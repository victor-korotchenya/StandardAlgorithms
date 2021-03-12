#pragma once
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<limits>
#include<stdexcept>
#include<string>
#include<typeinfo> // std::type_info
#include<utility>

namespace Standard::Algorithms::Utilities
{
    // Can be used to save memory
    // when many indexes are stored in a container,
    // and it is known in advance that the data size has certain limits.
    template<std::integral int_t, std::size_t max_size = static_cast<std::size_t>(std::numeric_limits<int_t>::max())>
    requires(0U < max_size)
    [[nodiscard]] constexpr auto check_size(
        const std::string &message, const std::size_t size, const bool can_be_maximum = false) -> int_t
    {
        assert(!message.empty());

        const auto maximum = can_be_maximum ? max_size : (max_size - 1ZU);

        if (maximum < size) [[unlikely]]
        {
            auto err = "Too large size (" + std::to_string(size) + ") for the type '" + typeid(int_t).name() +
                "' with maximum " + std::to_string(maximum) + ". " + message;

            throw std::runtime_error(err);
        }

        auto cast_size = static_cast<int_t>(size);

        assert(static_cast<std::size_t>(cast_size) <= maximum);

        return cast_size;
    }
} // namespace Standard::Algorithms::Utilities
