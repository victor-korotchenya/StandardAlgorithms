#pragma once
#include"../Utilities/require_utilities.h"
#include<iostream>
#include<queue>
#include<vector>

namespace Standard::Algorithms::Graphs
{
    constexpr auto show_intermediate_results = false;

    // Find a stable matching.
    template<std::integral vertex_t1>
    requires(std::is_nothrow_convertible_v<vertex_t1, std::size_t>)
    struct stable_matching final
    {
        stable_matching() = delete;

        // todo(p3): maybe ranks (back refere lists) must be computed to run in O(n*n), p 166 of 128 pr?

        using vertex_t = vertex_t1;
        using preference_t = std::vector<vertex_t>;
        using preference_list_t = std::vector<preference_t>;

        static constexpr void find(const preference_list_t &first_preferences,
            // The result for other part is symmetrical and
            // can be built from the result.
            preference_t &result, const preference_list_t &second_preferences)
        {
            const auto size = require_positive(first_preferences.size(), "first preferences size");
            require_equal(size, "second preferences size", second_preferences.size());

            std::vector<bool> bit_set;
            check_preference_list(0, bit_set, size, first_preferences);
            check_preference_list(1, bit_set, size, second_preferences);

            // To determine others' preferences quickly.
            auto back_preference_list = build_back_preference_list(second_preferences);

            run(first_preferences, size, back_preference_list, result);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                check_preference(-1, bit_set, size, result, 0);
            }
        }

private:
        static constexpr void check_preference_list(const std::int32_t number, std::vector<bool> &bit_set,
            const std::size_t size, const preference_list_t &preference_list)
        {
            for (std::size_t position{}; position < size; ++position)
            {
                const auto &preference = preference_list[position];
                check_preference(number, bit_set, size, preference, position);
            }
        }

        static constexpr void check_preference(const std::int32_t number, std::vector<bool> &bit_set,
            const std::size_t size, const preference_t &preference, const std::size_t position)
        {
            const auto preference_size = preference.size();

            const auto name = std::to_string(number) + " - " + std::to_string(position) + " preference ";

            require_equal(size, name + "size", preference_size);

            bit_set.assign(size, false);

            for (std::size_t ind2{}; ind2 < size; ++ind2)
            {
                const auto &value = preference[ind2];

                if (value < vertex_t{} || size <= static_cast<std::size_t>(value)) [[unlikely]]
                {
                    auto err = name + "at " + std::to_string(ind2) + " has an invalid value " + std::to_string(value);

                    throw std::invalid_argument(err);
                }

                if (bit_set[value]) [[unlikely]]
                {
                    auto err = name + "at " + std::to_string(value) + " has already been seen before";

                    throw std::invalid_argument(err);
                }

                bit_set[value] = true;
            }
        }

        static constexpr void run(const preference_list_t &first_preferences, const std::size_t size,
            const preference_list_t &back_preference_list, preference_t &result)
        {
            constexpr vertex_t initial_preference{};

            // Store the indexes to enable fast move to the next other instance.
            result.assign(size, initial_preference);

            constexpr auto free_assignment = static_cast<vertex_t>(initial_preference - static_cast<vertex_t>(1));

            preference_t other(size, free_assignment);

            std::queue<vertex_t> free_queue{};

            for (std::size_t index{}; index < size; ++index)
            {
                free_queue.push(static_cast<vertex_t>(index));
            }

            if constexpr (show_intermediate_results)
            {
                std::cout << "\nStart stable matching..\n";
            }

            while (!free_queue.empty())
            {
                const auto top = free_queue.front();
                const auto index = result[top];

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    require_greater(static_cast<vertex_t>(size), index, "index");
                }

                const auto bott = first_preferences[top][index];
                auto b_preference = other[bott];

                if (free_assignment == b_preference)
                {
                    if constexpr (show_intermediate_results)
                    {
                        std::cout << "Free " << top << " to " << bott << '\n';
                    }

                    other[bott] = top;
                    free_queue.pop();
                }
                else
                {
                    const auto &back_preference = back_preference_list[bott];

                    const auto &index_a = back_preference[top];
                    const auto &index_b = back_preference[b_preference];

                    if (const auto is_better = index_a < index_b; is_better)
                    {
                        if constexpr (show_intermediate_results)
                        {
                            std::cout << " " << top << " is better than " << b_preference << " for " << bott << '\n';
                        }

                        other[bott] = top;
                        free_queue.pop();
                        free_queue.push(b_preference);
                    }
                    else
                    {
                        if constexpr (show_intermediate_results)
                        {
                            std::cout << " " << top << " is not better than " << b_preference << " for " << bott
                                      << '\n';
                        }

                        b_preference = top;
                    }

                    ++result[b_preference];
                }
            }

            restore_values_from_indexes(first_preferences, size, result);
        }

        static constexpr void restore_values_from_indexes(
            const preference_list_t &first_preferences, const std::size_t size, preference_t &result)
        {
            if constexpr (show_intermediate_results)
            {
                std::cout << "\n\nFinal results:\n";
            }

            for (std::size_t ind2{}; ind2 < size; ++ind2)
            {
                const auto &index = result[ind2];

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    require_greater(static_cast<vertex_t>(size), index, "Error2 index");
                }

                result[ind2] = first_preferences[ind2][index];

                if constexpr (show_intermediate_results)
                {
                    std::cout << "M[ " << ind2 << " ] = " << result[ind2] << '\n';
                }
            }
        }

        [[nodiscard]] static constexpr auto build_back_preference_list(const preference_list_t &preference_list)
            -> preference_list_t
        {
            const auto size = preference_list.size();
            preference_list_t back_preference_list(size);

            for (std::size_t pos{}; pos < size; ++pos)
            {
                const auto &preference = preference_list[pos];
                auto &back_preference = back_preference_list[pos];
                back_preference.resize(size);

                for (std::size_t ind2{}; ind2 < size; ++ind2)
                {
                    const auto &pref = preference[ind2];
                    back_preference[pref] = static_cast<vertex_t>(ind2);
                }
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                std::vector<bool> bit_set;
                check_preference_list(-3, bit_set, size, back_preference_list);
            }

            return back_preference_list;
        }
    };
} // namespace Standard::Algorithms::Graphs
