#pragma once

namespace Standard::Algorithms
{
    // Let's get rid of macros.
    inline constexpr const auto is_debug =
#if _DEBUG
        true;
#else
        false;
#endif
} // namespace Standard::Algorithms
