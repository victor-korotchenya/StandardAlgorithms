#pragma once

//#include <complex>
//#include <valarray>
//
//#include <exception>
//#include <string>
//#include <sstream>

#define _USE_MATH_DEFINES
//#include <cmath>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template <typename Number = double>
            class FastFourierTransform final
            {
                FastFourierTransform() = delete;

            public:

                static void Compute(
                    const Number& pi, Number* AVal, Number* FTvl, int Nvl, int Nft);
            };

            template <typename Number >
            void FastFourierTransform< Number >::Compute(
                const Number& pi,
                Number* AVal, Number* FTvl, int Nvl, int Nft)
            {
                // todo: p0. do good fft.
            }
        }
    }
}