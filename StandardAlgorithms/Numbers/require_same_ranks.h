#pragma once
#include"../Utilities/iota_vector.h"
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    inline constexpr void validate_ranks(const std::size_t size, const std::vector<std::size_t> &ranks)
    {
        require_equal(size, "ranks size", ranks.size());

        if (0U == size)
        {
            return;
        }

        const auto &maxi = *std::max_element(ranks.begin(), ranks.end());
        require_greater(size, maxi, "max rank");

        std::vector<bool> bits(maxi + 1ZU);

        for (const auto &rank1 : ranks)
        {
#ifndef __clang__
            bits.at(rank1) = true;
#else
            // todo(p4): Remove when "constexpr at" in Clang.
            require_greater(maxi + 1ZU, rank1, "rank1");
            bits[rank1] = true;
#endif
        }

        if (const auto fiter = std::find(bits.begin(), bits.end(), false); fiter != bits.end()) [[unlikely]]
        {
            const auto rank1 = fiter - bits.begin();
            auto err = "The rank " + ::Standard::Algorithms::Utilities::zu_string(rank1) + " must have been used.";

            throw std::runtime_error(err);
        }
    }

    // Given {40, 10, 30, 40, 30 }, return ranks {2, 0, 1, 2, 1 }.
    // Compress or normalize values. See also compress_coordinates.
    // Expected time O(n*log(n)).
    [[nodiscard]] constexpr auto build_ranks(const auto &data) -> std::vector<std::size_t>
    {
        const std::size_t size = data.size();

        std::vector<std::size_t> ranks(size);

        auto indexes = ::Standard::Algorithms::Utilities::iota_vector<std::size_t>(size);

        std::sort(indexes.begin(), indexes.end(),
            [&data](const auto &index, const auto &index_2) -> bool
            {
                const auto &one = data[index];
                const auto &two = data[index_2];

                auto comp = one < two;
                return comp;
            });

        for (std::size_t pos = 1, rank1{}; pos < size; ++pos)
        {
            const auto &prev_index = indexes[pos - 1U];
            const auto &cur_index = indexes[pos];
            assert(prev_index < size && prev_index != cur_index && cur_index < size);

            const auto &pre_dator = data[prev_index];
            const auto &cur_ator = data[cur_index];
            assert(!(cur_ator < pre_dator));

            rank1 += pre_dator < cur_ator ? 1 : 0;
            ranks[cur_index] = rank1;
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            validate_ranks(size, ranks);
        }

        return ranks;
    }

    // Slow time O(n*n).
    [[nodiscard]] constexpr auto build_ranks_slow(const auto &data) -> std::vector<std::size_t>
    {
        const std::size_t size = data.size();

        std::vector<std::size_t> ranks(size);

        if (size < 2ZU)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                validate_ranks(size, ranks);
            }

            return ranks;
        }

        for (std::size_t rank1{},
             pre_pos = static_cast<std::size_t>(std::min_element(data.begin(), data.end()) - data.begin());
             ;)
        {
            assert(pre_pos < size);

            const auto &pre_dator = data[pre_pos];

            std::size_t greater_count{};
            std::size_t next_greater_ind{};

            for (std::size_t ind{}; ind < size; ++ind)
            {
                const auto &another = data[ind];
                if (another < pre_dator)
                {// Already processed.
                    continue;
                }

                if (pre_dator == another)
                {
                    ranks[ind] = rank1;
                    continue;
                }

                assert(pre_dator < another);

                if (0U == greater_count || another < data[next_greater_ind])
                {
                    next_greater_ind = ind;
                }

                ++greater_count;
            }

            if (0U == greater_count)
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    validate_ranks(size, ranks);
                }

                return ranks;
            }

            pre_pos = next_greater_ind;
            ++rank1;
        }
    }

    // Expected time O(n*log(n)).
    template<class first_t, class last_t>
    constexpr auto require_same_ranks(
        const first_t &first, const std::string &name, const last_t &last, const bool throw_on_error = true) -> bool
    {
        assert(!name.empty());

        {
            using first_pure = std::remove_cvref_t<first_t>;
            using last_pure = std::remove_cvref_t<last_t>;

            if constexpr (std::is_same_v<first_pure, last_pure>)
            {
                if (&first == &last)
                {
                    return true;
                }
            }
        }

        const std::size_t size = first.size();

        if (const auto last_size = last.size(); last_size != size) [[unlikely]]
        {
            if (throw_on_error)
            {
                require_equal(size, "Same rank sizes. " + name, last_size);

                std::unreachable();
            }

            return false;
        }

        if (size <= 1ZU)
        {
            return true;
        }

        const auto first_ranks = build_ranks(first);
        const auto last_ranks = build_ranks(last);

        if (first_ranks == last_ranks) [[likely]]
        {
            return true;
        }

        if (throw_on_error)
        {
            require_equal(first_ranks, "Same ranks. " + name, last_ranks);

            std::unreachable();
        }

        return false;
    }
} // namespace Standard::Algorithms::Numbers
