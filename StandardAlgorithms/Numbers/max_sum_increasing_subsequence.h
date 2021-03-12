#pragma once
#include"../Graphs/binary_indexed_tree.h"
#include"../Utilities/check_size.h"
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include"standard_operations.h"

namespace Standard::Algorithms::Numbers
{
    // Given a non-empty array of positive integers,
    // select strictly increasing items with maximum total sum.
    // Return the sum, and the chosen item indexes sorted.
    //
    // For example, given { 3, 2, 5 }, the sum is 8, and the indexes are { 0, 2 }.
    template<std::integral int_t,
        // To avoid overflows.
        std::integral long_int_t, std::unsigned_integral size_t1 = std::size_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    struct max_sum_increasing_subsequence final
    {
        max_sum_increasing_subsequence() = delete;

        using operation_t = long_int_t (*)(const long_int_t &, const long_int_t &);

        using binary_indexed_tree_t = ::Standard::Algorithms::Trees::binary_indexed_tree<long_int_t, operation_t>;

        using value_index_t = std::pair<int_t, size_t1>;

        // Time O(n * log(n)).
        // Return the pair(max sum, chosen indexes).
        [[nodiscard]] static constexpr auto compute_fast(const std::vector<int_t> &values)
            -> std::pair<long_int_t, std::vector<size_t1>>
        {
            throw_if_empty("values", values);
            // todo(p3): order the parameters properly in the validation API.
            require_all_positive<int_t>(values, "values");

            const auto size = Standard::Algorithms::Utilities::check_size<size_t1>("values size", values.size());

            const auto increasing_value_positions = to_increasing_value_positions(values, size);

            constexpr std::size_t avoid_extra_checks = 3U;

            binary_indexed_tree_t indexed_tree(size + avoid_extra_checks, max<long_int_t>, max<long_int_t>);

            auto result = compute_fast_max_sum(values, size, increasing_value_positions, indexed_tree);

            return result;
        }

        // Slow time O(n * n).
        [[nodiscard]] static constexpr auto compute_slow(const std::vector<int_t> &values)
            -> std::pair<long_int_t, std::vector<size_t1>>
        {
            throw_if_empty("values", values);
            require_all_positive<int_t>(values, "values");

            const auto size = Standard::Algorithms::Utilities::check_size<size_t1>("values size", values.size());

            std::vector<long_int_t> sums(size + 1LLU);
            std::vector<size_t1> previous_indexes(size);

            auto best_sum = sums[1] = static_cast<long_int_t>(values[0]);
            size_t1 last_index{};

            assert(long_int_t{} < best_sum && last_index < size);

            for (size_t1 ending = 1; ending < size; ++ending)
            {
                const auto &last_val = values[ending];
                assert(int_t{} < last_val);

                auto &cur_sum = sums[ending + 1U];
                cur_sum = last_val;

                auto &prev_index = previous_indexes[ending];
                size_t1 beginning{};

                do
                {
                    if (const auto &value = values[beginning]; !(value < last_val))
                    {
                        continue;
                    }

                    const auto &prev = sums[beginning + 1];
                    const auto candidate = static_cast<long_int_t>(prev + last_val);

                    assert(last_val <= candidate && prev < candidate);

                    if (cur_sum < candidate)
                    {
                        cur_sum = candidate;
                        prev_index = beginning;
                    }
                } while (++beginning < ending);

                if (best_sum < cur_sum)
                {
                    best_sum = cur_sum, last_index = ending;
                }
            }

            auto remainder = best_sum;
            std::vector<size_t1> chosen_indexes;

            for (;;)
            {
                assert(long_int_t{} < remainder && last_index < size);

                chosen_indexes.push_back(last_index);

                const auto &value = values[last_index];

                if (remainder < value) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error in compute_slow: remainder " << remainder << " < " << value << " values["
                        << last_index << "].";

                    throw_exception(str);
                }

                remainder -= value;

                if (long_int_t{} == remainder)
                {
                    break;
                }

                const auto &pre = previous_indexes[last_index];

                assert(pre < last_index);

                last_index = pre;
            }

            std::reverse(chosen_indexes.begin(), chosen_indexes.end());

            return { best_sum, std::move(chosen_indexes) };
        }

private:
        [[nodiscard]] static constexpr auto to_value_index(const std::vector<int_t> &values, const size_t1 &size)
            -> std::vector<value_index_t>
        {
            // The last entry will be (0, 0).
            std::vector<value_index_t> result(size + 1LLU);

            for (size_t1 index{}; index < size; ++index)
            {
                result[index] = std::make_pair(values[index], index);
            }

            std::sort(result.begin(), result.end() - 1);

            return result;
        }

        [[nodiscard]] static constexpr auto to_increasing_value_positions(
            const std::vector<int_t> &values, const size_t1 &size) -> std::vector<size_t1>
        {
            const auto value_index_pairs = to_value_index(values, size);

            std::vector<size_t1> result(size);

            for (size_t1 index{},
                 increasing_index =
                     // The tree index starts from 1.
                 1;
                 index < size; ++index)
            {
                const auto &[prev_val, prev_ind] = value_index_pairs[index];
                result[prev_ind] = increasing_index;

                const auto &cur_pair = value_index_pairs[index + 1];
                const auto are_different = prev_val != cur_pair.first;
                increasing_index += are_different ? 1 : 0;
            }

            return result;
        }

        inline static constexpr void check_last_index(const size_t1 &last_index, const size_t1 &size)
        {
            if (const auto good = 0U < last_index && last_index < size; good) [[likely]]
            {
                return;
            }

            auto err = "Inner error in compute_fast_max_sum: last_index " + std::to_string(last_index) + ", size " +
                std::to_string(size) + ".";

            throw std::runtime_error(err);
        }

        [[nodiscard]] static constexpr auto compute_fast_max_sum(const std::vector<int_t> &values, const size_t1 &size,
            const std::vector<size_t1> &increasing_value_positions, binary_indexed_tree_t &indexed_tree)
            -> std::pair<long_int_t, std::vector<size_t1>>
        {
            std::vector<long_int_t> previous_sums(size);

            long_int_t max_sum{};
            size_t1 last_index{};

            for (size_t1 index{}; index < size; ++index)
            {
                const auto &smallest_position = increasing_value_positions[index];
                const auto prev_val = indexed_tree.get(smallest_position);
                const auto &cur_val = values[index];

                auto &sum = previous_sums[index];
                sum = static_cast<long_int_t>(prev_val + cur_val);

                assert(long_int_t{} <= prev_val && prev_val < sum && cur_val <= sum);

                indexed_tree.set(smallest_position + 1, sum);

                if (max_sum < sum)
                {
                    max_sum = sum, last_index = index;
                }
            }

            std::vector<size_t1> chosen_indexes;
            auto remainder = max_sum;

            for (;;)
            {
                assert(long_int_t{} < remainder && last_index < size);

                chosen_indexes.push_back(last_index);

                const auto &value = values[last_index];

                if (remainder < value)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error in compute_fast: remainder " << remainder << " < " << value << " values["
                        << last_index << "].";

                    throw_exception(str);
                }

                remainder -= value;

                if (long_int_t{} == remainder)
                {
                    break;
                }

                check_last_index(last_index, size);

                while (previous_sums[--last_index] != remainder)
                {
                    check_last_index(last_index, size);
                }
            }

            std::reverse(chosen_indexes.begin(), chosen_indexes.end());

            return { max_sum, std::move(chosen_indexes) };
        }
    };
} // namespace Standard::Algorithms::Numbers
