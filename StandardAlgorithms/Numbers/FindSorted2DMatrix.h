#pragma once
#include <vector>
#include <limits>
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Input: key to search; array sorted by each column,
            //  and each row in increasing order.
            //Output: pair of array indexes or (SIZE_MAX, SIZE_MAX) when not found.
            template <typename Item,
                typename TLess = std::less< Item > >
                class FindSorted2DMatrix final
            {
                TLess Less;

            public:

                explicit FindSorted2DMatrix(const TLess& lessArg = {});

                using Result = std::pair<size_t, size_t>;

                //Running time is O(rows + columns).
                Result Search(
                    const std::vector< std::vector < Item > >& data,
                    const Item& key);
            };

            template <typename Item, typename TLess >
            FindSorted2DMatrix< Item, TLess >::FindSorted2DMatrix(const TLess& lessArg)
                : Less(lessArg)
            {
            }

            template <typename Item, typename TLess >
            typename FindSorted2DMatrix< Item, TLess >::Result
                FindSorted2DMatrix< Item, TLess >::Search(
                    const std::vector<std::vector < Item > >& data,
                    const Item& key)
            {
                const size_t size = data.size();
                if (0 == size)
                {
                    throw std::runtime_error("The 'data' must be not empty.");
                }

                const size_t rowSize = data[0].size();
                if (0 == rowSize)
                {
                    throw std::runtime_error("The row size must greater than 0.");
                }

                size_t row = 0;
                size_t column = rowSize - 1;

                for (;;)
                {
                    if (Less(key, data[row][column]))
                    {
                        if (0 == column)
                        {
                            break;
                        }

                        --column;
                    }
                    else if (Less(data[row][column], key))
                    {
                        ++row;
                        if (size == row)
                        {
                            break;
                        }

                        const size_t rowSize2 = data[row].size();
                        if (rowSize != rowSize2)
                        {
                            std::ostringstream ss;
                            ss << "The size (" << rowSize2 << ") of row ("
                                << row << ") must be same as that (" << rowSize << ") of the first row.";
                            StreamUtilities::throw_exception(ss);
                        }
                    }
                    else
                    {
                        return Result(row, column);
                    }
                }

                return Result(SIZE_MAX, SIZE_MAX);
            }
        }
    }
}