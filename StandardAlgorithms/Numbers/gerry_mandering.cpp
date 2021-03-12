#include"gerry_mandering.h"
#include"../Utilities/print_utilities.h"
#include"arithmetic.h"
#include<cstdlib> // std::exit
#include<iostream>
#include<optional>
#include<tuple>

namespace
{
    using precinct = Standard::Algorithms::Numbers::precinct;

    constexpr auto shall_print_precincts = false;
    constexpr std::size_t one = 1U;

    constexpr void check_precincts_size(const std::size_t size)
    {
        const auto is_even = 0U == (size & one);

        if (is_even && one < size) [[likely]]
        {
            return;
        }

        auto err = "The number of precincts " + ::Standard::Algorithms::Utilities::zu_string(size) +
            " must be even, and 2 or more.";

        throw std::runtime_error(err);
    }

    [[nodiscard]] auto pre_process_precincts(const std::vector<precinct> &precincts) noexcept(false)
        // votersPerPrecinct, totalVotersA, totalVotersB, aWinsPrecincts
        -> std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>
    {
        const auto size = precincts.size();

        check_precincts_size(size);

        const auto voters_per_precinct = Standard::Algorithms::Numbers::sum(precincts[0]);
        Standard::Algorithms::require_positive(voters_per_precinct, "total voters a precinct");

        std::size_t total_voters_a{};
        std::size_t total_voters_b{};
        std::size_t a_wins_precincts{};

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &precinct = precincts[index];

            if (const auto sum = Standard::Algorithms::Numbers::sum(precinct); voters_per_precinct != sum) [[unlikely]]
            {
                auto err = "The number of voters " + ::Standard::Algorithms::Utilities::zu_string(sum) +
                    " per precinct " + ::Standard::Algorithms::Utilities::zu_string(index) + " must be " +
                    ::Standard::Algorithms::Utilities::zu_string(voters_per_precinct) + ", the same for all of them.";

                throw std::runtime_error(err);
            }

            total_voters_a = Standard::Algorithms::Numbers::add_unsigned(total_voters_a, precinct.a_voters);

            total_voters_b = Standard::Algorithms::Numbers::add_unsigned(total_voters_b, precinct.b_voters);

            a_wins_precincts += precinct.b_voters < precinct.a_voters ? 1U : 0U;
        }

        assert(a_wins_precincts <= size);

