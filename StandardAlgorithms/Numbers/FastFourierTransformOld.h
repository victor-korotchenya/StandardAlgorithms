#pragma once
#include <complex>
#include <valarray>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template <typename Number = double>
            class FastFourierTransformOld final
            {
                FastFourierTransformOld() = delete;

            public:
                static void Compute(
                    const Number& pi,
                    std::valarray<std::complex<Number>>& data);

                static void ComputeInverse(
                    const Number& pi,
                    std::valarray<std::complex<Number>>& data);

            private:

                static void CheckInput(
                    const std::valarray<std::complex<Number>>& data);

                static void SimpleImpl(
                    const Number& twoPi,
                    std::valarray<std::complex<Number>>& data);
            };

            template <typename Number>
            void FastFourierTransformOld<Number>::Compute(
                const Number& pi,
                std::valarray<std::complex<Number>>& data)
            {
                CheckInput(data);

                const Number minustwoPi = -pi - pi;

                SimpleImpl(minustwoPi, data);
            }

            template <typename Number >
            void FastFourierTransformOld< Number >::ComputeInverse(
                const Number& pi,
                std::valarray<std::complex<Number>>& data)
            {
                CheckInput(data);

                const Number minustwoPi = -pi - pi;

                data = data.apply(std::conj); //Conjugate.

                SimpleImpl(minustwoPi, data);

                data = data.apply(std::conj);

                data /= static_cast<Number>(data.size());
            }

            template <typename Number >
            void FastFourierTransformOld< Number >::CheckInput(
                const std::valarray<std::complex<Number>>& data)
            {
                RequirePositive(data.size(), "data");
            }

            template <typename Number >
            void FastFourierTransformOld< Number>::SimpleImpl(
                const Number& minustwoPi,
                std::valarray<std::complex<Number>>& data)
            {
                const size_t n = data.size();
                if (n <= 1)
                {
                    return;
                }

                const size_t half = n / 2;

                std::valarray<std::complex<Number>> even = data[std::slice(0, half, 2)];
                SimpleImpl(minustwoPi, even);

                std::valarray<std::complex<Number>> odd = data[std::slice(1, half, 2)];
                SimpleImpl(minustwoPi, odd);

                const Number angle = minustwoPi / static_cast<Number>(n);

                for (size_t k = 0; k < half; ++k)
                {
                    const std::complex< Number > t = std::polar(1.0, angle * k) * odd[k];

                    data[k] = even[k] + t;
                    data[k + half] = even[k] - t;
                }
            }
        }
    }
}