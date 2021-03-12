#include"adjacent_divide_permutation.h"
#include"../Utilities/iota_vector.h"
#include"number_utilities.h" // sum_modulo
#include"shift.h"

namespace
{
    constexpr auto n_max = 40;

    [[nodiscard]] constexpr auto can_use(
        const std::int32_t prev, const std::vector<char> &allowed, const std::int32_t cur) -> bool
    {
        Standard::Algorithms::require_positive(prev, "prev");

        const auto can = cur % prev == 0 && allowed.at(cur / prev) != 0;
        return can;
    }

    [[nodiscard]] constexpr auto adj_div_perm_main_cycle(const std::int32_t size, const std::vector<char> &allowed,
        const std::int32_t mod, const bool is_debug) -> std::vector<std::int32_t>
    {
        assert(0 < size && size <= n_max && 0 < mod);

        const auto half = (size + 1) / 2;

        const auto presize = (1LLU << ::Standard::Algorithms::Numbers::to_unsigned(half)) | 1U;
        std::vector<std::int32_t> cur(presize);

        std::vector<std::int32_t> prev(cur.size());

        cur[0] = 1; // Empty mask.

        std::vector<std::vector<std::int32_t>> debug_details;
        if (is_debug)
        {
            debug_details.reserve(size + 1);
            debug_details.push_back(cur);
        }

        for (auto tod = 2; tod <= size; ++tod)
        {// (2**size * size**2) main cycle.
            std::swap(cur, prev);
            std::fill(cur.begin(), cur.end(), 0);

            const auto max_mask = ::Standard::Algorithms::Numbers::shift_left(1U, std::min(tod, half));

            for (std::uint32_t mask{}; mask < max_mask; ++mask)
            {
                const auto &old = prev[mask];
                if (old == 0)
                {
                    continue;
                }

                auto &val0 = cur[mask];
                Standard::Algorithms::Numbers::sum_modulo(old, val0, mod);

                for (auto from = 1; from < tod; ++from)
                {
                    auto new_mask = mask;

                    if (from <= n_max / 2)
                    {// Avoid 2**40 time.
                        const auto from_mask = ::Standard::Algorithms::Numbers::shift_left(1U, from - 1);
                        const auto is_valid = can_use(from, allowed, tod);

                        is_valid ? new_mask |= from_mask : new_mask &= ~from_mask;
                    }

                    assert(new_mask < max_mask);

                    auto &val = cur[new_mask];
                    Standard::Algorithms::Numbers::sum_modulo(old, val, mod);
                }
            }

            if (is_debug)
            {
                debug_details.push_back(cur);
            }
        }

        return cur;
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Numbers::adjacent_divide_permutation(const std::int32_t size,
    const std::vector<char> &allowed, const std::int32_t mod, const bool is_debug) -> std::vector<std::int32_t>
{
    require_positive(size, "size");
    require_positive(mod, "mod");
    require_less_equal(size, n_max, "size");

    auto cur = adj_div_perm_main_cycle(size, allowed, mod, is_debug);

    const auto half = (size + 1) / 2;
    const auto edge_mask = ::Standard::Algorithms::Numbers::shift_left(1, half);

    std::vector<std::int32_t> sol(size);

    for (std::int32_t mask{}; mask < edge_mask; ++mask)
    {
        assert(mask < edge_mask);

        const auto &old = cur[mask];
        if (old == 0)
        {
            continue;
        }

        const auto cnt = __builtin_popcount(mask);
        auto &val = sol.at(cnt);
        sum_modulo(old, val, mod);
    }

    return sol;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::adjacent_divide_permutation_slow(
    const std::int32_t size, const std::vector<char> &allowed, const std::int32_t mod) -> std::vector<std::int32_t>
{
    require_positive(size, "size");
    require_positive(mod, "mod");

    {
        constexpr auto small_n_max = 20;
        require_less_equal(size, small_n_max, "size");
    }

    std::vector<std::int32_t> ans(size);
    auto temp = ::Standard::Algorithms::Utilities::iota_vector<std::int32_t>(size, 1);

    do
    {
        std::int32_t cost{};

        for (std::int32_t index{}; index < size - 1; ++index)
        {
            const auto &prev = temp[index];
            const auto &cur = temp[index + 1];
            assert(0 < prev);

            cost += can_use(prev, allowed, cur) ? 1 : 0;
        }

        auto &val = ans[cost];

        if (!(++val < mod))
        {
            val -= mod;
        }
    } while (std::next_permutation(temp.begin(), temp.end()));

    return ans;
}
