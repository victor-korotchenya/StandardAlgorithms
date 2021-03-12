#pragma once
#include <sstream>
#include <vector>
#include <stdexcept>
#include "StreamUtilities.h"
#include "ExceptionUtilities.h"

namespace Privet::Algorithms
{
    class ConvertUtilities final
    {
        ConvertUtilities() = delete;

    public:
        //Geven a std::string "001", converts it into a std::vector {false, false, true};
        static void StringToBooleanVector(
            const std::string& data, std::vector< bool>& result);

        //Geven a std::string "0 -1, 123 ; 5", converts it into a std::vector {0, -1, 123, 5};
        template <typename CastFunction, typename Number >
        static void StringToNumberVector(
            CastFunction castFunction,
            const std::string& data, std::vector< Number >& result);

    private:
        inline static bool IsNumber(const unsigned c) noexcept
        {
            const bool result = (c - unsigned('0')) <= unsigned(9);
            return result;
        }

        //Ignore '+'.
        inline static bool IsSign(const unsigned c) noexcept
        {
            const bool result = '-' == c;
            return result;
        }

        inline static bool IsNumberOrSign(const unsigned c) noexcept
        {
            const bool result = IsNumber(c) || IsSign(c);
            return result;
        }
    };

    template <typename CastFunction, typename Number >
    void ConvertUtilities::StringToNumberVector(
        CastFunction castFunction,
        const std::string& data,
        std::vector<Number>& result)
    {
        ThrowIfNull(castFunction, "castFunction");

        const size_t size = data.size();

        result.clear();

        size_t i = 0;

        while (i < size)
        {
            //Skip white space.
            while (!IsNumberOrSign(data[i]))
                if (size <= ++i)
                    return;

            auto isNegative = false;

            if (IsSign(data[i]))
            {
                isNegative = true;

                ++i;

                //Skip white space.
                while (i < size && !IsNumberOrSign(data[i]))
                {
                    ++i;
                }

                if (size <= i)
                {
                    throw std::runtime_error("Convert error:\
There must be a number after the minus '-' sign at the end of the std::string.");
                }

                if (IsSign(data[i]))
                {
                    std::ostringstream ss;
                    ss << "Convert error:\
There are at least 2 minus '-' signs at index=" << i << ".";
                    StreamUtilities::throw_exception(ss);
                }
            }

            const size_t startIndex = i;

            ++i;
            while (i < size && IsNumber(data[i]))
            {
                ++i;
            }

            const size_t length = i - startIndex;
            const std::string subString = data.substr(startIndex, length);

            Number number = castFunction(subString);
            if (isNegative)
            {
                number = -number;
            }

            result.push_back(number);
            ++i;
        }
    }
}