#pragma once
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Graphs/BinaryTrie.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array of n>=2 unsigned integers,
            //return the maximum XOR of 2 elements, and their distinct indexes.
            //O(n*log(max_value)) time.
            template <typename Number>
            Number ArrayMaximumXorOf2Elements(
                const std::vector<Number>& values)
            {
                static_assert(std::is_unsigned<Number>::value);

                const auto size = values.size();
                if (size < 2)
                {
                    std::ostringstream ss;
                    ss << "The size(" << size << ") must be at least 2.";
                    throw StreamUtilities::throw_exception(ss);
                }

                ::Privet::Algorithms::Trees::BinaryTrie<Number> tree;

                for (const auto& v : values)
                {
                    tree.add(v);
                }

                const auto result = tree.max_xor();
                return result;
            }

            //O(n*n) time.
            template <typename Number>
            Number ArrayMaximumXorOf2Elements_slow(
                const std::vector<Number>& values)
            {
                static_assert(std::is_unsigned<Number>::value,
                    "The Number type must be unsigned.");

                const auto size = values.size();
                if (size < 2)
                {
                    std::ostringstream ss;
                    ss << "The size(" << size << ") must be at least 2.";
                    throw StreamUtilities::throw_exception(ss);
                }

                Number result = 0;
                for (Number i = 0; i < size - 1; ++i)
                {
                    for (Number j = i + 1; j < size; ++j)
                    {
                        const auto t = static_cast<Number>(values[i] ^ values[j]);
                        if (result < t)
                        {
                            result = t;
                        }
                    }
                }

                return result;
            }
        }
    }
}