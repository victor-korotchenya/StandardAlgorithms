#pragma once
#include<cassert>
#include<cstddef>
#include<stdexcept>
#include<string>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    inline constexpr void enum_partits_check_size(const std::size_t size)
    {
        constexpr auto max_size = 30U;

        if (max_size < size) [[unlikely]]
        {
            auto err = "Too large elements size " + std::to_string(size);

            throw std::runtime_error(err);
        }
    }

    template<class item_t>
    constexpr void enum_partits(std::vector<item_t> &elements, std::vector<std::vector<std::vector<item_t>>> &result)
    {
        if (elements.empty())
        {
            result.push_back({});

            return;
        }

        auto back = elements.back();
        elements.pop_back();

        std::vector<std::vector<std::vector<item_t>>> result2;
        enum_partits(elements, result2);

        assert(!result2.empty());

        for (auto &temp2 : result2)
        {
            temp2.push_back({ back });
            result.push_back(temp2);
            temp2.pop_back();

            for (std::int32_t index{}; index < static_cast<std::int32_t>(temp2.size()); ++index)
            {
                auto &temp3 = temp2[index];

                temp3.push_back(back);
                result.push_back(temp2);
                temp3.pop_back();
            }
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // A partition places all the items into disjoint non-empty subsets.
    // E.g. a set {a, b, c} has 5 partitions: {a, b, c}, {ab, c}, {ac, b}, {a, bc}, {abc}.
    // Iterative, quick result, less memory.
    template<class item_t, class lambda_t>
    constexpr void enumerate_partitions_by_one(const std::vector<item_t> &elements, lambda_t lambda)
    {
        if (elements.empty())
        {
            return;
        }

        Inner::enum_partits_check_size(elements.size());

        const auto size = static_cast<std::int32_t>(elements.size());

        std::vector<std::vector<item_t>> lists(1, elements);
        std::vector<std::int32_t> indexes(size);

        for (;;)
        {
            lambda(lists);

            auto index = size - 1;
            std::int32_t ind_2{};

            for (;; --index)
            {
                if (index <= 0)
                {
                    return;
                }

                ind_2 = indexes[index];

                auto &lis = lists[ind_2];
                assert(!lis.empty());

                lis.pop_back();

                if (!lis.empty())
                {
                    break;
                }

                lists.erase(lists.begin() + ind_2);
            }

            if (lists.size() <= static_cast<std::size_t>(++ind_2))
            {
                lists.push_back({});
            }

            do
            {
                indexes[index] = ind_2;

                lists[ind_2].emplace_back(elements[index]);

                ind_2 = 0;
            } while (++index < size);
        }
    }

    // A large std::vector might be unnecessarily created.
    template<class item_t, class lambda_t>
    constexpr void enumerate_partitions_slow(std::vector<item_t> &elements, lambda_t lambda)
    {
        if (elements.empty())
        {
            return;
        }

        Inner::enum_partits_check_size(elements.size());

        std::vector<std::vector<std::vector<item_t>>> temp;
        Inner::enum_partits(elements, temp);

        for (const auto &item : temp)
        {
            lambda(item);
        }
    }
} // namespace Standard::Algorithms::Numbers
