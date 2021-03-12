#include"polynomial_all_derivatives_tests.h"
#include"../Utilities/test_utilities.h"
#include"polynomial_all_derivatives.h"

namespace
{
    using floating_t = Standard::Algorithms::floating_point_type;

    constexpr void run_poly_subtest(const std::string &name, const auto &polynomial, const floating_t &point,
        const std::vector<floating_t> &expected_derivatives)
    {
        assert(!name.empty());

        try
        {
            const auto size = polynomial.size();
            ::Standard::Algorithms::ert::are_equal(size, expected_derivatives.size(), "expected derivatives size");

            std::vector<floating_t> actual_derivatives(size);

            Standard::Algorithms::Numbers::polynomial_all_derivatives_slow<floating_t>(
                polynomial, point, actual_derivatives);
            ::Standard::Algorithms::ert::are_equal_with_epsilon(
                expected_derivatives, actual_derivatives, "polynomial_all_derivatives_slow");

            actual_derivatives.assign(size, floating_t{});

            Standard::Algorithms::Numbers::polynomial_all_derivatives<floating_t>(polynomial, point, actual_derivatives);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(
                expected_derivatives, actual_derivatives, "polynomial_all_derivatives");
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << name << ", point " << point << ", error: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }

    constexpr void all_derivatives_tests()
    {
        // polynomial(x) = 10 + 20*x + 50*x*x + 1000*x*x*x
        const std::vector<floating_t> polynomial{// NOLINTNEXTLINE
            10, 20, 50, 1000
        };

        {
            constexpr floating_t point{};
            const std::vector<floating_t> expected_derivatives{// NOLINTNEXTLINE
                10, 20, 100, 6000
            };

            run_poly_subtest("point 0", polynomial, point, expected_derivatives);
        }
        {
            constexpr floating_t point{ -1 };
            const std::vector<floating_t> expected_derivatives{// NOLINTNEXTLINE
                -960, 2920, -5900, 6000
            };

            run_poly_subtest("point -1", polynomial, point, expected_derivatives);
        }
        {
            constexpr floating_t point{ 1 };
            const std::vector<floating_t> expected_derivatives{// NOLINTNEXTLINE
                1080, 3120, 6100, 6000
            };

            run_poly_subtest("point 1", polynomial, point, expected_derivatives);
        }
        {
            constexpr floating_t point{ 2 };
            const std::vector<floating_t> expected_derivatives{// NOLINTNEXTLINE
                8250, 12220, 12100, 6000
            };

            run_poly_subtest("point 2", polynomial, point, expected_derivatives);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::polynomial_all_derivatives_tests()
{
    all_derivatives_tests();
}
