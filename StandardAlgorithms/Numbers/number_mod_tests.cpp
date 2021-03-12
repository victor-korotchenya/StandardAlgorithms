#include"number_mod_tests.h"
#include"../Utilities/test_utilities.h"
#include"default_modulus.h"
#include"number_mod.h"
#include"permutation.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    constexpr auto mod = Standard::Algorithms::Numbers::default_modulus;

    using n_m = Standard::Algorithms::Numbers::number_mod<int_t, mod, long_int_t>;

    template<std::integral int_t>
    [[nodiscard]] constexpr auto factorial_naive(int_t number) -> int_t
    {
        constexpr int_t zero{};

        assert(!(number < zero));

        int_t result{ 1 };

        while (zero < number)
        {
            result *= number;
            --number;
        }

        return result;
    }

    constexpr void mod_tests()
    {
        {
            constexpr int_t first = 5;
            constexpr int_t second = -3;
            constexpr int_t res = first + second;

            n_m actual{ first };
            actual += n_m{ mod + second };

            ::Standard::Algorithms::ert::are_equal(n_m{ res }, actual, "+=");
        }
        {
            constexpr int_t first = 5;
            constexpr int_t second = -3;
            constexpr int_t res = first * second;

            n_m actual{ first };
            actual *= n_m{ second };

            ::Standard::Algorithms::ert::are_equal(n_m{ res }, actual, "*=");
        }
        {
            constexpr int_t first = 2;
            constexpr int_t second = -1;
            constexpr int_t res = first + second;

            n_m actual{ first };
            actual = actual + n_m{ second };

            ::Standard::Algorithms::ert::are_equal(n_m{ res }, actual, "+&&");
        }
        {
            constexpr int_t first = 2;
            constexpr int_t second = 30;
            constexpr int_t res = first * second;

            constexpr n_m some{ first };
            constexpr n_m another{ second };

            static_assert((some * another).value() == res, "product");
        }
        {
            constexpr int_t first = 8167;
            constexpr int_t second = 1;
            constexpr int_t res = first + second;

            constexpr auto actual = n_m{ first } + n_m{ second };

            static_assert(n_m{ res } == actual, "&&+&&");
        }
        {
            constexpr int_t first = 60;
            constexpr int_t second = 10;
            constexpr int_t res = first * second;

            constexpr auto actual = n_m{ first } * n_m{ second };

            static_assert(n_m{ res } == actual, "&&*&&");
        }
        {
            constexpr n_m one{ 1 };
            constexpr auto exponent = 123456789LLU;

            constexpr auto actual = Standard::Algorithms::Numbers::modular_power(one, exponent);

            static_assert(one == actual, "power(1, x)");
        }
        {
            constexpr int_t factorial_10 = 3628800;
            {
                constexpr long_int_t numb = 10;
                constexpr auto fact = factorial_naive(numb);
                static_assert(fact == factorial_10);
            }

            constexpr n_m f10{ factorial_10 };
            static_assert(factorial_10 == f10.value());

            constexpr auto exponent = 2;
            constexpr auto expected = 189347824;
            {
                constexpr auto cast_input = static_cast<long_int_t>(factorial_10);
                constexpr auto squared = cast_input * cast_input;
                constexpr auto squared_mod = squared % mod;

                static_assert(expected == squared_mod);
            }

            constexpr auto actual = Standard::Algorithms::Numbers::modular_power(f10, exponent);

            static_assert(n_m{ expected } == actual, "power(10!, 2)");
        }

        constexpr auto max_unsigned_checkable_power = 63U;

        for (std::uint32_t power{}; power <= max_unsigned_checkable_power; ++power)
        {
            constexpr n_m two{ 2 };

            const auto actual = Standard::Algorithms::Numbers::modular_power(two, power);

            constexpr std::uint64_t one = 1;

            const auto raised = (one << power) % static_cast<std::uint64_t>(mod);
            const n_m expected{ static_cast<long_int_t>(raised) };

            ::Standard::Algorithms::ert::are_equal(expected, actual, "power(2, " + std::to_string(power) + ')');
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::number_mod_tests()
{
    mod_tests();
}
