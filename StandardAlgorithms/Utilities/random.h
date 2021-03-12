#pragma once
// "random.h"
#include"../Numbers/arithmetic.h"
#include"random_generator.h"
#include<array>
#include<span>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Utilities
{
    template<class item_t, std::size_t extent = std::dynamic_extent>
    requires(0U < extent)
    constexpr void shuffle_data(std::span<item_t, extent> spa)
    {
        auto size = spa.size();
        if (size-- <= 1U)
        {
            return;
        }

        // It cannot be const.
        // NOLINTNEXTLINE
        random_t<std::size_t> rnd{};

        std::size_t index{};

        do
        {
            const auto random_index = rnd(index, size);
            std::swap(spa[index], spa[random_index]);
        } while (++index < size);
    }

    template<class item_t, std::size_t size>
    requires(0U < size)
    constexpr void shuffle_data(std::array<item_t, size> &arr)
    {
        std::span spa{ arr };
        shuffle_data<item_t, size>(spa);
    }

    template<class collection_t, class item_t = typename collection_t::value_type>
    constexpr void shuffle_data(collection_t &coll)
    {
        std::span spa(coll.begin(), coll.end());

        shuffle_data<item_t>(spa);
    }

    // It requires knowing the source size, which cannot be less than the sub_size.
    // See also std::sample
    template<class src_t, class dest_t>
    constexpr void random_sample(const src_t &source, const std::size_t sub_size, dest_t &destination)
    {
        destination.clear();

        if (sub_size == 0U)
        {
            return;
        }

        const std::size_t size = source.size();
        require_less_equal(sub_size, size, "sub size");

        auto max_index = size - sub_size;
        random_t<std::size_t> rnd(0U, max_index);
        std::vector<bool> chosen(size);

        do
        {
            auto index = rnd();

            if (chosen.at(index))
            {
                assert(index < max_index);
                index = max_index;
            }

            assert(index < size && !chosen.at(index));
            chosen.at(index) = true;

            destination.push_back(source.at(index));
        } while (++max_index < size);

        assert(destination.size() == sub_size);
    }

    void fill_random(std::vector<bool> &data, std::size_t size);

    void fill_random(std::size_t rows, std::vector<std::vector<bool>> &data, std::size_t columns);

    // todo(p2): prevent an endless loop throw e.g. when trying to generate 257 unique char values.
    template<class item_t,
        // It can be used to create negative values
        // e.g. when item_t is "std::int64_t", the RandomType must be "std::int32_t",
        // or otherwise the numbers will be non-negative.
        class random_type = item_t>
    constexpr void fill_random(std::vector<item_t> &data, const std::size_t size,
        // The cutter can be used to create datasets of smaller range.
        // I f (0 != cutter), the each added item will be taken modulo "cutter".
        const item_t &cutter = {},
        // Whether to generate only unique data.
        const bool is_unique_only = {})
    {
        data.clear();

        if (size == 0U)
        {
            return;
        }

        const auto has_cutter = item_t{} != cutter;
        if (has_cutter)
        {
            if constexpr (std::is_signed_v<item_t>)
            {
                if (Standard::Algorithms::Numbers::is_negative<item_t>(cutter))
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The cutter(" << cutter << ") must be positive.";
                    throw_exception(str);
                }
            }

            if (is_unique_only && static_cast<std::size_t>(cutter) < size)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Cannot make unique data of size " << size << " with the cutter " << cutter << ".";

                throw_exception(str);
            }
        }

        const random_type min_inclusive = has_cutter
            ? (std::numeric_limits<random_type>::is_signed ? random_type(1) - random_type(cutter) : random_type())
            : std::numeric_limits<random_type>::min();

        const random_type max_inclusive =
            has_cutter ? random_type(cutter) - random_type(1) : std::numeric_limits<random_type>::max();

        random_t<random_type> random_generator(min_inclusive, max_inclusive);

        data.resize(size);

        std::unordered_set<item_t> unique_items;
        std::size_t index{};

        do
        {
            for (auto &item = data[index];;)
            {
                const random_type random_value = random_generator();
                item = static_cast<item_t>(random_value);

                if (is_unique_only)
                {
                    const auto added = unique_items.insert(item);
                    if (!added.second)
                    {// Skip duplicates.
                        continue;
                    }
                }

                break;
            }
        } while (++index < size);
    }

    template<class random_t, char min_char = '0', char max_char = '9', class string_t = std::string>
    requires(min_char <= max_char)
    [[nodiscard]] constexpr auto random_string(
        random_t &rnd, const std::int32_t min_size = 1, const std::int32_t max_size = 10) -> string_t
    {
        assert(0 <= min_size && min_size <= max_size);

        const auto size = rnd(min_size, max_size);
        string_t val(size, 0);

        for (std::int32_t index{}; index < size; ++index)
        {
            val[index] = static_cast<char>(rnd(min_char, max_char));
        }

        return val;
    }

    template<class random_t, class int_t = typename random_t::int_t>
    [[nodiscard]] constexpr auto random_vector(random_t &rnd, const std::int32_t min_size = 1,
        const std::int32_t max_size = 10, const int_t min_value = {}, const int_t max_value = 10) -> std::vector<int_t>
    {
        assert(0 <= min_size && min_size <= max_size && min_value <= max_value);

        const auto size = rnd(min_size, max_size);
        std::vector<int_t> vec(size);

        const auto gen = [&]
        {
            return static_cast<int_t>(rnd(min_value, max_value));
        };
        std::generate(vec.begin(), vec.end(), gen);

        return vec;
    }

    // In case of failure, return an empty string.
    template<class random_t, class set_t, char min_char = '0', char max_char = '9', class string_t = std::string>
    [[nodiscard]] constexpr auto random_string_unique(random_t &rnd, set_t &uniq, const std::int32_t min_size = 1,
        const std::int32_t max_size = 10, const std::int32_t attempts = 10) -> string_t
    {
        assert(0 < min_size && min_size <= max_size && 0 < attempts);

        for (std::int32_t att{}; att < attempts; ++att)
        {
            auto str = random_string<random_t, min_char, max_char, string_t>(rnd, min_size, max_size);

            if (uniq.insert(str).second)
            {
                return str;
            }
        }

        return {};
    }
} // namespace Standard::Algorithms::Utilities
