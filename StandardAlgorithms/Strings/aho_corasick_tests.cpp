#include"aho_corasick_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"aho_corasick.h"
#include<unordered_set>

namespace
{
    constexpr auto min_size = 1;
    constexpr auto max_size = ::Standard::Algorithms::is_debug ? 10 : 20;

    constexpr auto test_keys_count = ::Standard::Algorithms::is_debug ? 8 : 32;
    constexpr auto max_short_length = 2;

    constexpr auto min_char = 'A';
    constexpr auto max_char = ::Standard::Algorithms::is_debug ? 'C' : 'Z';

    constexpr void add_short_keywords(
        std::unordered_set<std::string> &keywords, std::string &buffer, const std::int32_t length = max_short_length)
    {
        for (std::int32_t index = min_char; index <= max_char; ++index)
        {
            buffer.push_back(static_cast<char>(index));

            keywords.insert(buffer);

            if (1 < length)
            {
                add_short_keywords(keywords, buffer, length - 1);
            }

            buffer.pop_back();
        }
    }

    [[nodiscard]] auto build_keywords() -> std::unordered_set<std::string>
    {
        std::unordered_set<std::string> keywords;
        {
            std::string buffer;

            add_short_keywords(keywords, buffer);
        }

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        while (keywords.size() < test_keys_count)
        {
            keywords.emplace(
                Standard::Algorithms::Utilities::random_string<Standard::Algorithms::Utilities::random_t<std::int32_t>,
                    min_char, max_char>(rnd, min_size, max_size));
        }

        return keywords;
    }

    [[nodiscard]] constexpr auto build_description(const auto &keywords) -> std::string
    {
        std::string result("Aho-Corasick of ");
        result += std::to_string(keywords.size());
        result += ": ";

        for (const auto &key : keywords)
        {
            result += key;
            result += "\t";
        }

        result += ", text: ";
        return result;
    }

    [[nodiscard]] constexpr auto build_automaton(const auto &keywords)
        -> Standard::Algorithms::Strings::aho_corasick<std::string>
    {
        Standard::Algorithms::Strings::aho_corasick<std::string> aho(
            std::vector<std::string>(keywords.cbegin(), keywords.cend()));

        return aho;
    }

    constexpr void slow_find_expected(
        const auto &keywords, const std::string &text, std::vector<std::pair<std::string, std::int32_t>> &result)
    {
        result.clear();

        for (const auto &key : keywords)
        {
            auto index = std::string::size_type();

            for (;;)
            {
                index = text.find(key, index);

                if (std::string::npos == index)
                {
                    break;
                }

                result.emplace_back(key, static_cast<std::int32_t>(index));
                ++index;
            }
        }

        std::sort(result.begin(), result.end());
    }

    template<class key_t, class value_t>
    constexpr void ptr_to_value(const std::vector<std::pair<const key_t *, value_t>> &source,
        std::vector<std::pair<key_t, value_t>> &destination)
    {
        destination.clear();
        destination.reserve(source.size());

        std::transform(source.begin(), source.end(), std::back_inserter(destination),
            [] [[nodiscard]] (const std::pair<const key_t *, value_t> &pai)
            {
                return std::make_pair(*pai.first, pai.second);
            });

        std::sort(destination.begin(), destination.end());
    }
} // namespace

void Standard::Algorithms::Strings::Tests::aho_corasick_tests()
{
    // todo(p3): a class for the test cases.
    const auto keywords = build_keywords();
    const auto description = build_description(keywords);
    const auto aho = build_automaton(keywords);

    std::vector<std::pair<std::string, std::int32_t>> actual;
    auto expected = actual;

    std::vector<std::pair<const std::string *, std::int32_t>> aho_actual;
    Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

    for (const auto &key : keywords)
    {
        const auto text =
            Standard::Algorithms::Utilities::random_string<Standard::Algorithms::Utilities::random_t<std::int32_t>,
                min_char, max_char>(rnd, min_size, max_size) +
            key +
            Standard::Algorithms::Utilities::random_string<Standard::Algorithms::Utilities::random_t<std::int32_t>,
                min_char, max_char>(rnd, min_size, max_size);

        aho.find(text, aho_actual);
        ptr_to_value(aho_actual, actual);

        slow_find_expected(keywords, text, expected);
        ::Standard::Algorithms::ert::are_equal(expected, actual, description + text);
    }
}
