#pragma once
#include <type_traits>

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            template<class t>
            constexpr t epsilon_default()
            {
                static_assert(std::is_floating_point_v<t>);

                constexpr auto v = static_cast<t>(1e-8);
                assert(v > 0);
                return v;
            }
        }
    }
}