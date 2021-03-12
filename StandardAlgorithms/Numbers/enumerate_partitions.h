#pragma once

#include <vector>

namespace
{
    template<class t>
    void enum_partits(std::vector<t>& elements,
        std::vector<std::vector<std::vector<t>>>& result)
    {
        if (elements.empty())
        {
            result.push_back({});
            return;
        }

        auto back = elements.back();
        elements.pop_back();

        std::vector<std::vector<std::vector<t>>> result2;
        enum_partits(elements, result2);

        for (auto& r2 : result2)
        {
            r2.push_back({ back });
            result.push_back(r2);
            r2.pop_back();

            for (auto i = 0; i < static_cast<int>(r2.size()); ++i)
            {
                auto& s = r2[i];
                s.push_back(back);
                result.push_back(r2);
                s.pop_back();
            }
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //A large vector might be unnecessarily created.
            template<class t, class lam_t>
            void enumerate_partitions_all(std::vector<t>& elements, lam_t la)
            {
                if (elements.empty())
                    return;

                std::vector<std::vector<std::vector<t>>> temp;
                enum_partits(elements, temp);
                for (const auto& s : temp)
                    la(s);
            }

            // Iterative, quick result, less memory.
            template<class t, class lam_t>
            void enumerate_partitions_by_one(const std::vector<t>& elements, lam_t la)
            {
                if (elements.empty())
                    return;

                const auto size = static_cast<int>(elements.size());
                std::vector<int> indexes(size);
                std::vector<std::vector<t>>lists(1, elements);
                for (;;)
                {
                    la(lists);

                    auto i = size - 1;
                    int j;
                    for (;; --i)
                    {
                        if (i <= 0)
                            return;

                        j = indexes[i];
                        auto& lis = lists[j];
                        lis.pop_back();
                        if (lis.size() > 0)
                            break;

                        lists.erase(lists.begin() + j);
                    }

                    if (++j >= lists.size())
                        lists.push_back({});

                    do
                    {
                        indexes[i] = j;
                        lists[j].emplace_back(elements[i]);
                        j = 0;
                    } while (++i < size);
                }
            }
        }
    }
}