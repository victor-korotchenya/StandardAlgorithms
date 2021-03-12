#include "add_tests.h"

#include "CalcAngleTests.h"
#include "closest_pair_of_points_in_2d_plane_tests.h"
#include "convex_hull_tests.h"
#include "convex_hull_trick_last_concert.h"
#include "ConvexHullTrickTests.h"
#include "ConvexToPointPositionTests.h"
#include "is_simple_convex_polygon_tests.h"
#include "line_tests.h"
#include "min_length_2d_bitonic_tour_tests.h"
#include "point_tests.h"
#include "PointTests.h"
#include "rectangle_common_area_tests.h"
#include "SegmentedRegressionTests.h"

using namespace std;
using namespace Privet::Algorithms::Geometry::Tests;
using namespace Privet::Algorithms;

void Privet::Algorithms::Geometry::Tests::add_tests(vector<pair<string, test_function>>& tests, const string& longTimeRunningPrefix)
{
    tests.emplace_back("closest_pair_of_points_in_2d_plane_tests", &closest_pair_of_points_in_2d_plane_tests);

    tests.emplace_back("min_length_2d_bitonic_tour_tests", &min_length_2d_bitonic_tour_tests);

    tests.emplace_back("line_tests", &line_tests);

    tests.emplace_back("point_tests", &point_tests);

    tests.emplace_back("rectangle_common_area", &rectangle_common_area_tests);

    tests.emplace_back("is_simple_convex_polygon_tests", &is_simple_convex_polygon_tests);

    tests.emplace_back("convex_hull_tests", &convex_hull_tests);
    tests.emplace_back("convex_hull_trick_last_concert_tests", &convex_hull_trick_last_concert_tests);

    tests.emplace_back("ConvexHullTrickTests", &ConvexHullTrickTests);

    tests.emplace_back("CalcAngleTests", &CalcAngleTests);

    tests.emplace_back("ConvexToPointPositionTests", &ConvexToPointPositionTests);

    tests.emplace_back(longTimeRunningPrefix + "SegmentedRegressionTests", &SegmentedRegressionTests);

    tests.emplace_back("PointTests", &PointTests::Test);
}