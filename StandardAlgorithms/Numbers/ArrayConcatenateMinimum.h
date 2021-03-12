#pragma once

#include <string>
#include <ostream>
#include <vector>
#include <algorithm>
#include "Arithmetic.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an unsigned array {1, 123, 12 },
            // it must return a string "112123",
            // having minimum value after concatenating all array items.
            class ArrayConcatenateMinimum final
            {
                ArrayConcatenateMinimum() = delete;

            public:

                template <typename Number>
                static void Calc(
                    const std::vector<Number>& data,
                    std::string& result);

                //Whether a+b is not greater than b+a.
                static bool SumPredicate(const std::string& a, const std::string& b);
            };

            template <typename Number>
            void ArrayConcatenateMinimum::Calc(
                const std::vector<Number>& data,
                std::string& result)
            {
#ifdef _DEBUG
                AssertUnsigned<Number>("ArrayConcatenateMinimum.Calc");
#endif
                const auto size = data.size();
                if (0 == size)
                {
                    result.clear();
                    return;
                }

                std::vector<std::string> strings;
                strings.resize(size);

                for (size_t i = 0; i < size; ++i)
                {
                    strings[i] = std::to_string(data[i]);
                }

                std::sort(strings.begin(), strings.end(), SumPredicate);

                std::ostringstream ss;
                for (size_t i = 0; i < size; ++i)
                {
                    ss << strings[i];
                }

                result = ss.str();
            }

            inline bool ArrayConcatenateMinimum::SumPredicate(
                const std::string& a, const std::string& b)
            {
                const auto s1 = a + b;
                const auto s2 = b + a;
                const auto result = s1 <= s2;
                return result;
            }
        }
    }
}