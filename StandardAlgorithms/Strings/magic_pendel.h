#pragma once
// "magic_pendel.h"
#include"../Utilities/zu_string.h"
#include<cstdint>
#include<string>

namespace Standard::Algorithms::Strings
{
    struct magic_pendel final
    {
        std::string journal{};
        std::int32_t last_id{};
    };

    inline constexpr auto append(magic_pendel &magic, const std::string &message) noexcept(false) -> std::int32_t
    {
        assert(!message.empty());
        magic.journal += message;

        {
            constexpr const auto *const separator = "; ";

            magic.journal += separator;
        }

        return ++magic.last_id;
    }

    [[nodiscard]] inline constexpr auto next_id(const magic_pendel &magic) noexcept(false) -> std::string
    {
        return ::Standard::Algorithms::Utilities::zu_string(magic.last_id + 1);
    }
} // namespace Standard::Algorithms::Strings
