#pragma once
#include<algorithm>
#include<cassert>
#include<cmath>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<functional>
#include<tuple>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    [[nodiscard]] inline constexpr auto combine_hashes( // NOLINTNEXTLINE
        std::size_t one, std::size_t two) noexcept -> std::size_t
    {
        constexpr auto shift = 5U;

        auto three = (one | (one << shift)) ^ two;
        return three;
    }

    [[nodiscard]] inline constexpr auto combine_hashes( // NOLINTNEXTLINE
        std::size_t one, std::size_t two, // NOLINTNEXTLINE
        std::size_t three) noexcept -> std::size_t
    {
        const auto four = combine_hashes(one, two);
        auto five = combine_hashes(four, three);
        return five;
    }

    [[nodiscard]] inline constexpr auto combine_hashes( // NOLINTNEXTLINE
        std::size_t one, std::size_t two, // NOLINTNEXTLINE
        std::size_t three, std::size_t four) noexcept -> std::size_t
    {
        const auto five = combine_hashes(one, two);
        const auto six = combine_hashes(three, four);
        auto seven = combine_hashes(five, six);
        return seven;
    }

    template<std::unsigned_integral int_t, std::floating_point floating_t>
    [[nodiscard]] constexpr auto multiplication_method_hash(
        const int_t &prehash, const floating_t &coefficient, const int_t &size) noexcept -> int_t
    {
        assert(floating_t{} < coefficient && coefficient < 1 && int_t{} < size);

        const auto product = static_cast<floating_t>(static_cast<floating_t>(prehash) * coefficient);

        floating_t integer_part;
        const auto fractional_part = std::modf(product, &integer_part);

        auto almost = std::floor(size * fractional_part);

        auto hashimir = static_cast<int_t>(almost);
        assert(hashimir < size);

        return hashimir;
    }

    // std::unordered_set<std::pair<ping, pint>, Standard::Algorithms::Numbers::pair_hash> set1;
    struct pair_hash final
    {
        template<class r, class s>
        [[nodiscard]] constexpr auto operator() (const std::pair<r, s> &par) const noexcept -> std::size_t
        {
            const auto one = std::hash<r>()(par.first);
            const auto two = std::hash<s>()(par.second);
            auto three = combine_hashes(one, two);
            return three;
        }
    };

    struct tuple_hash final // todo(p3): C++23 tuple hash.
    {
        template<class r, class s>
        [[nodiscard]] constexpr auto operator() (const std::tuple<r, s> &tup) const noexcept -> std::size_t
        {
            const auto one = std::hash<r>()(std::get<0>(tup));
            const auto two = std::hash<s>()(std::get<1>(tup));
            auto three = combine_hashes(one, two);
            return three;
        }

        template<class r, class s, class t>
        [[nodiscard]] constexpr auto operator() (const std::tuple<r, s, t> &tup) const noexcept -> std::size_t
        {
            const auto one = std::hash<r>()(std::get<0>(tup));
            const auto two = std::hash<s>()(std::get<1>(tup));
            const auto three = std::hash<t>()(std::get<2>(tup));
            auto four = combine_hashes(one, two, three);
            return four;
        }

        template<class r, class s, class t, class u>
        [[nodiscard]] constexpr auto operator() (const std::tuple<r, s, t, u> &tup) const noexcept -> std::size_t
        {
            const auto one = std::hash<r>()(std::get<0>(tup));
            const auto two = std::hash<s>()(std::get<1>(tup));
            const auto three = std::hash<t>()(std::get<2>(tup));
            const auto four = std::hash<u>()(std::get<3>(tup));
            auto five = combine_hashes(one, two, three, four);
            return five;
        }

        template<class r, class s, class t, class u, class v>
        [[nodiscard]] constexpr auto operator() (const std::tuple<r, s, t, u, v> &tup) const noexcept -> std::size_t
        {
            const auto one = std::hash<r>()(std::get<0>(tup));
            const auto two = std::hash<s>()(std::get<1>(tup));
            const auto three = std::hash<t>()(std::get<2>(tup));
            const auto four = std::hash<u>()(std::get<3>(tup));
            const auto five = std::hash<v>()(std::get<4>(tup));
            const auto six = combine_hashes(one, two, three, four);
            auto seven = combine_hashes(five, six);
            return seven;
        }

        template<class r, class s, class t, class u, class v, class w>
        [[nodiscard]] constexpr auto operator() (const std::tuple<r, s, t, u, v, w> &tup) const noexcept -> std::size_t
        {
            const auto one = std::hash<r>()(std::get<0>(tup));
            const auto two = std::hash<s>()(std::get<1>(tup));
            const auto three = std::hash<t>()(std::get<2>(tup));
            const auto four = std::hash<u>()(std::get<3>(tup));
            const auto five = std::hash<v>()(std::get<4>(tup));
            const auto six = std::hash<w>()(std::get<5>(tup));
            const auto seven = combine_hashes(one, two, three, four);
            auto eight = combine_hashes(five, six, seven);
            return eight;
        }
    };

    // std::unordered_set<std::vector<std::int32_t>, Standard::Algorithms::Numbers::vector_hash>;
    struct vector_hash final
    {
        template<class item_t>
        [[nodiscard]] constexpr auto operator() (const std::vector<item_t> &vec) const noexcept -> std::size_t
        {
            std::size_t res{};
            auto hash = std::hash<item_t>();

            for (const auto &item : vec)
            {
                const auto has = hash(item);
                res = combine_hashes(res, has);
            }

            return res;
        }
    };
} // namespace Standard::Algorithms::Numbers
