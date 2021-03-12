#include"substring_of_minimum_positive_sum.h"
#include"../Utilities/project_constants.h"
#include"../Utilities/require_utilities.h"
#include<algorithm>
#include<numeric> // std::midpoint
#include<set>
#include<utility>

namespace
{
    struct mpsum_context final
    {
        const std::vector<std::int32_t> &data;

        std::vector<std::pair<std::int64_t, std::size_t>> buffer{};

        Standard::Algorithms::Numbers::first_last_sum best{};
    };

    // The sum needs about 0.14E10 integers for this code to break.
    constexpr auto easy_search_sentinel = std::numeric_limits<std::int64_t>::max() / 3;

    constexpr auto cutoff_size = 16U;

    constexpr void prepare_after_mid(std::size_t mid, mpsum_context &context, const std::size_t end1)
    {
        assert(mid < end1 && end1 <= context.data.size());

        context.buffer.clear();

        std::int64_t sum{};

        while (++mid < end1)
        {
            const auto &item = context.data[mid];
            sum += item;

            assert(-easy_search_sentinel < sum && sum < easy_search_sentinel);

            context.buffer.emplace_back(sum, mid);
        }

        context.buffer.emplace_back(easy_search_sentinel, std::numeric_limits<std::size_t>::max() / 2);

        std::sort(context.buffer.begin(), context.buffer.end());
    }

    constexpr void relax_begin_mid(const std::size_t begin0, mpsum_context &context, const std::size_t end1)
    {
        // [[assume(begin0 < end1 && end1 <= context.data.size())]];
        assert(begin0 < end1 && end1 <= context.data.size());

        const auto mid = std::midpoint(begin0, end1);
        std::int64_t sum{};
        auto index = mid;

        do
        {
            sum += context.data[index];

            assert(-easy_search_sentinel < sum && sum < easy_search_sentinel);

            if (0 < sum && sum < context.best.sum)
            {
                context.best.sum = static_cast<std::int32_t>(sum);
                context.best.begin = index;
                context.best.end = mid + 1U;
                // if (context.best.sum == 1)
                // {
                //      return;
                // }
            }

            const auto iter =
                std::lower_bound(context.buffer.cbegin(), context.buffer.cend(), std::pair(1 - sum, std::size_t{}));

            assert(iter != context.buffer.cend());

            const auto cand = iter->first + sum;

            if (0 < cand && cand < context.best.sum)
            {
                context.best.sum = static_cast<std::int32_t>(cand);
                context.best.begin = index;
                context.best.end = iter->second + 1U;
                // if (context.best.sum == 1)
                // {
                //      return;
                // }
            }
        } while (begin0 < index--);
    }

    constexpr void conquer_mpsum(const std::size_t begin0, mpsum_context &context, const std::size_t end1
#if _DEBUG
        ,
        std::int32_t depth = {}
#endif
    )
    {
        // [[assume(begin0 < end1 && end1 <= context.data.size())]];
        assert(begin0 < end1 && end1 <= context.data.size());

#if _DEBUG
        ++depth;

        assert(depth < ::Standard::Algorithms::Utilities::max_logn_algorithm_depth);
#endif

        const auto mid = std::midpoint(begin0, end1);

        if (begin0 < mid)
        {
            conquer_mpsum(begin0, context, mid
#if _DEBUG
                ,
                depth
#endif
            );
        }

        if (mid + 1U < end1)
        {
            conquer_mpsum(mid + 1U, context, end1
#if _DEBUG
                ,
                depth
#endif
            );
        }

        prepare_after_mid(mid, context, end1);
        relax_begin_mid(begin0, context, end1);
    }

    constexpr void conquer_mpsum_cutoff(const std::size_t begin0, mpsum_context &context, const std::size_t end1
#if _DEBUG
        ,
        std::int32_t depth = {}
#endif
    )
    {
        // [[assume(begin0 < end1 && end1 <= context.data.size() && cutoff_size < end1 - begin0)]];
        assert(begin0 < end1 && end1 <= context.data.size() && cutoff_size < end1 - begin0);

#if _DEBUG
        ++depth;

        assert(depth < ::Standard::Algorithms::Utilities::max_logn_algorithm_depth);
#endif

        const auto mid = std::midpoint(begin0, end1);
        if (begin0 < mid)
        {
            if (cutoff_size < mid - begin0)
            {
                conquer_mpsum_cutoff(begin0, context, mid
#if _DEBUG
                    ,
                    depth
#endif
                );
            }
            else
            {
                const auto temp =
                    Standard::Algorithms::Numbers::substring_of_minimum_positive_sum_slow(begin0, context.data, mid);

                if (0 < temp.sum && temp.sum < context.best.sum)
                {
                    context.best = temp;
                }
            }
        }

        if (mid + 1U < end1)
        {
            if (cutoff_size < end1 - mid - 1U)
            {
                conquer_mpsum_cutoff(mid + 1U, context, end1
#if _DEBUG
                    ,
                    depth
#endif
                );
            }
            else
            {
                const auto temp =
                    Standard::Algorithms::Numbers::substring_of_minimum_positive_sum_slow(mid + 1U, context.data, end1);

                if (0 < temp.sum && temp.sum < context.best.sum)
                {
                    context.best = temp;
                }
            }
        }

        prepare_after_mid(mid, context, end1);
        relax_begin_mid(begin0, context, end1);
    }

