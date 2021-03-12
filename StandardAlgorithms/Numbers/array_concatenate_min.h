#pragma once
#include"arithmetic.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Slow.
    [[nodiscard]] inline constexpr auto is_ab_less_ba_slow(const std::string &one, const std::string &two) noexcept(
        false) -> bool
    {
        const auto str_one = one + two;
        const auto str_two = two + one;
        auto result = str_one < str_two;
        return result;
    }

    // Whether a+b is less than than b+a.
    [[nodiscard]] inline constexpr auto is_ab_less_ba(const std::string &one, const std::string &two) noexcept(
        !::Standard::Algorithms::is_debug) -> bool
    {
        if (one.empty() || two.empty())
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(!is_ab_less_ba_slow(one, two));
            }

            return false;
        }

        const auto min_size = std::min(one.size(), two.size());
        const auto max_size = std::max(one.size(), two.size());

        {// Beginning
            const std::string_view one_v(one.data(), min_size);
            const std::string_view two_v(two.data(), min_size);

            const auto comp = one_v.compare(two_v);
            if (comp != 0 || min_size == max_size)
            {
                auto res1 = comp < 0;

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(res1 == is_ab_less_ba_slow(one, two));
                }

                return res1;
            }
        }

        const auto is_one_size_less = one.size() < two.size();

        const auto middle_size = max_size - min_size;
        assert(0U < middle_size && middle_size < max_size);

        {// Middle
            const auto &longest = is_one_size_less ? two : one;

            std::string_view forwarded(longest);
            forwarded.remove_prefix(min_size);

            const std::string_view one_v(is_one_size_less ? longest.data() : forwarded.data(), middle_size);

            const std::string_view two_v(!is_one_size_less ? longest.data() : forwarded.data(), middle_size);

            const auto comp = one_v.compare(two_v);
            if (comp != 0)
            {
                auto res1 = comp < 0;

                if constexpr (::Standard::Algorithms::is_debug)
                {
                    assert(res1 == is_ab_less_ba_slow(one, two));
                }

                return res1;
            }
        }
        {// Ending
            std::string_view temp_1{ two };
            std::string_view temp_2{ one };

            (is_one_size_less ? temp_1 : temp_2).remove_prefix(middle_size);

            const std::string_view one_v(temp_1.data(), min_size);
            const std::string_view two_v(temp_2.data(), min_size);

            const auto comp = one_v.compare(two_v);
            auto res1 = comp < 0;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(res1 == is_ab_less_ba_slow(one, two));
            }

            return res1;
        }
    }

    // Given an unsigned array {1, 123, 12 },
    // it must return a string "112123",
    // having minimum value after concatenating all array items.
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto array_concatenate_min(const std::vector<int_t> &data) -> std::string
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            assert_unsigned<int_t>("array_concatenate_min");
        }

        const auto size = data.size();
        if (0U == size)
        {
            return {};
        }

        std::vector<std::string> strings(size);

        for (std::size_t index{}; index < size; ++index)
        {
            strings[index] = std::to_string(data[index]);
        }

        std::sort(strings.begin(), strings.end(), is_ab_less_ba);

        auto str = ::Standard::Algorithms::Utilities::w_stream();

        for (std::size_t index{}; index < size; ++index)
        {
            str << strings[index];
        }

        return str.str();
    }
} // namespace Standard::Algorithms::Numbers
