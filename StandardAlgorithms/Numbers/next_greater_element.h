#pragma once

#include <stack>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // For each array item, find the next greater, or -1 if none.
            template<class t>
            void next_greater_element_slow(const std::vector<t>& ar,
                std::vector<int>& result)
            {
                result.resize(ar.size());
                if (ar.empty())
                    return;

                const auto n = static_cast<int>(ar.size());
                for (auto i = 0; i < n; ++i)
                {
                    auto ind = i;
                    for (auto j = i + 1; j < n; ++j)
                        if (ar[i] < ar[j])
                        {
                            ind = j;
                            break;
                        }

                    result[i] = i == ind ? -1 : ind;
                }
            }

            template<class t>
            void next_greater_element(const std::vector<t>& ar,
                std::vector<int>& result)
            {
                result.resize(ar.size());
                if (ar.empty())
                    return;

                const auto n = static_cast<int>(ar.size());
                std::stack<int> st;
                for (auto i = 0; i < n; ++i)
                {
                    while (st.size() && ar[st.top()] < ar[i])
                    {
                        const auto& top = st.top();
                        result[top] = i;
                        st.pop();
                    }

                    st.push(i);
                }

                while (st.size())
                {
                    const auto& top = st.top();
                    result[top] = -1;
                    st.pop();
                }
            }
        }
    }
}