#pragma once
#include"print.h"
#include"w_stream.h"
#include<cstddef>
#include<stdexcept>
#include<vector>

namespace Standard::Algorithms
{
    struct check_matrix_options final
    {
        bool square{};
        bool some_rows{};
        bool some_columns{};
        bool symmetric{};
        bool throw_on_error = true;
    };

    inline constexpr void verify_options(const check_matrix_options &options
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        if (options.symmetric && !options.square) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "When check matrix options symmetric is set, the square must also be set to true.";

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw std::runtime_error(str.str());
        }
    }

    namespace Inner
    {
        [[maybe_unused]] constexpr auto check_symmetry(const auto &data, const check_matrix_options &options
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
                ) -> bool
        {
            assert(options.symmetric && options.square);

            const auto size = data.size();

            for (std::size_t row{}; row < size; ++row)
            {
                const auto &ones = data[row];

                for (std::size_t column{}; column < row; ++column)
                {
                    const auto &one = ones[column];
                    const auto &two = data[column][row];

                    if (one == two) [[likely]]
                    {
                        continue;
                    }

                    if (!options.throw_on_error)
                    {
                        return false;
                    }

                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Broken symmetry: Data[" << row << ", " << column << "] is \n"
                        << one << " must be equal to \n"
                        << two << " which is Data[" << column << ", " << row << "].";

#ifndef __clang__
                    Standard::Algorithms::Utilities::print(str, loc);
#endif

                    throw std::runtime_error(str.str());
                }
            }

            return true;
        }
    } // namespace Inner

    // The 'data' must have the same sub-matrix sizes.
    template<class collection_t, class item_t = typename collection_t::value_type>
    [[maybe_unused]] constexpr auto require_matrix(
        const std::vector<collection_t> &data, const check_matrix_options &options = {}
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
            ) -> bool
    {
        verify_options(options
#ifndef __clang__
            ,
            loc
#endif
        );

        if (data.empty())
        {
            if (!options.some_rows && !options.some_columns) [[likely]]
            {
                return true;
            }

            if (!options.throw_on_error)
            {
                return false;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The matrix cannot be empty.";

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw std::runtime_error(str.str());
        }

        if (options.some_columns && data[0].empty()) [[unlikely]]
        {
            if (!options.throw_on_error)
            {
                return false;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The matrix must have at least 1 column.";

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw std::runtime_error(str.str());
        }

        const auto size = data.size();
        const auto required_row_size = options.square ? size : data[0].size();

        for (std::size_t row{}; row < size; ++row)
        {
            const auto row_size = data[row].size();

            if (required_row_size == row_size) [[likely]]
            {
                continue;
            }

            if (!options.throw_on_error)
            {
                return false;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            if (options.square)
            {
                str << "The row size " << row_size << " at row " << row << " must be equal to data size " << size
                    << ".";
            }
            else
            {
                str << "The row size " << row_size << " at row " << row << " must match the first row size "
                    << required_row_size << ".";
            }

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw std::runtime_error(str.str());
        }

        auto good = !options.symmetric ||
            Inner::check_symmetry(data, options
#ifndef __clang__
                ,
                loc
#endif
            );
        return good;
    }
} // namespace Standard::Algorithms
