#pragma once
#include"../Utilities/require_utilities.h"
#include<unordered_map>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an array of n>0 items, and a number k>0,
    // return the indexes of major elements, appearing >= k times in the array.
    //
    // E.g. n=9, k=3, there can be maximum 3 major items.
    // But n=9, k=4, there can be maximum 2 major items.
    template<class int_t>
    struct array_items_counting_over_times final
    {
        array_items_counting_over_times() = delete;

        // todo(p3): Using a std::vector instead of hash table could be faster for big "minimumCount".
        [[nodiscard]] static constexpr auto major_elements_indexes_fast(
            const std::vector<int_t> &data, const std::size_t minimum_count) -> std::vector<std::size_t>
        {
            const auto size = data.size();
            check_input(size, minimum_count);

            const auto result_size_max = size / minimum_count;
            assert(0U < result_size_max);

            const auto candidates = calc_candidates(data, result_size_max);
            const auto frequencies = calc_frequencies(data, candidates);

            auto result = add_majors(frequencies, minimum_count);
            return result;
        }

        // It can use more memory, potentially caching every item.
        [[nodiscard]] static constexpr auto major_elements_indexes_much_memory(
            const std::vector<int_t> &data, const std::size_t minimum_count) -> std::vector<std::size_t>
        {
            const auto size = data.size();
            check_input(size, minimum_count);

            const auto frequencies = calc_frequencies_slow(data);
            auto result = add_majors(frequencies, minimum_count);
            return result;
        }

private:
        using first_index_count_t = std::pair<std::size_t, std::size_t>;

        using frequencies_t = std::unordered_map<int_t, first_index_count_t>;

        using candidates_t = std::unordered_map<int_t, std::size_t>;

        static constexpr void check_input(const std::size_t size, const std::size_t minimum_count)
        {
            require_positive(size, "size");
            require_positive(minimum_count, "minimumCount");
            require_less_equal(minimum_count, size, "minimumCount");
        }

        [[nodiscard]] static constexpr auto calc_candidates(
            const std::vector<int_t> &data, const std::size_t result_size_max) -> candidates_t
        {
            const auto size = data.size();

            candidates_t result;
            result.reserve(result_size_max);

            candidates_t temp;

            for (std::size_t index{}; index < size; ++index)
            {
                const auto &datum = data[index];
                auto found = result.find(datum);

                if (result.end() != found)
                {
                    ++(found->second);
                    continue;
                }

                const auto shall_decrement_size = result_size_max <= result.size();
                if (shall_decrement_size)
                {
                    decrement_size(temp, result);
                }
                else
                {
                    result.emplace(datum, 1U);
                }
            }

            return result;
        }

        static constexpr void decrement_size(candidates_t &temp, candidates_t &result)
        {
            assert(&temp != &result);

            temp.clear();
            temp.reserve(result.size());

            for (const auto &datum : result)
            {
                if (1U < datum.second)
                {
                    temp.emplace(datum.first, datum.second - 1U);
                }
            }

            std::swap(temp, result);
        }

        [[nodiscard]] static constexpr auto calc_frequencies(
            const std::vector<int_t> &data, const candidates_t &candidates) -> frequencies_t
        {
            const auto size = data.size();

            frequencies_t result;
            result.reserve(candidates.size());

            for (std::size_t index{}; index < size; ++index)
            {
                const auto &datum = data[index];
                const auto number_counts_found = candidates.find(datum);

                if (candidates.end() == number_counts_found)
                {
                    continue;
                }

                auto &first_index_count_t = result[datum];

                if (first_index_count_t.second++ == 0U)
                {
                    first_index_count_t.first = index;
                }
            }

            return result;
        }

        [[nodiscard]] static constexpr auto add_majors(
            const frequencies_t &frequencies, const std::size_t minimum_count) -> std::vector<std::size_t>
        {
            assert(0U < minimum_count);

            std::vector<std::size_t> result;

            for (const auto &freq : frequencies)
            {
                if (minimum_count <= freq.second.second)
                {
                    result.push_back(freq.second.first);
                }
            }

            return result;
        }

        [[nodiscard]] static constexpr auto calc_frequencies_slow(const std::vector<int_t> &data) -> frequencies_t
        {
            const auto size = data.size();
            frequencies_t result{};

            for (std::size_t index{}; index < size; ++index)
            {
                const auto &datum = data[index];
                auto &fic = result[datum];

                if (fic.second++ == 0U)
                {
                    fic.first = index;
                }
            }

            return result;
        }
    };
} // namespace Standard::Algorithms::Numbers
