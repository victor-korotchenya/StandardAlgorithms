#include"min_time_copy_books_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"min_time_copy_books.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, long_int_t copiers, std::vector<int_t> &&books, long_int_t expected = {}) noexcept
            : base_test_case(std::move(name))
            , Books(std::move(books))
            , Copiers(copiers)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto books() const &noexcept -> const std::vector<int_t> &
        {
            return Books;
        }

        [[nodiscard]] constexpr auto copiers() const noexcept -> long_int_t
        {
            return Copiers;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> long_int_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Books.size(), "books size");
            Standard::Algorithms::require_all_positive(Books, "books");

            Standard::Algorithms::require_positive(Copiers, "copiers");

            Standard::Algorithms::require_non_negative(Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("books", Books, str);
            ::Standard::Algorithms::print_value("copiers", str, Copiers);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<int_t> Books;
        long_int_t Copiers;
        long_int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr int_t jazz = 10;
        constexpr int_t buki = 20;
        constexpr int_t vedi = 5;

        test_cases.push_back({ "cannot move", 2, { jazz, buki, vedi }, buki + vedi });

        test_cases.push_back({ "merge first 2", 2, { vedi, jazz, buki }, buki });

        test_cases.push_back({ "trivial", 1, { jazz }, jazz });

        test_cases.push_back({ "trivial 2", 2, { jazz }, jazz });

        test_cases.push_back({ "max of 2", 2, { jazz, buki }, buki });

        test_cases.push_back({ "1 copier - sum all", 1, { jazz, buki, vedi }, jazz + buki + vedi });

        test_cases.push_back({ "max of 3", 3, { jazz, buki, vedi }, buki });

        test_cases.push_back({ "max of 3, extra copier - no gain", 4, { jazz, buki, vedi }, buki });

        test_cases.push_back({ "same value, half copiers", 2, { vedi, vedi, vedi, vedi }, vedi + vedi });

        test_cases.push_back({ "same value, not enough copiers", 3, { vedi, vedi, vedi, vedi }, vedi + vedi });

        test_cases.push_back({ "same value, equal copiers", 4, { vedi, vedi, vedi, vedi }, vedi });

        test_cases.push_back({ "same value, more copiers", 4 + 1, { vedi, vedi, vedi, vedi }, vedi });


        constexpr int_t min_value = 1;
        constexpr int_t max_value = 100;
        Standard::Algorithms::Utilities::random_t<int_t> rnd(min_value, max_value);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 8;

            const auto copiers = rnd(min_size, max_size);

            auto books = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

            test_cases.emplace_back("random " + std::to_string(att), copiers, std::move(books));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::min_time_copy_books_using_binary_search<long_int_t, int_t>(
            test.books(), test.copiers());

        if (test.expected() != long_int_t{})
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "min_time_copy_books_using_binary_search");
        }

        const auto slow =
            Standard::Algorithms::Numbers::min_time_copy_books_slow<long_int_t, int_t>(test.books(), test.copiers());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "min_time_copy_books_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::min_time_copy_books_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
