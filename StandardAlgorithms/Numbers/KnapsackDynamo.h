#pragma once
#include <vector>
#include "Arithmetic.h"
#include "NumberUtilities.h"
#include "../Utilities/ExceptionUtilities.h"
#include <complex>

namespace
{
    //Returns whether at least one item has weight <= upperBoundWeight.
    template <class TKnapsackItem, class weight_t>
    weight_t CheckInputFisibility(const std::vector<TKnapsackItem>& items, const weight_t upperBoundWeight)
    {
        const auto size = items.size();
        RequirePositive(size, "items.size");
        RequirePositive(upperBoundWeight, "upperBoundWeight");

        weight_t sum{};

        for (size_t i = 0; i < size; ++i)
        {
            const auto& item = items[i];
            RequirePositive(item.Value, "items[i].Value");

            const auto& weight = item.Weight;
            RequirePositive(weight, "items[i].Weight");

            if (weight <= upperBoundWeight)
            {
                sum += weight;
                assert(sum > 0);
            }
        }

        return std::min(sum, upperBoundWeight);
    }

    template <class TKnapsackItem, class weight_t, class value_t>
    std::vector<value_t> ComputeMatrix_slow(const std::vector<TKnapsackItem>& items, const weight_t upperBoundWeight)
    {
        constexpr size_t one = 1;
        const auto size = items.size();
        const auto itemsSize_PlusOne = AddUnsigned(one, size);
        const auto upperBoundWeight_PlusOne = AddUnsigned(one, size_t(upperBoundWeight));

        const auto bigSize = MultiplyUnsigned(itemsSize_PlusOne, upperBoundWeight_PlusOne);

        //itemsSize - rows, upperBoundWeight - columns.
        std::vector<value_t> maxSumMatrix(bigSize);

        for (size_t row = 0; row < size; ++row)
        {
            const auto& item = items[row];

            for (size_t weight = 1; weight < upperBoundWeight_PlusOne; ++weight)
            {
                const auto leftIndex = row * upperBoundWeight_PlusOne + weight;
                const auto index = leftIndex + upperBoundWeight_PlusOne;
                if (weight < item.Weight)
                {
                    maxSumMatrix[index] = maxSumMatrix[leftIndex];
                    continue;
                }

                const auto upLeftIndex = leftIndex - static_cast<size_t>(item.Weight);
                const auto cand = item.Value + maxSumMatrix[upLeftIndex];
                maxSumMatrix[index] = std::max<value_t>(cand, maxSumMatrix[leftIndex]);
            }
        }

        return maxSumMatrix;
    }

    template <class TKnapsackItem, class weight_t, class value_t>
    value_t BacktrackResult_slow(
        const std::vector<TKnapsackItem>& items,
        const size_t upperBoundWeight,
        //itemsSize - rows, upperBoundWeight - columns.
        const std::vector<value_t>& maxSumMatrix,
        std::vector<size_t>& chosen_indexes)
    {
        RequirePositive(maxSumMatrix.size(), "maxSumMatrix.size");

        // todo: p1. this is wrong - the last item might be skipped?
        const auto& result = maxSumMatrix.back();
        RequirePositive(result, "result");

        constexpr size_t one = 1;
        const auto upperBoundWeight_PlusOne = AddUnsigned(one, size_t(upperBoundWeight));
        const auto itemsSize = items.size();

        auto row = itemsSize - one, column = upperBoundWeight;
        auto left = result;
        for (;;)
        {
            if (items[row].Weight <= column)
            {
                const auto leftIndex = row * upperBoundWeight_PlusOne + column;
                const auto upLeftIndex = leftIndex - static_cast<size_t>(items[row].Weight);
                const auto index = leftIndex + upperBoundWeight_PlusOne;

                if (maxSumMatrix[index] == items[row].Value + maxSumMatrix[upLeftIndex])
                {
                    chosen_indexes.push_back(row);

                    left -= items[row].Value;
                    if (!left)
                    {
                        std::reverse(chosen_indexes.begin(), chosen_indexes.end());
                        return result;
                    }

                    RequireGreater(column, items[row].Weight, "Inner error. Column");
                    column -= items[row].Weight;
                }
            }

            RequireGreater(row, 0u, "row");
            --row;
        }
    }

