#pragma once
#include"../Utilities/require_utilities.h"
#include"sparse_table_log_and_size_validate.h"
#include<array>
#include<bit>
#include<numeric>

namespace Standard::Algorithms::Numbers
{
    // Given an array A, find the min value index in A[start..stop) in O(1) time.
    template<class value_t, std::size_t log_size_max, std::size_t size_max = 1LLU << log_size_max>
    requires(sparse_table_log_and_size_validate(log_size_max, size_max))
    struct sparse_table_min final
    {
        using size_t1 = std::uint32_t; // Large enough for 29U.

        static constexpr size_t1 one = 1;

        [[nodiscard]] constexpr auto size() const noexcept -> size_t1
        {
            return Size;
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::array<value_t, size_max> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto data() &noexcept -> std::array<value_t, size_max> &
        {
            return Data;
        }

        // Fill in the 'data' array before.
        // Time O(n*log(n)).
        constexpr void init(const size_t1 size)
        {
            require_between(one, size, size_max, "size");
            this->Size = size;

            std::iota(Indexes[0].begin(), Indexes[0].begin() + size, size_t1{});

            for (auto logar = one; (one << logar) <= Size; ++logar)
            {
                const auto length = one << (logar - one);
                const auto max_index = Size - (one << logar);
                const auto &prevs = Indexes.at(logar - one);
                auto &currs = Indexes.at(logar);

                for (size_t1 index{}; index <= max_index; ++index)
                {
                    const auto &pre = prevs.at(index);
                    const auto &next = prevs.at(index + length);
                    currs.at(index) = Data.at(next) < Data.at(pre) ? next : pre;
                }
            }
        }

        // Return the index (not value).
        // Time O(1).
        [[nodiscard]] constexpr auto range_minimum_query_index(
            const size_t1 &start, const size_t1 &stop) const & -> const size_t1 &
        {
            assert(start < stop && stop <= Size);

            const auto loga = static_cast<size_t1>(log_base_2_down(stop - start));
            const auto &inds = Indexes.at(loga);
            const auto middle = stop - (one << loga);
            const auto &id1 = inds.at(start);
            const auto &id2 = inds.at(middle);

            return Data.at(id2) < Data.at(id1) ? id2 : id1;
        }

private:
        //  To be initialized later.
        std::array<value_t, size_max> Data{};
        std::array<std::array<size_t1, size_max>, log_size_max> Indexes{};

        size_t1 Size{};
    };
} // namespace Standard::Algorithms::Numbers
