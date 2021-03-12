#include <array>
#include "../Numbers/Arithmetic.h"
#include "../Utilities/ExceptionUtilities.h"
#include "../Numbers/Sort.h"
#include "suffix_array.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    // "abca" -> {0,1,2,0}.
    vector<int> compress_chars(const string& s)
    {
        const auto n = static_cast<int>(s.size());
        assert(n > 0);

        constexpr auto bits = 256;
        array<int, bits> a = { 0 };

        for (auto i = 0; i < n; ++i)
        {
            const auto pos = static_cast<signed char>(s[i]) + 128;
            a[pos] = 1;
        }

        for (auto i = 0, p = -1; i < bits; ++i)
            if (a[i])
                a[i] = ++p;

        vector<int> s2(n);
        for (auto i = 0; i < n; ++i)
        {
            const auto pos = static_cast<signed char>(s[i]) + 128;
            s2[i] = a[pos];
        }

        return s2;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            vector<int> suffix_array_slow(const string& s)
            {
                static_assert(1 == sizeof(char));
                const auto n = static_cast<int>(s.size());
                RequirePositive(n, "n");

                vector<pair<string, int>> pairs(n);
                for (auto i = 0; i < n; ++i)
                    pairs[i] = make_pair(s.substr(i), i);

                sort(pairs.begin(), pairs.end());

                vector<int> result(n);

                transform(pairs.begin(), pairs.end(), result.begin(), [](const pair<string, int>& p) -> int { return p.second; });

                return result;
            }

            vector<int> suffix_array(const string& s)
            {
                static_assert(1 == sizeof(char));
                const auto n = static_cast<int>(s.size());
                RequirePositive(n, "n");

                vector<int> result(n);
                if (n <= 1)
                    return result;

                const auto is_p2 = IsPowerOfTwo(n);
                const auto hi_bit = static_cast<int>(MostSignificantBit(n)),
                    iter_max = hi_bit - is_p2;
                assert(iter_max >= 0);

                vector<vector<pair<int, int>>> labels(n);
                auto s2 = compress_chars(s);

                for (auto iter = 0, w = 1; iter <= iter_max; ++iter, w <<= 1)
                {
                    {
                        auto i = 0;

                        do labels[s2[i]].emplace_back(s2[static_cast<int64_t>(i) + w], i);
                        while (++i + w < n);

                        do labels[s2[i]].emplace_back(-1, i);
                        while (++i < n);
                    }

                    for (auto i = 0, p = -1; i < n; ++i)
                    {
                        auto& lab = labels[i];
                        if (lab.empty())
                            continue;

                        // To have O(n*log(n)) time, the counting/bucket sort must be used.
                        // sort(lab.begin(), lab.end()); // This is slow.
                        sort(lab.begin(), lab.end());

                        const auto size = static_cast<int>(lab.size());
                        for (auto j = 0; j < size; ++j)
                        {
                            const auto& v = lab[j];
                            if (!j || lab[j - 1ll].first != lab[j].first)
                                ++p;

                            s2[v.second] = p;
                        }

                        lab.clear();
                    }
                }

                for (auto i = 0; i < n; ++i)
                    result[s2[i]] = i;

                return result;
            }
        }
    }
}