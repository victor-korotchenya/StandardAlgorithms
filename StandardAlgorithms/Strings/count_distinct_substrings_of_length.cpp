#include"count_distinct_substrings_of_length.h"
#include"../Utilities/require_utilities.h"
#include<set>

namespace
{
    template<std::int64_t base, std::int64_t mod>
    requires(1 < base && 1 < mod)
    [[nodiscard]] constexpr auto calc_powers(const std::int32_t size) -> std::vector<std::int32_t>
    {
        assert(0 < size);

        std::vector<std::int32_t> result(size + 1LL);
        result.at(0) = 1;

        for (std::int32_t index{}; index < size; ++index)
        {
            result[index + 1] = static_cast<std::int32_t>(result[index] * base % mod);
        }

        return result;
    }

    template<std::int64_t base, std::int64_t mod>
    requires(1 < base && 1 < mod)
    [[nodiscard]] constexpr auto calc_hashes(const std::string &str) -> std::vector<std::int32_t>
    {
        const auto size = static_cast<std::int32_t>(str.size());
        std::vector<std::int32_t> result(size + 1LL);

        for (std::int32_t index{}; index < size; ++index)
        {
            result[index + 1] = static_cast<std::int32_t>((result[index] * base + str[index] + mod) % mod);
        }

        return result;
    }

    template<std::int64_t mod>
    requires(1 < mod)
    [[nodiscard]] constexpr auto calc_hash(const std::vector<std::int32_t> &hashes,
        const std::vector<std::int32_t> &powers, const std::int32_t from_inc, const std::int32_t to_exc) -> std::int32_t
    {
        assert(0 <= from_inc && from_inc < to_exc);
        assert(to_exc < static_cast<std::int32_t>(hashes.size()));

        const auto mul = static_cast<std::int64_t>(powers[to_exc - from_inc]) * hashes[from_inc] % mod;

        auto dif = (hashes[to_exc] + mod - mul) % mod;
        assert(0 != from_inc || hashes[to_exc] == dif);

        return static_cast<std::int32_t>(dif);
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Strings::count_distinct_substrings_of_length(
    const std::string &str, const std::vector<std::int32_t> &lengths) -> std::vector<std::int32_t>
{
    const auto str_size = require_positive(static_cast<std::int32_t>(str.size()), "string size");

    const auto result_size = require_positive(static_cast<std::int32_t>(lengths.size()), "lengths size");

    constexpr std::int64_t base = 999999937;
    constexpr std::int64_t mod = static_cast<std::int32_t>(1e9) + 7;

    const auto powers = calc_powers<base, mod>(str_size);
    const auto hashes = calc_hashes<base, mod>(str);

    std::vector<std::int32_t> result(result_size);
    std::set<std::int32_t> uniq{};

    for (std::int32_t index{}; index < result_size; ++index)
    {
        const auto &len = lengths[index];
        const auto max_start = str_size - len;

        for (std::int32_t start{}; start <= max_start; ++start)
        {
            const auto hash = calc_hash<mod>(hashes, powers, start, start + len);
            uniq.insert(hash);
        }

        result[index] = static_cast<std::int32_t>(uniq.size());
        uniq.clear();
    }

    return result;
}

[[nodiscard]] auto Standard::Algorithms::Strings::count_distinct_substrings_of_length_slow(
    const std::string &str, const std::vector<std::int32_t> &lengths) -> std::vector<std::int32_t>
{
    const auto str_size = require_positive(static_cast<std::int32_t>(str.size()), "string size");

    const auto result_size = require_positive(static_cast<std::int32_t>(lengths.size()), "lengths size");

    std::vector<std::int32_t> result(result_size);
    std::set<std::string> uniq{}; // todo(p3): std::string_view

    for (std::int32_t index{}; index < result_size; ++index)
    {
        const auto &len = lengths[index];
        const auto max_start = str_size - len;

        for (std::int32_t start{}; start <= max_start; ++start)
        {
            uniq.insert(str.substr(start, len));
        }

        result[index] = static_cast<std::int32_t>(uniq.size());
        uniq.clear();
    }

    return result;
}
