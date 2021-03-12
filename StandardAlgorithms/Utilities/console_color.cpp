#include"console_color.h"
#include<cassert>
#include<cstdlib> // std::system

namespace
{
    void system_color(const char *const name)
    {
        assert(name != nullptr);
        // flawfinder: ignore suppress
        std::system(name);
    }
} // namespace

Standard::Algorithms::Colors::error_color::error_color()
{
    system_color("Color 04"); // Red front on black background.
}

Standard::Algorithms::Colors::success_color::success_color()
{
    system_color("Color 02"); // Green front on black background.
}

Standard::Algorithms::Colors::warn_color::warn_color()
{
    system_color("Color 06"); // Yellow front on black background.
}
