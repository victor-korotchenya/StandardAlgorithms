#pragma once
#include"../Utilities/zu_string.h"
#include<cassert>
#include<cstdint>
#include<stdexcept>
#include<string>

namespace Standard::Algorithms::Strings
{
    // Can be used for constructor/operator testing.
    template<class magic_pendel_t1>
    struct full_class final
    {
        using magic_pendel_t = magic_pendel_t1;

        constexpr explicit full_class(magic_pendel_t &magic)
            : Magic_pendel(magic)
            , Identif(append(magic, next_id(magic) + " ctor"))
        {
        }

        constexpr full_class(full_class &ins)
            : Magic_pendel(ins.Magic_pendel)
            , Identif(append(ins.Magic_pendel, next_id(ins.Magic_pendel) + " copy ctor from " + ins.ids()))
        {
        }

        constexpr full_class(full_class &&ins) noexcept(false)
            : Magic_pendel(ins.Magic_pendel)
            , Identif(append(ins.Magic_pendel, next_id(ins.Magic_pendel) + " move ctor from " + ins.ids()))
        {
        }

        constexpr auto operator= (const full_class &ins) &noexcept(false) -> full_class &
        {
            // NOLINTNEXTLINE NOLINT
            if (this == &ins) // Pacify the compiler: handle self-assignment.
            {
            }

            append(Magic_pendel, ids() + " copy op from " + ins.ids());
            return *this;
        }

        constexpr auto operator= (full_class &&ins) &noexcept(false) -> full_class &
        {
            append(Magic_pendel, ids() + " move op from " + ins.ids());
            return *this;
        }

        // NOLINTNEXTLINE
        constexpr ~full_class() noexcept(false) // okay for tests only.
        {
            append(Magic_pendel, ids() + " dtor");
        }

        [[nodiscard]] constexpr auto ids() const noexcept(false) -> std::string
        {
            return ::Standard::Algorithms::Utilities::zu_string(Identif);
        }

        [[nodiscard]] constexpr auto pendel() const &noexcept -> magic_pendel_t &
        {
            return Magic_pendel;
        }

private:
        magic_pendel_t &Magic_pendel;
        std::int32_t Identif;
    };
} // namespace Standard::Algorithms::Strings
