#pragma once
#include"../Numbers/matrix_antidiagonal.h"
#include"../Utilities/compute_core_count.h"
#include"../Utilities/is_debug.h"
#include<algorithm>
#include<cassert>
#include<cstddef>
#include<stdexcept>
#include<vector>

namespace Standard::Algorithms::Strings::Inner
{
    constexpr auto is_lcs_test = true;

    constexpr std::size_t profitable_lcs = // NOLINTNEXTLINE
        (is_lcs_test ? 2U : 32U);

    static_assert(0U < profitable_lcs);

    template<class string_t>
    inline constexpr void lcs_relax_cell(const auto &f_i, const auto &prevs, const string_t &second,
        const std::size_t column, std::vector<std::size_t> &curs) noexcept
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(column < second.size());
        }

        const auto &s_j = second[column];
        auto &cur = curs[column + 1U];

        if (f_i == s_j)
        {
            const auto &pre = prevs[column];
            cur = pre + 1U;
            // max_length = std::max(max_length, cur);
        }
        else
        {
            const auto &top = prevs[column + 1U];
            const auto &left = curs[column];
            cur = std::max(left, top);
        }
    }

    template<class string_t>
    inline constexpr void lcs_relax_cell_stencil(
        const auto &f_i, const std::size_t row, const string_t &second, const std::size_t column, auto &buffer) noexcept
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert(column < second.size());
        }

        const auto &s_j = second[column];
        auto &cur = buffer[row + 1U, column + 1U];

        if (f_i == s_j)
        {
            const auto &pre = buffer[row, column];
            cur = pre + 1U;
            // max_length = std::max(max_length, cur);
        }
        else
        {
            const auto &top = buffer[row, column + 1U];

            // The top position will be 1 less than that of the left.
            const auto &left = buffer[row + 1U, column];
            cur = std::max(left, top);
        }
    }

    template<class string_t>
    constexpr void lcs_calculate_matix(
        const string_t &first, std::vector<std::vector<std::size_t>> &buffer, const string_t &second) noexcept
    {
        // std::size_t max_length{};

        for (std::size_t row{}; row < first.size(); ++row)
        {
            const auto &f_i = first[row];
            const auto &prevs = buffer[row];
            auto &curs = buffer[row + 1U];

            for (std::size_t column{}; column < second.size(); ++column)
            {
                lcs_relax_cell<string_t>(f_i, prevs, second, column, curs);
            }
        }

        // return max_length;
    }

    template<class string_t, class subsequence_t>
    constexpr void lcs_backtrack(const string_t &first, const std::vector<std::vector<std::size_t>> &buffer,
        const string_t &second, subsequence_t &subsequence)
    {
        auto first_size = first.size();
        auto second_size = second.size();
        auto length_left = buffer[first_size][second_size];

        if (length_left == 0U)
        {
            return;
        }

        subsequence.resize(length_left);

        do
        {
            assert(0U < first_size && 0U < second_size);

            while (length_left == buffer[first_size - 1U][second_size - 1U])
            {// Go by diagonal until a smaller length is encountered.
                assert(1U < first_size && 1U < second_size);

                --first_size;
                --second_size;
            }

            assert(0U < first_size);

            while (length_left == buffer[first_size][second_size - 1U])
            {// Go left.
                assert(1U < second_size);

                --second_size;
            }

            while (length_left == buffer[first_size - 1U][second_size])
            {// Go up.
                assert(1U < first_size);

                --first_size;
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (first_size == 0U || second_size == 0U || first[first_size - 1U] != second[second_size - 1U])
                    [[unlikely]]
                {
                    throw std::runtime_error("first[first_size - 1] != second[second_size - 1] in lcs_backtrack.");
                }
            }

            subsequence[--length_left] = first[first_size - 1U];
        } while (0U < length_left);
    }

    template<class string_t, class subsequence_t>
    constexpr void lcs_backtrack_ant(
        const string_t &first, const auto &buffer, const string_t &second, subsequence_t &subsequence)
    {
        auto first_size = first.size();
        auto second_size = second.size();
        auto length_left = buffer[first_size, second_size];

        if (length_left == 0U)
        {
            return;
        }

        subsequence.resize(length_left);

        do
        {
            assert(0U < first_size && 0U < second_size);

            while (length_left == buffer[first_size - 1U, second_size - 1U])
            {// Go by diagonal until a smaller length is encountered.
                assert(1U < first_size && 1U < second_size);

                --first_size;
                --second_size;
            }

            assert(0U < first_size);

            while (length_left == buffer[first_size, second_size - 1U])
            {// Go left.
                assert(1U < second_size);

                --second_size;
            }

            while (length_left == buffer[first_size - 1U, second_size])
            {// Go up.
                assert(1U < first_size);

                --first_size;
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (first_size == 0U || second_size == 0U || first[first_size - 1U] != second[second_size - 1U])
                    [[unlikely]]
                {
                    throw std::runtime_error("first[first_size - 1] != second[second_size - 1] in lcs_backtrack.");
                }
            }

            subsequence[--length_left] = first[first_size - 1U];
        } while (0U < length_left);
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    // This is not the fastest way to compute LCS.
    // Can be used to find 'Approximate string matching' with optional 'max errors' number >= 0.
    template<class string_t, class subsequence_t>
    constexpr void longest_common_subsequence(const string_t &first, const string_t &second, subsequence_t &subsequence)
    {
        subsequence.clear();

        if (first.empty() || second.empty())
        {
            return;
        }

        // todo(p3): can be implemented using O(n) size - use crossing idea.
        std::vector<std::vector<std::size_t>> buffer(first.size() + 1U, std::vector<std::size_t>(second.size() + 1U));

        Inner::lcs_calculate_matix<string_t>(first, buffer, second);

        Inner::lcs_backtrack<string_t, subsequence_t>(first, buffer, second, subsequence);
    }

    // Tested on 10K strings:
    // - Without OpenMP, it is about 3 times slower then the sequential code.
    // - With OpenMP and 4K profitable size, it is still 1.5 times slower.
    template<class string_t, class subsequence_t>
    void parallel_longest_common_subsequence(
        const string_t &raw_first, const string_t &raw_second, subsequence_t &subsequence)
    {
        subsequence.clear();

        const auto is_sec_less = raw_second.size() < raw_first.size();
        const auto &first = is_sec_less ? raw_second : raw_first;
        const auto &second = !is_sec_less ? raw_second : raw_first;

        const auto min_size = static_cast<std::size_t>(first.size());
        if (0ZU == min_size)
        {
            return;
        }

        const auto max_size = static_cast<std::size_t>(second.size());
        const auto edge_size = min_size - 1ZU + max_size;
        assert(min_size <= max_size);

        std::vector<std::vector<std::size_t>> buffer(min_size + 1ZU, std::vector<std::size_t>(max_size + 1ZU, 0ZU));

        const auto cores = Standard::Algorithms::Utilities::compute_core_count(min_size);
        assert(0U < cores);

        // Let |first| = 3, |second| = 6. Compute by an anti-diagonal:
        // 0`
        //
        // 0 1`
        // 1`
        // ..
        // 0 1 2 3 4 5
        // 1 2 3 4 5 6
        // 2 3 4 5 6 7`
        for (std::size_t diag{}; diag < edge_size; ++diag)
        {
            const auto tyapka = std::min(diag + 1ZU, min_size);
            const auto count = std::min(tyapka, edge_size - diag);
            [[maybe_unused]] const auto parallel_threads = count < Inner::profitable_lcs ? single_core : cores;

#pragma omp parallel for default(none) shared(first, second, buffer, diag, max_size, tyapka) \
    num_threads(parallel_threads) schedule(static)
            for (auto row = diag < max_size ? 0ZU : diag + 1ZU - max_size; row < tyapka; ++row)
            {
                const auto &f_i = first[row];
                const auto column = diag - row;
                const auto &prevs = buffer[row];
                auto &curs = buffer[row + 1U];
                Inner::lcs_relax_cell<string_t>(f_i, prevs, second, column, curs);
            }
        }

        Inner::lcs_backtrack<string_t, subsequence_t>(first, buffer, second, subsequence);
    }

    // todo(p3): split the 2D-array grid into 32-by-32 black clouds; once a dark cloud is computed,
    // its lower and righter counterparts can be activated then.
    // todo(p3): del OMP, and explicitly manage the tasks, removing false-sharing.

    // Tested on 10K strings:
    // - Without OpenMP, it is about 1.2 times slower then the sequential code.
    // - With OpenMP and 32 profitable size, it is sometimes 1.3 times faster.
    template<class string_t, class subsequence_t>
    void parallel_longest_common_subsequence_ant(
        const string_t &raw_first, const string_t &raw_second, subsequence_t &subsequence)
    {
        subsequence.clear();

        const auto is_sec_less = raw_second.size() < raw_first.size();
        const auto &first = is_sec_less ? raw_second : raw_first;
        const auto &second = !is_sec_less ? raw_second : raw_first;

        const auto min_size = static_cast<std::size_t>(first.size());
        if (0ZU == min_size)
        {
            return;
        }

        const auto max_size = static_cast<std::size_t>(second.size());
        const auto edge_size = min_size - 1ZU + max_size;
        assert(min_size <= max_size);

        Standard::Algorithms::Numbers::matrix_antidiagonal<std::size_t> buffer(min_size + 1ZU, max_size + 1ZU);
        buffer.resize_antidiagonal(0);
        buffer.resize_antidiagonal(1);

        const auto cores = Standard::Algorithms::Utilities::compute_core_count(min_size);
        assert(0U < cores);

        // Let |first| = 3, |second| = 6. Compute by an anti-diagonal:
        // 0`
        //
        // 0 1`
        // 1`
        // ..
        // 0 1 2 3 4 5
        // 1 2 3 4 5 6
        // 2 3 4 5 6 7`
        for (std::size_t diag{}; diag < edge_size; ++diag)
        {
            {
                constexpr auto extra_rows_left_plus_top = 2ZU;

                buffer.resize_antidiagonal(diag + extra_rows_left_plus_top);
            }

            const auto start_row = diag < max_size ? 0ZU : 1ZU + diag - max_size;
            const auto tyapka = std::min(diag + 1ZU, min_size);
            assert(0U < tyapka && tyapka < tyapka + 1ZU);

            const auto count = std::min(tyapka, edge_size - diag);
            [[maybe_unused]] const auto parallel_threads = count < Inner::profitable_lcs ? single_core : cores;

#pragma omp parallel for default(none) shared(first, second, buffer, diag, max_size, start_row, tyapka) \
    num_threads(parallel_threads) schedule(static)
            for (auto row = start_row; row < tyapka; ++row)
            {
                const auto &f_i = first[row];
                const auto column = diag - row;
                Inner::lcs_relax_cell_stencil<string_t>(f_i, row, second, column, buffer);
            }
        }

        Inner::lcs_backtrack_ant<string_t, subsequence_t>(first, buffer, second, subsequence);
    }
} // namespace Standard::Algorithms::Strings
