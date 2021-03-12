#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <stdexcept>
#include "StreamUtilities.h"
#include "ExceptionUtilities.h"

namespace Privet::Algorithms
{
    class VectorUtilities final
    {
        VectorUtilities() = delete;

    public:
        template <typename T >
        static void EraseBySwapWithLast(std::vector<T>& data, const size_t index);

        template <typename T >
        static void FreeAndClear(std::vector< T*>& data);

        //Swaps the element at "index" with the last item
        // and pops the last element.
        template <typename T >
        static void SwapAndEraseFast(std::vector< T*>& data, const size_t index);

        //From C to C++.
        template <typename T>
        static void Append(const size_t length, const T* const data, std::vector<T>& v);

        // The input must be sorted.
        template <class v_t>
        static void remove_duplicates(v_t& values)
        {
            auto it = std::unique(values.begin(), values.end());
            values.erase(it, values.end());
        }

        template <class v_t>
        static void sort_remove_duplicates(v_t& values)
        {
            std::sort(values.begin(), values.end());
            remove_duplicates<v_t>(values);
        }

        //Returns the first element, or throws an std::runtime_error in case of zero size.
        template <typename T>
        static const T& First(const std::vector<T>& data);

        //Returns the last element, or throws an std::runtime_error in case of zero size.
        template <typename T>
        static const T& Last(const std::vector<T>& data);

        //The 'data' must have the same sub-matrix sizes.
        template <typename T, typename TCollection>
        static bool IsMatrix(const std::vector<TCollection>& data, const bool shallBeSquare = false, const bool throwOnException = true);

        //The 'data' must have the same sub-matrix sizes.
        template <typename T>
        static void Transpose(const std::vector<std::vector<T> >& data, std::vector<std::vector<T>>& result);

        //Sums the numbers in vertical bars from (i, 0) to (i, j),
        // and puts the result to sums[i][j].
        //The 'source' must have the same sub-matrix sizes.
        template <typename TNumber, typename TNumber2 >
        static void SumVerticalMatrices(const std::vector<std::vector<TNumber>>& source,
            std::vector<std::vector<TNumber2>>& sums, const bool includeFirstZeroRow = false);

        template <typename TSize >
        static bool CheckAreaHasAllTrueValues(const std::vector<std::vector< bool > >& data,
            const TSize x1Inclusive, const TSize y1Inclusive,
            const TSize x2Exclusive, const TSize y2Exclusive,
            const std::string& errorMessage, const bool throwOnException = true);

        //Copy source to "result", treating "result" as 2d array [rows][columns].
        template <typename T, size_t columns >
        static void Copy2D(const T source[][columns], const size_t rows, std::vector<std::vector< T > >& result);

        // If the size is positive,
        // fills the 'result' with values from (addedValue)
        // to (addedValue + size - 1).
        template <typename Item >
        static void Fill(const size_t size, std::vector< Item >& result, const size_t addedValue = 0);

        // If the size is positive,
        // fills the 'result' with values from (addedValue + size)
        // to (1 + addedValue).
        template <typename Item >
        static void FillReverse(const size_t size, std::vector< Item >& result, const size_t addedValue = 0);

        template <typename T>
        static void RequireArrayIsSorted(const T& data, const std::string& message, const bool requireUnique = false)
        {
            const size_t size = data.size();
            if (size <= 1)
                return;

            for (size_t i = 0; i < size - 1; ++i)
            {
                if (data[i + 1] < data[i])
                {
                    std::ostringstream ss;
                    ss << "The element " << data[i + 1] << " at index=" << (i + 1)
                        << " is less than " << data[i] << " at index=" << i
                        << "; size=" << size << ".";

                    StreamUtilities::throw_exception(ss, message);
                }

                if (requireUnique && data[i + 1] == data[i])
                {
                    std::ostringstream ss;
                    ss << "The element " << data[i + 1] << " at index=" << (i + 1)
                        << " is equal to " << data[i] << " at index=" << i
                        << "; size=" << size << ".";

                    StreamUtilities::throw_exception(ss, message);
                }
            }
        }

        //Whether the "data" is distinct, unique, non-repeating.
        template<typename T>
        static bool HasUniqueValues(const std::vector<T>& data);

    private:
        template <typename T >
        static void Delete(T* t);
    };

    template <typename T>
    void VectorUtilities::EraseBySwapWithLast(std::vector<T>& data, const size_t index)
    {
#ifdef _DEBUG
        if (data.size() <= index)
        {
            std::ostringstream ss;
            ss << "Error in EraseBySwapWithLast: data.size="
                << data.size() << " <= index=" << index << ".";
            StreamUtilities::throw_exception(ss);
        }
#endif
        std::swap(data[index], data[data.size() - 1]);
        data.pop_back();
    }

