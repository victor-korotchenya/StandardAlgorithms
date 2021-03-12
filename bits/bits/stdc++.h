#pragma once

//#include<bits/stdc++.h>

// print a double precision float.
// printf("%0.9f\n", answer);
//

#define _CRT_SECURE_NO_WARNINGS
#define D_SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

//gcd
#ifndef __gcd
template<typename t>
t __gcd(t a, t b) {
    while (b) {
        auto r = a % b;
        a = b;
        b = r;
    }
    return a;
}
#endif

#ifndef _GLIBCXX_NO_ASSERT
#include <cassert>
#endif

#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cwctype>

#if __cplusplus >= 201103L
#include <complex>
#include <cfenv>
#include <cinttypes>

// Now obsolete:
//#include <cstdalign>
//#include <cstdbool>
//#include <ctgmath>

#include <cstdint>
#include <cuchar>
#endif

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#if __cplusplus >= 201103L
#include <array>
#include <atomic>
#include <chrono>
#include <codecvt>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#endif

#if __cplusplus >= 201402L
#include <shared_mutex>
#endif

#if __cplusplus >= 201703L
#include <any>
#include <charconv>
// #include <execution>
#include <filesystem>
#include <optional>
#include <memory_resource>
#include <string_view>
#include <variant>
#endif

#if __cplusplus > 201703L
#include <bit>
#include <compare>
#include <concepts>
#include <numbers>
#include <ranges>
#include <span>
#include <stop_token>
// #include <syncstream>
#include <version>
#endif

#include <intrin.h>
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanReverse64)
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanForward64)

#ifndef __builtin_clzll
//https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
//Returns the number of leading 0-bits in x, starting at the most significant bit position.
//If x is 0, the result is undefined.
//a=1, ret=63
//2, 62
//3, 62
//4, 61
//7, 61
//8, 60
inline int __builtin_clzll(uint64_t a)
{
    assert(a);
    unsigned long leading_zero = 0;

    //if (_BitScanReverse(&leading_zero, a))
    //_BitScanReverse64(unsigned long * _Index, unsigned __int64 _Mask))
    _BitScanReverse64(&leading_zero, a);
    {
        return 63 - leading_zero;
    }
    //      return 32;
}
//a=1, ret=31
//2, 30
//3, 30
//4, 29
//7, 29
//8, 28
inline int __builtin_clz(unsigned int a)
{
    //(unsigned char _BitScanReverse(unsigned long * _Index, unsigned long _Mask))
    assert(a);
    unsigned long leading_zero = 0;
    _BitScanReverse(&leading_zero, a);
    return 31 - leading_zero;
}
#endif

#ifndef __lg
// 1 => 0, 2 => 1, 3 => 1
// 4 => 2, 7 => 2, 8 => 3, 16 => 4.
inline int __lg(uint64_t __n)
{
    auto result = sizeof(uint64_t) * 8 - 1 - __builtin_clzll(__n);
    return static_cast<int>(result);
}
#endif

#ifndef __builtin_popcount
inline int __builtin_popcount(unsigned __n)
{
    auto result = __popcnt(__n);
    return static_cast<int>(result);
}
inline int __builtin_popcountll(uint64_t __n)
{
    auto result = __popcnt64(__n);
    return static_cast<int>(result);
}
#endif

#ifndef __builtin_ctz
// 1,3,5,7 => 0
// 2,6 => 1
// 4,12 => 2
// 8 => 3
inline int __builtin_ctz(unsigned __n)
{
    unsigned long result = 0;
    _BitScanForward(&result, __n);
    return static_cast<int>(result);
}
inline int __builtin_ctzll(uint64_t __n)
{
    unsigned long result = 0;
    _BitScanForward64(&result, __n);
    return static_cast<int>(result);
}
#endif

// using int128_t = std::int64_t;