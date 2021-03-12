#pragma once

#include <cassert>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Report n-th order differences considering a[-1] = 0.
            //   a 1 3 6 10 15 21 28
            //dif0 1 2 3  4  5  6  7
            //dif1   1 1  1  1  1  1 // stop when diff becomes 0.
            template<class call_t, class value_t>
            void report_diffs(call_t rep, std::vector<value_t>& diffs, value_t curr)
            {
                assert(diffs.size());
                for (auto i = 0;;)
                {
                    const auto di = curr - diffs[i];
                    if (!di)
                    {
                        diffs.resize(i + 1);
                        break;
                    }

                    rep(di);
                    diffs[i] = curr;
                    curr = di;
                    if (++i == static_cast<int>(diffs.size()))
                    {
                        diffs.push_back(di);
                        break;
                    }
                }
            }
        }
    }
}