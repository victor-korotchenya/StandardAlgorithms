#include"fast_fourier_transform_tests.h"
#include"../Utilities/ert.h"
#include"fast_fourier_transform.h"
#include"number_comparer.h"
#include"polynomial_multiplier.h"
#include<array>

namespace
{
    using floating_t = Standard::Algorithms::floating_point_type;
    using complex_t = std::complex<floating_t>;

    constexpr auto epsilon = Standard::Algorithms::epsilon_default<floating_t>();

    constexpr Standard::Algorithms::Numbers::number_comparer<floating_t>
        // todo(p4): create a struct for AreEqualComplexNumbers?
        comparer{ epsilon, &Standard::Algorithms::Numbers::absolute_value<floating_t> };

    [[nodiscard]] inline constexpr auto are_equal_complex_numbers(
        const complex_t *const a_ptr, const complex_t *const b_ptr) -> bool
    {
        const auto &one = *(Standard::Algorithms::throw_if_null("AreEqualComplexNumbers one", a_ptr));

        const auto &two = *(Standard::Algorithms::throw_if_null("AreEqualComplexNumbers two", b_ptr));

        auto result = comparer.are_equal(one.real(), two.real()) && comparer.are_equal(one.imag(), two.imag());

        return result;
    }

    template<class item_t, class item_two, std::size_t size>
    constexpr void require_equal(const std::array<item_t, size> &expected, const std::valarray<item_two> &actual,
        std::vector<item_two> &temp, const char *const name)
    {
        assert(name != nullptr);

        temp.assign(std::begin(actual), std::end(actual));

        ::Standard::Algorithms::ert::are_equal(
            expected.data(), size, temp.data(), temp.size(), name, are_equal_complex_numbers);
    }

    void fft_test()
    {
        constexpr auto size = 8U;

        constexpr std::array<std::int32_t, size> source_int{// NOLINTNEXTLINE
            1, 1, 1, 1, 0, 0, 0, 0
        };

        constexpr std::array<complex_t, size> source_complex{// NOLINTNEXTLINE
            1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0
        };

        constexpr std::array<complex_t, size> expected_fft{// NOLINTNEXTLINE
            complex_t(4), complex_t(1, -2.4142135623730949), // NOLINTNEXTLINE
            complex_t(0), complex_t(1, -0.41421356237309492), // NOLINTNEXTLINE
            complex_t(0), complex_t(1, 0.41421356237309492), // NOLINTNEXTLINE
            complex_t(0), complex_t(1, 2.4142135623730949)
        };

        std::vector<complex_t> temp;
        {
            std::valarray<complex_t> data(source_complex.data(), size);

            Standard::Algorithms::Numbers::fast_fourier_transform<floating_t>::compute(data);

            require_equal(expected_fft, data, temp, "FFT");

            Standard::Algorithms::Numbers::fast_fourier_transform<floating_t>::compute_inverse(data);

            require_equal(source_complex, data, temp, "Inverse FFT");
        }
        {
            std::valarray<complex_t> target;
            Standard::Algorithms::Numbers::fast_fourier_transform<floating_t>::compute_slow(source_int, target);

            require_equal(expected_fft, target, temp, "FFT slow");
        }
    }

    constexpr void poly_multiply_test()
    {
        using int_t = std::int32_t;

        // 2 + 4*x + 0*x*x + 16*x*x*x + 32*x*x*x*x
        const std::vector<int_t> arr0{ 2, 4, 0, 16, 32 };

        // 3 + 9*x + 27*x*x
        const std::vector<int_t> arr1{ 3, 9, 27 };

        // (2 + 4*x + 0*x*x + 16*x*x*x + 32*x*x*x*x) * (3 + 9*x + 27*x*x)
        // == 6 + 30*x + 90*x^x + 156*x^3 + 240 * x^4 + 720*x^5 + 864*x^6
        const std::vector<complex_t> expected{// NOLINTNEXTLINE
            6, 30, 90, 156, 240, 720, 864, 0
        };

        std::vector<complex_t> actual;

        Standard::Algorithms::Numbers::polynomial_multiplier<int_t, floating_t>(arr0, arr1, actual);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "polynomial_multiplier", are_equal_complex_numbers);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::fast_fourier_transform_tests()
{
    fft_test();
    poly_multiply_test();
}
