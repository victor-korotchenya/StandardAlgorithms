#pragma once
#include <unordered_map>
#include <stack>
#include "../Utilities/StreamUtilities.h"

namespace Privet::Algorithms::Numbers
{
    //There is no check for overflow.
    template <typename int_t>
    class Fibonacci final
    {
        Fibonacci() = delete;

    public:
        //O(n) time.
        static void Slow(const int_t& n, int_t& result);

        //O(log(n)) time.
        template <typename Hasher = std::hash<int_t>,
            typename NumberCache = std::unordered_map<int_t, int_t, Hasher>>
            static void Fast(const int_t& n, NumberCache& numberCache, int_t& result);

    private:
        static void LessThanTwo(const int_t& n, int_t& result)
        {
            if (n < int_t(0))
            {
                std::ostringstream ss;
                ss << "Error: n(" << n << ") must be non-negative.";
                StreamUtilities::throw_exception(ss);
            }

            result = n;
        }
    };

    template <typename int_t >
    void Fibonacci< int_t >::Slow(const int_t& n, int_t& result)
    {
        if (n < int_t(2))
        {
            LessThanTwo(n, result);
            return;
        }

        int_t old;
        int_t n1 = int_t(0);

        result = int_t(1);
        for (int_t i = int_t(1); i < n; ++i)
        {
            old = result;
            result += n1;
            n1 = old;
        }
    }

    //f(n)   = f(n-1) + f(n-2);
    //f(n-1) = f(n-1) + 0;
    //
    //[f(n)]   = [1, 1] * [f(n-1)]
    //[f(n-1)] = [1, 0]   [f(n-2)]
    //
    //[f(n)]   = [1, 1]^2 * [f(n-2)]
    //[f(n-1)] = [1, 0]     [f(n-3)]
    //
    //  [f(n)]   = [1, 1]^(n-1) * [f(1)]
    //  [f(n-1)] = [1, 0]         [f(0)]
    //
    //  [f(n+1), f(n)] = [1, 1]^n
    //  [f(n), f(n-1)] = [1, 0]
    //
    //[f(n+b)]   = [1, 1]^(n+b-1) * [f(1)]
    //[f(n+b-1)] = [1, 0]           [f(0)]
    //
    //[f(n+b)]   = [1, 1]^(n) * [1, 1]^(b-1) * [f(1)]
    //[f(n+b-1)] = [1, 0]       [1, 0]         [f(0)]
    //
    //[f(n+b)]   = [f(n+1), f(n)] * [f(b)]
    //[f(n+b-1)] = [f(n), f(n-1)]   [f(b-1)]
    //
    //  f(n+b) = f(n+1)f(b) + f(n)f(b-1);
    template <typename int_t >
    template <typename Hasher, typename NumberCache >
    void Fibonacci< int_t >::Fast(
        const int_t& n,
        NumberCache& numberCache,
        int_t& result)
    {
        const auto findNumber = numberCache.find(n);
        if (numberCache.end() != findNumber)
        {
            result = findNumber->second;
            return;
        }

        if (n < int_t(2))
        {
            LessThanTwo(n, result);
            return;
        }

        std::stack< int_t > middleItems;

        const int_t zero = int_t(0);
        const int_t one = int_t(1);
        const int_t two = int_t(2);

        numberCache[zero] = zero;
        numberCache[one] = one;
        numberCache[two] = one;

        middleItems.push(n);

        int_t half, halfOther, temp;
        do
        {
        Lavel_Start_Over:
            const int_t number = middleItems.top();
            half = number / two;

            const auto findHalf = numberCache.find(half);
            const bool shallReStart = numberCache.end() == findHalf;
            if (shallReStart)
            {
                middleItems.push(half);
            }

            //TODO: p3. make it template.
            //const auto d = number  &1;
            const auto d = number.digitAt(0);

            const bool isEven = 0 == (d & 1);
            if (isEven)
            {
                halfOther = half - one;
            }
            else
            {
                halfOther = half + one;
            }

            const auto findOther = numberCache.find(halfOther);
            if (numberCache.end() == findOther)
            {
                middleItems.push(halfOther);
                goto Lavel_Start_Over;
            }

            if (shallReStart)
            {
                goto Lavel_Start_Over;
            }

            if (isEven)
            {//f(n+b) = f(n+1)f(b) + f(n)f(b-1);
             //f(2n) = (f(n+1) + f(n-1))* f(n) = (f(n) + f(n-1) + f(n-1))* f(n)
             //      = (2*f(n-1) + f(n))* f(n);
                temp = ((two * findOther->second) + findHalf->second)
                    * findHalf->second;
            }
            else
            {//f(2n+1) = f(n+1)f(n+1) + f(n)f(n);
                temp = (findOther->second * findOther->second)
                    + (findHalf->second * findHalf->second);
            }

            numberCache.insert(std::make_pair(number, temp));
            middleItems.pop();
        } while (!middleItems.empty());

        const auto findResult = numberCache.find(n);
        if (numberCache.end() == findResult)
        {
            std::ostringstream ss;
            ss << "Internal error: there must be value for n=" << n << ".";
            StreamUtilities::throw_exception(ss);
        }

        result = findResult->second;
    }
}