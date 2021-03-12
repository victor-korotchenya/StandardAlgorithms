#pragma once

//TODO: p1. look at #include <safeint.h>

#ifdef _WIN64
#include <intrin.h>
#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(__popcnt)
#pragma intrinsic(__popcnt64)
#endif

#include <algorithm>
#include <cassert>
#include <cmath>
#include <exception>
#include <functional>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <sstream>
#include <type_traits>
#include "../Utilities/StreamUtilities.h"
#include "../disable_constructors.h"

#ifndef _WIN32

#ifndef min
#define min(a,b) ( (a) < (b) ? (a) : (b) )
#endif

#ifndef max
#define max(a,b) ( (a) < (b) ? (b) : (a) )
#endif

#endif

#ifndef isinf
#define isinf(x) (!_finite(x))
#endif

inline double AbsoluteValue(const double& a) noexcept
{
    const double result = fabs(a);
    return result;
}

inline bool IsInfinite(const double& a)
{
    const bool result = isinf(a);
    return result;
}

struct ArithmeticException final : std::exception
{
    explicit ArithmeticException(const char* const& message)
        : std::exception(message)
    {
    }
};

#ifdef _DEBUG

template <typename T >
void AssertSigned(const std::string& functionName)
{
#pragma warning(push)
#pragma warning(disable : 4127)
    {
        const T t = T(T(0) - T(1));
        if (0 < t)
        {
            const auto message = "The '" + functionName
                + "' function is for signed numbers only.";
            throw std::runtime_error(message.c_str());
        }
    }
#pragma warning(pop)
}

template <typename T >
void AssertUnsigned(const std::string& functionName)
{
#pragma warning(push)
#pragma warning(disable : 4127)
    {
        const T t = T(T(0) - T(1));
        if (t < 0)
        {
            const auto message = "The '" + functionName
                + "' function is for unsigned numbers only.";
            throw std::runtime_error(message.c_str());
        }
    }
#pragma warning(pop)
}

#endif

//For signed numbers.
template <typename T>
typename std::enable_if<std::numeric_limits<T>::is_signed, bool>::type
IsNegative(const T& value)
{
    return std::numeric_limits<T>::is_signed && value < T(0);
}

//For unsigned numbers.
template <typename T>
typename std::enable_if<!std::numeric_limits<T>::is_signed, bool>::type
IsNegative(const T&)
{
    return false;
}

template <typename T >
constexpr int sign(const T val)
{
    return (T(0) < val) - (val < T(0));
};

//This is for unsigned numbers only!
//Example: const auto c = MultiplyUnsigned<unsigned long long>(1, 2, (ULLONG_MAX));
template <typename T >
T MultiplyUnsigned(T const a, T const b)
{
    constexpr T maxValue = std::numeric_limits<T>::max();
    static_assert(std::is_unsigned_v<T>);
#ifdef _DEBUG
    AssertUnsigned< T >(__FUNCTION__);
#endif

    if (0 == b)
        return 0;

    if ((maxValue) / b < a)
    {
        std::ostringstream ss;
        ss << "Multiplying " << a << " and " << b << " will produce an overflow.";
        throw ArithmeticException(ss.str().c_str());
    }

    const T result = a * b;
    return result;
}

//This is for signed numbers only!
template <typename T >
T MultiplySigned(T const a, T const b)
{
    static_assert(std::is_signed_v<T>);
#ifdef _DEBUG
    AssertSigned< T >(__FUNCTION__);
#endif

    if (0 == a)
        return 0;

    const T result = a * b;
    if (b == result / a)
        return result;
    {
        std::ostringstream ss;
        ss << "Multiplying " << a << " and " << b << " will produce an overflow.";
        throw ArithmeticException(ss.str().c_str());
    }
}

//This is for unsigned numbers only!
template <typename T >
T AddUnsigned(T const a, T const b)
{
#ifdef _DEBUG
    AssertUnsigned<T>(__FUNCTION__);
#endif

    const T result = a + b;
    if (result < a)
    {
        std::ostringstream ss;
        ss << "Adding " << a << " and " << b << " will produce an overflow.";
        const std::string s = ss.str();
        throw ArithmeticException(s.c_str());
    }

    return result;
}

//This is for signed numbers only!
template <typename T >
T AddSigned(T const a, T const b)
{
    //TODO: p1. look at #include <safeint.h>

#ifdef _DEBUG
    AssertSigned<T>(__FUNCTION__);
#endif

    const T result = a + b;
    if (a < T(0) ? b < result : result < b)
    {
        std::ostringstream ss;
        ss << "Adding " << a << " and " << b << " will produce an overflow.";
        const std::string s = ss.str();
        throw ArithmeticException(s.c_str());
    }

    return result;
}

template <typename T>
typename std::enable_if<std::numeric_limits<T>::is_signed, bool>::type
add_checked(T const a, T const b)
{
    return AddSigned<T>(a, b);
}

