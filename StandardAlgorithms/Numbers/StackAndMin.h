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
            //Although it is more memory efficient, than "StackAndMinSimple",
            // the default implementation does NOT check for overflow!
            template <typename T,
                //To check for add overflow.
                typename TAdd = T(*)(const T&, const T&),
                //To check for subtract overflow.
                typename TSubtract = TAdd,
                typename TLess = std::less<T>>
                class StackAndMin final
            {
                TAdd _Add;
                TSubtract _Subtract;
                TLess _Less;

                std::stack< T > _Data;
                T _Minimum;

            public:
                StackAndMin(
                    //This implementation does NOT check for overflow.
                    TAdd add = [](const T& a, const T& b) -> T
                    {
                        return a + b;
                    },
                    //Here is NO check for overflow.
                        TSubtract subtract = [](const T& a, const T& b) -> T
                    {
                        return a - b;
                    },
                        const TLess& less = {});

                bool empty() const noexcept
                {
                    const bool result = _Data.empty();
                    return result;
                }

                size_t size() const noexcept
                {
                    const auto result = _Data.size();
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

            template <typename T, typename TAdd, typename TSubtract, typename TLess >
            StackAndMin< T, TAdd, TSubtract, TLess >::StackAndMin(
                TAdd add,
                TSubtract subtract,
                const TLess& less)
                : _Add(add), _Subtract(subtract), _Less(less),
                _Data(), _Minimum()
            {
            }

            template <typename T, typename TAdd, typename TSubtract, typename TLess >
            const T& StackAndMin< T, TAdd, TSubtract, TLess >::top() const
            {
                RequireNonEmpty();
                const T& result = _Less(_Data.top(), _Minimum)
                    ? _Minimum : _Data.top();
                return result;
            }

            template <typename T, typename TAdd, typename TSubtract, typename TLess >
            const T& StackAndMin< T, TAdd, TSubtract, TLess >::Minimum() const
            {
                RequireNonEmpty();
                return _Minimum;
            }

            template <typename T, typename TAdd, typename TSubtract, typename TLess >
            void StackAndMin< T, TAdd, TSubtract, TLess >::push(const T& value)
            {
                if (_Data.empty())
                {
                    _Minimum = value;
                    _Data.push(value);
                }
                else if (//_Minimum <= value
                    IsLessOrEqual(_Minimum, value))
                {
                    _Data.push(value);
                }
                else
                {//value < _Minimum
                    const auto doubleValue = _Add(value, value);
                    const auto newValue = _Subtract(doubleValue, _Minimum);
                    _Data.push(newValue);
                    _Minimum = value;
                }
            }

            template <typename T, typename TAdd, typename TSubtract, typename TLess >
            void StackAndMin< T, TAdd, TSubtract, TLess >::pop()
            {
                RequireNonEmpty();

                if (_Less(_Data.top(), _Minimum))
                {
                    const auto doubleVelue = _Add(_Minimum, _Minimum);
                    const auto newValue = _Subtract(doubleVelue, _Data.top());
                    _Minimum = newValue;
                }

                _Data.pop();
            }

            template <typename T, typename TAdd, typename TSubtract, typename TLess >
            void StackAndMin< T, TAdd, TSubtract, TLess >::RequireNonEmpty() const
            {
                if (_Data.empty())
                {
                    throw std::runtime_error("The stack is empty.");
                }
            }

            template <typename T, typename TAdd, typename TSubtract, typename TLess >
            bool StackAndMin< T, TAdd, TSubtract, TLess >::IsLessOrEqual(
                const T& x, const T& y) const
            {
                const bool result = //x <= y
                    !_Less(y, x);
                return result;
            }
        }
    }
}