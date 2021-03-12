#pragma once
#include <vector>
#include <complex>
#include <valarray>
#include "Arithmetic.h"
#include "FastFourierTransformOld.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //FFT.
            template <typename InputNumber = int,
                typename Number = double,
                typename Complex = std::complex< Number >
            >
                class PolynomialMultiplier final
            {
                PolynomialMultiplier() = delete;

            public:
                static void Multiply(
                    const Number& pi,
                    const std::vector< InputNumber >& polyA,
                    const std::vector< InputNumber >& polyB,
                    std::vector< Complex >& result);

            private:

                static void CopyData(
                    const size_t sizePadded,
                    const std::vector< InputNumber >& data,
                    std::valarray< Complex >& result);
            };

            template <typename InputNumber,
                typename Number,
                typename Complex >
                void PolynomialMultiplier< InputNumber, Number, Complex >::Multiply(
                    const Number& pi,
                    const std::vector< InputNumber >& polyA,
                    const std::vector< InputNumber >& polyB,
                    std::vector< Complex >& result)
            {
                const size_t sizeA = polyA.size();
                if (0 == sizeA)
                {
                    throw std::runtime_error("First polynomial must be not empty.");
                }

                const size_t sizeB = polyB.size();
                if (0 == sizeB)
                {
                    throw std::runtime_error("Second polynomial must be not empty.");
                }

                // Having (1 + x)(2 + x) actually produces x^2.
                const size_t sizePadded = RoundToGreaterPowerOfTwo_t(sizeA + sizeB);

                std::valarray< Complex > data1;
                CopyData(sizePadded, polyA, data1);
                FastFourierTransformOld< Number >::Compute(pi, data1);

                std::valarray< Complex > data2;
                CopyData(sizePadded, polyB, data2);
                FastFourierTransformOld< Number >::Compute(pi, data2);

                std::valarray< Complex > reversedMatrix;
                reversedMatrix.resize(sizePadded);
                for (size_t i = 0; i < sizePadded; ++i)
                {
                    reversedMatrix[i] = data1[i] * data2[i];
                }

                FastFourierTransformOld< Number >::ComputeInverse(pi, reversedMatrix);

                result.resize(sizePadded);
                for (size_t i = 0; i < sizePadded; ++i)
                {
                    result[i] = reversedMatrix[i];
                }
            }

            template <typename InputNumber,
                typename Number,
                typename Complex >
                void PolynomialMultiplier< InputNumber, Number, Complex >::CopyData(
                    const size_t sizePadded,
                    const std::vector< InputNumber >& data,
                    std::valarray< Complex >& result)
            {
                result.resize(sizePadded);

                const size_t size = data.size();

                for (size_t i = 0; i < size; ++i)
                {
                    result[i] = data[i];
                }
            }
        }
    }
}