template <typename T>
typename std::enable_if<!std::numeric_limits<T>::is_signed_v, bool>::type
add_checked(T const a, T const b)
{
    return AddUnsigned<T>(a, b);
}

//This is for signed numbers only!
template <typename T >
T SubtractSigned(T const a, T const b)
{
#ifdef _DEBUG
    AssertSigned< T >(__FUNCTION__);
#endif

    const T result = a - b;
    if ((0 < b) == (result < a))
    {
        return result;
    }

    std::ostringstream ss;
    ss << "Subtracting " << a << " minus " << b << " will produce an overflow.";
    const std::string s = ss.str();
    throw ArithmeticException(s.c_str());
}

//Zero must not be an input to th_is function!
//Numbers starts from 0, that is CountTailZeroes(2) == 1.
//Given 8 == (1000 in binary), the function returns 3.
inline unsigned CountTailZeroes(
    //It is limited to 64-bit numbers.
    const unsigned long long value)
{
    assert(value);
#ifdef _WIN64
    unsigned long result;
#ifdef _DEBUG
    const char isOk =
#endif
        _BitScanForward64(&result, value);
#ifdef _DEBUG
    if (!isOk)
    {
        std::ostringstream ss;
        ss << "Error in " << (__FUNCDNAME__) << "(" << value << ").";
        StreamUtilities::throw_exception(ss);
    }
#endif
    return static_cast<unsigned>(result);
#else
    //Linux
    // int __builtin_ffsl (unsigned long)
    //  Similar to __builtin_ffs, except the argument type is unsigned long.
    //
    // Built-in Function: int __builtin_ffsll (unsigned long long)
    //  Similar to __builtin_ffs, except the argument type is unsigned long long.
    //
    // Built-in Function: int __builtin_ffs (unsigned int x)
    //  Returns one plus the index of the least significant 1-bit of x,
    // or if x is zero, returns zero.
    auto ret = __builtin_ffsl(value);
    auto result = (unsigned)(ret - 1);
    return result;
#endif
}

//Zero must not be an input to th_is function!
//Returns the number of leading 0-bits in value,
//starting at the most significant bit position.
//E.g given {1, 3, 7, 8, 15, 16}, returns {0, 1, 2, 3, 3, 4}.
//
//_BitScanReverse, _BitScanReverse64
inline unsigned MostSignificantBit(const unsigned long int value)
{
    assert(value);
#ifdef _WIN64
    unsigned long result;

#ifdef _DEBUG
    char isOk =
#endif
        _BitScanReverse(&result, value);

#ifdef _DEBUG
    if (!isOk)
    {
        std::ostringstream ss;
        ss << "Error in " << (__FUNCDNAME__) << "(" << value << ").";
        StreamUtilities::throw_exception(ss);
    }
#endif

    return static_cast<unsigned>(result);
#else
    //Linux

    //Returns the number of leading 0-bits in value,
    //starting at the most significant bit position.
    //If value is 0, the result is undefined.
    const unsigned numberOfLeading0 = __builtin_clz(value);
    auto result = static_cast<unsigned>(31 - numberOfLeading0);
    return result;
#endif
}

//See also MostSignificantBit.
inline unsigned MostSignificantBit64(const unsigned long long int value)
{
    assert(value);
#ifdef _WIN64
    unsigned long result;

#ifdef _DEBUG
    char isOk =
#endif
        _BitScanReverse64(&result, value);

#ifdef _DEBUG
    if (!isOk)
    {
        std::ostringstream ss;
        ss << "Error in " << (__FUNCDNAME__) << "(" << value << ").";
        StreamUtilities::throw_exception(ss);
    }
#endif

    return static_cast<unsigned>(result);
#else
    //Linux

    //Returns the number of leading 0-bits in value,
    //starting at the most significant bit position.
    //If value is 0, the result is undefined.
    const unsigned numberOfLeading0 = __builtin_clzll(value);
    auto result = static_cast<unsigned>(63 - numberOfLeading0);
    return result;
#endif
}

// todo: p1. use C++20 popcount and similar.
inline unsigned PopCount(const std::uint32_t value)
{
    const auto
#ifdef _WIN64
        result = static_cast<unsigned>(__popcnt(value));
#else
        //Built-in Function: int __builtin_popcountl (unsigned long)
        // Similar to __builtin_popcount, except the argument type is unsigned long.
        //
        //Built-in Function: int __builtin_popcount (unsigned int x)
        // Returns the number of 1-bits in x.
        //
        //Built-in Function: int __builtin_popcountll (unsigned long long)
        // Similar to __builtin_popcount, except the argument type is unsigned long long.
        result = static_cast<unsigned>(__builtin_popcount(value));
#endif
    return result;
}

//unsigned __int64 __popcnt64(unsigned __int64)
inline unsigned PopCount(const std::uint64_t value)
{
    const auto
#ifdef _WIN64
        result = static_cast<unsigned>(__popcnt64(value));
#else
        //Built-in Function: int __builtin_popcountl (unsigned long)
        // Similar to __builtin_popcount, except the argument type is unsigned long.
        //
        //Built-in Function: int __builtin_popcount (unsigned int x)
        // Returns the number of 1-bits in x.
        //
        //Built-in Function: int __builtin_popcountll (unsigned long long)
        // Similar to __builtin_popcount, except the argument type is unsigned long long.
        result = static_cast<unsigned>(__builtin_popcountll(value));
#endif
    return result;
}

