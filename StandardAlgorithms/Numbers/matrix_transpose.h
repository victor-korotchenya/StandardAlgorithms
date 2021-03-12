#pragma once
#include"../Utilities/is_debug.h"
#include"../Utilities/throw_exception.h"
#include<concepts>
#include<cstddef>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    template<class item_t, class dest_item_t = item_t>
    requires(std::convertible_to<item_t, dest_item_t>)
    [[nodiscard]] constexpr auto matrix_transpose(const std::vector<item_t> &source)
        -> std::vector<std::vector<dest_item_t>>
    {
        const auto size = source.size();
        std::vector<std::vector<dest_item_t>> transposed(size);

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &item = source[index];
            transposed[index].push_back(static_cast<dest_item_t>(item));
        }

        return transposed;
    }

    // The 'source' must have the same sub-matrix sizes for each row.
    template<class item_t, class dest_item_t>
    requires(std::convertible_to<item_t, dest_item_t>)
    constexpr void matrix_transpose(
        const std::vector<std::vector<item_t>> &source, std::vector<std::vector<dest_item_t>> &destination)
    {
        const auto size = source.size();
        if (0U == size)
        {
            destination.clear();
            return;
        }

        const auto row_size = source[0].size();
        destination.resize(row_size);

        for (auto &items : destination)
        {
            items.resize(size);
        }

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &data_row = source[index];

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (const auto row_size2 = data_row.size(); row_size != row_size2)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Rows must have the same size, but first row size " << row_size << " != row size "
                        << row_size2 << " at index " << index << ".";

                    throw_exception(str);
                }
            }

            for (std::size_t ind_2{}; ind_2 < row_size; ++ind_2)
            {
                destination[ind_2][index] = static_cast<dest_item_t>(data_row[ind_2]);
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
