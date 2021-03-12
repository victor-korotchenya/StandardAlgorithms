#include <cassert>
#include "z_alg.h"
using namespace std;

namespace
{
    void go_right(const string& s, const int n, const int i, int& left, int& right, vector<int>& z)
    {
        assert(i < n);
        left = i;
        while (right < n && s[right - left] == s[right])
            ++right;
        z[i] = right - left;
        --right;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            vector<int> z_array_slow(const string& s)
            {
                const auto n = static_cast<int>(s.size());
                vector<int> z(n);
                for (auto i = 0; i < n; ++i)
                {
                    auto v = 0;
                    while (i + v < n && s[v] == s[i + v])
                        ++v;
                    z[i] = v;
                }
                return z;
            }

            vector<int> z_array(const string& s)
            {
                const auto n = static_cast<int>(s.size());
                vector<int> z(n);
                z[0] = n;
                auto left = 0, right = 0;
                for (auto i = 1; i < n; ++i)
                {
                    if (right < i)
                    {
                        right = i;
                        go_right(s, n, i, left, right, z);
                    }
                    else
                    {
                        if (z[i - left] < right - i + 1)
                            z[i] = z[i - left];
                        else
                            go_right(s, n, i, left, right, z);
                    }
                }
                return z;
            }

            vector<int> string_borders_slow(const string& s)
            {
                const auto n = static_cast<int>(s.size());
                vector<int> result;
                for (auto i = 1; i < n; ++i)
                {
                    auto ok = 1;
                    for (auto j = 0; j < i && ok; ++j)
                        ok = s[j] == s[n - i + j];
                    if (ok)
                        result.push_back(i);
                }
                return result;
            }

            vector<int> string_borders(const string& s)
            {
                const auto n = static_cast<int>(s.size());
                const auto z = z_array(s);
                assert(z.size() == s.size());

                vector<int> result;
                for (auto i = 1; i < n; ++i)
                {
                    if (i == z[n - i])
                        result.push_back(i);
                }
                return result;
            }
        }
    }
}