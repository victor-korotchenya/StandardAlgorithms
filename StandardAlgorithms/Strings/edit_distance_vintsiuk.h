#pragma once
#include"../Numbers/arithmetic.h"
#include"../Numbers/sort.h"
#include"../Utilities/check_size.h"
#include<algorithm>
#include<vector>

namespace Standard::Algorithms::Strings
{
    // todo(p2): If both sorted, time O(m + n).
    // In a general case, O(m*n) cannot be improved?
    // todo(p2): solve for an array of 3 or more strings.
    //   for k=3, if a[p]=b[q]=c[r] then x[p,q,r] = 1+ x[p-1,q-1,r-1]
    //  else = max(del from a, from b, or c).
    template<class collection_t, std::integral int_t>
    [[nodiscard]] constexpr auto edit_distance_vintsiuk(const int_t add_cost, const collection_t &source,
        const int_t change_cost, const collection_t &destination, const int_t delete_cost) -> int_t
    {
        if constexpr (std::is_signed_v<int_t>)
        {
            require_non_negative<int_t>(add_cost, "add cost");
            require_non_negative<int_t>(change_cost, "change cost");
            require_non_negative<int_t>(delete_cost, "delete cost");
        }

        auto source_size = Standard::Algorithms::Utilities::check_size<int_t>("source size", source.size());
        auto destination_size =
            Standard::Algorithms::Utilities::check_size<int_t>("destination size", destination.size());

        auto min_size = std::min(source_size, destination_size);

        int_t begin_matches{};

        while (begin_matches < min_size && source[begin_matches] == destination[begin_matches])
        {
            ++begin_matches;
        }

        min_size -= begin_matches;
        // todo(p2): "abc", "ab abc"
        // todo(p2): "abc", "abbc"

        constexpr int_t one{ 1 };

        int_t end_matches{};

        while (end_matches < min_size &&
            source[source_size - one - end_matches] == destination[destination_size - one - end_matches])
        {
            ++end_matches;
        }

        source_size -= end_matches;
        destination_size -= end_matches;

        if (begin_matches == source_size)
        {
            assert(source_size <= destination_size);

            auto result1 =
                Standard::Algorithms::Numbers::multiply_unsigned<int_t>(destination_size - source_size, add_cost);

            return result1;
        }

        if (begin_matches == destination_size)
        {
            assert(destination_size <= source_size);

            auto result1 =
                Standard::Algorithms::Numbers::multiply_unsigned<int_t>(source_size - destination_size, delete_cost);

            return result1;
        }

        const auto source_symbols_to_match = static_cast<int_t>(source_size - begin_matches);
        const auto destination_symbols_to_match = static_cast<int_t>(destination_size - begin_matches);

        std::vector<int_t> previous(one + destination_symbols_to_match);
        std::vector<int_t> current(one + destination_symbols_to_match);

        for (int_t index{}, temp_cost{}; index <= destination_symbols_to_match; ++index, temp_cost += delete_cost)
        {
            current[index] = temp_cost;
        }

        for (int_t index{}; index < source_symbols_to_match; ++index)
        {
            std::swap(previous, current);
            current[0] = Standard::Algorithms::Numbers::multiply_unsigned<int_t>(index + one, delete_cost);

            for (int_t ind_2{}; ind_2 < destination_symbols_to_match; ++ind_2)
            {
                const auto &sci = source[begin_matches + index];
                const auto &tacj = destination[begin_matches + ind_2];

                const auto add = static_cast<int_t>(current[ind_2] + add_cost);
                const auto chan = static_cast<int_t>(previous[ind_2] + (sci == tacj ? int_t{} : change_cost));
                const auto del = static_cast<int_t>(previous[ind_2 + one] + delete_cost);

                current[ind_2 + one] = Standard::Algorithms::Numbers::min3(add, chan, del);
            }
        }

        const auto &result = current[destination_symbols_to_match];
        return result;
    }
} // namespace Standard::Algorithms::Strings
