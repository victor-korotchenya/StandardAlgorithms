#pragma once
#include"../Utilities/throw_exception.h"
#include<stack>
#include<unordered_map>

namespace Standard::Algorithms::Numbers
{
    // There is no check for an overflow.
    template<std::integral int_t>
    struct fibonacci final
    {
        fibonacci() = delete;

        // f(0) = 0
        // f(1) = 1
        // f(n) = f(n-1) + f(n-2). BTW, it could be used for negative numbers too.
        // f(2) = 1
        // f(3) = 2
        // f(4) = 3
        // f(5) = 5
        // f(6) = 8
        // f(7) = 13
        //
        // Let's use a two-by-two matrix representation.
        // f(n) = f(n-1) + f(n-2)
        // f(n-1) = f(n-1) + 0
        //
        // [f(n)] = [1, 1] * [f(n-1)]
        // [f(n-1)] = [1, 0] * [f(n-2)]
        //
        // [f(n)] = [1, 1]^2 * [f(n-2)]
        // [f(n-1)] = [1, 0]^2 * [f(n-3)]
        //
        // [f(n)] = [1, 1]^(n-1) * [f(1)]
        // [f(n-1)] = [1, 0]^(n-1) * [f(0)]
        //
        // [f(n+1), f(n)] = [1, 1]^n
        // [f(n), f(n-1)] = [1, 0]^n
        //
        // [f(n+b)] = [1, 1]^(n+b-1) * [f(1)]
        // [f(n+b-1)] = [1, 0]^(n+b-1) * [f(0)]
        //
        // [f(n+b)] = [1, 1]^(n) * [1, 1]^(b-1) * [f(1)]
        // [f(n+b-1)] = [1, 0]^(n) * [1, 0]^(b-1) * [f(0)]
        //
        // [f(n+b)] = [f(n+1), f(n)] * [f(b)]
        // [f(n+b-1)] = [f(n), f(n-1)] * [f(b-1)]
        //
        // Final formula:
        // f(n+b) = f(n+1) * f(b) + f(n) * f(b-1)
        //
        // Time O(log(n)).
        template<class hasher_t = std::hash<int_t>, class number_cache_t = std::unordered_map<int_t, int_t, hasher_t>>
        static constexpr void fast(const int_t &size, number_cache_t &number_cache, int_t &result)
        {
            if (const auto iter = number_cache.find(size); number_cache.end() != iter) [[likely]]
            {
                result = iter->second;

                return;
            }

            if (size < two) [[unlikely]]
            {
                less_then_two(size, result);

                initialize(number_cache);

                return;
            }

            initialize(number_cache);

            std::stack<int_t> work;
            work.push(size);

            do
            {
                auto top = work.top();
                const auto half = static_cast<int_t>(top / two);
                const auto find_half = number_cache.find(half);
                const auto shall_re_start = number_cache.end() == find_half;

                if (shall_re_start)
                {
                    work.push(half);
                }

                const auto is_even = zero == (top & one);
                auto half_other = static_cast<int_t>(is_even ? (half - one) : (half + one));

                const auto find_other = number_cache.find(half_other);
                if (number_cache.end() == find_other)
                {
                    work.push(std::move(half_other));
                    continue;
                }

                if (shall_re_start)
                {
                    continue;
                }

                auto temp = static_cast<int_t>(is_even ? // f(n+b) = f(n+1) * f(b) + f(n) * f(b-1)
                                                         // f(n+n) = (f(n+1) + f(n-1)) * f(n) = (f(n) + f(n-1) + f(n-1))
                                                         // * f(n) = f(n+n) = (2*f(n-1) + f(n))* f(n);
                        ((find_other->second * two) + find_half->second) * find_half->second
                                                       : // f(n+b) = f(n+1) * f(b) + f(n) * f(b-1)
                        // f(n+ 1+n) = f(n+1) * f(1+n) + f(n) * f(1+n-1) =
                        // f(2*n+1) = f(n+1) * f(n+1) + f(n) * f(n)
                        (find_other->second * find_other->second) + (find_half->second * find_half->second));

                number_cache.emplace(std::move(top), std::move(temp));

                work.pop();
            } while (!work.empty());

            if (const auto find_result = number_cache.find(size); number_cache.end() != find_result)
            {
                result = find_result->second;

                return;
            }

            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Internal error: there must be a value for size " << size << ".";

                throw_exception(str);
            }
        }

        // Slow time O(n).
        static constexpr void slow(const int_t &size, int_t &result)
        {
            if (size < two) [[unlikely]]
            {
                less_then_two(size, result);

                return;
            }

            int_t prev{};

            result = one;

            for (auto index = one; index < size; ++index)
            {
                auto old = result;
                result += prev;
                prev = std::move(old);
            }
        }

private:
        template<class number_cache_t>
        static constexpr void initialize(number_cache_t &number_cache)
        {
            number_cache[zero] = zero;
            number_cache[one] = one;
            number_cache[two] = one;
        }

        static constexpr void less_then_two(const int_t &size, int_t &result)
        {
            if constexpr (std::is_signed_v<int_t>)
            {
                if (size < zero)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The size " << size << " must be non-negative.";

                    throw_exception(str);
                }
            }

            result = zero < size ? one : zero;
        }

        static constexpr int_t zero{};
        static constexpr int_t one = 1;
        static constexpr int_t two = 2;
    };
} // namespace Standard::Algorithms::Numbers
