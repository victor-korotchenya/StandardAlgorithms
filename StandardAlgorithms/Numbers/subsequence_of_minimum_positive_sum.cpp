#include"subsequence_of_minimum_positive_sum.h"
#include<algorithm>
#include<cassert>
#include<stdexcept>
#include<string>
#include<utility>

namespace
{
    struct mps_context final
    {
        const std::vector<std::int32_t> &data;

        std::int64_t current_sum{};
        std::vector<std::size_t> current_indexes{};

        std::int32_t minimum_positive_sum{};
        std::vector<std::size_t> minimum_chosen_indexes{};
    };

    constexpr auto max_data_size = 20U;

    constexpr void mps_visit(mps_context &context, std::size_t index = 0)
    {
        assert(index < context.data.size() && context.data.size() <= max_data_size);

        if (index + 1U < context.data.size())
        {// skip
            mps_visit(context, index + 1U);
        }

        // take
        context.current_indexes.push_back(index);
        context.current_sum += context.data[index];

        if (0 < context.current_sum && context.current_sum < context.minimum_positive_sum)
        {
            context.minimum_positive_sum = static_cast<std::int32_t>(context.current_sum);
            context.minimum_chosen_indexes.clear();
            context.minimum_chosen_indexes.insert(
                context.minimum_chosen_indexes.begin(), context.current_indexes.begin(), context.current_indexes.end());
        }

        if (index + 1U < context.data.size())
        {
            mps_visit(context, index + 1U);
        }

        context.current_indexes.pop_back();
        context.current_sum -= context.data[index];
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Numbers::subsequence_of_minimum_positive_sum_slow(
    const std::vector<std::int32_t> &data, std::vector<std::size_t> &chosen_indexes) -> std::int32_t
{
    chosen_indexes.clear();

    const auto positive_iter = std::find_if(data.cbegin(), data.cend(),
        [] [[nodiscard]] (const auto &val)
        {
            return 0 < val;
        });

    if (positive_iter == data.cend())
    {
        return -1;
    }

    // todo(p4): if no negatives, then can immediately return the minimum positive value.

    if (max_data_size < data.size()) [[unlikely]]
    {
        throw std::runtime_error(
            "subsequence_of_minimum_positive_sum_slow. Too large size " + std::to_string(data.size()));
    }

    mps_context context{ data };
    context.current_indexes.reserve(data.size());
    context.minimum_chosen_indexes.reserve(data.size());

    context.minimum_positive_sum = *positive_iter;
    context.minimum_chosen_indexes.push_back(positive_iter - data.cbegin());

    mps_visit(context); // todo(p4): go down from the size as it is cheaper to compare with 0.

    assert(0 < context.minimum_positive_sum && context.minimum_positive_sum <= *positive_iter &&
        !context.minimum_chosen_indexes.empty());

    assert(context.current_sum == 0 && context.current_indexes.empty());

    chosen_indexes.insert(
        chosen_indexes.begin(), context.minimum_chosen_indexes.begin(), context.minimum_chosen_indexes.end());

    return context.minimum_positive_sum;
}

// todo(p3): NP-Hard? [[nodiscard]] auto Standard::Algorithms::Numbers::subsequence_of_minimum_positive_sum(const
// std::vector<std::int32_t> &data,
//     std::vector<std::size_t> &chosen_indexes) -> std::int32_t
//{
//     return subsequence_of_minimum_positive_sum_slow(data, chosen_indexes);
// }
