#pragma once
#include <unordered_set>
#include <vector>
#include "../Geometry/point.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given 2D-array data[n][n], and a string "pattern",
            // returns whether the data contains the pattern.
            // A cell inside has 8 neighbors; one in the corner - 3.
            //Each cell can be opened only once.
            class FindStringPatternIn2DArray final
            {
                FindStringPatternIn2DArray() = delete;

            public:
                using TDistance = size_t;
                using TPoint = Geometry::point2d<TDistance>;

                static bool Find(
                    const std::vector<std::string>& data,
                    const std::string& pattern,
                    std::vector<TPoint>& points);

            private:
                using TUsedPoints = std::unordered_set<TPoint>;

                struct Context final
                {
                    TDistance Size;
                    TDistance Index;
                    const std::vector< std::string >* Data;
                    const std::string* Pattern;
                    TUsedPoints* UsedPoints;
                };

                static bool FindHelper(
                    Context* const context,
                    //The row and column both start from 1.
                    const TDistance row, const TDistance column);

                static void FillFoundPoints(
                    const TDistance size,
                    const TDistance patternSize,
                    const TUsedPoints& usedPoints,
                    std::vector< TPoint >& points);
            };
        }
    }
}