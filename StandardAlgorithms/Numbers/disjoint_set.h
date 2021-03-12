#pragma once
#include"../Utilities/iota_vector.h"
#include"../Utilities/require_utilities.h"
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    struct disjoint_set_exception final : std::runtime_error
    {
        inline explicit disjoint_set_exception(const char *const &message)
            : std::runtime_error(message)
        {
        }

        inline explicit disjoint_set_exception(const std::string &message)
            : std::runtime_error(message)
        {
        }
    };

    // DSU, disjoint set union-find structure.
    // It is not thread-safe.
    template<std::integral int_t, bool is_printable = false>
    requires(sizeof(int_t) <= sizeof(std::size_t))
    struct disjoint_set final
    {
        constexpr explicit disjoint_set(const int_t &size)
            : Parents(Standard::Algorithms::Utilities::iota_vector<int_t>(require_positive(size, "size")))
            , Sizes(size, 1)
            , Nexts(is_printable ? Standard::Algorithms::Utilities::iota_vector<int_t>(size) : std::vector<int_t>{})
            , Disjoint_set_count(size)
        {
        }

        [[nodiscard]] constexpr auto count() const &noexcept -> const int_t &
        {
            return Disjoint_set_count;
        }

        [[nodiscard]] constexpr auto parent(int_t node) const -> int_t
        {
            check_node(node);

            // It should be faster on average then
            // setting the root for all predecessors.
            while (Parents[node] != node)
            {
                node = Parents[node] = Parents[Parents[node]];
            }

            return node;
        }

        // Some children 'Sizes' might get broken - not a big deal.
        constexpr void self_parent(const int_t &node)
        {
            check_node(node);

            const auto root = parent(node);
            if (root == node)
            {
                return;
            }

            if constexpr (is_printable)
            {
                throw std::runtime_error("Unsupported operation in a printable DSU: self_parent.");
            }

            assert(Sizes[node] < Sizes[root]);

            std::swap(Sizes[node], Sizes[root]);

            Parents[root] = node;
            Parents[node] = node;
        }

        // Prefer the one when both sizes are equal.
        [[maybe_unused]] constexpr auto unite(int_t one, int_t two) -> bool
        {
            assert(int_t{} < Disjoint_set_count);

            one = parent(one);
            two = parent(two);

            if (one == two)
            {
                return false;
            }

            if (Sizes[one] < Sizes[two])
            {
                std::swap(one, two);
            }

            Parents[two] = one;
            Sizes[one] += Sizes[two];

            --Disjoint_set_count;
            assert(int_t{} < Disjoint_set_count);

            if constexpr (is_printable)
            {
                // Before:
                // one -> next_1 -> .. -> one
                // two -> next_2 -> .. -> two
                auto next_1 = Nexts[one];
                auto next_2 = Nexts[two];
                // After:
                // one -> next_2 -> .. -> two -> next_1 -> .. -> one
                Nexts[one] = next_2;
                Nexts[two] = next_1;
            }

            return true;
        }

        [[nodiscard]] constexpr auto are_connected(int_t one, int_t two) const -> bool
        {
            one = parent(one);
            two = parent(two);

            auto conn = one == two;
            return conn;
        }

        template<class print_func_t, class = std::enable_if<is_printable>>
        constexpr void print_set(print_func_t func, const int_t &node) const
        {
            throw_if_null_skip_non_pointer("DSU print set function", func);
            check_node(node);

            [[maybe_unused]] std::uint64_t steps{};

            auto cur = node;

            do
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    if (Sizes.size() < ++steps) [[unlikely]]
                    {
                        auto err = "Too many steps(" + std::to_string(steps) +
                            ") in DSU print set - the Nexts might be broken.";

                        throw std::runtime_error(err);
                    }
                }

                assert(!(cur < int_t{}) && static_cast<std::size_t>(cur) < Nexts.size());

                func(cur);
                cur = Nexts.at(cur);
            } while (node != cur);
        }

private:
        constexpr void check_node(const int_t &node) const
        {
            assert(int_t{} < Disjoint_set_count);

            if (!(int_t{} <= node && node < static_cast<int_t>(Sizes.size()))) [[unlikely]]
            {
                auto err = "disjoint_set bad node " + std::to_string(node) + ".";

                throw disjoint_set_exception(err);
            }
        }

        mutable std::vector<int_t> Parents;
        std::vector<int_t> Sizes; // todo(p4): log2(size) uses fewer bits, but could be slower - measure?

        // todo(p4): del a member field when (!is_printable).
        std::vector<int_t> Nexts;

        int_t Disjoint_set_count;
    };
} // namespace Standard::Algorithms::Numbers
