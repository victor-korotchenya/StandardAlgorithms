#pragma once
#include<algorithm>
#include<cassert>
#include<cstddef>
#include<functional>
#include<stdexcept>
#include<string>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Trees
{
    // Only for small volume tests. No duplicates.
    // Order statistic tree has extra find ops:
    // - key with given rank.
    // - rank of given key.
    template<class key_t, class less_t = std::less<key_t>>
    struct order_statistics_tree_slow final
    {
        constexpr explicit order_statistics_tree_slow(less_t less = {})
            : Less(less)
        {
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<key_t> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Data.size();
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool
        {
            return size() == 0U;
        }

        [[nodiscard]] constexpr auto lower_bound_rank(const key_t &key) -> std::pair<key_t, std::size_t>
        {
            if (is_empty())
            {
                return {};
            }

            sort_data();

            auto ite = std::lower_bound(Data.cbegin(), Data.cend(), key, Less);
            const auto rank1 = static_cast<std::size_t>(ite - Data.cbegin());

            if (ite == Data.cend())
            {
                --ite;
            }

            return { *ite, rank1 };
        }

        [[nodiscard]] constexpr auto contains(const key_t &key) -> bool
        {
            return !is_empty() && lower_bound_rank(key).first == key;
        }

        [[nodiscard]] constexpr auto select(std::size_t rank1) & -> const key_t &
        {
            if (const auto size1 = size(); !(rank1 < size1)) [[unlikely]]
            {
                throw std::runtime_error(
                    "Invalid rank " + std::to_string(rank1) + " for size " + std::to_string(size1) + ".");
            }

            sort_data();

            return Data[rank1];
        }

        constexpr auto insert(const key_t &key) -> bool
        {
            if (const auto ite = find_helper(key); ite != Data.cend())
            {
                return false;
            }

            Data.push_back(key);

            return true;
        }

        constexpr auto erase(const key_t &key) -> bool
        {
            auto ite = find_helper(key);

            if (ite == Data.cend())
            {
                return false;
            }

            Data.erase(ite);

            return true;
        }

        constexpr void clear() noexcept
        {
            Data.clear();
        }

private:
        [[nodiscard]] constexpr auto find_helper(const key_t &key) const noexcept
        {
            const auto pred = [key, this](const key_t &other)
            {
                const auto equ = this->are_equal(key, other);
                return equ;
            };

            auto ite = std::find_if(Data.cbegin(), Data.cend(), pred);
            return ite;
        }

        constexpr void sort_data()
        {
            std::sort(Data.begin(), Data.end(), Less);
        }

        [[nodiscard]] constexpr auto are_equal(const key_t &one, const key_t &two) const noexcept -> bool
        {
            const auto equ = !Less(one, two) && !Less(two, one);
            return equ;
        }

        std::vector<key_t> Data{};
        less_t Less;
    };
} // namespace Standard::Algorithms::Trees
