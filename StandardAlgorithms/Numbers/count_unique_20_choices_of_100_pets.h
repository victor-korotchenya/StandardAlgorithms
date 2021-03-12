#pragma once
#include <cassert>
#include <type_traits>
#include <unordered_set>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template <class int_t, int_t mod>
    int_t count_unique_20_choices_of_100_pets_slow_rec(
        const std::vector<std::vector<int_t>>& ar,
        std::unordered_set<int_t>& uniq,
        const int_t pos)
    {
        assert(pos >= 0 && pos < static_cast<int_t>(ar.size()) && ar.size() && ar.size() <= 20);

        int_t result = 0;
        const auto& a = ar[pos];
        for (const auto& v : a)
        {
            if (!uniq.insert(v).second)
                continue;

            if (pos)
            {
                const auto ad = count_unique_20_choices_of_100_pets_slow_rec<int_t, mod>(ar, uniq, pos - 1);
                result += ad;
            }
            else
                ++result;

            if (result >= mod)
                result -= mod;

            const auto del = uniq.erase(v);
            assert(del == 1 && result >= 0 && result < mod);
        }

        return result;
    }

    template <class int_t>
    std::vector<std::vector<int_t>> get_pet_man(
        const std::vector<std::vector<int_t>>& man_pet)
    {
        const auto size = static_cast<int_t>(man_pet.size());
        RequirePositive(size, "man_pet.size");

        std::vector<std::vector<int_t>> pet_man;

        for (int_t man = 0; man < size; ++man)
        {
            const auto& pets = man_pet[man];
            RequirePositive(pets.size(), "man_pet[i].size");

            for (const auto& p : pets)
            {
                assert(p >= 0);
                if (static_cast<int_t>(pet_man.size()) <= p)
                    pet_man.resize(p + 1);

                pet_man[p].push_back(man);
            }
        }

        return pet_man;
    }

    template<class int_t, int_t mod>
    int_t count_unique_20_choices_of_100_pets_rec(
        const std::vector<std::vector<int_t>>& pet_man,
        // {Man mask, pet} -> count.
        std::vector<std::vector<int_t>>& buf,
        const int_t man_mask_max,
        // Going through each pet guarantees uniqueness.
        const int_t pet_index,
        const int_t man_mask = 0)
    {
        static_assert(std::is_signed_v<int_t>);
        assert(pet_index >= 0 && man_mask <= man_mask_max);

        auto& result = buf[man_mask][pet_index];
        assert(result < 0);

        if (pet_index)
        {
            const auto& exclude_pet = buf[man_mask][pet_index - 1];
            if (exclude_pet >= 0)
                result = exclude_pet;
            else
                result = count_unique_20_choices_of_100_pets_rec<int_t, mod>(pet_man, buf, man_mask_max, pet_index - 1, man_mask);

            assert(result >= 0 && result < mod);
        }
        else
            result = 0;

        const auto& man = pet_man[pet_index];
        for (const auto& m : man)
        {
            const auto is_man_with_pet = man_mask & (int_t(1) << m);
            if (is_man_with_pet)
                continue;

            // Man 'm' now owns a pet at 'pet_index'.
            const auto man_mask2 = man_mask | (int_t(1) << m);
            int_t ad;
            if (man_mask2 == man_mask_max)
                ad = 1;
            else if (!pet_index)
                continue;
            else
            {
                const auto& include_pet = buf[man_mask2][pet_index - 1];
                if (include_pet >= 0)
                    ad = include_pet;
                else
                    ad = count_unique_20_choices_of_100_pets_rec<int_t, mod>(pet_man, buf, man_mask_max, pet_index - 1, man_mask2);
            }

            result += ad;
            if (result >= mod)
                result -= mod;
            assert(result >= 0 && result < mod);
        }

        return result;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // There are m <= 20 men, owning p <= 100 pet kinds where m > 0, p > 0.
            // Count choices: every man selects a pet kind he has, all m selected pets are unique.
            // Pet kinds start from 0.
            // Sample. 3 man to 5 pets: {{1,2,3}, {0}, {3,2}}.
            // There are 4 choices: {1,0,3}, {1,0,2}, {3,0,2}, {2,0,3}.
            //
            // Time O(2**man * pet kinds * average pets per man).
            template<class int_t, int_t mod = 1000 * 1000 * 1000 + 7>
            int_t count_unique_20_choices_of_100_pets(const std::vector<std::vector<int_t>>& man_pet)
            {
                static_assert(std::is_signed_v<int_t> && sizeof(int_t) >= 4);

                const auto size = static_cast<int_t>(man_pet.size());
                RequirePositive(size, "man_pet.size");
                RequirePositive(man_pet[0].size(), "man_pet[0].size");

                constexpr auto size_max = 30;
                if (size > size_max)
                {
                    assert(0);
                    const auto err = "Too large (" + std::to_string(size) + ") size.";
                    throw std::invalid_argument(err);
                }

                const auto pet_man = get_pet_man<int_t>(man_pet);
                const auto pets = static_cast<int_t>(pet_man.size());
                const auto man_mask_edge = int_t(1) << size;
                assert(man_mask_edge > 0 && pets > 0);

                // {Man mask, unique_pet_index} -> count.
                std::vector<std::vector<int_t>> buf(man_mask_edge,
                    std::vector<int_t>(pets + 1, -int_t(1)));

                const auto result = count_unique_20_choices_of_100_pets_rec<int_t, mod>(pet_man, buf, man_mask_edge - int_t(1), pets - 1);
                assert(result >= 0 && result < mod);
                return result;
            }

            // Slow.
            // Time O(2**pets * man * pets * log(pets)).
            template<class int_t, int_t mod = 1000 * 1000 * 1000 + 7>
            int_t count_unique_20_choices_of_100_pets_slow(const std::vector<std::vector<int_t>>& ar)
            {
                static_assert(std::is_signed_v<int_t>);

                const auto size = static_cast<int_t>(ar.size());
                RequirePositive(size, "ar.size");

                {
                    const auto row_size = static_cast<int_t>(ar[0].size());
                    RequirePositive(row_size, "ar[0].size");
                    assert(size <= 20 && row_size <= 20);
                }

                std::unordered_set<int_t> uniq;
                const auto result = count_unique_20_choices_of_100_pets_slow_rec<int_t, mod>(ar, uniq, size - 1);
                assert(result >= 0 && result < mod);
                return result;
            }
        }
    }
}