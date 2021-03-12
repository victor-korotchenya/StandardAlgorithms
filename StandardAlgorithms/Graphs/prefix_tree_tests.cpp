#include"prefix_tree_tests.h"
#include"../Utilities/ert.h"
#include"prefix_tree.h"
#include"prefix_tree_add_words.h"
#include"prefix_tree_performance_tests.h"
#include<functional>

namespace
{
    // This must not be in any word.
    constexpr auto non_existing_symbol = '_';

    constexpr auto read_words() -> std::vector<std::string>
    {
        std::vector<std::string> words;
        // be
        // beach
        // beaches
        // cat
        // cats
        // crossing
        // cross-ing
        // spoon
        // tea
        // teach
        // teacher
        // teachers
        // teachers ok
        // teachers.ok
        // teachers.oks
        // teaching
        // team
        // tear
        // teaspoon
        // tempo
        // xenon
        // zoo
        // zulu

        words.emplace_back("zoo");

        words.emplace_back("tea");
        // Add a duplicate.
        words.push_back(words.back());

        const auto *const teach = "teach";
        words.emplace_back(teach);

        words.emplace_back("tempo");
        words.emplace_back("spoon");

        // A duplicate.
        words.emplace_back(teach);

        words.emplace_back("teacher");
        words.emplace_back("team");
        words.emplace_back("tear");
        words.emplace_back("cross-ing");
        words.emplace_back("teaspoon");
        words.emplace_back("crossing");
        words.emplace_back("teachers");
        words.emplace_back("teachers.oks");
        words.emplace_back("teachers ok");
        words.emplace_back("teachers.ok");

        words.emplace_back("cats");
        words.emplace_back("cat");

        words.emplace_back("zulu");
        words.emplace_back("teaching");

        words.emplace_back("beach");
        words.emplace_back("beaches");
        words.emplace_back("be");
        words.emplace_back("xenon");

        return words;
    }

    using prefix_and_limit_t = std::pair<std::string, std::size_t>;
    using expected_and_message_t = std::pair<std::vector<std::string>, std::string>;
    using prefix_test_case_t = std::pair<prefix_and_limit_t, expected_and_message_t>;

    constexpr void add_test_cases(std::vector<prefix_test_case_t> &test_cases)
    {
        constexpr auto default_count_limit_max = 5U;

        {
            test_cases.emplace_back(prefix_and_limit_t("xn", default_count_limit_max),
                expected_and_message_t({}, "Same first letter, second is not."));
        }
        {
            const auto *const cats = "cats";

            test_cases.emplace_back(prefix_and_limit_t(cats, default_count_limit_max),
                expected_and_message_t(std::vector<std::string>{ cats }, "Cats."));
        }
        {
            constexpr auto count_limit_max = 7U;

            std::vector<std::string> expected;
            expected.reserve(count_limit_max);

            expected.emplace_back("teach");
            expected.emplace_back("teacher");
            expected.emplace_back("teachers");
            expected.emplace_back("teachers ok");

            expected.emplace_back("teachers.ok");
            expected.emplace_back("teachers.oks");
            expected.emplace_back("teaching");

            test_cases.emplace_back(prefix_and_limit_t("teach", count_limit_max),
                expected_and_message_t(std::move(expected), "Word => Few words."));
        }
        {
            std::vector<std::string> expected;
            expected.reserve(default_count_limit_max);

            expected.emplace_back("teach");
            expected.emplace_back("teacher");
            expected.emplace_back("teachers");
            expected.emplace_back("teachers ok");
            expected.emplace_back("teaching");

            test_cases.emplace_back(prefix_and_limit_t("teac", default_count_limit_max),
                expected_and_message_t(std::move(expected), "Sub-word => Few words."));
        }
        {
            constexpr auto count_limit_max = 4U;

            std::vector<std::string> expected;
            expected.reserve(count_limit_max);

            expected.emplace_back("tea");
            expected.emplace_back("teach");
            expected.emplace_back("team");
            expected.emplace_back("tempo");

            test_cases.emplace_back(prefix_and_limit_t("t", count_limit_max),
                expected_and_message_t(std::move(expected), "Many words with limit."));
        }

        test_cases.emplace_back(
            prefix_and_limit_t("a", default_count_limit_max), expected_and_message_t({}, "Non-existing prefix."));

        test_cases.emplace_back(prefix_and_limit_t("bazooka", 0U), expected_and_message_t({}, "Non-existing word."));

        test_cases.emplace_back(prefix_and_limit_t("zoom", default_count_limit_max),
            expected_and_message_t({}, "Non-existing word with long initial prefix."));

        {
            std::vector<std::string> expected;
            expected.emplace_back("beach");
            expected.emplace_back("beaches");

            test_cases.emplace_back(prefix_and_limit_t("beac", default_count_limit_max),
                expected_and_message_t(std::move(expected), "Only 2 matching words."));
        }
        {
            test_cases.emplace_back(prefix_and_limit_t("teasp", default_count_limit_max),
                expected_and_message_t(std::vector<std::string>{ "teaspoon" }, "One matching word."));
        }
        {
            const auto *const word = "teaspoon";

            test_cases.emplace_back(prefix_and_limit_t(word, default_count_limit_max),
                expected_and_message_t(std::vector<std::string>{ word }, "One complete word."));
        }
        {
            test_cases.emplace_back(prefix_and_limit_t("cat", default_count_limit_max),
                expected_and_message_t(std::vector<std::string>{ "cat", "cats" }, "Cat."));
        }
    }

