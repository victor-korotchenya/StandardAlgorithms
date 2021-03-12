#pragma once
#include"../Utilities/require_utilities.h"
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<class int_t>
    [[nodiscard]] constexpr auto pet_mans(const std::vector<std::vector<int_t>> &man_pets)
        -> std::vector<std::vector<int_t>>
    {
        const auto size = require_positive(static_cast<int_t>(man_pets.size()), "man pets size");

        std::vector<std::vector<int_t>> pet_mans;

        for (int_t man{}; man < size; ++man)
        {
            const auto &pets = man_pets[man];

            require_positive(pets.size(), "man pets[index].size");

            for (const auto &pet : pets)
            {
                assert(0 <= pet);

                if (static_cast<int_t>(pet_mans.size()) <= pet)
                {
                    pet_mans.resize(pet + 1LL);
                }

                pet_mans.at(pet).push_back(man);
            }
        }

        return pet_mans;
    }

    template<std::signed_integral int_t, int_t mod>
    [[nodiscard]] constexpr auto count_unique_20_choices_of_100_pets_rec(const int_t &man_mask_max,
        const std::vector<std::vector<int_t>> &pet_mans,
        // Going through each pet guarantees uniqueness.
        const int_t &pet_index,
        // {Man mask, pet} -> count.
        std::vector<std::vector<int_t>> &buffer, const int_t &man_mask) -> int_t
    {
        assert(0 <= pet_index && man_mask <= man_mask_max);

        auto &result = buffer[man_mask][pet_index];
        assert(result < 0);

        if (0 < pet_index)
        {
            const auto &exclude_pet = buffer[man_mask][pet_index - 1];

            if (const auto must_compute = exclude_pet < 0; must_compute)
            {
                result = count_unique_20_choices_of_100_pets_rec<int_t, mod>(
                    man_mask_max, pet_mans, pet_index - 1, buffer, man_mask);
            }
            else
            {
                result = exclude_pet;
            }

            assert(0 <= result && result < mod);
        }
        else
        {
            result = 0;
        }

        constexpr int_t one = 1;
        ;
        const auto &men = pet_mans[pet_index];

        for (const auto &man : men)
        {
            if (const auto man_has_pet = man_mask & (one << man); man_has_pet != 0)
            {
                continue;
            }

            // The 'man' now owns a pet at 'pet_index'.
            const auto man_mask2 = man_mask | (one << man);
            int_t add{};

            if (const auto all_men_gathered = man_mask2 == man_mask_max; all_men_gathered)
            {
                add = 1;
            }
            else if (const auto no_more_pets = pet_index == 0; no_more_pets)
            {
                continue;
            }
            else if (const auto &include_pet = buffer[man_mask2][pet_index - 1]; 0 <= include_pet)
            {// Already computed.
                add = include_pet;
            }
            else
            {
                add = count_unique_20_choices_of_100_pets_rec<int_t, mod>(
                    man_mask_max, pet_mans, pet_index - 1, buffer, man_mask2);
            }

            assert(0 <= add && add < mod);

            result += add;

            if (!(result < mod))
            {
                result -= mod;
            }

            assert(0 <= result && result < mod);
        }

        return result;
    }

    template<std::signed_integral int_t, int_t mod>
    [[nodiscard]] constexpr auto count_unique_20_choices_of_100_pets_slow_rec(
        const std::vector<std::vector<int_t>> &man_pets, std::unordered_set<int_t> &uniq, const int_t &pos) -> int_t
    {
        assert(0 <= pos && pos < static_cast<int_t>(man_pets.size()));

        const auto &pets = man_pets[pos];
        int_t result{};

        for (const auto &pet : pets)
        {
            if (!uniq.insert(pet).second)
            {
                continue;
            }

            if (0 < pos)
            {
                const auto add = count_unique_20_choices_of_100_pets_slow_rec<int_t, mod>(man_pets, uniq, pos - 1);

                result += add;
            }
            else
            {
                ++result;
            }

            if (!(result < mod))
            {
                result -= mod;
            }

            [[maybe_unused]] const auto del = uniq.erase(pet);

            assert(del == 1U && 0 <= result && result < mod);
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // There are 0 < m <= 20 men, owning 0 < p <= 100 unique pets.
    // Count choices: every man selects a pet he has, all m selected pets are unique.
    // Pets start from 0.
    // Sample. 3 man to 5 pets: {{1,2,3}, {0}, {3,2}}.
    // There are 4 choices: {1,0,2}, {1,0,3}, {2,0,3}, {3,0,2}.
    //
    // Time O(2**man * pets * average pets per man).
    template<std::signed_integral int_t, int_t mod>
    requires(sizeof(std::int32_t) <= sizeof(int_t) && 0 < mod)
    [[nodiscard]] constexpr auto count_unique_20_choices_of_100_pets(const std::vector<std::vector<int_t>> &man_pets)
        -> int_t
    {
        const auto men = require_positive(static_cast<int_t>(man_pets.size()), "man pets size");
        {
            constexpr auto greater = 31U;
            require_greater(greater, man_pets.size(), "size");
        }

        require_positive(man_pets[0].size(), "man pets[0].size");

        const auto pet_mans = Inner::pet_mans<int_t>(man_pets);
        const auto pets = require_positive(static_cast<int_t>(pet_mans.size()), "pet men size");

        constexpr int_t one = 1;

        const auto man_mask_edge = one << men;
        assert(0 < man_mask_edge);

        // {Man mask, unique_pet_index} -> count.
        std::vector<std::vector<int_t>> buffer(man_mask_edge, std::vector<int_t>(pets + 1LL, -one));

        auto result = Inner::count_unique_20_choices_of_100_pets_rec<int_t, mod>(
            man_mask_edge - one, pet_mans, pets - one, buffer, 0);

        assert(0 <= result && result < mod);

        return result;
    }

    // Slow.
    // Time O(2**pets * man * pets * log(pets)).
    template<std::signed_integral int_t, int_t mod>
    requires(int_t{} < mod)
    [[nodiscard]] constexpr auto count_unique_20_choices_of_100_pets_slow(
        const std::vector<std::vector<int_t>> &man_pets) -> int_t
    {
        const auto size = require_positive(static_cast<int_t>(man_pets.size()), "man pets size");

        {
            constexpr auto greater = 21U;

            require_greater(greater, man_pets.size(), "man pets size");

            const auto cols = man_pets[0].size();
            require_positive(static_cast<int_t>(cols), "man pets[0] size");
            require_greater(greater, cols, "man pets[0] size");
        }

        std::unordered_set<int_t> uniq{};

        auto result = Inner::count_unique_20_choices_of_100_pets_slow_rec<int_t, mod>(man_pets, uniq, size - 1);

        assert(0 <= result && result < mod);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
