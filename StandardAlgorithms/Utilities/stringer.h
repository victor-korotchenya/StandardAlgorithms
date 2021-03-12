#pragma once
#include<cassert>
#include<concepts>
#include<cstddef>
#include<string>
#include<utility>

namespace Standard::Algorithms::Utilities::Inner
{
    inline constexpr bool is_string_move_noexcept =
        std::is_nothrow_move_constructible_v<std::string> &&noexcept(std::declval<std::string>().size());

    static_assert(is_string_move_noexcept);
} // namespace Standard::Algorithms::Utilities::Inner

namespace Standard::Algorithms::Utilities
{
    struct stringer final
    {
        inline constexpr explicit stringer(const std::string &str = {})
            : String(str)
            , Initial_size(String.size())
        {
        }

        inline constexpr explicit stringer(std::string &&str) noexcept(Inner::is_string_move_noexcept)
            : String(std::move(str))
            , Initial_size(String.size())
        {
        }

        [[nodiscard]] inline constexpr explicit(false) // explicit - bad idea. // NOLINTNEXTLINE
        operator const std::string & () const &noexcept
        {
            return String;
        }

        [[nodiscard]] inline constexpr auto c_str() const &noexcept -> const char *
        {
            return String.c_str();
        }

        inline constexpr auto initialize(std::string &&str) &noexcept(Inner::is_string_move_noexcept) -> stringer &
        {
            String = std::move(str);
            Initial_size = String.size();

            return *this;
        }

        inline constexpr auto initialize(const std::string &str) &noexcept(false) -> stringer &
        {
            String = str;
            Initial_size = String.size();

            return *this;
        }

        inline constexpr auto reset_append(const std::string &str) &noexcept(false) -> stringer &
        {
            assert(Initial_size <= String.size());

            String.resize(Initial_size);

            return append(str);
        }

        inline constexpr auto append(const std::string &str) &noexcept(false) -> stringer &
        {
            assert(Initial_size <= String.size());

            String += str;

            return *this;
        }

private:
        std::string String;
        std::size_t Initial_size;
    };
} // namespace Standard::Algorithms::Utilities
