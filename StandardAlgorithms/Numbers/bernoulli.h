#pragma once
#include<cassert>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // todo(p2): Bernoulli add comments.
    template<class fraction_t, class int_t>
    [[nodiscard]] constexpr auto bernoulli(std::vector<fraction_t> &temp, const int_t &size) -> fraction_t
    {
        assert(!(size < int_t{}));

        temp.clear();
        temp.reserve(size + 1LL);

        constexpr int_t num_zero{};

        for (auto index = num_zero; index <= size; ++index)
        {
            temp.emplace_back(1, index + 1);

            for (auto ind_2 = index; num_zero < ind_2; --ind_2)
            {
                const auto &next = temp[ind_2];
                auto &pre = temp[ind_2 - 1];

                pre = static_cast<fraction_t>((pre - next) * static_cast<fraction_t>(ind_2));
            }
        }

        assert(!temp.empty());

        const auto &front = temp.at(0);
        auto res = size == 1 ? -front : front;
        return res;
    }
} // namespace Standard::Algorithms::Numbers
