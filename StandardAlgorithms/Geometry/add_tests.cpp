#include"add_tests.h"
#include"calc_angle_tests.h"
#include"closest_pair_of_points_in_2d_plane_tests.h"
#include"convex_hull_tests.h"
#include"convex_hull_trick_tests.h"
#include"convex_min_weight_triangulation_tests.h"
#include"convex_to_point_position_tests.h"
#include"is_simple_convex_polygon_tests.h"
#include"line_tests.h"
#include"min_length_2d_bitonic_tour_tests.h"
#include"point_tests.h"
#include"rectangle_area_tests.h"
#include"rectangle_common_area_tests.h"
#include"segmented_regression_tests.h"
#include"simplex_method_tests.h"

namespace
{
    constexpr auto five = 5;
} // namespace

void Standard::Algorithms::Geometry::Tests::add_tests(
    std::vector<test_function> &tests, const std::string &long_time_running_prefix)
{
    // tests.emplace_back("line_segments_intersection_tests", &line_segments_intersection_tests, ?);

    tests.emplace_back("closest_pair_of_points_in_2d_plane_tests", &closest_pair_of_points_in_2d_plane_tests, 3);

    tests.emplace_back("min_length_2d_bitonic_tour_tests", &min_length_2d_bitonic_tour_tests, 4);

    tests.emplace_back("line_tests", &line_tests, 4);

    tests.emplace_back("point_tests", &point_tests, five);

    tests.emplace_back("rectangle_common_area_tests", &rectangle_common_area_tests, 2);

    tests.emplace_back("is_simple_convex_polygon_tests", &is_simple_convex_polygon_tests, 1);

    tests.emplace_back("convex_min_weight_triangulation_tests", &convex_min_weight_triangulation_tests, 1);

    tests.emplace_back("convex_hull_tests", &convex_hull_tests, 2);

    tests.emplace_back("convex_hull_trick_tests", &convex_hull_trick_tests, 2);

    tests.emplace_back("simplex_method_tests", &simplex_method_tests, 1);

    tests.emplace_back("calc_angle_tests", &calc_angle_tests, 1);

    tests.emplace_back("convex_to_point_position_tests", &convex_to_point_position_tests, 1);

    tests.emplace_back(long_time_running_prefix + "segmented_regression_tests", &segmented_regression_tests, 2);

    tests.emplace_back("rectangle_area_tests", &rectangle_area_tests, 1);
}