    template <typename T >
    void VectorUtilities::FreeAndClear(std::vector< T*>& data)
    {
        const size_t size = data.size();

        for (size_t i = 0; i < size; ++i)
        {
            VectorUtilities::Delete<T>(data[i]);
            data[i] = nullptr;
        }

        data.clear();
    }

    template <typename T >
    void VectorUtilities::Delete(T* t)
    {
        delete t;
    }

    //Fast erase runs in O(1).
    template <typename T >
    void VectorUtilities::SwapAndEraseFast(std::vector< T*>& data, const size_t index)
    {
        const size_t size = data.size();
        if (size <= index)
        {
            std::ostringstream ss;
            ss << "Error: size (" << size << ") <= index ("
                << index << ") in SwapAndErase.";
            const std::string s = ss.str();
            throw std::runtime_error(s.c_str());
        }

        std::swap(data[size - 1], data[index]);

        data.pop_back();
    }

    template <typename T >
    void VectorUtilities::Append(const size_t length, const T* const data, std::vector< T >& v)
    {
        if (nullptr == data)
            throw std::runtime_error("nullptr == data in Append.");

        for (size_t i = 0; i < length; ++i)
            v.push_back(data[i]);
    }

    template <typename T>
    const T& VectorUtilities::First(const std::vector<T>& data)
    {
        if (data.empty())
            throw std::runtime_error("The std::vector must have at least one element.");

        const auto& result = data[0];
        return result;
    }

    template <typename T>
    const T& VectorUtilities::Last(const std::vector<T>& data)
    {
        if (data.empty())
            throw std::runtime_error("The std::vector must have at least one element.");

        const auto& result = data.back();
        return result;
    }

    template <typename T, typename TCollection>
    bool VectorUtilities::IsMatrix(const std::vector< TCollection >& data, const bool shallBeSquare, const bool throwOnException)
    {
        const size_t size = data.size();

        if (size)
        {
            const size_t requiredRowSize = shallBeSquare ? size : data[0].size();

            for (size_t i = 0; i < size; ++i)
            {
                const size_t rowSize = data[i].size();

                if (requiredRowSize != rowSize)
                {
                    if (throwOnException)
                    {
                        std::ostringstream ss;
                        if (shallBeSquare)
                        {
                            ss << "The row size(" << rowSize << ") at index=" << i
                                << " must be equal to data size(" << size << ").";
                        }
                        else
                        {
                            ss << "The row size(" << rowSize << ") at index=" << i
                                << " must match the first row size(" << requiredRowSize << ").";
                        }

                        StreamUtilities::throw_exception(ss);
                    }

                    return false;
                }
            }
        }

        return true;
    }

    template <typename T >
    void VectorUtilities::Transpose(const std::vector<std::vector< T > >& source, std::vector<std::vector < T > >& target)
    {
        const size_t size = source.size();
        if (0 == size)
        {
            target.clear();
            return;
        }

        const size_t rowSize = source[0].size();
        target.resize(rowSize);

        for (size_t j = 0; j < rowSize; j++)
            target[j].resize(size);

        for (size_t i = 0; i < size; ++i)
        {
            const std::vector < T >& dataRow = source[i];

#ifdef _DEBUG
            const size_t rowSize2 = dataRow.size();
            if (rowSize != rowSize2)
            {
                std::ostringstream ss;
                ss << "Rows must have the same size, but first row size("
                    << rowSize << ") != row size (" << rowSize2
                    << ") at index=" << i << ".";
                StreamUtilities::throw_exception(ss);
            }
#endif
            for (size_t j = 0; j < rowSize; j++)
                target[j][i] = dataRow[j];
        }
    }

    template <typename TNumber, typename TNumber2 >
    void VectorUtilities::SumVerticalMatrices(const std::vector<std::vector< TNumber > >& source,
        std::vector<std::vector< TNumber2 > >& sums, const bool includeFirstZeroRow)
    {
        const size_t size = source.size();
        if (0 == size)
        {
            throw std::runtime_error("0 == source.size");
        }

        const size_t rowSize = source[0].size();
        if (0 == rowSize)
        {
            throw std::runtime_error("0 == source[0].size");
        }

        const size_t extraOne = includeFirstZeroRow ? 1 : 0;

        sums.resize(size + extraOne);

        if (includeFirstZeroRow)
        {
            sums[0].assign(rowSize, TNumber2(0));
        }

        for (size_t row = 0; row < size; ++row)
        {
            const size_t currentIndex = row + extraOne;
            std::vector< TNumber2 >& sumsRow = sums[currentIndex];
            sumsRow.assign(rowSize, TNumber2(0));

            const bool isFirstRow = 0 == row;

            const size_t previousIndex = isFirstRow ? 0 : currentIndex - 1;
            const std::vector< TNumber2 >& previouSumsRow = sums[previousIndex];

            const std::vector< TNumber >& sourceRow = source[row];

            for (size_t column = 0; column < rowSize; ++column)
            {
                if (isFirstRow)
                {
                    sumsRow[column] = sourceRow[column];
                }
                else
                {
                    sumsRow[column] = sourceRow[column] + previouSumsRow[column];
                }
            }
        }
    }