template <typename T >
constexpr int compare(const T a, const T b)
{
    return a < b ? -1 : b < a;
};

template <typename T>
constexpr bool GreaterThanFunction(const T a, const T b)
{
    const bool result = b < a;
    return result;
};

//template <typename T>
//struct GreaterThanClass final
//{
//  bool operator() (const T &a, const T &b) const
//  {
//    const bool result = b < a;
//    return result;
//  }
//};

template <typename T >
constexpr bool IsPowerOfTwo(const T number)
{
    return (0 != number) &&
        //Delete the only 1-bit.
        (0 == (number & (number - T(1))));
}

//Given 0, return 0.
//1 -> 1.
//2..3 -> 2.
//4..7 -> 4.
//8..15 -> 8.
//16..31 -> 16.
inline uint32_t RoundToLesserPowerOfTwo(const uint32_t number)
noexcept
{
    if (!number)
        return 0;
    const auto hi_bit = MostSignificantBit(number);
    const auto result = uint32_t(1) << hi_bit;
    return result;
}

//Given 0 or 1, return 1.
//2 -> 2.
//3..4 -> 4.
//5..8 -> 8.
//9..16 -> 16.
inline uint32_t RoundToGreaterPowerOfTwo(const uint32_t number)
noexcept
{
    if (!number)
        return 1;

    uint32_t result = number - 1u;
    result |= result >> 1;
    result |= result >> 2;
    result |= result >> 4;
    result |= result >> 8;
    result |= result >> 16;
    ++result;
    return result;
}

//See also RoundToGreaterPowerOfTwo.
inline uint64_t RoundToGreaterPowerOfTwo_t(const uint64_t number)
noexcept
{
    if (!number)
        return 1;

    uint64_t result = number - 1u;
    result |= result >> 1;
    result |= result >> 2;
    result |= result >> 4;
    result |= result >> 8;
    result |= result >> 16;
    result |= result >> 32;
    ++result;
    return result;
}

// Given an 1011001,
// get back 1111111 where all 0s after the highest 1 bit are set to 1.
// Given 0, return 0.
inline uint32_t xor_max(uint32_t a) noexcept
{
    a |= a >> 1;
    a |= a >> 2;
    a |= a >> 4;
    a |= a >> 8;
    a |= a >> 16;
    return a;
}

inline uint64_t xor_max(uint64_t a) noexcept
{
    a |= a >> 1;
    a |= a >> 2;
    a |= a >> 4;
    a |= a >> 8;
    a |= a >> 16;
    a |= a >> 32;
    return a;
}

//Must be divisible by alignment.
inline unsigned long long PadByAlignment(const unsigned long long value, const unsigned long long alignment)
noexcept
{
    const unsigned char reminder = static_cast<unsigned char>(value & (alignment - 1));
    const unsigned long long result = (reminder)
        ? value + alignment - reminder
        : value;
    return result;
}

// Given 0x9A, returns 0x9A9A9A9A9A9A9A9A.
inline unsigned long long ConvertByDuplicating(const unsigned char bitValue)
noexcept
{
    const int length = sizeof(unsigned long long);

    unsigned long long bitValueCast = bitValue;
    unsigned long long result = bitValueCast;
    for (int i = 1; i < length; ++i)
    {
        bitValueCast = bitValueCast << 8;
        result |= bitValueCast;
    }

    return result;
}

template<typename Number>
bool is_even(const Number& a)
{
    const auto result = 0 == (a & 1);
    return result;
}

template <typename T>
bool is_within(
    const T& minInclusive,
    const T& maxInclusive,
    const T& value)
{
    static_assert(std::is_unsigned<T>::value);

    if (maxInclusive < minInclusive)
    {
        std::ostringstream ss;
        ss << "The minInclusive(" << minInclusive
            << ") cannot exceed maxInclusive(" << maxInclusive << ").";
        StreamUtilities::throw_exception(ss);
    }

    const T limitsDelta = maxInclusive - minInclusive;
    const T valueDelta = value - minInclusive;

    const auto result = valueDelta <= limitsDelta;
    return result;
}

//Implements the <= comparison assuming TPointer is a pointer.
template <class T,
    typename TComparer = std::less_equal<T>>
    struct less_equal_byPtr final
{
    using TheT = T;
    using TheComparer = TComparer;

    TComparer Comparer;

    bool operator()(const T* left, const T* right) const
    {
        const bool result =
            left == right
            || nullptr == left
            || (nullptr != right) && (Comparer(*left, *right));
        return result;
    }
};

template <class number>
number abs_diff(const number a, const number b)
{
    const number result = b < a
        ? a - b
        : b - a;
    return result;
}