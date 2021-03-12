#pragma once
#include"../Utilities/require_utilities.h"

namespace Standard::Algorithms::Numbers
{
    // Given a set of n>0 coins, each having >0 value,
    // you make the first step meaning either left-most or right-most coin is opened;
    // Then the enemy makes his step; and so on until there are coins.
    // The aim is to maximize your total value.
    template<class int_t>
    struct coin_line_max_sum final
    {
        coin_line_max_sum() = delete;

        [[nodiscard]] static constexpr auto min_guaranteed_value_fast(const std::vector<int_t> &values) -> int_t
        {
            throw_if_empty("values", values);

            const auto size = values.size();

            {
                buffer_t buffer_del(size);
                // todo(p4): only validate.
                validate_and_set_diagonal(values, buffer_del);
            }

            if (1U == size)
            {
                return values[0];
            }

            // Assuming the player 2 plays efficiently,
            // the buffer[x][y] is the first player's maximum for coins from x to y.
            std::vector<std::vector<int_t>> buffer(size - 1U, std::vector<int_t>(size, int_t{}));

            auto sum = values.back();

            {
                constexpr auto delta = 1U;

                for (std::size_t index{}; index < size - delta; ++index)
                {
                    buffer[index][index + delta] = std::max(values[index], values[index + delta]);

                    sum += values[index];
                }
            }

            for (std::size_t delta = 3; delta <= size; delta += 2U)
            {
                for (std::size_t xxx{}, yyy = delta; yyy < size; ++xxx, ++yyy)
                {
                    //[x][y - 2]                  [x][y]
                    //            [x + 1][y - 1]
                    //                            [x + 2][y]
                    const auto bottom = static_cast<int_t>(values[xxx] +
                        std::min( // Enemy chooses next after our choice.
                            buffer[xxx + 2U][yyy],
                            // ...          before y.
                            buffer[xxx + 1U][yyy - 1U]));

                    const auto left = static_cast<int_t>(values[yyy] +
                        std::min( // Enemy chooses next after x.
                            buffer[xxx + 1U][yyy - 1U],
                            // ...          before our choice.
                            buffer[xxx][yyy - 2U]));

                    buffer[xxx][yyy] = std::max( // We select either the x.
                        bottom,
                        // or the y.
                        left);
                }
            }

            if (const auto is_odd = 0U != (size & 1U); is_odd)
            {
                const auto &enemy_value1 = buffer[0][size - 2U];
                const auto &enemy_value2 = buffer[1][size - 1U];

                auto result = static_cast<int_t>(std::max(sum - enemy_value1, sum - enemy_value2));

                return result;
            }

            const auto &result = buffer[0][size - 1U];
            return result;
        }

        // Slow.
        [[nodiscard]] static constexpr auto min_guaranteed_value_slow(const std::vector<int_t> &values) -> int_t
        {
            throw_if_empty("values", values);

            const auto size = values.size();
            buffer_t buffer(size);

            // 5,  8,  2       given, sum=15
            //  5/8  2/8       our min/max
            //    8/7          result=7

            // a/b              c/d
            //   min(b,d) / sum - min(b,d)

            // 8,  2,  1       given, sum=11
            //  2/8  1/2       our min/max
            //    2/9          result=9
            //
            // 5,  8,  2,  1   given, sum=16
            //  5/8  2/8  1/2  our min/max
            //    8/7  2/9
            //      7/9        they = (sum-result)=7, result = max(5+2, 1+8)=9.
            //
            // 5,  8,  2,  1,   4   given, sum=20
            //  5/8  2/8  1/2  1/4  our min/max
            //    8/7  2/9  2/5
            //      7/9  5/10
            //       9/11           they = (sum-result)=9, result = max(5+5, 4+7)=11.
            validate_and_set_diagonal(values, buffer);

            auto sum = values[0];

            for (std::size_t delta = 1; delta < size; ++delta)
            {
                sum += values[delta];

                const auto new_size = size - delta;
                buffer[delta].resize(new_size);

                for (std::size_t start_index{}; start_index < new_size; ++start_index)
                {
                    const auto left =
                        static_cast<int_t>(values[start_index] + buffer[delta - 1U][start_index + 1U].first);

                    const auto right =
                        static_cast<int_t>(values[start_index + delta] + buffer[delta - 1U][start_index].first);

                    const auto t_max = std::max(left, right);

                    const auto region_sum = static_cast<int_t>(
                        buffer[delta - 1U][start_index + 1U].first + buffer[delta - 1U][start_index + 1U].second);

                    const auto t_min = static_cast<int_t>(values[start_index] + region_sum - t_max);

                    buffer[delta][start_index] = std::make_pair(t_min, t_max);
                }
            }

            auto result = static_cast<int_t>(sum - buffer[size - 1U][0].first);

            if (const auto &result_two = buffer[size - 1U][0].second; result_two != result)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();

                str << "There might have been an overflow: resultTwo " << result_two << " != result " << result
                    << " for size " << size << ".";

                throw_exception(str);
            }

            return result;
        }

private:
        using buffer_t = std::vector<std::vector<std::pair<int_t, int_t>>>;

        static constexpr void validate_and_set_diagonal(const std::vector<int_t> &values, buffer_t &buffer)
        {
            const auto size = values.size();
            assert(0U < size);

            auto &buf = buffer.at(0);
            buf.resize(size);

            constexpr int_t zero{};

            for (std::size_t index{}; index < size; ++index)
            {
                const auto &value = values[index];

                if (value <= zero)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error: value " << value << " at index " << index << " must be positive.";

                    throw_exception(str);
                }

                // todo(p3): avoid copying.
                buf[index].second = value;
            }
        }
    };
} // namespace Standard::Algorithms::Numbers
