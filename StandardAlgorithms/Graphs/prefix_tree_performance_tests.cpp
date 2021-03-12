#include"prefix_tree_performance_tests.h"
#include"../Utilities/ert.h"
#include"prefix_tree_add_words.h"
#include<filesystem>
#include<fstream>
#include<iostream>

namespace
{
    [[nodiscard]] auto read_words(const std::size_t limit = 0U) -> std::vector<std::string>
    {
        const std::string filename =
            R"(C:\Projects\Words English_Word_List__corncob_lowercase.txt)"; // todo(p3): hard-coded path.
        const std::filesystem::path path{ filename };

        {
            const auto status = std::filesystem::status(path);
            if (std::filesystem::is_symlink(status)) [[unlikely]]
            {
                auto err = "Symlinks are not allowed on the file '" + filename + "' error.";
                throw std::runtime_error(err);
            }

            if (!std::filesystem::is_regular_file(status)) [[unlikely]]
            {
                auto err = "Must be a regular file '" + filename + "', but it is not.";
                throw std::runtime_error(err);
            }
        }

        std::ifstream read_stream;
        // flawfinder: ignore suppress
        read_stream.open(path);

        if (!read_stream.is_open() || !read_stream.good()) [[unlikely]]
        {
            throw std::runtime_error("Cannot read words in prefix_tree_performance_tests from '" + filename + "'.");
        }

        const auto has_limit = 0U < limit;

        std::vector<std::string> words;
        std::string line;

        while (read_stream.good() && !read_stream.eof())
        {
            read_stream >> line;

            // todo(p3): line.trim();

            if (!line.empty())
            {
                words.push_back(std::move(line));

                if (has_limit && limit <= words.size())
                {
                    break;
                }
            }
        }

        return words;
    }

    constexpr void run_some_tests(
        const std::vector<std::string> &words, const Standard::Algorithms::Trees::prefix_tree &prefix_tree)
    {
        std::vector<std::string> result;

        for (const auto &word : words)
        {
            constexpr auto count_limit = 2U;

            prefix_tree.search(word, result, count_limit);

            if (result.empty())
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The word '" << word << "' was not found in the prefix tree.";
                Standard::Algorithms::throw_exception(str);
            }
        }
    }
} // namespace

void Standard::Algorithms::Trees::prefix_tree_add_words(
    const std::vector<std::string> &words, Standard::Algorithms::Trees::prefix_tree &prefix_tree)
{
    std::size_t index{};

    try
    {
        for (const auto &word : words)
        {
            auto cop = word;
            prefix_tree.add_word(std::move(cop));
            ++index;
        }
    }
    catch (const std::exception &exc)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "Error adding word '" << words.at(index) << "' with index " << index << " to the prefix tree.";
        str << " Exception: " << exc.what();

        throw_exception(str);
    }
} // namespace

void Standard::Algorithms::Trees::Tests::prefix_tree_performance_tests()
{
    // NOLINTNEXTLINE
    constexpr std::size_t wordLimit = ::Standard::Algorithms::is_debug ? 10 : 0;

    // NOLINTNEXTLINE
    constexpr auto max_attempts = ::Standard::Algorithms::is_debug ? 4 : 10;

    const auto words = read_words(wordLimit);
    if (words.empty()) [[unlikely]]
    {
        throw std::runtime_error("There are no read words in prefix_tree_performance_tests.");
    }

    Standard::Algorithms::Trees::prefix_tree prefix_tree{};

    for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
    {
        prefix_tree.clear();
        prefix_tree_add_words(words, prefix_tree);
        run_some_tests(words, prefix_tree);
    }
}
