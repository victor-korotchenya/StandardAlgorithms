#include"elevator_min_moves.h"
#include"../Utilities/require_utilities.h"
#include"to_unsigned.h"
#include<bit>
#include<numeric>

namespace
{
    using large_t = std::int64_t;

    using min_moves_remaining_weight_t = std::pair<std::int32_t, large_t>;

    constexpr auto max_size = 30;

    constexpr auto inf1 = std::numeric_limits<std::int32_t>::max();

    constexpr auto infinity = static_cast<large_t>(max_size * 2LL * static_cast<large_t>(inf1));

    static_assert(inf1 < infinity);

    constexpr void emm_validate(const std::vector<std::int32_t> &weights, const large_t &max_weight)
    {
        Standard::Algorithms::require_non_negative(max_weight, "max weight");

        {
            const auto *const name = "the number of people";

            const auto size = Standard::Algorithms::require_positive(weights.size(), name);

            Standard::Algorithms::require_greater(static_cast<std::size_t>(max_size), size, name);
        }

        const auto [min_it, max_it] = std::minmax_element(weights.cbegin(), weights.cend());

        {
            const auto &min = *min_it;

            Standard::Algorithms::require_non_negative(min, "weight");
        }
        {
            const auto &top = *max_it;

            Standard::Algorithms::require_less_equal(top, max_weight, "weight");
        }
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Numbers::elevator_min_moves(
    const std::vector<std::int32_t> &weights, const large_t max_weight) -> std::int32_t
{
    emm_validate(weights, max_weight);

    const auto total_sum = std::accumulate(weights.cbegin(), weights.cend(), large_t{});

    if (total_sum <= max_weight)
    {
        return 1;
    }

    const auto size = static_cast<std::int32_t>(weights.size());
    const auto edge_mask = 1U << weights.size();

    assert(0 < total_sum && 2 <= size && size <= max_size && 0U < edge_mask && max_weight < infinity);

    std::vector<min_moves_remaining_weight_t> buffer(edge_mask);

    buffer[0].first = 1;

    for (auto mask = 1U; mask < edge_mask; ++mask)
    {
        auto &buf = buffer[mask];

        buf = { max_size + 1, infinity };

        auto cop = mask;

        do
        {// Go thru only the set bits in the mask.
            const auto index = std::countr_zero(cop);
            const auto bit = 1U << Standard::Algorithms::Numbers::to_unsigned(index);

            assert(0U < bit && bit <= cop && cop <= mask && (mask & bit) != 0U && 0 <= index && index < size);

            cop ^= bit;

            const auto &without_current = buffer[mask ^ bit];

            auto best = without_current;
            auto &min_moves = best.first;
            auto &remaining_weight = best.second;

            const auto &current_weight = weights[index];

            if (const auto cand = remaining_weight + current_weight; max_weight < cand)
            {
                ++min_moves;
                remaining_weight = current_weight;
            }
            else
            {
                remaining_weight = cand;
            }

            assert(0 < min_moves && min_moves <= size && 0 <= remaining_weight && remaining_weight <= max_weight);

            buf = std::min(buf, best);

            assert(0 < buf.first && buf.first <= size && 0 <= buf.second && buf.second <= max_weight);
        } while (0U < cop);
    }

    auto &result = buffer.back().first;

    assert(0 < result && total_sum / max_weight <= result && result <= size);

    return result;
}
