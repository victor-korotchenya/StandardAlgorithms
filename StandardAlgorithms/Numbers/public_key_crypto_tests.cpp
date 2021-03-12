#include"public_key_crypto_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"public_key_crypto.h"
#include<array>

namespace
{
    using int_t = std::int64_t;

    [[nodiscard]] constexpr auto req_prima(const int_t value) -> int_t
    {
        return Standard::Algorithms::Numbers::ensur_prima(value);
    }

    constexpr auto prim3 = req_prima(3);
    constexpr auto prim5 = req_prima(5);
    constexpr auto prim7 = req_prima(7);
    constexpr auto prim31 = req_prima(31);
    constexpr auto prim59 = req_prima(59);
    constexpr auto prim79 = req_prima(79);

    constexpr int_t moda = (prim79 - 1) * (prim59 - 1);
    static_assert(prim79 < moda && prim59 < moda);

    constexpr int_t mod_inv = 2335;
    static_assert(1 == (prim31 * mod_inv % moda) && int_t{} < mod_inv && mod_inv < moda);

    template<class item_t, std::size_t size_one, std::size_t size_two>
    requires(size_one <= size_two && 0U < size_two)
    [[nodiscard]] constexpr auto first_or_second(const std::array<item_t, size_one> &firsts, const std::size_t pos,
        const std::array<item_t, size_two> &seconds) -> const item_t &
    {
        assert(pos < seconds.size());

        const auto &result = pos < firsts.size() ? firsts.at(pos) : seconds.at(pos);
        return result;
    }

    constexpr std::array small_primes{ prim79, prim59, prim31 };

    template<std::size_t arr_size = 0>
    struct genadzi final
    {
        constexpr explicit genadzi(const std::array<int_t, arr_size> &defaults = {})
            : Prim0(first_or_second(defaults, 0, small_primes))
            , Prim1(first_or_second(defaults, 1, small_primes))
            , Prim2(first_or_second(defaults, 2, small_primes))
        {
        }

        [[nodiscard]] constexpr auto operator() () -> int_t
        {
            switch (shots_fired++)
            {
            case 0: // NOLINTNEXTLINE
                return Prim0; // NOLINTNEXTLINE
            case 1: // NOLINTNEXTLINE
                return Prim1; // NOLINTNEXTLINE
            case 2: // NOLINTNEXTLINE
                return Prim2; // NOLINTNEXTLINE
            default:
                --shots_fired;
                throw std::runtime_error("No more shots to generate a random number are left.");
            }
        }

private:
        const int_t Prim0;
        const int_t Prim1;
        const int_t Prim2;
        std::size_t shots_fired{};
    };

    [[nodiscard]] constexpr auto def_test_keys()
    {
        return Standard::Algorithms::Numbers::rsa_keys<int_t>{ prim79, prim59, prim79 * prim59, prim31, mod_inv };
    }

    [[nodiscard]] constexpr auto gen_attempt_one_strict()
    {
        genadzi gena{};

        constexpr auto max_attempts = 1U;

        const auto actual = Standard::Algorithms::Numbers::gen_rsa_keys<int_t, max_attempts>(gena);

        constexpr auto expected_keys = def_test_keys();
        ::Standard::Algorithms::ert::are_equal(expected_keys, actual, "gen_rsa_keys 1 attempt");

        return true;
    }

    constexpr auto gen_attempt_one_same_keys_fail(const std::string &name, const auto &first_primes)
    {
        assert(!name.empty());

        const auto *const expected_message_gen1 = "All 1 attempts to generate RSA keys have failed.";

        const auto message_att = "Must be a limited number of attempts. " + name;

        ::Standard::Algorithms::ert::expect_exception<std::out_of_range>(
            expected_message_gen1,
            [&first_primes]()
#ifndef __clang__ // todo(p3): Check [[noreturn]] in lambdas?
                [[noreturn]]
#endif
            {
                constexpr auto max_attempts = 1U;

                genadzi gena(first_primes);

                [[maybe_unused]] const auto actual =
                    Standard::Algorithms::Numbers::gen_rsa_keys<int_t, max_attempts>(gena);
            },
            message_att);

        return true;
    }

    [[nodiscard]] constexpr auto rsa_test() -> bool
    {
        constexpr auto keys = def_test_keys();

        constexpr int_t plain_expected = prim79 * prim59 / 2; // 2'330
        static_assert(int_t{} < plain_expected && plain_expected < keys.primes_product);

        constexpr int_t cypher_expected = 3'378;
        static_assert(int_t{} < cypher_expected && cypher_expected < keys.primes_product);

        {
            int_t cypher_text{};
            Standard::Algorithms::Numbers::encrypt_rsa<int_t>(plain_expected, keys, cypher_text);

            ::Standard::Algorithms::ert::are_equal(cypher_expected, cypher_text, "Encrypt RSA");
        }
        {
            int_t plain_text{};
            Standard::Algorithms::Numbers::decrypt_rsa<int_t>(cypher_expected, keys, plain_text);

            ::Standard::Algorithms::ert::are_equal(plain_expected, plain_text, "Decrypt RSA");
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::public_key_crypto_tests()
{
    static_assert(gen_attempt_one_strict());
    static_assert(rsa_test());

    // todo(p3): review in C++26 static_assert(gen_attempt_one_same_keys_fail());

    gen_attempt_one_same_keys_fail("3 divides 4*6; public key gen fails.", std::array{ prim5, prim7, prim3 });

    gen_attempt_one_same_keys_fail("two repetitions", std::array{ prim59, prim59 });
}
