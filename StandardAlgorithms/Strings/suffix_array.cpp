#include"suffix_array.h"
#include"../Numbers/logarithm.h"
#include"../Numbers/to_unsigned.h"
#include"../Utilities/check_size.h"
#include"../Utilities/floating_point_type.h"
#include"../Utilities/is_debug.h"
#include<algorithm>
#include<array>
#include<limits>
#include<numeric> // std::midpoint
#include<stdexcept>
#include<string_view>
#include<tuple>

#ifdef __GNUG__
// #include<parallel/algorithm> // __gnu_parallel::sort
#endif

namespace
{
    using c_t = unsigned char;
    using right_index_t = std::pair<std::int32_t, std::int32_t>;

    constexpr auto max_items = 256;

    // Time O(1).
    [[nodiscard]] inline constexpr auto require_small_size(const std::size_t size1) -> std::int32_t
    {
        constexpr auto sar_small_size = 2'000'000'000;
        constexpr auto can_be_maximum = true;

        static_assert(0 < sar_small_size);

        return Standard::Algorithms::Utilities::check_size<std::int32_t, sar_small_size>("size", size1, can_be_maximum);
    }

    // "abca" -> {{0,1,2,0}, 3}.
    // Time O(n).
    [[nodiscard]] constexpr auto compress_chars(const std::string_view str, const std::int32_t addon = 0)
        -> std::pair<std::vector<std::int32_t>, std::int32_t>
    {
        static_assert(1 == sizeof(char));

        const auto size = str.size();
        std::vector<std::int32_t> compressed(size);

        if (size <= 1U)
        {
            if (0U < size)
            {
                compressed[0] = addon;
            }

            return { std::move(compressed), static_cast<std::int32_t>(size) };
        }

        std::array<std::int32_t, max_items> mapped{ 0 };

        for (const auto &cha : str)
        {
            const auto pos = static_cast<c_t>(cha);
            mapped.at(pos) = 1;
        }

        std::int32_t domain_size{};

        for (auto &cnt : mapped)
        {
            if (0 == cnt)
            {
                continue;
            }

            assert(!(domain_size < 0) && domain_size < max_items);

            cnt = addon + domain_size;
            ++domain_size;
        }

        assert(0 < domain_size && domain_size <= max_items);

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &cha = str[index];
            const auto pos = static_cast<c_t>(cha);
            compressed[index] = mapped.at(pos);
        }

