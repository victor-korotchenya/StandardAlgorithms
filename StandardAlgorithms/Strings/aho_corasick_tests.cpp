#include <unordered_set>
#include "aho_corasick.h"
#include "../Utilities/PrintUtilities.h"
#include "aho_corasick_tests.h"
#include "../test_utilities.h"
#include "../Utilities/Random.h"

using namespace std;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    constexpr auto min_size = 1;
    constexpr auto min_char = 'A';
#ifdef _DEBUG
    constexpr auto max_size = 10;
    constexpr auto max_char = 'C';
    constexpr auto count = 8;
#else
    constexpr auto max_size = 20;
    constexpr auto max_char = 'Z';
    constexpr auto count = 32;
#endif

    void add_short_keywords(unordered_set<string>& keywords, string& buffer, int length)
    {
        if (length <= 0)
            return;

        for (auto i = min_char; i <= max_char; ++i)
        {
            buffer.push_back(i);

            keywords.insert(buffer);
            add_short_keywords(keywords, buffer, length - 1);

            buffer.pop_back();
        }
    }

    unordered_set<string> build_keywords()
    {
        unordered_set<string> keywords;
        {
            string buffer;

            constexpr auto max_short_length = 2;
            add_short_keywords(keywords, buffer, max_short_length);
        }

        IntRandom r;
        while (keywords.size() < count)
            keywords.emplace(random_string<IntRandom, min_char, max_char>(r, min_size, max_size));

        return keywords;
    }

    string build_description(const unordered_set<string>& keywords)
    {
        string result("aho_corasick of ");
        result += to_string(keywords.size());
        result += ": ";

        for (const auto& key : keywords)
        {
            result += key;
            result += "\t";
        }

        result += ", text: ";
        return  result;
    }

    aho_corasick<string> build_automaton(const unordered_set<string>& keywords)
    {
        aho_corasick<string> result(vector<string>(keywords.begin(), keywords.end()));
        return result;
    }

    void slow_find_expected(const unordered_set<string>& keywords, const string& text, vector<pair<string, int>>& result)
    {
        result.clear();

        for (const auto& p : keywords)
        {
            auto index = string::size_type();
            for (;;)
            {
                index = text.find(p, index);
                if (string::npos == index)
                    break;

                result.push_back(make_pair(p, static_cast<int>(index)));
                ++index;
            }
        }

        sort(result.begin(), result.end());
    }

    template<typename key_t, typename value_t>
    void ptr_to_value(const vector<pair<const key_t*, value_t>>& source,
        vector<pair<key_t, value_t>>& target)
    {
        target.clear();
        target.reserve(source.size());

        std::transform(source.begin(), source.end(), back_inserter(target),
            [](const pair<const key_t*, value_t>& p) -> auto
        {
            return make_pair(*p.first, p.second);
        });

        sort(target.begin(), target.end());
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            namespace Tests
            {
                void aho_corasick_tests()
                {
                    // TODO: use a class for the test cases.
                    const auto keywords = build_keywords();
                    const auto description = build_description(keywords);
                    const auto aho = build_automaton(keywords);

                    vector<pair<string, int>> actual, expected;
                    vector<pair<const string*, int>> aho_actual;
                    IntRandom r;

                    for (const auto& key : keywords)
                    {
                        const auto text = random_string<IntRandom, min_char, max_char>(r, min_size, max_size) +
                            key +
                            random_string<IntRandom, min_char, max_char>(r, min_size, max_size);

                        aho.find(text, aho_actual);
                        ptr_to_value(aho_actual, actual);

                        slow_find_expected(keywords, text, expected);
                        Assert::AreEqual(actual, expected, description + text);
                    }
                }
            }
        }
    }
}