    template <typename TSize >
    bool VectorUtilities::CheckAreaHasAllTrueValues(const std::vector<std::vector< bool > >& data,
        const TSize x1Inclusive, const TSize y1Inclusive,
        const TSize x2Exclusive, const TSize y2Exclusive,
        const std::string& errorMessage, const bool throwOnException)
    {
        const size_t size = data.size();
        if (0 == size)
        {
            throw std::runtime_error("0 == size");
        }

        const size_t rowSize = data[0].size();
        if (0 == rowSize)
        {
            throw std::runtime_error("0 == rowSize");
        }

        if (x2Exclusive <= x1Inclusive)
        {
            std::ostringstream ss;
            ss << "Error in checking true values:\
The initial X-coordinate (" << x1Inclusive <<
") must be smaller than last one (" << x2Exclusive << ").";

            if (!errorMessage.empty())
            {
                ss << " " << errorMessage;
            }

            StreamUtilities::throw_exception(ss);
        }

        if (y2Exclusive <= y1Inclusive)
        {
            std::ostringstream ss;
            ss << "Error in checking true values:\
The initial Y-coordinate (" << y1Inclusive <<
") must be smaller than last one (" << y2Exclusive << ").";

            if (!errorMessage.empty())
            {
                ss << " " << errorMessage;
            }

            StreamUtilities::throw_exception(ss);
        }

        if (size < x2Exclusive)
        {
            std::ostringstream ss;
            ss << "Error in checking true values:\
The last exclusive X-coordinate (" << x2Exclusive <<
") must not exceed size (" << size << ").";

            if (!errorMessage.empty())
            {
                ss << " " << errorMessage;
            }

            StreamUtilities::throw_exception(ss);
        }

        for (TSize row = x1Inclusive; row < x2Exclusive; ++row)
        {
            const size_t rowSize1 = data[row].size();
            if (rowSize1 < y2Exclusive)
            {
                std::ostringstream ss;
                ss << "Error in checking true values:\
The last exclusive Y-coordinate (" << y2Exclusive <<
") must not exceed row [" << row << "] size (" << rowSize1 << ").";

                if (!errorMessage.empty())
                {
                    ss << " " << errorMessage;
                }

                StreamUtilities::throw_exception(ss);
            }

            for (TSize column = y1Inclusive; column < y2Exclusive; ++column)
            {
                if (!data[row][column])
                {
                    if (throwOnException)
                    {
                        std::ostringstream ss;
                        ss << "There is false value in data["
                            << row << "][" << column << "].";

                        if (!errorMessage.empty())
                        {
                            ss << " " << errorMessage;
                        }

                        StreamUtilities::throw_exception(ss);
                    }

                    return false;
                }
            }
        }

        return true;
    }

    template <typename T, size_t columns >
    void VectorUtilities::Copy2D(const T source[][columns], const size_t rows, std::vector<std::vector< T > >& result)
    {
        if (0 < rows)
        {
            ThrowIfNull(source, "source");
        }

        result.resize(rows);

        const size_t rowSizeBytes = sizeof(T) * columns;

        for (size_t i = 0; i < rows; ++i)
        {
            const T* const sourceRow = source[i];

            std::vector< T >& resultRow = result[i];
            resultRow.resize(columns);

            memcpy(resultRow.data(), sourceRow, rowSizeBytes);
        }
    }

    template <typename Item >
    void VectorUtilities::Fill(const size_t size, std::vector< Item >& result, const size_t addedValue)
    {
        result.reserve(result.size() + size);

        for (size_t i = 0; i < size; ++i)
        {
#pragma warning( push )
#pragma warning( disable : 4267)
            //Warning	C4267 'argument': conversion from 'size_t' to '', possible loss of data.
            result.push_back(Item(i + addedValue));
#pragma warning( pop )
        }
    }

    template <typename Item >
    void VectorUtilities::FillReverse(const size_t size, std::vector< Item >& result, const size_t addedValue)
    {
        result.reserve(result.size() + size);

        for (size_t i = 0; i < size; ++i)
        {
#pragma warning( push )
#pragma warning( disable : 4267)
            //Warning	C4267 'argument': conversion from 'size_t' to '', possible loss of data.
            result.push_back(Item(size - i + addedValue));
#pragma warning( pop )
        }
    }

    template <typename T>
    bool VectorUtilities::HasUniqueValues(const std::vector<T>& data)
    {
        std::unordered_set<T> dict;

        for (const T& datum : data)
        {
            const auto added = dict.insert(datum);
            if (!added.second)
            {
                return false;
            }
        }

        return true;
    }
}