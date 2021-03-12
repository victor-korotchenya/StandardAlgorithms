#pragma once
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Numbers
{
    template <typename T, typename TSize>
    class one_time_queue final
    {
        static_assert(std::is_integral_v<TSize>);

        const TSize _size;
        TSize _head, _tail;
        T* _q;

        one_time_queue(one_time_queue&) = delete;
        one_time_queue& operator = (one_time_queue&) = delete;

        one_time_queue(one_time_queue&&) = delete;
        one_time_queue& operator =(one_time_queue&&) = delete;

    public:
        using item_type = T;
        using size_type = TSize;

        explicit one_time_queue(TSize capacity) :
            _size(RequirePositive(capacity, "capacity")), _head(0), _tail(0),
            _q(new T[capacity])
        {
        }

        ~one_time_queue()
        {
            delete[] _q;
        }

        TSize size() const
#ifndef _DEBUG
            noexcept
#endif
        {
#ifdef _DEBUG
            check();
#endif
            return _tail - _head;
        }

        bool empty() const
#ifndef _DEBUG
            noexcept
#endif
        {
            return !size();
        }

        // The array is not reset.
        void reset()
        {
            _tail = _head = 0;
#ifdef _DEBUG
            check();
#endif
        }

        T* data() const noexcept
        {
            return _q;
        }

        void push(T value)
        {
            if (_size <= _tail)
            {
                std::ostringstream ss;
                ss << "The one_time_queue is full (" << _size << ").";
                StreamUtilities::throw_exception(ss);
            }

            _q[_tail] = value;
            ++_tail;
#ifdef _DEBUG
            check();
#endif
        }

        T pop()
        {
            if (empty())
                throw std::runtime_error("The one_time_queue is empty at pop.");

            auto result = _q[_head];
            ++_head;
#ifdef _DEBUG
            try
            {
                check();
            }
            catch (...)
            {
                --_head;
                throw;
            }
#endif
            // Prevent loitering.
            _q[_head - 1] = {};
            return result;
        }

        T& top() const
        {
            if (empty())
                throw std::runtime_error("The one_time_queue is empty at top.");
#ifdef _DEBUG
            check();
#endif
            auto& result = _q[_head];
            return result;
        }

    private:
#ifdef _DEBUG
        void check() const
        {
            if (_tail < _head)
            {
                std::ostringstream ss;
                ss << "Error tail(" << _tail
                    << ") < head(" << _head
                    << ") in one_time_queue.";
                StreamUtilities::throw_exception(ss);
            }
        }
#endif
    };
}