        return { std::move(compressed), domain_size };
    }

    constexpr void push_labels(const std::vector<std::int32_t> &compressed, const std::int64_t shift,
        std::vector<std::vector<right_index_t>> &labels)
    {
        const auto size = static_cast<std::int32_t>(compressed.size());
        assert(2 <= size && shift < size);

        std::int32_t index{};

        do
        {
            assert(0 < shift && index + shift < static_cast<std::int64_t>(compressed.size()));

            const auto &right = compressed.at(index + shift);
            const auto &left = compressed[index];
            labels.at(left).emplace_back(right, index);
        } while (++index + shift < size);

        do
        {
            assert(index < size);

            constexpr auto right = -1;
            const auto &left = compressed[index];
            labels.at(left).emplace_back(right, index);
        } while (++index < size);
    }

    [[nodiscard]] constexpr auto set_compressed_clear_labs(
        std::vector<std::int32_t> &compressed, std::vector<std::vector<right_index_t>> &labels) -> bool
    {
        const auto size = static_cast<std::int32_t>(compressed.size());
        assert(0 < size && labels.size() == compressed.size());

        auto are_labels_unique = true;

        for (std::int32_t counter = -1; auto &lab : labels)
        {
            if (lab.empty())
            {
                continue;
            }

            // todo(p3): To have O(n*log(n)) time, a O(n) counting/bucket sort must be used.
            std::sort(lab.begin(), lab.end());

            const auto labsize = lab.size();
            if (labsize != 1U)
            {
                are_labels_unique = false;
            }

            for (std::size_t ind2{}; ind2 < labsize; ++ind2)
            {
                // The labels is a 2D array, so it is already 'sorted' by left.
                const auto &[right, index] = lab[ind2];

                if (ind2 == 0U || lab[ind2 - 1U].first != right)
                {
                    ++counter;

                    assert(!(counter < 0) && counter < size);
                }

                compressed[index] = counter;
            }

            lab.clear();
        }

        return are_labels_unique;
    }

    constexpr void ensure_unique_labels(std::vector<std::int32_t> &compressed)
    {
        const auto size = static_cast<std::int32_t>(compressed.size());
        const auto maximum_iterations = Standard::Algorithms::Numbers::log_base_2_up(compressed.size());

        assert(2 <= size && size == static_cast<std::int64_t>(compressed.size()) && 0 < maximum_iterations);

        std::int64_t shift = 1;

        std::vector<std::vector<right_index_t>> labels(size);

        for (std::int32_t iteration{};; shift *= 2)
        {
            push_labels(compressed, shift, labels);

            const auto are_labels_unique = set_compressed_clear_labs(compressed, labels);

            if (are_labels_unique || !(++iteration < maximum_iterations))
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    [[maybe_unused]] const auto total_capacity =
                        std::accumulate(labels.cbegin(), labels.cend(), std::uint64_t{},
                            [] [[nodiscard]] (const std::uint64_t sum1, const auto &laba) constexpr
                            {
                                return sum1 + laba.capacity();
                            });

                    using floating_t = Standard::Algorithms::floating_point_type;

                    // It is seldom greater than (n**0.3 + 5).
                    // NOLINTNEXTLINE
                    const auto ration = static_cast<floating_t>(total_capacity) / static_cast<floating_t>(size);

                    // NOLINTNEXTLINE
                    if (1 < ration)
                    {
                    }
                }

                return;
            }
        }
    }

    constexpr void count_sort(const std::vector<std::int32_t> &str, const std::vector<std::int32_t> &source,
        const std::int32_t offset, const std::int32_t count, std::vector<std::int32_t> &temp_occurrences,
        const std::int32_t domain_size, std::vector<std::int32_t> &destination)
    {
        assert(0 < domain_size && !(offset < 0) && 0 < count && count <= static_cast<std::int32_t>(source.size()));

        constexpr auto zeros_could_be_at_end = 1;
        constexpr auto value_from_index_one = 1;
        temp_occurrences.assign(domain_size + value_from_index_one + zeros_could_be_at_end, 0);

        for (std::int32_t index{}; index < count; ++index)
        {
            const auto pos = source[index] + offset;
            assert(!(pos < 0) && pos < static_cast<std::int32_t>(str.size()));

            const auto &cha = str[pos];
            assert(!(cha < 0) && cha <= domain_size);

            ++temp_occurrences.at(cha + value_from_index_one);
        }

        constexpr auto next_index = 1;
        for (auto index = value_from_index_one + next_index; index <= domain_size; ++index)
        {
            temp_occurrences[index] += temp_occurrences[index - 1];
        }

        for (std::int32_t index{}; index < count; ++index)
        {
            const auto pos = source[index] + offset;
            const auto &cha = str[pos];
            auto &occur = temp_occurrences[cha];

            assert(!(occur < 0) && occur < static_cast<std::int32_t>(destination.size()));

            destination[occur++] = source[index];
        }
    }

    [[nodiscard]] constexpr auto remap(const std::int32_t n00, const std::vector<std::int32_t> &str,
        const std::int32_t n12, std::vector<std::int32_t> &temp_occurrences, const std::int32_t domain_size,
        std::vector<std::int32_t> &s12, std::vector<std::int32_t> &sa12) -> std::int32_t
    {
        assert(0 < n12 && n12 <= static_cast<std::int32_t>(sa12.size()) && 0 < domain_size);

        count_sort(str, s12, 2, n12, temp_occurrences, domain_size, sa12);
        count_sort(str, sa12, 1, n12, temp_occurrences, domain_size, s12);
        count_sort(str, s12, 0, n12, temp_occurrences, domain_size, sa12);

        std::int32_t changes{};
        auto c00 = -max_items - 1;
        std::int32_t c11{};
        std::int32_t c22{};

        for (std::int32_t index{}; index < n12; ++index)
        {
            const auto &val = sa12[index];

            assert(!(val < 0) && val < static_cast<std::int32_t>(str.size()) - 2);

            if (str[val] != c00 || str[val + 1] != c11 || str[val + 2] != c22)
            {
                ++changes;
                c00 = str[val];
                c11 = str[val + 1];
                c22 = str[val + 2];
            }

            const auto quot = val / 3;
            const auto ind = val % 3 == 1 ? quot : quot + n00;

            assert(!(ind < 0) && ind < static_cast<std::int32_t>(s12.size()));

            s12[ind] = changes;
        }

        assert(0 < changes);
        return changes;
    }

    constexpr void suffix_array_inner( // prototype
        std::int32_t size, const std::vector<std::int32_t> &str, std::int32_t domain_size,
        std::vector<std::int32_t> &sar);

    constexpr void compute_s12(const std::int32_t n12, const std::int32_t domain_size, std::vector<std::int32_t> &s12,
        std::vector<std::int32_t> &sa12)
    {
        assert(!(n12 < 0) && n12 <= static_cast<std::int32_t>(sa12.size()) && 0 < domain_size);

        if (const auto is_computed = domain_size == n12; is_computed)
        {
            for (std::int32_t index{}; index < n12; ++index)
            {
                const auto &val = s12[index];
                assert(0 < val && val - 1 < static_cast<std::int32_t>(sa12.size()));

                sa12[val - 1] = index;
            }

            return;
        }

        suffix_array_inner(n12, s12, domain_size, sa12);

        for (std::int32_t index{}; index < n12; ++index)
        {
            const auto &val = sa12[index];
            assert(!(val < 0) && val < static_cast<std::int32_t>(s12.size()));

            s12[val] = index + 1;
        }
    }

    constexpr void compute_s0(const std::vector<std::int32_t> &str, const std::int32_t n00,
        const std::vector<std::int32_t> &sa12, const std::int32_t n12, std::vector<std::int32_t> &temp_occurrences,
        const std::int32_t domain_size, std::vector<std::int32_t> &sa0)
    {
        assert(!(n00 < 0) && !(n12 < 0) && n12 <= static_cast<std::int32_t>(sa12.size()) && 0 < domain_size);

        std::vector<std::int32_t> s00(n00);

        for (std::int32_t index{}, ind2 = -1; index < n12; ++index)
        {
            const auto &val = sa12[index];
            if (val < n00)
            {
                assert(!(ind2 + 1 < 0) && ind2 < static_cast<std::int32_t>(s00.size()));

                s00[++ind2] = 3 * val;
            }
        }

        count_sort(str, s00, 0, n00, temp_occurrences, domain_size, sa0);
    }

    [[nodiscard]] constexpr auto compute_index(
        const std::int32_t pos, const std::vector<std::int32_t> &sa12, const std::int32_t size)
    {
        assert(!(pos < 0) && pos < static_cast<std::int32_t>(sa12.size()));

        const auto &val = sa12[pos];
        if (val < size)
        {
            return val * 3 + 1;
        }

        return (val - size) * 3 + 2;
    }

    [[nodiscard]] constexpr auto is_suffix12_before_s0(const std::int32_t n00, const std::vector<std::int32_t> &str,
        const std::int32_t index, const std::vector<std::int32_t> &s12, const std::int32_t ind2,
        const std::vector<std::int32_t> &sa12, const std::int32_t ind3) -> bool
    {
        assert(!(ind3 < 0) && ind3 < static_cast<std::int32_t>(sa12.size()));

        const auto &val = sa12[ind3];

        if (const auto use_v1 = val < n00; use_v1)
        {
            const std::tuple left1{ str.at(index), s12.at(val + n00) };
            const std::tuple right1{ str.at(ind2), s12.at(ind2 / 3) };

            return left1 <= right1;
        }
        {
            const std::tuple left1{ str.at(index), str.at(index + 1), s12.at(val - n00 + 1) };

            const std::tuple right1{ str.at(ind2), str.at(ind2 + 1), s12.at(ind2 / 3 + n00) };

            return left1 <= right1;
        }
    }

    constexpr void merge_sorted_suffixes(const std::vector<std::int32_t> &str, const std::int32_t n00,
        const std::vector<std::int32_t> &s12, const std::int32_t n12, const std::vector<std::int32_t> &sa0,
        std::int32_t pos0, const std::vector<std::int32_t> &sa12, std::vector<std::int32_t> &sar)
    {
        auto kkk = -1;
        auto ppp = 0;

        while (pos0 != n12 && ppp != n00)
        {
            assert(!(ppp < 0) && ppp < static_cast<std::int32_t>(sa0.size()));
            assert(!(kkk + 1 < 0) && kkk + 1 < static_cast<std::int32_t>(sar.size()));

            const auto index = compute_index(pos0, sa12, n00);
            const auto &ind2 = sa0[ppp];

            if (is_suffix12_before_s0(n00, str, index, s12, ind2, sa12, pos0))
            {
                sar[++kkk] = index;
                ++pos0;
            }
            else
            {
                sar[++kkk] = ind2;
                ++ppp;
            }
        }

        while (ppp < n00)
        {
            assert(!(kkk + 1 < 0) && kkk + 1 < static_cast<std::int32_t>(sar.size()));
            assert(!(ppp < 0) && ppp < static_cast<std::int32_t>(sa0.size()));

            sar[++kkk] = sa0[ppp++];
        }

        while (pos0 < n12)
        {
            assert(!(kkk + 1 < 0) && kkk + 1 < static_cast<std::int32_t>(sar.size()));

            sar[++kkk] = compute_index(pos0++, sa12, n00);
        }
    }

    constexpr void suffix_array_inner(const std::int32_t size, const std::vector<std::int32_t> &str,
        const std::int32_t domain_size, std::vector<std::int32_t> &sar)
    {
        assert(0 < size && 0 < domain_size);

        const auto third = size / 3;
        const auto remainder = size % 3;
        const auto has_remainder = remainder != 0 ? 1 : 0;
        const auto is_remainder_one = remainder == 1 ? 1 : 0;
        const auto n00 = third + has_remainder;

        // const auto n12 = (third * 2) | has_remainder;
        const auto n12 = Standard::Algorithms::Numbers::to_signed(Standard::Algorithms::Numbers::to_unsigned(third * 2) |
            Standard::Algorithms::Numbers::to_unsigned(has_remainder));

        std::vector<std::int32_t> s12(n12 + 3);
        std::vector<std::int32_t> sa12(n12 + 3);
        std::vector<std::int32_t> sa0(n00);

        for (auto index = 1, ind2 = -1; index < size + is_remainder_one; ++index)
        {
            if (index % 3 != 0)
            {
                assert(!(ind2 + 1 < 0) && ind2 + 1 < static_cast<std::int32_t>(s12.size()));

                s12[++ind2] = index;
            }
        }

        std::vector<std::int32_t> temp_occurrences;
        const auto domain_size2 = remap(n00, str, n12, temp_occurrences, domain_size, s12, sa12);

        compute_s12(n12, domain_size2, s12, sa12);
        compute_s0(str, n00, sa12, n12, temp_occurrences, domain_size, sa0);
        merge_sorted_suffixes(str, n00, s12, n12, sa0, is_remainder_one, sa12, sar);
    }

    constexpr void lcp_from_sa(
#if _DEBUG
        [[maybe_unused]] const std::string_view original_str,
#endif
        const std::vector<std::int32_t> &str, std::vector<std::int32_t> &lcp, const std::vector<std::int32_t> &sar)
    {
        const auto size = static_cast<std::int32_t>(sar.size());
        lcp.assign(size, 0);

        std::vector<std::int32_t> ranks(size);

        for (std::int32_t index{}; index < size; ++index)
        {
            const auto &val = sar[index];
            assert(!(val < 0) && val < size);

#if _DEBUG
            [[maybe_unused]] const auto subs = original_str.substr(val);
#endif

            ranks[val] = index;
        }

        for (std::int32_t index{}, matches{}; index < size; ++index)
        {
            const auto &rank = ranks[index];
            assert(0 <= rank && rank < size);

            if (rank == 0)
            {
                continue;
            }

            const auto &pre_ind = sar[rank - 1];
            const auto max_match = size - std::max(pre_ind, index);

#if _DEBUG
            [[maybe_unused]] const auto lefts = original_str.substr(pre_ind);
            [[maybe_unused]] const auto rights = original_str.substr(index);
#endif

            while (matches < max_match && str.at(index + matches) == str.at(pre_ind + matches))
            {
                ++matches;
            }

            lcp[rank] = matches;

            if (0 < matches)
            {
                --matches;
            }
        }
    }

    [[nodiscard]] constexpr auto is_text_suffix_greater(const std::string &text, const std::string &pattern,
        std::int32_t suffix_pos) -> std::pair<std::int32_t, std::int32_t>
    {
        const auto text_size = static_cast<std::int32_t>(text.size());
        const auto pat_size = static_cast<std::int32_t>(pattern.size());

        assert(0 < pat_size && pat_size <= text_size);

        for (std::int32_t pat_ind{};;)
        {
            assert(pat_ind < pat_size && suffix_pos < text_size);

            const auto &psu = pattern[pat_ind];
            const auto &tcha = text[suffix_pos];
            if (psu < tcha)
            {
                return { 1, 0 };
            }

            if (tcha < psu)
            {
                return { -1, 0 };
            }

            if (++pat_ind == pat_size)
            {
                const auto res = suffix_pos + 1 - pat_size;
                assert(!(res < 0) && res < text_size);

                return { 0, res };
            }

            if (++suffix_pos == text_size)
            {
                return { -1, 0 };
            }
        }
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Strings::suffix_array_slow(
    const std::string &str, std::vector<std::int32_t> *plcp) -> std::vector<std::int32_t>
{
    const auto size = require_small_size(str.size());
    const std::string_view view(str);
    std::vector<std::pair<std::string_view, std::int32_t>> pairs(size);

    for (std::int32_t index{}; index < size; ++index)
    {
        pairs[index] = std::make_pair(view.substr(index), index);
    }

    std::sort(pairs.begin(), pairs.end()); // This is slow.

    std::vector<std::int32_t> result(size);

    std::transform(pairs.begin(), pairs.end(), result.begin(),
        [] [[nodiscard]] (const auto &par) constexpr
        {
            return par.second;
        });

    if (plcp == nullptr)
    {
        return result;
    }

    auto &lcp = *plcp;
    lcp.assign(size, 0);

    for (auto index = 1; index < size; ++index)
    {
        const auto &prev = pairs[index - 1].first;
        const auto &cur = pairs[index].first;
        const auto min_size = static_cast<std::int32_t>(std::min(prev.size(), cur.size()));

        auto &prefix_length = lcp[index];

        while (prefix_length < min_size && prev[prefix_length] == cur[prefix_length])
        {
            ++prefix_length;
        }
    }

    return result;
}

[[nodiscard]] auto Standard::Algorithms::Strings::suffix_array_slow_still(const std::string &str)
    -> std::vector<std::int32_t>
{
    static_assert(1 == sizeof(char));

    const auto size = require_small_size(str.size());
    std::vector<std::int32_t> result(size);

    if (size <= 1)
    {
        return result;
    }

    auto compressed = compress_chars(str).first;
    ensure_unique_labels(compressed);

    for (std::int32_t index{}; index < size; ++index)
    {
        const auto &cha = compressed[index];
        assert(!(cha < 0) && cha < size);

        result.at(cha) = index;
    }

    return result;
}

void Standard::Algorithms::Strings::suffix_array_other(
    const std::string_view str, std::vector<std::int32_t> &sar, std::vector<std::int32_t> *const lcp)
{
    const auto size = require_small_size(str.size());
    if (size <= 1)
    {
        sar.assign(size, 0);

        if (lcp != nullptr)
        {
            lcp->assign(size, 0);
        }

        return;
    }

    constexpr auto extra = 3;
    constexpr auto ensure_no_zero_char_in_str = 1;

    auto temp = compress_chars(str, ensure_no_zero_char_in_str);

    auto &compressed_str = temp.first;
    compressed_str.resize(size + extra);
    sar.assign(size + extra, 0);

    const auto domain_size = temp.second + ensure_no_zero_char_in_str;
    suffix_array_inner(size, compressed_str, domain_size, sar);
    sar.resize(size);

    {
        constexpr auto sentinel_to_easy_compare = -1;
        compressed_str[size] = sentinel_to_easy_compare;
    }

    if (lcp == nullptr)
    {
        return;
    }

    lcp_from_sa(
#if _DEBUG
        str,
#endif
        compressed_str, *lcp, sar);
}

[[nodiscard]] auto Standard::Algorithms::Strings::suffix_array_search_slow(const std::string &text,
    const std::vector<std::int32_t> &suffix_array_of_text, const std::string &pattern) -> std::int32_t
{
    assert(text.size() == suffix_array_of_text.size());

    if (pattern.empty())
    {
        return {};
    }

    if (text.size() < pattern.size())
    {
        return -1;
    }

    const auto text_size = require_small_size(text.size());
    assert(0 < text_size);

    const auto pat_size = static_cast<std::int32_t>(pattern.size());
    assert(0 < pat_size && pat_size <= text_size && pattern.size() <= text.size());

    std::int32_t left{};
    auto right = text_size - 1;

    do
    {
        // [[assume(!(right < left))]];

        const auto mid = std::midpoint(left, right);
        const auto &suffix_pos = suffix_array_of_text[mid];
        const auto comp = is_text_suffix_greater(text, pattern, suffix_pos);
        if (comp.first == 0)
        {
            assert(!(comp.second < 0) && comp.second < text_size);

            return comp.second;
        }

        const auto is_text_greater = 0 < comp.first;
        if (is_text_greater)
        {
            right = mid - 1;
        }
        else
        {
            left = mid + 1;
        }
    } while (left <= right);

    return -1;
}
