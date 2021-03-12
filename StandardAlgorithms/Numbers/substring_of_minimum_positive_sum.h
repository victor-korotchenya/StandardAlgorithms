#pragma once
#include<cstddef>
#include<cstdint>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    struct first_last_sum final
    {
        std::size_t begin{};
        std::size_t end{};
        std::int32_t sum = -1;
    };

    // Minimum positive substring sum: choose a consecutive substring so that
    // the sum of its numbers is minimal and positive.
    // Return sum = -1 when no positive number.
    // For example, given {-9, -8, 10}, return sum=2, begin=1, end=3. Here 2 = -8 + 10.
    // Overall limitations: the data size must not exceed 1E9, especially for the slow version.
    //
    // Time O(n*n).
    [[nodiscard]] auto substring_of_minimum_positive_sum_slow(
        std::size_t from, const std::vector<std::int32_t> &data, std::size_t tod) -> first_last_sum;

    // Time O(n*log(n)*log(n)).
    // 150 times faster than 'slow' on 50'000 numbers.
    [[nodiscard]] auto substring_of_minimum_positive_sum_better(const std::vector<std::int32_t> &data)
        -> first_last_sum;

    // Calls the slow version for small sizes.
    // It is about 1.06 times faster than 'better'.
    // Time O(n*log(n)*log(n)).
    [[nodiscard]] auto substring_of_minimum_positive_sum_better2(const std::vector<std::int32_t> &data)
        -> first_last_sum;

    // Time O(n*log(n)).
    // It is about 1.5 times faster than 'better2' on 50K size.
    //
    // Sample data = {  200,     -20,      10,       50,     -32, -1200}.
    // Right sums   = { -992, -1192, -1172, -1182, -1232, -1200}.
    // When processing 50,
    // the sums from 10 to left 200 are: {190, -10, 10}. Compare these
    // to the first 3 right sums { -992, -1192, -1172} with -1182 subtracted:
    //  190 = -992 - (-1182); .. ; 10 = -1172 - (-1182).
    // Thus, the algorithm is:
    //  1) Compute right to left partial sums.
    //  2) Process the numbers from left to right:
    //  2.1) Try the value at data[i] as the sum.
    //  2.2) Find the lower bound to the left of i, excluding i,
    //         for the key = right_sums[i] - data[i] + 1,
    //         try a candidate = bound - right_sums[i] + data[i].
    //         For example, for i=3, data[i] = 50,
    //         key = -1182 - 50 + 1= -1231,
    //         and lower bound for key among { -992, -1192, -1172} is -1192.
    //         The candidate = -1192 - (-1182) + 50 = 40 is the best sum found so far.
    // Answer.sum = -20 + 10 + 50 -32 = 8.
    [[nodiscard]] auto substring_of_minimum_positive_sum(const std::vector<std::int32_t> &data) -> first_last_sum;

    // todo(p2): faster using e.g. Boas Tree.
} // namespace Standard::Algorithms::Numbers
