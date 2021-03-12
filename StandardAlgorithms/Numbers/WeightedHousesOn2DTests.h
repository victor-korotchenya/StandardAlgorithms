#pragma once
#include "WeightedHousesOn2D.h"

class WeightedHousesOn2DTests final
{
    WeightedHousesOn2DTests() = delete;

public:
    static void Test();

private:
    static void Test4();
    static void TestRandom(const int maxX, const int maxY);

    static unsigned int SolveByPermutation(
        const std::vector<Privet::Algorithms::Geometry::PointAndWeight<Privet::Algorithms::Geometry::point2d<int>, unsigned int> >& houses,
        const int radius,
        const size_t maximumStationCount,
        const int maxX,
        const int maxY,
        std::vector<Privet::Algorithms::Geometry::point2d<int>>& chosenPlacements);
};