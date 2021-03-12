#pragma once
#include"../Utilities/project_constants.h"
#include"../Utilities/require_utilities.h"
#include<bit>
#include<cmath>
#include<complex>
#include<valarray>

namespace Standard::Algorithms::Numbers
{
    // todo(p2): good FFT, DFT.
    template<std::floating_point floating_t, class complex_t = std::complex<floating_t>>
    struct fast_fourier_transform final
    {
        fast_fourier_transform() = delete;

        // Given a vector D of size n == 2**p > 0,
        // F=FFT(D) is a vector F of size also n such that
        // F[s] = sum(D[t]* e**(-2*pi*i/n*s*t), where t=0..(n-1), and i=sqrt(-1)).

        // Time O(n*log(n)).
        static constexpr void compute(std::valarray<complex_t> &data)
        {
            const auto size = data.size();
            check_size(size);

            if (size == 1U)
            {
                return;
            }

            simple_impl(data);
        }

        // Time O(n*log(n)).
        static constexpr void compute_inverse(std::valarray<complex_t> &data)
        {
            const auto size = data.size();
            check_size(size);

            if (size == 1U)
            {
                return;
            }

            data = data.apply(std::conj); // Conjugate.

            simple_impl(data);

            data = data.apply(std::conj);

            data /= static_cast<floating_t>(data.size());
        }

        // Slow time O(n*n).
        static constexpr void compute_slow(const auto &source, std::valarray<complex_t> &target)
        {
            const auto size = source.size();
            check_size(size);
            target.resize(size);

            if (size == 1U)
            {
                target[0] = complex_builder(source[0]);
                return;
            }

            const auto dangler = static_cast<floating_t>(minus_two_pi / static_cast<floating_t>(size));

            for (std::size_t sss{}; sss < size; ++sss)
            {
                const auto theta_add = static_cast<floating_t>(dangler * static_cast<floating_t>(sss));
                floating_t theta{};

                auto &targe = target[sss];
                targe = {};

                for (std::size_t ttt{}; ttt < size; ++ttt, theta += theta_add)
                {
                    const auto &sour = source[ttt];
                    const auto addon = complex_builder(sour) *
                        // By Euler's formula, e**(theta*i)
                        complex_t(std::cos(theta), std::sin(theta));

                    targe += addon;
                }
            }
        }

private:
        static constexpr void check_size(const std::size_t size)
        {
            if (const auto is_power_of_two = std::has_single_bit(size); is_power_of_two) [[likely]]
            {
                return;
            }

            auto err = "The size " + std::to_string(size) + " must be a power of 2.";

            throw std::runtime_error(err);
        }

        template<class raw_val_t>
        [[nodiscard]] static constexpr auto complex_builder(const raw_val_t &val) noexcept -> decltype(auto)
        {
            using val_t = std::remove_cvref_t<raw_val_t>;

            if constexpr (std::is_same_v<val_t, complex_t>)
            {
                return val;
            }
            else
            {
                return complex_t{ static_cast<floating_t>(val) };
            }
        }

        // Time O(n*log(n)).
        static constexpr void simple_impl(std::valarray<complex_t> &data
#if _DEBUG
            ,
            std::uint32_t depth = {}
#endif
        )
        {
#if _DEBUG
            {
                ++depth;

                assert(0U < depth && depth <= ::Standard::Algorithms::Utilities::stack_max_size);
            }
#endif

            const auto size = data.size();
            const auto half = size / 2U;

            assert(2U <= size && size == half + half);

            std::valarray<complex_t> evens = data[std::slice(0U, half, 2U)];

            if (2U <= half)
            {
                simple_impl(evens
#if _DEBUG
                    ,
                    depth
#endif
                );
            }

            std::valarray<complex_t> odds = data[std::slice(1U, half, 2U)];

            if (2U <= half)
            {
                simple_impl(odds
#if _DEBUG
                    ,
                    depth
#endif
                );
            }

            constexpr floating_t radius{ 1 };
            const auto dangle = static_cast<floating_t>(minus_two_pi / static_cast<floating_t>(size));
            floating_t theta{};

            for (std::size_t index{}; index < half; ++index, theta += dangle)
            {
                const auto &eve = evens[index];
                const auto &odd = odds[index];
                const auto delta = std::polar(radius, theta) * odd;

                data[index] = eve + delta;
                data[index + half] = eve - delta;
            }
        }

        static constexpr floating_t the_pi{ 3.1415926535897932384626433832795028841971693993751 };

        static constexpr auto minus_two_pi = static_cast<floating_t>(-the_pi - the_pi);

        static_assert(floating_t{} < the_pi && minus_two_pi < floating_t{});
    };
} // namespace Standard::Algorithms::Numbers
