#include"constructor_tests.h"
#include"test_utilities.h"
#include<array>
#include<atomic>

namespace
{
    using int_t = std::int32_t;
    using atomy_t = std::atomic<int_t>;

    constexpr int_t die = 100;
    constexpr int_t ato_count = 6;

    const auto *const expected_error = "Blow up!";

    struct tse_exception final : std::runtime_error
    {
        explicit tse_exception(const char *const message)
            : std::runtime_error(message)
        {
            assert(message != nullptr);
        }
    };

    struct add_ref_t final
    {
        // NOLINTNEXTLINE
        add_ref_t(atomy_t &maker, atomy_t &destroyer)
            : Destroyer(destroyer)
        {
            ::Standard::Algorithms::ert::not_equal(&maker, &destroyer, "Maker and destroyer atomics must be different.");

            ++maker;
        }

        ~add_ref_t() noexcept
        {
            Destroyer += die;
        }

        add_ref_t(const add_ref_t &) = delete;
        auto operator= (const add_ref_t &) & -> add_ref_t & = delete;
        add_ref_t(add_ref_t &&) noexcept = delete;
        auto operator= (add_ref_t &&) &noexcept -> add_ref_t & = delete;

private:
        atomy_t &Destroyer;
    };

    struct throw_up_in_ctor_t final
    {
        explicit throw_up_in_ctor_t(atomy_t &before)
        {
            ++before;
            throw tse_exception(expected_error);
        }
    };

    struct questionable_t final
    {
        explicit questionable_t(std::array<atomy_t, ato_count> &atos)
            : Odin(atos[0], atos[1])
            // Ensure no memory leaks in a partially initialized object.
            // Only Odin's destructor must be called.
            , Must_explode(atos[2])
            , Dva(atos[3], atos[4])
        {
            ++atos[4 + 1];

            throw std::runtime_error("Aramis has been unexpected.");
        }

private:
        add_ref_t Odin;
        throw_up_in_ctor_t Must_explode;
        add_ref_t Dva;
    };
} // namespace

void Standard::Algorithms::Utilities::Tests::constructor_tests()
{
    std::array<atomy_t, ato_count> atos{};

    for (const auto &ato : atos)
    {
        constexpr int_t zero{};

        ::Standard::Algorithms::ert::are_equal(zero, ato.load(), "Initially, all atomics must have zero values.");
    }

    try
    {
        const questionable_t question(atos);
        throw std::runtime_error("Portos has been unexpected too.");
    }
    catch (const tse_exception &exc)
    {
        const auto *const actual_error = exc.what();

        ::Standard::Algorithms::ert::are_equal(expected_error, actual_error, "Thrown error message");
    }

    const std::array<int_t, ato_count> expected_calls{ 1, die, 1, 0, 0, 0 };

    for (int_t index{}; index < ato_count; ++index)
    {
        const auto &expected = expected_calls.at(index);
        const auto actual = atos.at(index).load();

        ::Standard::Algorithms::ert::are_equal(expected, actual, "expected call at " + std::to_string(index));
    }
}
