#pragma once

#include <functional> // less
#include <stdexcept>
#include <stack>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Stack, returning minimum value.
            template <typename T,
                typename TLess = std::less<T>>
                class StackAndMinSimple final
            {
                TLess _Less;

                std::stack<T> _Data;
                std::stack<T> _Minimums;

            public:

                explicit StackAndMinSimple(const TLess& less = {});

                bool empty() const noexcept
                {
                    const bool result = _Data.empty();
                    return result;
                }

                size_t size() const noexcept
                {
                    const size_t result = _Data.size();
                    return result;
                }

                const T& top() const;
                const T& Minimum() const;

                void push(const T& value);

                void pop();

            private:
                void RequireNonEmpty() const;

                bool IsLessOrEqual(const T& x, const T& y) const;
            };

            template <typename T, typename TLess >
            StackAndMinSimple< T, TLess >::StackAndMinSimple(
                const TLess& less)
                : _Less(less), _Data(), _Minimums()
            {
            }

            template <typename T, typename TLess >
            const T& StackAndMinSimple< T, TLess >::top() const
            {
                RequireNonEmpty();
                const T& result = _Data.top();
                return result;
            }

            template <typename T, typename TLess >
            const T& StackAndMinSimple< T, TLess >::Minimum() const
            {
                RequireNonEmpty();
                const T& result = _Minimums.top();
                return result;
            }

            template <typename T, typename TLess >
            void StackAndMinSimple< T, TLess >::push(const T& value)
            {
                const bool hasMinimum = _Data.empty()
                    || IsLessOrEqual(value, _Minimums.top());
                if (hasMinimum)
                {
                    _Minimums.push(value);
                }

                try
                {
                    _Data.push(value);
                }
                catch (...)
                {
                    if (hasMinimum)
                    {//Repair.
                        _Minimums.pop();
                    }

                    throw;
                }
            }

            template <typename T, typename TLess >
            void StackAndMinSimple< T, TLess >::pop()
            {
                RequireNonEmpty();

                const bool hasMinimum = IsLessOrEqual(_Data.top(), _Minimums.top());
                if (hasMinimum)
                {
                    _Minimums.pop();
                }

                try
                {
                    _Data.pop();
                }
                catch (...)
                {
                    if (hasMinimum)
                    {//Repair.
                        _Minimums.push(_Data.top());
                    }

                    throw;
                }
            }

            template <typename T, typename TLess >
            void StackAndMinSimple< T, TLess >::RequireNonEmpty() const
            {
                if (_Data.empty())
                {
                    throw std::runtime_error("The stack is empty.");
                }
            }

            template <typename T, typename TLess >
            bool StackAndMinSimple< T, TLess >::IsLessOrEqual(
                const T& x, const T& y) const
            {
                const bool result = //x <= y
                    !_Less(y, x);
                return result;
            }
        }
    }
}