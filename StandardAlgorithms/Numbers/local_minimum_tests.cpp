#include"local_minimum_tests.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/random.h"
#include"../Utilities/remove_duplicates.h"
#include"../Utilities/test_utilities.h"
#include"local_minimum.h"

namespace
{
    using item_t = std::int32_t;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<item_t> &&items) noexcept
            : base_test_case(std::move(name))
            , Items(std::move(items))
        {
        }

        [[nodiscard]] constexpr auto items() const &noexcept -> const std::vector<item_t> &
        {
            return Items;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Items.size(), "items size");

            auto cop = Items;

            Standard::Algorithms::sort_remove_duplicates(cop);

            ::Standard::Algorithms::ert::are_equal(Items.size(), cop.size(), "items must be unique");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("items", Items, str);
        }

private:
        std::vector<item_t> Items;
    };

    constexpr void loc_min_check_result(
        const char *const name, const std::vector<item_t> &items, const std::size_t index)
    {
        assert(name != nullptr);

        Standard::Algorithms::require_greater(items.size(), index, name);

        const auto &cur = items[index];

        if (0U < index)
        {
            const auto &prev = items[index - 1U];

            ::Standard::Algorithms::ert::greater(prev, cur, name);
        }

        if (index < items.size() - 1U)
        {
            const auto &next = items[index + 1U];

            ::Standard::Algorithms::ert::greater(next, cur, name);
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr auto min_size = 1;
        constexpr auto max_size = 20;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size = rnd();

            auto items = Standard::Algorithms::Utilities::iota_vector<item_t>(size);
            Standard::Algorithms::Utilities::shuffle_data(items);

            test_cases.emplace_back("Random" + std::to_string(att), std::move(items));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        {
            const auto index = Standard::Algorithms::Numbers::local_minimum<item_t>(test.items());

            loc_min_check_result("local_minimum", test.items(), index);
        }
        {
            const auto index = Standard::Algorithms::Numbers::local_minimum_slow<item_t>(test.items());

            loc_min_check_result("local_minimum_slow", test.items(), index);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::local_minimum_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
