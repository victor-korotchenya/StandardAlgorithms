 #pragma once
#include <stdexcept>
#include <vector>

namespace Privet
{
  namespace Algorithms
  {
    namespace Numbers
    {
      //Given an array A[0..(n-1)], calculate another array B[0..(n-1)],
      // where  B[i] = PROD(A[j] for all j) / A[i].
      // Note that A[i] can be zero.
      template <typename Number = long long int>
      class ArrayProductDividingCurrentIndex final
      {
        ArrayProductDividingCurrentIndex() = delete;

      public:
        //Overflow check is not performed.
        static std::vector<Number> CalcSlow(const std::vector<Number> &data);

        //Overflow check is not performed.
        static std::vector<Number> Calc(const std::vector<Number> &data);

      private:
        static void CheckSize(const size_t size);
      };

      template <typename Number>
      std::vector<Number> ArrayProductDividingCurrentIndex<Number>::CalcSlow(
        const std::vector<Number> &data)
      {
        const size_t size = data.size();
        CheckSize(size);

        std::vector<Number> result(size);

        for (size_t i = 0; i < size; ++i)
        {
          Number actual = 1;
          for (size_t j = 0; j < size; ++j)
          {
            if (i != j)
            {
              actual *= data[j];
            }
          }

          result[i] = actual;
        }

        return result;
      }

      template <typename Number>
      std::vector<Number> ArrayProductDividingCurrentIndex<Number>::Calc(
        const std::vector<Number> &data)
      {
        const size_t size = data.size();
        CheckSize(size);

        std::vector<Number> result(size);
        result[0] = Number(1);

        for (size_t i = 1; i < size; ++i)
        {
          result[i] = result[i - 1] * data[i - 1];
        }

        // 0 1 2 3 4
        // A B C D E
        // 1 A AB ABC ABCD
        //            ABCD - it is already done
        //        ABC      - to be multiplied by E.

        Number temp{ 1 };

        size_t j = size - 2;
        for(;;)
        {
          temp *= data[j + 1];
          result[j] *= temp;
          if (0 == j)
          {
            break;
          }

          --j;
        }

        return result;
      }

      template <typename Number>
      void ArrayProductDividingCurrentIndex<Number>::CheckSize(const size_t size)
      {
        if (size < 2)
        {
          throw std::runtime_error("The 'data size' must be at least 2.");
        }
      }
    }
  }
}