    void improve_mps(
        const std::vector<std::int32_t> &data, std::int64_t sum, Standard::Algorithms::Numbers::first_last_sum &best)
    {
        assert(0 < best.sum);

        std::set<std::pair<std::int64_t, std::size_t>> se1;

        se1.emplace(easy_search_sentinel, std::numeric_limits<std::size_t>::max() / 2);

        for (std::size_t index{}; index < data.size(); ++index)
        {
            constexpr std::int64_t one = 1;

            const auto &datum = data[index];
            const auto key = sum - datum + one;

            assert(-easy_search_sentinel < key && key < easy_search_sentinel);

            const auto iter = se1.lower_bound(std::pair(key, std::size_t{}));
            assert(iter != se1.end());

            if (const auto candidate = iter->first - (sum - datum); 0 < candidate && candidate < best.sum)
            {
                best.begin = iter->second;
                best.end = index + 1U;
                best.sum = static_cast<std::int32_t>(candidate);

                assert(best.begin < best.end && best.end <= data.size());
                if (best.sum == 1)
                {
                    break;
                }
            }

            se1.emplace(sum, index);
            sum -= datum;
        }
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Numbers::substring_of_minimum_positive_sum_slow(const std::size_t from,
    const std::vector<std::int32_t> &data, const std::size_t tod) -> Standard::Algorithms::Numbers::first_last_sum
{
    assert(from <= tod && tod <= data.size());

    first_last_sum result{};
    auto min_sum = std::numeric_limits<std::int64_t>::max();

    for (auto begin0 = from; begin0 < tod; ++begin0)
    {
        std::int64_t sum{};

        for (auto ind_2 = begin0; ind_2 < tod; ++ind_2)
        {
            sum += data[ind_2];

            if (sum <= 0 || !(sum < min_sum))
            {
                continue;
            }

            min_sum = sum;
            result.begin = begin0;
            result.end = ind_2 + 1U;
        }
    }

    if (min_sum == std::numeric_limits<std::int64_t>::max())
    {
        assert(result.sum == -1 && result.begin == 0 && result.end == 0);
    }
    else
    {
        assert(0 < min_sum && min_sum <= std::numeric_limits<std::int32_t>::max() && from <= result.begin &&
            result.begin < result.end && result.end <= tod);

        result.sum = static_cast<std::int32_t>(min_sum);
    }

    return result;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::substring_of_minimum_positive_sum_better(
    const std::vector<std::int32_t> &data) -> Standard::Algorithms::Numbers::first_last_sum
{
    const auto positive_iter = std::find_if(data.cbegin(), data.cend(),
        [] [[nodiscard]] (const auto &val)
        {
            return 0 < val;
        });

    if (positive_iter == data.cend())
    {
        return {};
    }

    mpsum_context context{ data };
    auto &best = context.best;
    assert(best.sum == -1);

    best.begin = positive_iter - data.cbegin();
    best.end = best.begin + 1U;
    best.sum = *positive_iter;

    assert(best.begin < best.end && best.end <= data.size() && 0 < best.sum);

    if (data.size() == 1U || context.best.sum == 1)
    {
        return best;
    }

    if (3U <= data.size())
    {
        context.buffer.reserve(data.size() - data.size() / 2 - 1U);
    }

    conquer_mpsum(0, context, data.size());

    assert(best.sum <= *positive_iter);
    assert(best.begin < best.end && best.end <= data.size() && 0 < best.sum);

    return best;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::substring_of_minimum_positive_sum_better2(
    const std::vector<std::int32_t> &data) -> Standard::Algorithms::Numbers::first_last_sum
{
    if (data.size() <= cutoff_size)
    {
        return substring_of_minimum_positive_sum_slow(0, data, data.size());
    }

    const auto positive_iter = std::find_if(data.cbegin(), data.cend(),
        [] [[nodiscard]] (const auto &val)
        {
            return 0 < val;
        });

    if (positive_iter == data.cend())
    {
        return {};
    }

    mpsum_context context{ data };
    auto &best = context.best;
    assert(best.sum == -1);

    best.begin = positive_iter - data.cbegin();
    best.end = best.begin + 1U;
    best.sum = *positive_iter;

    assert(best.begin < best.end && best.end <= data.size() && 0 < best.sum);

    if (data.size() == 1U || context.best.sum == 1)
    {
        return best;
    }

    if (3U <= data.size())
    {
        context.buffer.reserve(data.size() - data.size() / 2U - 1U);
    }

    conquer_mpsum_cutoff(0, context, data.size());

    assert(best.sum <= *positive_iter);
    assert(best.begin < best.end && best.end <= data.size() && 0 < best.sum);

    return best;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::substring_of_minimum_positive_sum(const std::vector<std::int32_t> &data)
    -> Standard::Algorithms::Numbers::first_last_sum
{
    {
        constexpr auto max_size = 1'000'000'000U;

        require_less_equal(data.size(), max_size, "size");
    }

    constexpr std::int64_t one = 1;
    constexpr auto upps = one + std::numeric_limits<std::int32_t>::max();

    static_assert(one < upps);

    std::int64_t sum{};
    auto best_sum = upps;
    std::size_t begin{};
    auto has_negative = false;

    for (std::size_t index{}; index < data.size(); ++index)
    {
        const auto &datum = data[index];
        sum += datum;

        if (datum < 0)
        {
            has_negative = true;
        }
        else if (0 < datum && datum < best_sum)
        {
            if (datum == 1)
            {
                return { index, index + 1U, 1 };
            }

            best_sum = datum;
            begin = index;
        }
    }

    if (const auto no_positive = best_sum == upps; no_positive)
    {
        return {};
    }

    first_last_sum best{ begin, begin + 1U, static_cast<std::int32_t>(best_sum) };

    assert(best.begin < best.end && best.end <= data.size() && 0 < best.sum);

    if (!has_negative || data.size() == 1U)
    {
        return best;
    }

    improve_mps(data, sum, best);

    assert(0 < best.sum && best.sum <= best_sum);
    assert(best.begin < best.end && best.end <= data.size());

    return best;
}
