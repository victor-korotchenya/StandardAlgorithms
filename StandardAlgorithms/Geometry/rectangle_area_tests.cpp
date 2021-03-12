#include"rectangle_area_tests.h"
#include"../Utilities/test_utilities.h"
#include"point.h"
#include"rectangle2d.h"

namespace
{
    using int_t = std::uint32_t;
    using long_int_t = std::uint64_t;

    using point_t = Standard::Algorithms::Geometry::point2d<int_t>;
    using rectangle_t = Standard::Algorithms::Geometry::rectangle2d<int_t, long_int_t, point_t>;

    constexpr auto len5 = 5;

    [[nodiscard]] constexpr auto area_tests() -> bool
    {
        {
            constexpr rectangle_t rectangle{};
            const auto area = rectangle.area();
            ::Standard::Algorithms::ert::are_equal(0U, area, "Zero area1");
        }
        {
            constexpr point_t point{ 3, len5 };
            constexpr rectangle_t rectangle(point, point);
            constexpr auto area = rectangle.area();
            ::Standard::Algorithms::ert::are_equal(0U, area, "Zero area2");
        }
        {
            constexpr point_t top{ 3, len5 };
            constexpr point_t bottom{ top.x + 1, top.y };
            constexpr rectangle_t rectangle{ top, bottom };
            constexpr auto area = rectangle.area();
            ::Standard::Algorithms::ert::are_equal(0U, area, "Zero area3");
        }
        {
            constexpr int_t delta = 1;
            constexpr point_t top{ 3, len5 };
            constexpr point_t bottom{ top.x, top.y + delta };
            constexpr rectangle_t rectangle{ top, bottom };
            constexpr auto area = rectangle.area();
            ::Standard::Algorithms::ert::are_equal(0U, area, "Zero area4");
        }
        {
            constexpr int_t delta = 1;

            // NOLINTNEXTLINE
            constexpr point_t top{ 30, 571 };
            constexpr point_t bottom{ top.x + delta, top.y + delta };
            constexpr rectangle_t rectangle{ top, bottom };
            constexpr auto area = rectangle.area();
            ::Standard::Algorithms::ert::are_equal(delta * delta, area, "Area5");
        }
        {
            constexpr int_t delta = 17;

            // NOLINTNEXTLINE
            constexpr point_t top{ 1, 572 };
            constexpr point_t bottom{ top.x + delta, top.y + delta };
            constexpr rectangle_t rectangle{ top, bottom };
            constexpr auto area = rectangle.area();
            ::Standard::Algorithms::ert::are_equal(delta * delta, area, "Area6");
        }
        {
            constexpr int_t delta_x = 17;
            constexpr int_t delta_y = 256;

            // NOLINTNEXTLINE
            constexpr point_t top{ 102, 57209 };
            constexpr point_t bottom{ top.x + delta_x, top.y + delta_y };
            constexpr rectangle_t rectangle{ top, bottom };
            constexpr auto area = rectangle.area();
            ::Standard::Algorithms::ert::are_equal(delta_x * delta_y, area, "Area7");
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Geometry::Tests::rectangle_area_tests()
{
    static_assert(area_tests());
}
