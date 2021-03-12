#pragma once
#include <queue>

template<typename T>
static void iota(std::queue<T>& q, const size_t size)
{
    for (size_t i{}; i < size; ++i)
        q.push(T(i));
}