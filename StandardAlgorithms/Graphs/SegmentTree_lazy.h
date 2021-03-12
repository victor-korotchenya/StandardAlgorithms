#pragma once
#include <vector>
#include "BinaryTreeUtilities.h"
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template <typename Number>
    Number set_lazy(const Number& a, const Number& b)
    {
        if (a)
        {
        }
        return b;
    }
}

namespace Privet::Algorithms::Trees
{
    // For tests only - use segment_tree_add_sum or similar.
    template <typename Number, typename Operation,
        typename AddOperation = Number(*)(const Number&, const Number&)>
        class SegmentTree_lazy  final
    {
        std::vector<Number> _Data, _lazy;

        Operation _Operation;
        AddOperation _AddOperation;
        const Number _Zero;
        const size_t _MaxIndexExclusive, _Height;

    public:

        explicit SegmentTree_lazy(size_t initialSize,
            Operation operation = {},
            //E.g. 0 for addition, 1 for multiplication.
            Number zero = {},
            AddOperation addOperation = &set_lazy<Number>)
            : _Data(BinaryTreeUtilities<>::calc_tree_size(initialSize), zero),
            _lazy(initialSize, zero),
            _Operation(operation),
            _AddOperation(addOperation),
            _Zero(zero),
            _MaxIndexExclusive(initialSize),
            _Height(RequirePositive(BinaryTreeUtilities<>::calc_tree_height(initialSize),
                "Height in SegmentTree_lazy()."))
        {
        }

        explicit SegmentTree_lazy(const std::vector<Number>& data,
            Operation operation = {},
            Number zero = {},
            AddOperation addOperation = &set_lazy<Number>)
            : SegmentTree_lazy(data.size(), operation, zero, addOperation)
        {
            std::copy(data.begin(), data.end(), _Data.begin() + data.size());
            for (auto i = _MaxIndexExclusive - 1; 0 != i; --i)
            {
                _Data[i] = _Operation(
                    _Data[BinaryTreeUtilities<>::left_child(i)],
                    _Data[BinaryTreeUtilities<>::right_child(i)]);
            }
        }

        inline size_t max_index_exclusive() const
        {
            return _MaxIndexExclusive;
        }

        void set(size_t index, const Number& value)
        {
            set(index, index + 1, value);
        }
        Number get(size_t index)
        {
            auto result = get(index, index + 1);
            return result;
        }

        //const std::vector<Number>& push_changes_to_leaves()
        //{
        //    for (auto parent = 1; parent < _MaxIndexExclusive; ++parent)
        //    {
        //        const auto left = BinaryTreeUtilities<>::left_child(parent);
        //        const auto right = BinaryTreeUtilities<>::right_child(parent);
        //        if (_Zero != _lazy[parent])
        //        {
        //            set_value(left, _lazy[parent]);
        //            set_value(right, _lazy[parent]);
        //            _lazy[parent] = _Zero;
        //        }

        //        _Data[left] = _Operation(_Data[left], _Data[parent]);
        //        _Data[right] = _Operation(_Data[right], _Data[parent]);
        //        _Data[parent] = _Zero;
        //    }

        //    return _Data;
        //}

        //The "index" starts from 0.
        void set(size_t leftInclusive, size_t rightExclusive, const Number& value)
        {
            BinaryTreeUtilities<>::check_indexes(
                leftInclusive, rightExclusive,
                _MaxIndexExclusive, "SegmentTree_lazy::set");

            leftInclusive += _MaxIndexExclusive;
            rightExclusive += _MaxIndexExclusive;

            const auto left = leftInclusive, right = rightExclusive;
            push_lazy_down(leftInclusive);
            push_lazy_down(rightExclusive - 1);
            while (leftInclusive < rightExclusive)
            {
                if (leftInclusive & 1)
                {
                    set_value(leftInclusive++, value);
                }
                if (rightExclusive & 1)
                {
                    set_value(rightExclusive - 1, value);
                }

                leftInclusive >>= 1;
                rightExclusive >>= 1;
            }

            restore_up(left);
            restore_up(right - 1);
        }

        //Return the aggregate on the interval [leftInclusive, rightExclusive).
        Number get(size_t leftInclusive, size_t rightExclusive)
        {
            BinaryTreeUtilities<>::check_indexes(
                leftInclusive, rightExclusive,
                _MaxIndexExclusive, "SegmentTree_lazy::get");

            leftInclusive += _MaxIndexExclusive;
            rightExclusive += _MaxIndexExclusive;
            push_lazy_down(leftInclusive);
            push_lazy_down(rightExclusive - 1);
            auto result = _Zero;
            while (leftInclusive < rightExclusive)
            {
                if (leftInclusive & 1)
                {
                    result = _Operation(result, _Data[leftInclusive++]);
                }
                if (rightExclusive & 1)
                {
                    result = _Operation(_Data[rightExclusive - 1], result);
                }

                leftInclusive >>= 1;
                rightExclusive >>= 1;
            }

            return result;
        }

    private:
        void restore_up(size_t index)
        {
            while (1 < index)
            {
                index >>= 1;
                const auto is_empty = _Zero == _lazy[index];
                const auto left = index << 1;
                const auto right = left | 1;
                _Data[index] = _Operation(
                    is_empty ? _Data[left] : _AddOperation(_Data[left], _lazy[index]),
                    is_empty ? _Data[right] : _AddOperation(_Data[right], _lazy[index]));
            }
        }

        void push_lazy_down(const size_t index)
        {
            auto h = _Height;
            do
            {
                const auto parent = index >> h;
                if (_Zero != _lazy[parent])
                {
                    set_value(parent << 1, _lazy[parent]);
                    set_value((parent << 1) | 1, _lazy[parent]);
                    _lazy[parent] = _Zero;
                }
            } while (--h);
        }

        inline void set_value(const size_t index, const Number& value)
        {
            _Data[index] = _AddOperation(_Data[index], value);
            if (index < _MaxIndexExclusive)
            {
                _lazy[index] = _AddOperation(_lazy[index], value);
            }
        }
    };
}