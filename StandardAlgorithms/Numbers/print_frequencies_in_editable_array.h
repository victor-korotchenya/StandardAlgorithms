#pragma once
#include"../Utilities/require_utilities.h"
#include<cmath> // abs
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an editable array A of signed numbers, where the elements are from 1 to A.size,
    // print the frequencies (occurrences) of the elements using O(1) memory.
    struct print_frequencies_in_editable_array final
    {
        print_frequencies_in_editable_array() = delete;

        template<std::signed_integral int_t>
        requires(sizeof(int_t) <= sizeof(std::size_t))
        static constexpr void print_frequencies(std::vector<int_t> &numbers) noexcept(false)
        {
            const auto size = check_input_return_size<int_t>(numbers);
            compute<int_t>(numbers, size);
            take_absolute<int_t>(numbers);
        }

private:
        template<std::signed_integral int_t>
        [[nodiscard]] static constexpr auto check_input_return_size(const std::vector<int_t> &numbers) -> int_t
        {
            {
                const auto size = require_positive(numbers.size(), "size");

                constexpr auto edge_size = static_cast<std::size_t>(std::numeric_limits<int_t>::max());

                static_assert(0U < edge_size);

                require_greater(edge_size, size, "size");
            }

            const auto size = require_positive(static_cast<int_t>(numbers.size()), "size");

            const auto bad_iter = std::find_if(numbers.cbegin(), numbers.cend(),
                [size](const auto &numb)
                {
                    constexpr int_t zero{};

                    return !(zero < numb) || size < numb;
                });

            if (bad_iter == numbers.cend())
            {
                return size;
            }

            const auto index = bad_iter - numbers.cbegin();
            const auto &viola = numbers.at(index);

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The number " << viola << " at index " << index << " must be in the range [1, " << size << "].";

            throw_exception(str);
        }

        template<std::signed_integral int_t>
        static constexpr void compute(std::vector<int_t> &numbers, const int_t &size)
        {
            constexpr int_t zero{};
            constexpr int_t one = 1;

            assert(zero < size);

            int_t index{};

            do
            {
                auto &numb = numbers.at(index);

                if (zero < numb)
                {
                    const auto expected_index = static_cast<int_t>(numb - one);
                    auto &expected = numbers.at(expected_index);

                    if (zero < expected)
                    {
                        numb = expected;
                        expected = -one;
                        continue;
                    }

                    --expected;
                    assert(expected < zero);

                    numb = zero;
                }

                ++index;
            } while (index < size);
        }

        template<std::signed_integral int_t>
        static constexpr void take_absolute(std::vector<int_t> &numbers)
        {
            for (auto &numb : numbers)
            {
                constexpr int_t zero{};

                numb = static_cast<int_t>(std::abs(numb));

                assert(!(numb < zero));
            }
        }
    };
} // namespace Standard::Algorithms::Numbers
