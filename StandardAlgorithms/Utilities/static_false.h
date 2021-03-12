#pragma once
#include<cassert>
#include<concepts>
#include<type_traits>

namespace Standard::Algorithms::Inner
{
    template<class t1>
    struct static_falser final : std::false_type
    {
        using t = t1;
    };
} // namespace Standard::Algorithms::Inner

namespace Standard::Algorithms
{
    // Use sample:
    //
    // if constexpr(std:is_signed_v<t>) { return staff; }
    // else{// Force a compilation error.
    //  static_assert(static_false<t>);}
    template<class t>
    inline constexpr bool static_false = Inner::static_falser<t>::value;
} // namespace Standard::Algorithms
