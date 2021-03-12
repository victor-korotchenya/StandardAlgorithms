#pragma once
#include"arithmetic.h"
#include"gcd.h"

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::integral value_t, std::integral weight_t, class knapsack_item_t>
    [[nodiscard]] constexpr auto sum_available_items(const std::vector<knapsack_item_t> &items,
        const weight_t &upper_bound_weight, std::vector<std::size_t> &chosen_indexes) -> std::pair<value_t, weight_t>
    {
        require_positive(items.size(), "items size");
        {
            constexpr auto greater1 = std::numeric_limits<std::size_t>::max() - 2U;

            require_greater(greater1, items.size(), "items size");
        }

        require_positive(upper_bound_weight, "upperBoundWeight");
        {
            constexpr weight_t greater1 = std::numeric_limits<weight_t>::max() - 2;

            require_greater(greater1, upper_bound_weight, "upper bound weight");
        }

        chosen_indexes.clear();
        chosen_indexes.reserve(items.size());

        value_t value_sum{};
        weight_t weight_sum{};

        for (std::size_t index{}; index < items.size(); ++index)
        {
            const auto &item = items[index];
            const auto &value = require_positive(item.value, "items[index].value");

            const auto &weight = item.weight;
            if (upper_bound_weight < weight)
            {
                continue;
            }

            require_positive(weight, "items[index].weight");

            weight_sum += weight;
            require_less_equal(weight, weight_sum, "partial weight sum");

            value_sum += value;
            require_less_equal(value, value_sum, "partial value sum");

            chosen_indexes.push_back(index);
        }

        return std::make_pair(value_sum, weight_sum);
    }

    template<std::integral value_t, class knapsack_item_t>
    [[nodiscard]] constexpr auto compute_matrix_dyn_slow(
        const std::vector<knapsack_item_t> &items, const std::size_t upper_bound_weight) -> std::vector<value_t>
    {
        assert(!items.empty() && 0U < upper_bound_weight);

        constexpr std::size_t one = 1;

        const auto size = items.size();
        const auto items_size_plus_one = Standard::Algorithms::Numbers::add_unsigned(one, size);

        const auto upper_bound_weight_plus_one = Standard::Algorithms::Numbers::add_unsigned(one, upper_bound_weight);

        const auto big_size =
            Standard::Algorithms::Numbers::multiply_unsigned(items_size_plus_one, upper_bound_weight_plus_one);

        // itemsSize - rows, upperBoundWeight - columns.
        std::vector<value_t> max_sum_matrix(big_size);

        for (std::size_t row{}; row < size; ++row)
        {
            const auto &item = items[row];

            for (std::size_t weight = 1; weight < upper_bound_weight_plus_one; ++weight)
            {
                const auto left_index = row * upper_bound_weight_plus_one + weight;

                const auto index = left_index + upper_bound_weight_plus_one;

                const auto item_weight = static_cast<std::size_t>(item.weight);
                if (weight < item_weight)
                {
                    max_sum_matrix[index] = max_sum_matrix[left_index];
                    continue;
                }

                const auto up_left_index = left_index - item_weight;

                const auto cand = static_cast<value_t>(item.value + max_sum_matrix[up_left_index]);

                max_sum_matrix[index] = std::max(cand, max_sum_matrix[left_index]);
            }
        }

        return max_sum_matrix;
    }

    template<std::integral value_t, class knapsack_item_t>
    [[nodiscard]] constexpr auto backtrack_result_slow(const std::vector<knapsack_item_t> &items,
        const std::size_t upper_bound_weight,
        // itemsSize - rows, upperBoundWeight - columns.
        const std::vector<value_t> &max_sum_matrix, std::vector<std::size_t> &chosen_indexes) -> value_t
    {
        chosen_indexes.clear();

        require_positive(max_sum_matrix.size(), "maxSumMatrix size");

        const auto &result = require_positive(max_sum_matrix.back(), "result");

        constexpr std::size_t one = 1;

        const auto upper_bound_weight_plus_one = Standard::Algorithms::Numbers::add_unsigned(one, upper_bound_weight);
        const auto items_size = items.size();

        auto row = items_size - one;
        auto column = upper_bound_weight;

        for (auto left = result;;)
        {
            const auto &item = items[row];
            const auto item_weight = static_cast<std::size_t>(item.weight);

            if (item_weight <= column)
            {
                const auto left_index = row * upper_bound_weight_plus_one + column;

                const auto up_left_index = left_index - item_weight;

                const auto index = left_index + upper_bound_weight_plus_one;

                if (max_sum_matrix[index] == item.value + max_sum_matrix[up_left_index])
                {
                    chosen_indexes.push_back(row);
                    left -= item.value;

                    if (value_t{} == left)
                    {
                        std::reverse(chosen_indexes.begin(), chosen_indexes.end());

                        return result;
                    }

                    require_greater(column, item.weight, "Inner error. Column");

                    column -= item.weight;
                }
            }

            require_greater(row, 0U, "row");
            --row;
        }
    }

    template<std::integral weight_t, class knapsack_item_t>
    [[nodiscard]] constexpr auto eval_weight_gcd(const std::vector<knapsack_item_t> &items, const weight_t &max_weight)
        -> weight_t
    {
        assert(weight_t{} < max_weight);

        const auto size = require_positive(items.size(), "items size");

        constexpr weight_t one = 1;
        weight_t gcd1{};
        std::size_t index{};

        for (; index < size; ++index)
        {
            const auto &weight = items[index].weight;
            assert(weight_t{} < weight);

            if (max_weight < weight)
            {
                continue;
            }

            if (weight == one)
            {
                return one;
            }

            gcd1 = weight;
            break;
        }

        require_greater(gcd1, one, "gcd1");

        while (++index < size)
        {
            const auto &weight = items[index].weight;
            assert(weight_t{} < weight);

            if (max_weight < weight)
            {
                continue;
            }

            gcd1 = ::Standard::Algorithms::Numbers::gcd_int(gcd1, weight);
            assert(weight_t{} < gcd1);

            if (gcd1 == one)
            {
                return one;
            }
        }

        assert(one < gcd1);
        return gcd1;
    }

    template<std::signed_integral value_t, std::integral weight_t, class knapsack_item_t>
    constexpr void knapsack_dynamo_rec(const std::vector<knapsack_item_t> &items,
        std::vector<std::vector<value_t>> &buffer, const std::size_t row, const weight_t &weight_left)
    {
        assert(0U < row && row <= items.size() && row < buffer.size() && weight_t{} < weight_left);

        auto &cur_value = buffer[row][weight_left];
        assert(cur_value < value_t{});

        const auto &item = items[row - 1U];

        if (const auto can_include = !(weight_left < item.weight); can_include)
        {
            const auto left2 = static_cast<weight_t>(weight_left - item.weight);
            const auto &included_sum = buffer[row - 1U][left2];

            if (included_sum < value_t{})
            {
                knapsack_dynamo_rec<value_t, weight_t, knapsack_item_t>(items, buffer, row - 1U, left2);
            }

            cur_value = item.value + included_sum;

            assert(value_t{} <= included_sum && value_t{} < item.value && item.value <= cur_value);
        }

        const auto &excluded_sum = buffer[row - 1U][weight_left];
        if (excluded_sum < value_t{})
        {
            knapsack_dynamo_rec<value_t, weight_t, knapsack_item_t>(items, buffer, row - 1U, weight_left);
        }

        assert(value_t{} <= excluded_sum);

        cur_value = std::max(cur_value, excluded_sum);
    }

    template<std::integral value_t, std::integral weight_t, class knapsack_item_t>
    [[nodiscard]] constexpr auto backtrack_result(const std::vector<knapsack_item_t> &items,
        const std::vector<std::vector<value_t>> &buffer, const weight_t &upper_bound_weight,
        std::vector<std::size_t> &chosen_indexes) -> value_t
    {
        require_positive(items.size(), "items size");
        require_positive(buffer.size(), "buffer size");
        require_positive(buffer.back().size(), "buffer.back().size");

        chosen_indexes.clear();

        value_t left_value{};
        weight_t left_weight{};

        const auto &back1 = buffer.back();

        for (weight_t cand_weight = 1; cand_weight <= upper_bound_weight; ++cand_weight)
        {
            const auto &cand = back1[cand_weight];
            if (!(left_value < cand))
            {
                continue;
            }

            assert(value_t{} < cand);

            left_value = cand;
            left_weight = cand_weight;
        }

        const auto result = require_positive(left_value, "result");

        for (auto row = items.size();;)
        {
            require_greater(row, 0U, "row");

            assert(value_t{} < left_value && weight_t{} < left_weight);

            const auto &item = items[row - 1U];

            if (item.weight <= left_weight && item.value <= left_value)
            {
                const auto &prev = buffer[row - 1U];
                const auto &prev_value = prev[left_weight - item.weight];
                const auto &cur = buffer[row];
                const auto &cur_value = cur[left_weight];

                if (cur_value == item.value + prev_value)
                {
                    chosen_indexes.push_back(row - 1U);
                    left_value -= item.value;
                    left_weight -= item.weight;

                    assert(value_t{} <= left_value && weight_t{} <= left_weight);

                    if (value_t{} == left_value)
                    {
                        std::reverse(chosen_indexes.begin(), chosen_indexes.end());

                        return result;
                    }
                }
            }

            assert(2U <= row);
            --row;
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    template<class value_t, class weight_t>
    struct knapsack_item_t final
    {
        value_t value{};
        weight_t weight{};
    };

    // todo(p2): FPTAS approximation.

    // Given an array of items, and a weight bound,
    // choose items of maximum value sum and with weight sum not exceeding the bound.
    // Each item has both positive value and weight.
    // todo(p3): An O(2**n) algorithm may run faster for small n.
    template<std::signed_integral value_t, std::integral weight_t,
        class knapsack_item_t = knapsack_item_t<value_t, weight_t>>
    [[nodiscard]] constexpr auto knapsack_dynamo(const std::vector<knapsack_item_t> &items,
        const weight_t &upper_bound_weight, std::vector<std::size_t> &chosen_indexes, const bool may_reduce = true)
        -> value_t
    {
        // todo(p4): remove useless items with (item.weight > upperBoundWeight), but mind the indexes.

        const auto value_weight =
            Inner::sum_available_items<value_t, weight_t, knapsack_item_t>(items, upper_bound_weight, chosen_indexes);

        const auto &sum_weight = value_weight.second;

        if (const auto has_nothing = weight_t{} == sum_weight; has_nothing)
        {
            assert(chosen_indexes.empty() && value_t{} == value_weight.first);

            return {};
        }

        const auto size = items.size();

        assert(0U < size && value_t{} < value_weight.first && weight_t{} < sum_weight);

        if (const auto select_all_possible = sum_weight <= upper_bound_weight; select_all_possible)
        {
            assert(!chosen_indexes.empty());

            return value_weight.first;
        }

        if (const weight_t one = 1, gcd1 = Inner::eval_weight_gcd<weight_t, knapsack_item_t>(items, upper_bound_weight);
            one < gcd1)
        {
            if (!may_reduce) [[unlikely]]
            {
                throw std::invalid_argument("Only 1 reduction is allowed.");
            }

            std::vector<knapsack_item_t> items2;
            items2.reserve(size);

            for (const auto &item : items)
            {
                if (upper_bound_weight < item.weight)
                {
                    continue;
                }

                auto cop = item;
                auto &weight = cop.weight;
                weight /= gcd1;
                assert(weight_t{} < weight);

                items2.push_back(std::move(cop));
            }

            assert(!items2.empty());

            auto reduced = knapsack_dynamo<value_t, weight_t, knapsack_item_t>(
                items2, upper_bound_weight / gcd1, chosen_indexes, false);

            assert(value_t{} < reduced && !chosen_indexes.empty());
            return reduced;
        }

        std::vector<std::vector<value_t>> buffer(
            size + 1U, std::vector<value_t>(upper_bound_weight + 1, -static_cast<value_t>(1)));

        buffer[0].assign(buffer[0].size(), value_t{});

        for (auto &buf : buffer)
        {
            buf[0] = value_t{};
        }

        Inner::knapsack_dynamo_rec<value_t, weight_t, knapsack_item_t>(items, buffer, size, upper_bound_weight);

        auto result = Inner::backtrack_result<value_t, weight_t, knapsack_item_t>(
            items, buffer, upper_bound_weight, chosen_indexes);

        assert(value_t{} < result && !chosen_indexes.empty());

        return result;
    }

    // Visit all positions - still slow.
    template<std::integral value_t, std::integral weight_t, class knapsack_item_t = knapsack_item_t<value_t, weight_t>>
    [[nodiscard]] constexpr auto knapsack_dynamo_slow_still(const std::vector<knapsack_item_t> &items,
        const weight_t &upper_bound_weight, std::vector<std::size_t> &chosen_indexes) -> value_t
    {
        if (const auto value_weight = Inner::sum_available_items<value_t, weight_t, knapsack_item_t>(
                items, upper_bound_weight, chosen_indexes);
            weight_t{} == value_weight.second)
        {
            chosen_indexes.clear();
            return {};
        }

        const auto size = items.size();

        std::vector<std::vector<value_t>> buffer(size + 1U, std::vector<value_t>(upper_bound_weight + 1, value_t{}));

        for (std::size_t row{}; row < size; ++row)
        {
            const auto &item = items[row];
            const auto &prev = buffer[row];
            auto &cur = buffer[row + 1U];

            {// Exclude the current item.
                const auto max_weight = std::min(upper_bound_weight, item.weight);

                for (weight_t weight = 1;
                     // If the current item weight is too large,
                     // the following 'for' cycle will be skipped.
                     // So, the condition (weight < maxWeight) is not good enough - it must be "<=".
                     weight <= max_weight; ++weight)
                {
                    cur[weight] = prev[weight];
                }
            }

            for (auto weight = item.weight; weight <= upper_bound_weight; ++weight)
            {
                const auto &prev_value = prev[weight - item.weight];
                const auto cand = static_cast<value_t>(item.value + prev_value);

                cur[weight] = std::max(cand, prev[weight]);
            }
        }

        auto result = Inner::backtrack_result<value_t, weight_t, knapsack_item_t>(
            items, buffer, upper_bound_weight, chosen_indexes);

        assert(value_t{} < result && !chosen_indexes.empty());

        return result;
    }

    // Slow because all positions are computed.
    template<std::integral value_t, std::integral weight_t, class knapsack_item_t = knapsack_item_t<value_t, weight_t>>
    requires(sizeof(weight_t) <= sizeof(std::size_t))
    [[nodiscard]] constexpr auto knapsack_dynamo_slow(const std::vector<knapsack_item_t> &items,
        const weight_t &upper_bound_weight, std::vector<std::size_t> &chosen_indexes) -> value_t
    {
        chosen_indexes.clear();

        if (const auto value_weight = Inner::sum_available_items<value_t, weight_t, knapsack_item_t>(
                items, upper_bound_weight, chosen_indexes);
            weight_t{} == value_weight.second)
        {
            return {};
        }

        const auto max_sum_matrix = Inner::compute_matrix_dyn_slow<value_t, knapsack_item_t>(
            items, static_cast<std::size_t>(upper_bound_weight));

        auto result = Inner::backtrack_result_slow<value_t, knapsack_item_t>(
            items, static_cast<std::size_t>(upper_bound_weight), max_sum_matrix, chosen_indexes);

        assert(value_t{} < result && !chosen_indexes.empty());

        return result;
    }
} // namespace Standard::Algorithms::Numbers
