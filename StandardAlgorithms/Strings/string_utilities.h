#pragma once
#include <algorithm>
#include <cassert>
#include <cctype> // tolower
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include "../Assert.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            template<class string_t, class int_t>
            bool is_palindrome(const string_t& s, const int_t size)
            {
                const auto half_size = size >> 1;
                assert(half_size >= 0);

                for (int_t i = 0; i < half_size; ++i)
                    if (s[i] != s[size - 1 - i])
                        return false;

                return true;
            }

            template<class pair_t, class int_t>
            void test_check_pair(const std::string& name, const pair_t& p, const int_t size)
            {
                if (p.second > 0)
                {
                    Assert::GreaterOrEqual(p.first, 0, name + ".f");
                    Assert::Greater(p.second, p.first, name + ".s");
                    Assert::GreaterOrEqual(size, static_cast<int_t>(p.second), name + ".s");
                }
                else
                    Assert::AreEqual(0, p.first, name + ".f");
            }

            template<class string_t, class string_t2>
            int find_count_max2(const string_t& text, const string_t2& pattern)
            {
                size_t pos = size_t() - size_t(1);
                auto count = 0;
                for (;;)
                {
                    pos = text.find(pattern, pos + 1u);
                    if (pos == std::string::npos)
                        return count;

                    if (++count == 2)
                        return 2;
                }
            }
        }

        template<class string_t>
        void ToLower(string_t& s)
        {
            for (auto& c : s)
                c = char(tolower(c));
        }
    }
}