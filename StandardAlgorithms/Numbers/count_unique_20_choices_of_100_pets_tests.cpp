#include"count_unique_20_choices_of_100_pets_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"count_unique_20_choices_of_100_pets.h"
#include"default_modulus.h"

namespace
{
    using int_t = std::int32_t;

    constexpr int_t mod = Standard::Algorithms::Numbers::default_modulus;
    constexpr int_t not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<std::vector<int_t>> &&man_pets, int_t expected) noexcept
            : base_test_case(std::move(name))
            , Man_pets(std::move(man_pets))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto man_pets() const &noexcept -> const std::vector<std::vector<int_t>> &
        {
            return Man_pets;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const int_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Man_pets.size(), "man pets size");

            if (not_computed < Expected)
            {
                Standard::Algorithms::require_non_negative(Expected, "expected");
                Standard::Algorithms::require_greater(mod, Expected, "expected");
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("man pets", Man_pets, str);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<std::vector<int_t>> Man_pets;
        int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "base1", { { 1, 2, 3 }, { 0 }, { 3, 2 } }, 4 });

        constexpr auto min_count = 1;
        constexpr auto max_count = 6;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_count, max_count);
        std::unordered_set<int_t> uniq{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto men = rnd();
            const auto total_pets = rnd((men + 1) / 2, max_count);

            std::vector<std::vector<int_t>> man_pets(men);

            for (int_t man{}; man < men; ++man)
            {
                auto count_pets = rnd(1, total_pets);
                auto &pets = man_pets[man];
                pets.resize(count_pets);

                uniq.clear();

                --count_pets;

                for (int_t index{}; index <= count_pets;)
                {
                    auto &pet = pets[index];
                    pet = rnd(0, count_pets);
                    index += uniq.insert(pet).second ? 1 : 0;
                }
            }

            test_cases.emplace_back("Random" + std::to_string(att), std::move(man_pets), not_computed);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::count_unique_20_choices_of_100_pets<int_t, mod>(test.man_pets());

        if (not_computed < test.expected())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "count_unique_20_choices_of_100_pets");
        }

        const auto slow =
            Standard::Algorithms::Numbers::count_unique_20_choices_of_100_pets_slow<int_t, mod>(test.man_pets());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "count_unique_20_choices_of_100_pets_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::count_unique_20_choices_of_100_pets_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