    constexpr void run_prefix_tree_tests(
        const std::vector<prefix_test_case_t> &test_cases, const Standard::Algorithms::Trees::prefix_tree &prefix_tree)
    {
        std::vector<std::string> actual;

        for (const auto &test : test_cases)
        {
            const auto &prefix = test.first.first;
            const auto &limit = test.first.second;
            prefix_tree.search(prefix, actual, limit);

            const auto &expected = test.second.first;
            const auto &message = test.second.second;

            ::Standard::Algorithms::ert::are_equal(expected, actual, message);
        }
    }

    constexpr void check_word(const std::string &word)
    {
        Standard::Algorithms::throw_if_empty("word", word);

        if (const auto citer = std::find(word.cbegin(), word.cend(), non_existing_symbol); citer != word.cend())
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The word '" << word << "' must not have this symbol: '" << non_existing_symbol << "'.";
            Standard::Algorithms::throw_exception(str);
        }
    }

    constexpr void run_prefix_tree_tests_4words(
        const std::vector<std::string> &words, const Standard::Algorithms::Trees::prefix_tree &prefix_tree)
    {
        std::vector<std::string> actual;

        for (const auto &word : words)
        {
            check_word(word);

            constexpr auto limit = 3U;
            prefix_tree.search(word, actual, limit);

            if (const auto iter = std::find(actual.cbegin(), actual.cend(), word); iter == actual.cend())
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The word '" << word << "' must present in the tree. The returned result size " << actual.size()
                    << ".";
                Standard::Algorithms::throw_exception(str);
            }

            const auto word2 = non_existing_symbol + word;
            prefix_tree.search(word2, actual, limit);
            ::Standard::Algorithms::ert::are_equal(0U, actual.size(), word2);

            const auto word3 = word2 + non_existing_symbol;
            prefix_tree.search(word3, actual, limit);
            ::Standard::Algorithms::ert::are_equal(0U, actual.size(), word3);
        }
    }

    constexpr void expect_exception_for_prefix_tree()
    {
        const auto empty_prefix_func = []()
        {
            Standard::Algorithms::Trees::prefix_tree prefix_tree;
            prefix_tree.add_word("gut");

            std::vector<std::string> actual;
            prefix_tree.search(std::string{}, actual);
        };

        ::Standard::Algorithms::ert::expect_exception<std::exception>(
            "The 'prefix' argument must be not empty.", empty_prefix_func, "Empty prefix.");
    }
} // namespace

void Standard::Algorithms::Trees::Tests::prefix_tree_tests()
{
    const auto words = read_words();

    Standard::Algorithms::Trees::prefix_tree prefix_tree;
    prefix_tree_add_words(words, prefix_tree);

    std::vector<prefix_test_case_t> test_cases;
    add_test_cases(test_cases);

    run_prefix_tree_tests(test_cases, prefix_tree);
    run_prefix_tree_tests_4words(words, prefix_tree);

    expect_exception_for_prefix_tree();
}