    template <class weight_t, class TKnapsackItem>
    weight_t eval_weight_gcd(const std::vector<TKnapsackItem>& items, const weight_t max_weight)
    {
        const auto size = items.size();
        assert(size);

        weight_t g = {};
        auto i = 0u;

        for (; i < size; ++i)
        {
            const auto& w = items[i].Weight;
            assert(w > 0);

            if (w <= max_weight)
            {
                if (w == 1)
                    return 1;

                g = w;
                break;
            }
        }

        assert(g > 1);
        while (++i < size)
        {
            const auto& w = items[i].Weight;
            if (w <= max_weight)
            {
                g = gcd(g, w);
                assert(g > 0);
                if (g == 1)
                    return 1;
            }
        }

        assert(g > 1);
        return g;
    }

    template <class TKnapsackItem, class weight_t, class value_t>
    void KnapsackDynamo_rec(
        const std::vector<TKnapsackItem>& items,
        std::vector<std::vector<value_t>>& buf,
        const size_t row,
        const weight_t weight_left)
    {
        static_assert(std::is_signed_v<value_t>);

        assert(items.size() > 0 && buf.size() > 0 && row > 0 && row <= items.size() && row < buf.size() && weight_left > 0);

        auto& b = buf[row][weight_left];
        assert(b < 0);

        const auto& item = items[row - 1];
        if (item.Weight <= weight_left)
        {
            const auto left2 = weight_left - item.Weight;
            const auto& included_sum = buf[row - 1][left2];

            if (included_sum < 0)
                KnapsackDynamo_rec<TKnapsackItem, weight_t, value_t>(items, buf, row - 1, left2);

            b = item.Value + included_sum;
            assert(included_sum >= 0 && item.Value > 0 && b > 0);
        }

        const auto& excluded_sum = buf[row - 1][weight_left];
        if (excluded_sum < 0)
            KnapsackDynamo_rec<TKnapsackItem, weight_t, value_t>(items, buf, row - 1, weight_left);

        assert(excluded_sum >= 0);
        if (b < excluded_sum)
            b = excluded_sum;
    }

    template <class TKnapsackItem, class weight_t, class value_t>
    value_t BacktrackResult(
        const std::vector<TKnapsackItem>& items,
        const std::vector<std::vector<value_t>>& buf,
        const weight_t upperBoundWeight,
        std::vector<size_t>& chosen_indexes)
    {
        static_assert(std::is_signed_v<value_t>);

        RequirePositive(items.size(), "items.size");
        RequirePositive(buf.size(), "buf.size");
        RequirePositive(buf.back().size(), "buf.back().size");

        value_t left{};
        weight_t weight{};

        for (weight_t w = 1; w <= upperBoundWeight; ++w)
        {
            const auto& cur = buf.back();
            const auto& cand = cur[w];
            if (left < cand)
            {
                assert(cand > 0);
                left = cand;
                weight = w;
            }
        }

        const auto result = left;
        auto row = items.size();
        for (;;)
        {
            assert(left > 0 && weight > 0 && row > 0);
            RequireGreater(row, 0u, "row");

            const auto& item = items[row - 1];
            if (item.Weight <= weight && item.Value <= left)
            {
                const auto& prev = buf[row - 1], & cur = buf[row];
                const auto& ps = prev[weight - item.Weight];
                if (cur[weight] == item.Value + ps)
                {
                    chosen_indexes.push_back(row - 1);
                    left -= item.Value;
                    weight -= item.Weight;
                    if (!left)
                    {
                        assert(weight >= 0); // When all included + have extra.
                        std::reverse(chosen_indexes.begin(), chosen_indexes.end());
                        return result;
                    }
                }
            }

            assert(row > 1);
            --row;
        }
    }
}

namespace Privet::Algorithms::Numbers
{
    template <class value_t, class weight_t>
    struct knapsack_item_t final
    {
        value_t Value;
        weight_t Weight;

        knapsack_item_t(value_t value = {}, weight_t weight = {})
            : Value(value), Weight(weight)
        {
        }
    };

