#pragma once
#include"../Numbers/bit_utilities.h"
#include"../Numbers/to_unsigned.h"
#include"../Utilities/require_utilities.h"
#include<bit> // std::popcount
#include<numeric>
#include<optional>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    [[nodiscard]] [[gnu::const]] inline constexpr auto find_false(const std::vector<bool> &flags) noexcept
        -> std::optional<std::size_t>
    {
        const auto ite = std::find(flags.cbegin(), flags.cend(), false);
        if (ite == flags.cend())
        {
            return std::nullopt;
        }

        const auto atom = ite - flags.cbegin();
        return std::make_optional(static_cast<std::size_t>(atom));
    }

    template<std::unsigned_integral int_t, class subset_t>
    constexpr void demand_distinctness(
        const int_t &atom_count, const subset_t &subset, const std::size_t &ide, std::vector<bool> &flags)
    {
        flags.assign(atom_count, false);

        for (const auto &val : subset)
        {
            require_greater(atom_count, val, "atom count vs value");

            if (flags[val]) [[unlikely]]
            {
                throw std::invalid_argument("A subset " + ::Standard::Algorithms::Utilities::zu_string(ide) +
                    " has a duplicate value '" + ::Standard::Algorithms::Utilities::zu_string(val) + "'; '" +
                    ::Standard::Algorithms::Utilities::zu_string(atom_count) + "' atom count.");
            }

            flags[val] = true;
        }
    }

    template<std::unsigned_integral int_t, class subset_t>
    constexpr void validate_min_set_cover(
        const int_t &atom_count, const std::vector<subset_t> &subsets, std::vector<bool> &flags)
    {
        const auto size = subsets.size();
        {
            constexpr std::size_t max_log2 = 31;
            constexpr std::size_t bits = sizeof(int_t) << 3U;
            static_assert(0U < bits);

            require_greater(std::min(max_log2, bits), size, "subsets count");
        }

        flags.assign(atom_count, false);

        const auto ge_atom_count = [atom_count](const auto &val)
        {
            return !(val < atom_count);
        };

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &sub = subsets[index];
            require_positive(sub.size(), "subsets[" + ::Standard::Algorithms::Utilities::zu_string(index) + "] count");

            if (const auto ite = std::find_if(sub.cbegin(), sub.cend(), ge_atom_count); ite != sub.cend()) [[unlikely]]
            {
                throw std::invalid_argument("A subset " + ::Standard::Algorithms::Utilities::zu_string(index) +
                    " has an invalid atom '" + ::Standard::Algorithms::Utilities::zu_string(*ite) + "' >= '" +
                    ::Standard::Algorithms::Utilities::zu_string(atom_count) + "' atom count.");
            }

            for (const auto &val : sub)
            {
                flags[val] = true;
            }
        }

        if (const auto opt = find_false(flags); opt.has_value()) [[unlikely]]
        {
            throw std::invalid_argument("An atom '" + ::Standard::Algorithms::Utilities::zu_string(opt.value()) +
                "' must belong to a subset; '" + ::Standard::Algorithms::Utilities::zu_string(atom_count) +
                "' atom count.");
        }

        // Must be done after ensuring all the atoms are in [0, atom_count).
        for (std::size_t index{}; index < size; ++index)
        {
            const auto &sub = subsets[index];
            demand_distinctness<int_t, subset_t>(atom_count, sub, index, flags);
        }
    }

    template<std::unsigned_integral int_t, class subset_t>
    constexpr auto are_atoms_covered_by_union(const int_t atom_count, const std::vector<subset_t> &subsets,
        std::vector<bool> &flags, const std::uint64_t mask) -> bool
    {
        flags.assign(atom_count, false);

        const auto size = subsets.size();
        assert(0U < atom_count && 0U < mask && 0U < size);

        for (int_t index{}; index < size; ++index)
        {
            if (((mask >> index) & 1U) == 0U)
            {
                continue;
            }

            const auto &sub = subsets[index];

            for (const auto &atom : sub)
            {
                assert(atom < atom_count);

                flags[atom] = true;
            }
        }

        const auto opt = find_false(flags);
        return !opt.has_value();
    }

    template<std::unsigned_integral int_t, class subset_t>
    constexpr auto set_cover_mask(const int_t atom_count, const std::vector<subset_t> &subsets,
        std::vector<bool> &flags, const std::int32_t candidate_count) -> int_t
    {
        const auto size = subsets.size();

        assert(0U < atom_count && 0 < candidate_count && static_cast<std::uint64_t>(candidate_count) <= size);

        constexpr int_t one = 1;
        const auto edge_mask = static_cast<int_t>(one << size);

        auto mask = static_cast<int_t>((one << ::Standard::Algorithms::Numbers::to_unsigned(candidate_count)) - one);

        do
        {
            assert(0U < mask && mask < edge_mask);

            if (are_atoms_covered_by_union<int_t, subset_t>(atom_count, subsets, flags, mask))
            {
                return mask;
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                require_positive(mask, "mask");
            }

            const auto next_mask = Standard::Algorithms::Numbers::next_greater_same_pop_count(mask);
            assert(mask < next_mask);

            mask = next_mask;
        } while (mask < edge_mask);

        return {};
    }

    template<std::unsigned_integral int_t>
    struct msc_context_approx final
    {
        constexpr msc_context_approx(const std::size_t subsets_size, const int_t &atom_count)
            : Atom_count(atom_count)
            , Ide_sizes(subsets_size)
            , Size_minus_one_ides(atom_count)
            , Atom_ides(atom_count)
        {
            assert(0U < subsets_size && 0U < atom_count);
        }

        [[nodiscard]] constexpr auto atom_count() const &noexcept -> const int_t &
        {
            return Atom_count;
        }

        [[nodiscard]] constexpr auto ide_sizes() &noexcept -> std::vector<int_t> &
        {
            return Ide_sizes;
        }

        [[nodiscard]] constexpr auto size_minus_one_ides() &noexcept -> std::vector<std::vector<int_t>> &
        {
            return Size_minus_one_ides;
        }

        [[nodiscard]] constexpr auto atom_ides() &noexcept -> std::vector<std::vector<int_t>> &
        {
            return Atom_ides;
        }

private:
        const int_t Atom_count;
        std::vector<int_t> Ide_sizes;
        std::vector<std::vector<int_t>> Size_minus_one_ides;
        std::vector<std::vector<int_t>> Atom_ides;
    };

    // Each atom is deleted once.
    // Total time for all subsets is O(sum(|subset|)) = O(m).
    template<std::unsigned_integral int_t, class subset_t>
    constexpr void delete_atoms(auto &context, const subset_t &subset, [[maybe_unused]] const int_t &ide)
    {
        assert(!subset.empty());

        for (const auto &atom : subset)
        {
            assert(atom < context.atom_count());

            auto &del_ides = context.atom_ides().at(atom); // Some atoms might have been already deleted.

            for (const auto &del_ide : del_ides)
            {
                auto &cusi = context.ide_sizes().at(del_ide);
                assert(0U < cusi || (0U == cusi && ide == del_ide));

                if (0U == cusi || 0U == --cusi)
                {
                    continue;
                }

                assert(cusi < context.atom_count());
                context.size_minus_one_ides().at(cusi - 1U).push_back(del_ide);
            }

            del_ides.clear();
            del_ides.shrink_to_fit();
        }
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Min set cover MSC: find min number of subsets,
    // formed on distinct items called atoms,
    // provided that subsets are distinct and non-empty (slightly simplifies things),
    // and the distinct items from the chosen MSC subsets combined are precisely the atoms.
    // Each atom must belong to at least 1 subset.
    //
    // todo(p3): Small opts. While possible, do:
    // - Remove subsets that belong to a larger or equal subset e.g. {1, 6} belongs to {0, 1, 4, 6, 8} or {6, 1}.
    // - If there is an atom belonging to only 1 subset, include that subset to the result and remove the subset atoms.
    //
    // todo(p3): Small opt. Run O(log(n)) approximation [find a subset with max # of uncovered atoms],
    // and use its output as an upper bound?
    // Time complexity O(2**n * m), where n = |subsets|, m is total # of all items in all subsets.
    template<std::unsigned_integral int_t, class subset_t>
    requires(sizeof(std::uint32_t) <= sizeof(int_t))
    constexpr auto minimum_set_cover_slow(const int_t atom_count, const std::vector<subset_t> &subsets) -> int_t
    {
        std::vector<bool> flags;

        Inner::validate_min_set_cover<int_t, subset_t>(atom_count, subsets, flags);

        if (!(1U < atom_count) || subsets.size() <= 1ZU)
        {
            return 0U < atom_count ? 1 : 0;
        }

        // Binary search for the subset count minimum.
        std::int32_t left = 1;
        auto right = static_cast<std::int32_t>(subsets.size()) - 1;

        constexpr int_t one = 1;

        auto best_mask = static_cast<int_t>((one << subsets.size()) - one); // Include all subsets.

        while (left <= right)
        {
            const auto mid = std::midpoint(left, right);
            const auto mask = Inner::set_cover_mask<int_t, subset_t>(atom_count, subsets, flags, mid);

            if (mask != 0U)
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    const auto bit_count = std::popcount(mask);
                    assert(bit_count == mid);
                }

                right = mid - 1, best_mask = mask;
            }
            else
            {
                left = mid + 1;
            }
        }

        if (best_mask == 0U) [[unlikely]]
        {
            throw std::runtime_error("Inner error in min Set Cover: zero best mask.");
        }

        return best_mask;
    }

    // Selecting a largest-subset heuristic is log(n) approximation.
    // todo(p5): Enable large sizes; return a vector, not a mask.
    // Time complexity O(n + m).
    template<std::unsigned_integral int_t, class subset_t>
    constexpr auto minimum_set_cover_approximation(const int_t &atom_count, const std::vector<subset_t> &subsets)
        -> int_t
    {
        {
            std::vector<bool> flags;
            Inner::validate_min_set_cover<int_t, subset_t>(atom_count, subsets, flags);
        }

        if (!(1U < atom_count) || subsets.size() <= 1ZU)
        {
            return 0U < atom_count ? 1 : 0;
        }

        constexpr int_t one = 1;

        Inner::msc_context_approx<int_t> context(subsets.size(), atom_count);

        for (int_t index{}; const auto &subset : subsets)
        {
            if (const auto cover_all_atoms = !(subset.size() < atom_count); cover_all_atoms)
            {
                return one << index;
            }

            assert(!subset.empty());
            context.ide_sizes().at(index) = subset.size();
            context.size_minus_one_ides().at(subset.size() - 1U).push_back(index);

            for (const auto &nuclear_power_atom : subset)
            {
                assert(nuclear_power_atom < atom_count);

                context.atom_ides().at(nuclear_power_atom).push_back(index);
            }

            ++index;
        }

        for (int_t best_mask{}, atom_count_minus_one = static_cast<int_t>(atom_count - one);;)
        {
            assert(atom_count_minus_one < atom_count);

            auto &currs = context.size_minus_one_ides().at(atom_count_minus_one);

            while (!currs.empty())
            {
                const auto ide = currs.back();
                assert(ide < subsets.size());

                currs.pop_back();

                {
                    auto &cusi = context.ide_sizes().at(ide);
                    if (cusi <= atom_count_minus_one)
                    {// The atom in the subset has already been deleted.
                        continue;
                    }

                    assert(cusi == atom_count_minus_one + 1ZU && 0U < (one << ide));

                    cusi = {};
                }

                best_mask |= one << ide;

                const auto &subset = subsets.at(ide);
                Inner::delete_atoms(context, subset, ide);
            }

            if (0U == atom_count_minus_one--)
            {
                assert(0U < best_mask);

                return best_mask;
            }

            currs.shrink_to_fit();
        }
    }
} // namespace Standard::Algorithms::Graphs
