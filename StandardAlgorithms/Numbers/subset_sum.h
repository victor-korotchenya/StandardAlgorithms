#pragma once
// "subset_sum.h"
#include"../Utilities/check_size.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::integral int_t, std::integral long_int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto check_input_max_sum(const std::vector<int_t> &numbers, const long_int_t &sum_limit)
        -> bool
    {
        {
            constexpr long_int_t one{ 1 };
            constexpr long_int_t maxi = std::numeric_limits<long_int_t>::max() - one - one;

            static_assert(one < maxi);

            require_between(one, sum_limit, maxi, "sum limit");
        }

        if (numbers.empty())
        {
            return false;
        }

        auto has = false;

        for (const auto &number : numbers)
        {
            require_positive(number, "number");

            if (const auto large = sum_limit < number; !large)
            {
                has = true;
            }
        }

        return has;
    }

    template<std::floating_point floating_t, std::integral long_int_t>
    struct merge_sum_rounder final
    {
        constexpr merge_sum_rounder(const long_int_t &sum_limit, const floating_t &multiply_coefficient)
            : Sum_limit(require_positive(sum_limit, "Sum limit"))
            , Multiply_coefficient(require_greater(multiply_coefficient, floating_t{ 1 }, "Multiply coefficient"))
        {
        }

        [[nodiscard]] constexpr auto max_sum() const &noexcept -> const long_int_t &
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(1U < Computed_sums.size() && long_int_t{} == Computed_sums[0]);
                assert(std::is_sorted(Computed_sums.begin(), Computed_sums.end()));
                assert(!(Max_sum < Computed_sums.back()));
            }

            return Max_sum;
        }

        constexpr void add(const long_int_t &number)
        {
            assert(long_int_t{} < number);

            add_without_rounding(number);
            round_off();
        }