    //There is no check for overflows.
    //power(2, n) may run faster for high weights.
    template <class value_t, class weight_t,
        class TKnapsackItem = knapsack_item_t<value_t, weight_t>>
        value_t KnapsackDynamo(const std::vector<TKnapsackItem>& items, const weight_t upperBoundWeight, std::vector<size_t>& chosen_indexes)
    {
        static_assert(std::is_signed_v<value_t>);

        chosen_indexes.clear();
        const auto max_weight = CheckInputFisibility(items, upperBoundWeight);
        if (!max_weight)
            return {};

        const auto size = items.size();
        assert(size > 0 && max_weight > 0);
        {
            const auto g = eval_weight_gcd<weight_t, TKnapsackItem>(items, max_weight);

            if (g > 1)
            {
                std::vector<TKnapsackItem> items2;
                items2.reserve(size);

                for (const auto& item : items)
                {
                    if (max_weight < item.Weight)
                        continue;

                    auto cop = item;
                    auto& weight = cop.Weight;
                    weight /= g;
                    assert(weight > 0);

                    items2.push_back(cop);
                }

                assert(items2.size());

                const auto reduced = KnapsackDynamo<value_t, weight_t, TKnapsackItem>(items2, max_weight / g, chosen_indexes);

                assert(reduced > 0 && chosen_indexes.size());
                return reduced;
            }
        }

        std::vector<std::vector<value_t>> buf(size + 1u,
            std::vector<value_t>(max_weight + 1, -1));

        buf[0].assign(buf[0].size(), 0);

        for (auto& b : buf)
            b[0] = 0;

        KnapsackDynamo_rec<TKnapsackItem, weight_t, value_t>(items, buf, size, max_weight);

        const auto result = BacktrackResult<TKnapsackItem, weight_t, value_t>(items, buf, max_weight, chosen_indexes);

        assert(result > 0);
        return result;
    }

    // Visit all states - slow.
    template <class value_t, class weight_t,
        class TKnapsackItem = knapsack_item_t<value_t, weight_t>>
        value_t KnapsackDynamo_slow_still(const std::vector<TKnapsackItem>& items, const weight_t upperBoundWeight, std::vector<size_t>& chosen_indexes)
    {
        chosen_indexes.clear();

        const auto max_weight = CheckInputFisibility(items, upperBoundWeight);
        if (!max_weight)
            return {};

        const auto size = items.size();
        std::vector<std::vector<value_t>> buf(size + 1u,
            std::vector<value_t>(upperBoundWeight + 1));

        for (size_t row = 0; row < size; ++row)
        {
            const auto& prev = buf[row];
            auto& cur = buf[row + 1];

            const auto& item = items[row];
            {
                const auto edge = std::min(upperBoundWeight, item.Weight);
                for (weight_t weight = 1; weight <= edge; ++weight)
                    cur[weight] = prev[weight];
            }

            for (auto weight = item.Weight; weight <= upperBoundWeight; ++weight)
            {
                const auto& ps = prev[weight - item.Weight];
                const auto cand = item.Value + ps;
                cur[weight] = std::max<value_t>(cand, prev[weight]);
            }
        }

        const auto result = BacktrackResult<TKnapsackItem, weight_t, value_t>(items, buf, upperBoundWeight, chosen_indexes);

        assert(result > 0);
        return result;
    }

    // Slow because all states are computed.
    template <class value_t, class weight_t,
        class TKnapsackItem = knapsack_item_t<value_t, weight_t>>
        value_t KnapsackDynamo_slow(const std::vector<TKnapsackItem>& items, const weight_t upperBoundWeight, std::vector<size_t>& chosen_indexes)
    {
        chosen_indexes.clear();

        if (!CheckInputFisibility(items, upperBoundWeight))
            return {};

        const auto maxSumMatrix = ComputeMatrix_slow<TKnapsackItem, weight_t, value_t>(items, upperBoundWeight);

        const auto result = BacktrackResult_slow<TKnapsackItem, weight_t, value_t>(items, size_t(upperBoundWeight), maxSumMatrix, chosen_indexes);

        return result;
    }
}