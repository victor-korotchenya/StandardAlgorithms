#pragma once
#include"arithmetic.h" // round_to_greater_power_of_two
#include"fast_fourier_transform.h"
#include<algorithm> // transform
#include<complex>
#include<functional> // multiplies
#include<valarray>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Use FFT to quickly compute a convolution in O(n*log(n)).
    template<class input_number, std::floating_point floating_t, class complex_t = std::complex<floating_t>>
    requires(sizeof(input_number) <= sizeof(floating_t))
    constexpr void polynomial_multiplier(const std::vector<input_number> &poly_a,
        const std::vector<input_number> &poly_b, std::vector<complex_t> &result)
    {
        const auto size_a = poly_a.size();
        if (0U == size_a) [[unlikely]]
        {
            throw std::runtime_error("The first polynomial must be not empty.");
        }

        const auto size_b = poly_b.size();
        if (0U == size_b) [[unlikely]]
        {
            throw std::runtime_error("The second polynomial must be not empty.");
        }

        // (10 + x)*(200 + x**2) actually produces x**3.
        const auto size_padded = round_to_greater_power_of_two(size_a + size_b);
        {
            const auto *const name = "Padded size";
            require_greater(size_padded, size_a, name);
            require_greater(size_padded, size_b, name);
        }

        const auto convolut = [size_padded](const auto &polynom) -> std::valarray<complex_t>
        {
            std::valarray<complex_t> conv(size_padded);
            std::copy(polynom.begin(), polynom.end(), std::begin(conv));
            fast_fourier_transform<floating_t>::compute(conv);

            return conv;
        };

        auto reversed_matrix = convolut(poly_a);
        {
            auto conv_2 = convolut(poly_b);

            std::transform(std::begin(reversed_matrix), std::end(reversed_matrix), std::begin(conv_2),
                std::begin(reversed_matrix), std::multiplies<>());
        }

        fast_fourier_transform<floating_t>::compute_inverse(reversed_matrix);

        result.resize(size_padded);

        std::copy(std::begin(reversed_matrix), std::end(reversed_matrix), result.begin());
    }
} // namespace Standard::Algorithms::Numbers