private:
        constexpr void add_without_rounding(const long_int_t &number)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(!Computed_sums.empty());
                assert(std::is_sorted(Computed_sums.begin(), Computed_sums.end()));
            }

            Tempos.resize(Computed_sums.size());
            Virtuosos.clear();

            for (std::size_t index{}; index < Computed_sums.size(); ++index)
            {
                const auto &prick = Computed_sums[index];
                Tempos[index] = prick;

                const auto cand = static_cast<long_int_t>(prick + number);

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(!(prick < long_int_t{}) && prick < cand); // todo(p4): overflow check.
                }

                if (!(Sum_limit < cand)) // todo(p4): Maybe it is faster to std::copy to Tempos, and then
                {// break when (Sum_limit < cand) because of the sorted data?
                    Virtuosos.push_back(cand);
                    Max_sum = std::max(Max_sum, cand);
                }
            }
        }

        constexpr void round_off()
        {
            assert(!Computed_sums.empty());

            if (Virtuosos.empty())
            {
                return;
            }

            Computed_sums.assign(1, long_int_t{});
            Prev_rounded = {};

            std::size_t index{};
            std::size_t index_2{};
            merge_boss(index, index_2);

            merge_slave(index, Tempos);
            merge_slave(index_2, Virtuosos);
        }

        constexpr void merge_boss(std::size_t &index, std::size_t &index_2)
        {
            assert(index <= Tempos.size() && index_2 <= Virtuosos.size() && !Computed_sums.empty());

            while (index < Tempos.size() && index_2 < Virtuosos.size())
            {
                const auto &item = Tempos[index];
                const auto &item_2 = Virtuosos[index_2];

                const auto is_tut = item_2 < item;
                const auto &smallest = is_tut ? item_2 : item;
                ++(is_tut ? index_2 : index);

                try_insert(smallest);
            }
        }

        constexpr void merge_slave(std::size_t &index, const std::vector<long_int_t> &items)
        {
            assert(index <= items.size() && !Computed_sums.empty());

            while (index < items.size())
            {
                const auto &item = items[index];
                try_insert(item);
                ++index;
            }
        }

        constexpr void try_insert(const long_int_t &next_item)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(!Computed_sums.empty());
                assert(floating_t{ 1 } < Multiply_coefficient);
                assert(!(Prev_rounded < floating_t{}));
                assert(!(next_item < long_int_t{}));
            }

            const auto &back1 = Computed_sums.back();

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(!(next_item < back1));
            }

            const auto cand = static_cast<floating_t>(next_item);

            if (!(Prev_rounded < cand))
            {
                return;
            }

            Computed_sums.push_back(next_item);

            auto mini = static_cast<floating_t>(cand * Multiply_coefficient);
            assert(Prev_rounded < mini);

            Prev_rounded = mini;
        }

        const long_int_t Sum_limit;
        const floating_t Multiply_coefficient;
        floating_t Prev_rounded{};

        std::vector<long_int_t> Computed_sums{ long_int_t{} };
        std::vector<long_int_t> Tempos{};
        std::vector<long_int_t> Virtuosos{};
        long_int_t Max_sum{};
    };
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // It is a well-known NP-complete problem.
    // Given n positive numbers, choose a subset with a maximum sum, not exceeding a limit.
    // If all the numbers are greater than the sum limit, return 0.

    inline constexpr auto subset_sum_approx_ratio = 2;

    // Naive, but fast 0.5 approximation.
    // Naive time O(sort(n)).
    template<std::integral int_t, std::integral long_int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto maximum_subset_sum_approximation_half(const std::vector<int_t> &numbers,
        const long_int_t &sum_limit, std::vector<std::size_t> &chosen_indexes) -> long_int_t
    {
        chosen_indexes.clear();

        if (!Inner::check_input_max_sum<int_t, long_int_t>(numbers, sum_limit))
        {
            return {};
        }

        long_int_t max_sum{};

        auto indexes = Standard::Algorithms::Utilities::iota_vector<std::size_t>(numbers.size());

        {
            const auto descending_order = [&numbers](const auto &pos, const auto &pos_2) -> bool
            {
                const auto &odin = numbers[pos];
                const auto &diva = numbers[pos_2];
                return diva < odin;
            };

            std::sort(indexes.begin(), indexes.end(), descending_order);
        }

        for (const auto &index : indexes)
        {
            const auto &numb = numbers.at(index);

            auto cand = static_cast<long_int_t>(max_sum + static_cast<long_int_t>(numb));

            if (sum_limit < cand || !(max_sum < cand))
            {
                continue;
            }

            max_sum = cand;
            chosen_indexes.push_back(index);
        }

        assert(long_int_t{} < max_sum);
        return max_sum;
    }

    template<std::floating_point floating_t>
    [[nodiscard]] constexpr auto is_valid_ptas_epsilon(const floating_t &epsilon) noexcept -> bool
    {
        constexpr floating_t zero{};
        constexpr floating_t one{ 1 };
        static_assert(zero < one);

        auto valid = zero < epsilon && epsilon < one && one < one + epsilon;
        return valid;
    }

    // (1.0 + epsilon) approximation.
    // Due to rounding, it might be costly to restore the chosen numbers
    // e.g. by keeping all the added/removed values for each number.
    template<std::floating_point floating_t, std::integral int_t, std::integral long_int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto maximum_subset_sum_fptas_approximation(
        const std::vector<int_t> &numbers, const long_int_t &sum_limit, const floating_t &epsilon) -> long_int_t
    {
        if (!is_valid_ptas_epsilon(epsilon))
        {
            throw std::invalid_argument("Bad epsilon");
        }

        if (!Inner::check_input_max_sum<int_t, long_int_t>(numbers, sum_limit))
        {
            return {};
        }

        const auto size = numbers.size();
        const auto delta = static_cast<floating_t>(epsilon / floating_t{ 2 } / static_cast<floating_t>(size));

        if (!is_valid_ptas_epsilon(delta))
        {
            throw std::invalid_argument("Bad delta");
        }

        const auto coeff = static_cast<floating_t>(delta + floating_t{ 1 });
        assert(floating_t{ 1 } < coeff);

        Inner::merge_sum_rounder<floating_t, long_int_t> rounder(sum_limit, coeff);

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &number = numbers[index];
            if (!(sum_limit < number))
            {
                rounder.add(static_cast<long_int_t>(number));
            }
        }

        auto max_sum = require_positive(rounder.max_sum(), "maximum_subset_sum_fptas_approximation max sum");
        require_less_equal(max_sum, sum_limit, "maximum_subset_sum_fptas_approximation max_sum vs sum_limit");

        return max_sum;
    }

    // Pseudo-polynomial time, space O(n*sum_limit).
    // todo(p4): overflow check.
    // todo(p4): take all when (all items sum <= limit).
    template<std::integral int_t, std::integral long_int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    [[nodiscard]] constexpr auto maximum_subset_sum(const std::vector<int_t> &numbers, const long_int_t &sum_limit,
        std::vector<std::size_t> &chosen_indexes) -> long_int_t
    {
        chosen_indexes.clear();

        if (!Inner::check_input_max_sum<int_t, long_int_t>(numbers, sum_limit))
        {
            return {};
        }

        constexpr long_int_t zero{};
        constexpr long_int_t one{ 1 };

        const auto numbers_size = numbers.size();

        // {first i numbers, sum} -> is the sum reachable.
        std::vector<std::vector<bool>> reachables(numbers_size + 1LLU, std::vector<bool>(sum_limit + one, false));

        constexpr auto can_always_make_0_sum = true;
        reachables[0][0] = can_always_make_0_sum;

        for (std::size_t index{}; index < numbers_size; ++index)
        {
            const auto &numb = numbers[index];
            const auto &prevs = reachables[index];

            auto &currs = reachables[index + 1U];
            currs[0] = can_always_make_0_sum;

            for (auto sum = one; sum <= sum_limit; ++sum)
            {
                currs[sum] = prevs[sum] || (numb <= sum && prevs[sum - numb]);
            }

            // todo(p3): Break early once currs[sum_limit] becomes true.
        }

        assert(reachables[numbers_size][0]);

        auto rest = sum_limit;

        while (!reachables[numbers_size][rest])
        {
            --rest;
        }

        const auto max_sum = require_positive(rest, "maximum_subset_sum must return a positive value.");

        for (auto last_index = numbers_size;;)
        {// Make a greedy choice.
            if (const auto &numb = numbers[last_index - 1U]; numb <= rest && reachables[last_index][rest - numb])
            {
                chosen_indexes.push_back(last_index - 1U);
                rest -= numb;

                if (zero == rest)
                {
                    return max_sum;
                }
            }

            if (0U == last_index--) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The remaining sum " << rest << " must be zero. Result " << max_sum << ".";

                throw_exception(str);
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
