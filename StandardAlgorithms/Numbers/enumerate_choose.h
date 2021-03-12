#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // todo(p2): co_return generator in C++23.
    // Given m=2, n=5, enumerate (5!/2!/3! = 10) all m size subsets in some order, and endlessly repeating:
    // {0, 1}, {0, 2}, {0, 3}, {0, 4},
    // {1, 2}, {1, 3}, {1, 4},
    // {2, 3}, {2, 4},
    // {3, 4}.
    // See also definitiv_operator_feed.
    template<std::integral int_t>
    struct enumerate_choose final
    {
        constexpr enumerate_choose(const int_t &m_smaller, const int_t &n_larger)
            : Mmm(require_positive(m_smaller, "m smaller"))
            , Nnn(n_larger)
            , Current(m_smaller)
            , Bits(n_larger, false)
        {
            require_less_equal(m_smaller, n_larger, "m vs n");

            for (int_t index{}; index < m_smaller; ++index)
            {
                Current[index] = index;
                Bits[index] = true;
            }
        }

        [[nodiscard]] constexpr auto current() const &noexcept -> const std::vector<int_t> &
        {
            return Current;
        }

        [[nodiscard]] constexpr auto next() -> bool
        {
            assert(int_t{} < Mmm && !(Nnn < Mmm) && static_cast<std::size_t>(Nnn) == Bits.size());

            auto has_next = std::prev_permutation(Bits.begin(), Bits.end());

            Current.clear();

            for (int_t index{}; index < Nnn; ++index)
            {
                if (Bits[index])
                {
                    Current.push_back(index);
                }
            }

            assert(int_t{} < Mmm && static_cast<std::size_t>(Mmm) == Current.size());

            return has_next;
        }

private:
        int_t Mmm;
        int_t Nnn;
        std::vector<int_t> Current;
        std::vector<bool> Bits;
    };

    // Deduction guide.
    template<std::integral int_t, std::integral int_t_2>
    enumerate_choose(int_t, int_t_2) -> enumerate_choose<std::common_type_t<int_t, int_t_2>>;
} // namespace Standard::Algorithms::Numbers
