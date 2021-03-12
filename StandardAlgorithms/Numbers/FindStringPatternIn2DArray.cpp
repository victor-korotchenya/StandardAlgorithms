#include "Arithmetic.h"
#include "../Utilities/StreamUtilities.h"
#include "../Utilities/VectorUtilities.h"
#include "FindStringPatternIn2DArray.h"

using namespace std;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

bool FindStringPatternIn2DArray::Find(
    const vector< string >& data,
    const string& pattern,
    vector< TPoint >& points)
{
    const auto size = data.size();
    if (0 == size)
    {
        throw exception("The data must be not empty.");
    }

    const auto patternSize = pattern.size();
    if (0 == patternSize)
    {
        throw exception("The pattern must be not empty.");
    }

    points.clear();

    const auto sizeSquared = MultiplyUnsigned(size, size);
    if (patternSize > sizeSquared)
    {
        return false;
    }

    VectorUtilities::IsMatrix< char, string >(data, true);

    //If size = 5, then row = [1..5].
    //These 2 rows/columns to be marked as "always used": 0, 6.
    const auto lastRowColumn = size + 1;
    const size_t usedPointsSizeMaxEstimate = (size << 2) + patternSize;

    TUsedPoints usedPoints(usedPointsSizeMaxEstimate);

    for (size_t i = 0; i <= lastRowColumn; i++)
    {
        //Rows: 0 and last.
        usedPoints.insert(TPoint(0, i));
        usedPoints.insert(TPoint(lastRowColumn, i));

        //Columns: 0 and last.
        usedPoints.insert(TPoint(i, 0));
        usedPoints.insert(TPoint(i, lastRowColumn));
    }

    Context context;
    context.Size = size;
    context.Index = 0;
    context.Data = &data;
    context.Pattern = &pattern;
    context.UsedPoints = &usedPoints;

    for (size_t row = 1; row <= size; ++row)
    {
        for (size_t column = 1; column <= size; ++column)
        {
            const bool result = FindHelper(&context, row, column);
            if (result)
            {
                FillFoundPoints(size, patternSize, usedPoints, points);
                return true;
            }
        }
    }

    return false;
}

//The row and column both start from 1.
bool FindStringPatternIn2DArray::FindHelper(
    Context* const context,
    const TDistance row,
    const TDistance column)
{
    const char c = (*(context->Pattern))[context->Index];

    const vector< string >& data = *(context->Data);
    TUsedPoints& usedPoints = *(context->UsedPoints);

    const TPoint p = TPoint(row, column);
    const auto found = usedPoints.find(p);
    if (usedPoints.end() == found
        //Not found - it must come before the next line to
        // avoid array bound errors.
        && c == data[row - 1][column - 1])
    {
        usedPoints.insert(p);
        context->Index++;

        if (context->Index == context->Size)
        {//Last symbol has been matched.
            return true;
        }

        //Try to match more in 8 neighbors.

        const bool result =
            //TODO: p3. use stack.
            FindHelper(context, row - 1, column - 1) ||
            FindHelper(context, row - 1, column) ||
            FindHelper(context, row - 1, column + 1) ||
            //
            FindHelper(context, row, column - 1) ||
            FindHelper(context, row, column + 1) ||
            //
            FindHelper(context, row + 1, column - 1) ||
            FindHelper(context, row + 1, column) ||
            FindHelper(context, row + 1, column + 1);
        if (result)
        {
            return true;
        }

        //TODO: cheating.
        usedPoints.erase(p);
        context->Index--;
    }

    return false;
}

void FindStringPatternIn2DArray::FillFoundPoints(
    const TDistance size,
    const TDistance patternSize,
    const TUsedPoints& usedPoints,
    vector< TPoint >& points)
{
    points.resize(patternSize);

    size_t count = 0;

    const auto end = usedPoints.cend();

    for (auto it = usedPoints.cbegin(); end != it; ++it)
    {
        const TPoint& p = *it;
        if (1 <= p.X && p.X <= size
            && 1 <= p.Y && p.Y <= size)
        {
            points[count] = TPoint(p.X - 1, p.Y - 1);

            if (++count == patternSize)
            {
                return;
            }
        }
    }

    ostringstream ss;
    ss << "Internal error: only " << count
        << " out of " << patternSize
        << " point(s) has(have) been found.";
    StreamUtilities::throw_exception(ss);
}