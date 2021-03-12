#pragma once
#include <cmath>//log2
#include <stdexcept>
#include <vector>

namespace Privet::Algorithms::Trees
{
    // Given a matrix n*m,
    // calculate the min(or max) value in a rectangle [x1,y1] [x2,y2],
    // where 0 <= x1 <= x2  < n and 0 <= y1 <= y2 < m.
    // O(1) get time because min[a..b..c..d] == min(min[a..c], min[b..d]).
    template <typename Number,
        size_t n_max, size_t log_n_max, size_t m_max, size_t log_m_max,
        typename Operation,
        typename data_t = std::vector<std::vector<Number>>>
        class SparseTable_RangeMinimumQuery_2D final
    {
        static_assert(log_n_max < n_max, "Expect (log_n_max < n_max)");
        static_assert(log_m_max < m_max, "Expect (log_m_max < m_max)");
        Number _Data[n_max][log_n_max + 1][m_max][log_m_max + 1];
        Operation _Operation;

    public:
        explicit SparseTable_RangeMinimumQuery_2D(
            const data_t& data,
            Operation operation = {})
            : _Operation(operation)
        {
            for (size_t row = 0; row < n_max; ++row)
            {
                for (size_t col = 0; col < m_max; ++col)
                {
                    _Data[row][0][col][0] = data[row][col];
                }
            }

            for (size_t row = 0; row < n_max; ++row)
            {
                for (size_t ly = 0; ly < log_m_max; ++ly)
                {
                    const auto ad = static_cast<size_t>(1) << ly;
                    for (size_t col = 0; col + ad < m_max; ++col)
                    {
                        _Data[row][0][col][ly + 1] =
                            _Operation(_Data[row][0][col][ly],
                                _Data[row][0][col + ad][ly]);
                    }
                }
            }

            for (size_t lx = 0; lx < log_n_max; ++lx)
            {
                const auto ad = static_cast<size_t>(1) << lx;
                for (size_t row = 0; row + ad < n_max; ++row)
                {
                    for (size_t ly = 0; ly <= log_m_max; ++ly)
                    {
                        for (size_t col = 0; col < m_max; ++col)
                        {
                            _Data[row][lx + 1][col][ly] =
                                _Operation(_Data[row][lx][col][ly],
                                    _Data[row + ad][lx][col][ly]);
                        }
                    }
                }
            }
        }

        //Select in a region [x1, y1] and [x2, y2] inclusively.
        Number get(size_t x1, size_t y1, size_t x2_incl, size_t y2_incl) const
        {
#ifdef _DEBUG
            {
                auto isOk = x1 <= x2_incl && x2_incl < n_max
                    && y1 <= y2_incl && y2_incl < m_max;
                if (!isOk)
                    throw std::runtime_error(
                        "Bad rectangle coordinates in SparseTable_RangeMinimumQuery_2D.");
            }
#endif
            const auto lenx = x2_incl + 1 - x1, leny = y2_incl + 1 - y1;

            // Note. 'int __lg(uint64_t)' might be faster than log2.
            const auto logx = static_cast<size_t>(std::log2(lenx)),
                logy = static_cast<size_t>(std::log2(leny));

            const auto y3 = y2_incl + 1 - (static_cast<size_t>(1) << logy);
            const auto res1 = _Operation(_Data[x1][logx][y1][logy],
                _Data[x1][logx][y3][logy]);

            const auto x3 = x2_incl + 1 - (static_cast<size_t>(1) << logx);
            const auto res2 = _Operation(_Data[x3][logx][y1][logy],
                _Data[x3][logx][y3][logy]);

            const auto result = _Operation(res1, res2);
            return result;
        }
    };
}