        return { voters_per_precinct, total_voters_a, total_voters_b, a_wins_precincts };
    }

    constexpr void calc_matrix(const std::vector<precinct> &precincts,
        [[maybe_unused]] const std::size_t voters_max_exclusive,
        std::vector<std::vector<std::vector<std::vector<bool>>>> &possibles) noexcept(!shall_print_precincts)
    {
        const auto size = precincts.size();
        const auto half_size = size >> 1U;
        assert(0U < half_size);

        possibles[0][0][0][0] = true;

        [[maybe_unused]] std::size_t row{};

        if constexpr (shall_print_precincts)
        {
            std::cout << " row | initial_precincts | first_distr_sel_precincts | first_distr_a_voters | "
                         "second_distr_a_voters | case1 | case2 | from_row\n"
                      << " r0 | 0 | 0 | 0 | 0\n";
        }

        for (std::size_t sum_a{},
             // Consider precincts from 0 to that inclusive.
             initial_precincts{};
             initial_precincts < size; ++initial_precincts)
        {
            const auto &a_voters = precincts[initial_precincts].a_voters;

            assert(sum_a <= sum_a + a_voters); // No overflow.
            assert(sum_a + a_voters < voters_max_exclusive); // Party B has min 1 voter overall.

            sum_a += a_voters;

            const auto &prevs = possibles[initial_precincts];
            auto &curs = possibles[initial_precincts + one];

            const auto max_selected_precincts = std::min(half_size, initial_precincts + one);

            for ( // # of precincts, selected for district 1.
                auto first_distr_sel_precincts = one; first_distr_sel_precincts <= max_selected_precincts;
                ++first_distr_sel_precincts)
            {
                for ( // There are at least that many voters for A in district 1.
                    std::size_t first_distr_a_voters{}; first_distr_a_voters <= sum_a; ++first_distr_a_voters)
                {
                    // The voters of A in both districts cannot exceed sumA.
                    const auto sec_max = sum_a - first_distr_a_voters;

                    const auto can_join_district_1 = a_voters <= first_distr_a_voters;

                    // There are at least that many voters for A in district 2.
                    auto second_distr_a_voters = can_join_district_1
                        ? std::size_t{} // Might join either district.
                        : a_voters; // Must have >= aVoters to join district 2.

                    for (; second_distr_a_voters <= sec_max; ++second_distr_a_voters)
                    {
                        // Place the last precinct in district 1.
                        // There are enough voters for the last precinct,
                        // and 1 less precinct is allowed.
                        const auto case1 = can_join_district_1 &&
                            prevs[first_distr_sel_precincts -
                                1U] // # of precincts in district 1 increases - must subtract 1.
                                 [first_distr_a_voters - a_voters][second_distr_a_voters];

                        const auto can_join_district_2 = a_voters <= second_distr_a_voters;

                        // Place the last precinct in district 2.
                        const auto case2 = can_join_district_2 &&
                            prevs[first_distr_sel_precincts] // The number of precincts in district 1 does not change.
                                 [first_distr_a_voters][second_distr_a_voters - a_voters];

                        if (case1 || case2)
                        {
                            curs[first_distr_sel_precincts][first_distr_a_voters][second_distr_a_voters] = true;

                            if constexpr (shall_print_precincts)
                            {
                                static const auto *const separ = " | ";
                                static const auto *const yes = "yes";
                                static const auto *const no1 = "no";

                                std::cout << " r" << ++row << separ << (initial_precincts + one) << separ
                                          << first_distr_sel_precincts << separ << first_distr_a_voters << separ
                                          << second_distr_a_voters << separ << (case1 ? yes : no1) << separ
                                          << (case2 ? yes : no1) << "\n";
                            }
                        }
                    }
                }
            }
        }

        if constexpr (shall_print_precincts)
        {
            std::exit(0);
        }
    }

    [[nodiscard]] constexpr auto find_min_allowed_voters(const std::size_t voters_min_inclusive,
        const std::vector<std::vector<bool>> &possible2, const std::size_t voters_max_exclusive)
        -> std::optional<std::pair<std::size_t, std::size_t>>
    {
        assert(0U < voters_min_inclusive && voters_min_inclusive < voters_max_exclusive);

        for (auto voters_distr_1 = voters_min_inclusive; voters_distr_1 < voters_max_exclusive; ++voters_distr_1)
        {
            const auto &possible3 = possible2.at(voters_distr_1);

            for (auto voters_distr_2 = voters_min_inclusive; voters_distr_2 < voters_max_exclusive; ++voters_distr_2)
            {
                if (possible3.at(voters_distr_2))
                {
                    return std::make_optional(std::make_pair(voters_distr_1, voters_distr_2));
                }
            }
        }

        return std::nullopt;
    }

    constexpr void backtrack(const std::vector<precinct> &precincts,
        const std::pair<std::size_t, std::size_t> &allowed_voter_pair,
        const std::vector<std::vector<std::vector<std::vector<bool>>>> &possibles,
        std::vector<std::size_t> &chosen_indexes)
    {
        auto initial_precincts = precincts.size();
        auto first_distr_sel_precincts = initial_precincts >> 1U;

        auto voters_distr_1 = allowed_voter_pair.first;
        auto voters_distr_2 = allowed_voter_pair.second;

        assert(!precincts.empty() && precincts.size() + 1U == possibles.size());
        assert(0U < first_distr_sel_precincts && 0U < voters_distr_1 && 0U < voters_distr_2);

        chosen_indexes.clear();

        for (;;)
        {
            assert(0U < initial_precincts && 0U < first_distr_sel_precincts && 0U < voters_distr_1);

            assert(0U < first_distr_sel_precincts && 0U < voters_distr_1);

            assert(possibles[initial_precincts][first_distr_sel_precincts][voters_distr_1][voters_distr_2]);

            // r9 | 4 | 2 | 12 | 12 | yes | no | r4 // The winning position, voters_distr_1 = 12, voters_distr_2 = 12.
            // r4 | 3 | 1 | 7 | 12 | no | yes | r2 // 9
            // r2 | 2 | 1 | 7 | 3 | no | yes | r1 // 3
            // r1 | 1 | 1 | 7 | 0 | yes | no | r0 // 7

            const auto &prec = precincts.at(initial_precincts - 1U);
            const auto &prevs = possibles[initial_precincts - 1U];

            if (const auto can_join_district_1 = prec.a_voters <= voters_distr_1,
                case1 = can_join_district_1 &&
                    prevs[first_distr_sel_precincts - 1U][voters_distr_1 - prec.a_voters][voters_distr_2];
                case1)
            {
                chosen_indexes.push_back(--initial_precincts);

                assert(
                    chosen_indexes.size() < 2U || chosen_indexes.back() < chosen_indexes[chosen_indexes.size() - 2U]);

                voters_distr_1 -= prec.a_voters;

                if (0U == --first_distr_sel_precincts)
                {
                    assert(0U == voters_distr_1);

                    std::reverse(chosen_indexes.begin(), chosen_indexes.end());

                    return;
                }

                continue;
            }

            if (const auto can_join_district_2 = prec.a_voters <= voters_distr_2,
                case2 = can_join_district_2 &&
                    prevs[first_distr_sel_precincts][voters_distr_1][voters_distr_2 - prec.a_voters];
                case2) [[likely]]
            {
                --initial_precincts;
                voters_distr_2 -= prec.a_voters;
                continue;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Inner error in Backtrack: neither case found. Initial precincts" << initial_precincts
                << ", selected precincts" << first_distr_sel_precincts << ", voters in district 1" << voters_distr_1
                << ", voters in district 2" << voters_distr_2;

            ::Standard::Algorithms::print("chosen indexes", chosen_indexes, str);

            throw std::runtime_error(str.str());
        }
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Numbers::sum(const precinct &preci) noexcept(false) -> std::size_t
{
    auto result = add_unsigned(preci.a_voters, preci.b_voters);

    return result;
}

auto Standard::Algorithms::Numbers::operator<< (std::ostream &str, const precinct &preci) -> std::ostream &
{
    str << "(" << preci.a_voters << ", " << preci.b_voters << ")";

    return str;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::gerry_mandering_partition(
    const std::vector<precinct> &precincts, std::vector<std::size_t> &chosen_indexes) -> bool
{
    const auto [votersPerPrecinct, totalVotersA, totalVotersB, aWinsPrecincts] = pre_process_precincts(precincts);

    const auto size = precincts.size();
    const auto half_size = size >> 1U;

    chosen_indexes.clear();

    if (aWinsPrecincts < half_size || totalVotersA - one <= totalVotersB)
    // 11 vs 9 - can win in both halves e.g. 7 > 6, 4 > 3.
    // However, 10 vs 9 - cannot win in both districts.
    // Let 10 = a1 + a2, 9 = b1 + b2.
    // A to win: b1 < a1 && b2 < a2.
    // Then b1 + 1 <= a1, b2 + 1 <= a2.
    // Sum b1 + b2 + 2 <= a1 + a2.
    // 9 + 2 <= 10, or 11 <= 10 is never true.
    {
        return false;
    }

    require_positive(totalVotersA, "totalVotersA");

    if (const auto a_wins_every_precinct = aWinsPrecincts == size; a_wins_every_precinct)
    {
        chosen_indexes.resize(half_size);

        std::iota(chosen_indexes.begin(), chosen_indexes.end(), std::size_t{});

        return true;
    }

    // todo(p4): impl other special (quick to check) cases.

    require_positive(totalVotersB, "totalVotersB");

    const auto half_size_plus_one = half_size + one;
    const auto size_plus_one = add_unsigned(one, size);

    const auto voters_max_exclusive = multiply_unsigned(size, votersPerPrecinct);

    // If votersPerPrecinct is odd say 3, and size = 4*k = 12,
    // then total voters 36, winning district min voters = 36/4 + 1 = 10.
    // Check {10 vs 8}, and {10 vs 8}, good.
    //
    // If votersPerPrecinct is odd say 3, and size = 4*k + 2 = 10,
    // 30 voters, winning district min voters = 30/4 + 1 = 7 + 1 = 8.
    // Check {8 vs 7}, and {8 vs 7}, good.
    const auto voters_min_inclusive = (voters_max_exclusive >> 2U) + one;

    std::vector<std::vector<std::vector<std::vector<bool>>>> possibles(size_plus_one,
        std::vector<std::vector<std::vector<bool>>>(half_size_plus_one,
            std::vector<std::vector<bool>>(voters_max_exclusive, std::vector<bool>(voters_max_exclusive, false))));

    calc_matrix(precincts, voters_max_exclusive, possibles);

    const auto allowed_voter_pair =
        find_min_allowed_voters(voters_min_inclusive, possibles[size][half_size], voters_max_exclusive);

    if (!allowed_voter_pair.has_value())
    {
        return false;
    }

    assert(voters_min_inclusive <= allowed_voter_pair.value().first &&
        voters_min_inclusive <= allowed_voter_pair.value().second);

    backtrack(precincts, allowed_voter_pair.value(), possibles, chosen_indexes);

    return true;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::gerry_mandering_partition_slow(
    const std::vector<precinct> &precincts, std::vector<std::size_t> &chosen_indexes) -> bool
{
    const auto size = precincts.size();
    check_precincts_size(size);

    {
        constexpr auto greater1 = 31U;
        require_greater(greater1, size, "Slow size");
    }

    chosen_indexes.clear();

    const auto half_size = size >> 1U;
    assert(0U < half_size);

    std::vector<bool> includes(size);

    for (auto index = half_size; index < size; ++index)
    {
        includes[index] = true;
    }

    do
    {
        // No constant
        // NOLINTNEXTLINE
        std::pair<std::size_t, std::size_t> selected{};

        // No constant
        // NOLINTNEXTLINE
        std::pair<std::size_t, std::size_t> rest{};

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &precinct = precincts[index];

            auto &par = includes[index] ? selected : rest;

            par.first = Standard::Algorithms::Numbers::add_unsigned(par.first, precinct.a_voters);

            par.second = Standard::Algorithms::Numbers::add_unsigned(par.second, precinct.b_voters);
        }

        if (const auto good = selected.second < selected.first && rest.second < rest.first; !good)
        {
            continue;
        }

        for (std::size_t index{}; index < size; ++index)
        {
            if (includes[index])
            {
                chosen_indexes.push_back(index);
            }
        }

        assert(half_size == chosen_indexes.size());

        return true;
    } while (std::next_permutation(includes.begin(), includes.end()));

    return false;
}
