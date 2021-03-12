#pragma once
#include <concepts>
#include <iomanip>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include "Utilities/PrintUtilities.h"
#include "Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms
{
    template<class c, class v = typename c::value_type, class size_type = c::size_type>
    concept container = std::unsigned_integral<size_type> &&
        std::default_initializable<c> &&
        std::copy_constructible<c> &&
        requires (c a) {
            { a.size() }     -> std::same_as<size_type>;
            { a.empty() }    -> std::convertible_to<bool>;
    };

    template <typename T1, typename T2>
    static bool ComparePtr(const T1* const a, const T2* const b)
    {
        if (a == b)
            return true;

        if (nullptr == a || nullptr == b)
            return false;

        const auto result = *a == *b;
        return result;
    }

    template <typename T>
    std::ostringstream& output_value(
        std::ostringstream& str,
        const T& a)
    {
        str << a;
        return str;
    }

    //Print "std::string", surrounded with quotes (').
    template <>
    inline std::ostringstream& output_value<std::string>(
        std::ostringstream& str,
        const std::string& a)
    {
        str << "'" << a << "'";
        return str;
    }

    //Print "unsigned char" as "unsigned int" so that
    // people could read it.
    template <>
    inline std::ostringstream& output_value<unsigned char>(
        std::ostringstream& str,
        const unsigned char& a)
    {
        str << static_cast<unsigned>(a);
        return str;
    }

    //Print "char" as "int" so that
    // people could read it.
    template <>
    inline std::ostringstream& output_value<char>(
        std::ostringstream& str,
        const char& a)
    {
        str << static_cast<int>(a);
        return str;
    }

    template <>
    inline std::ostringstream& output_value<double>(
        std::ostringstream& str,
        const double& a)
    {
        const auto prec = StreamUtilities::GetMaxDoublePrecision();
        str << std::setprecision(prec) << a;
        return str;
    }

    template <>
    inline std::ostringstream& output_value<long double>(
        std::ostringstream& str,
        const long double& a)
    {
        const auto prec = StreamUtilities::GetMaxDoublePrecision();
        str << std::setprecision(prec) << a;
        return str;
    }

    template <typename T1, typename T2>
    void NotEqualThrowException(
        const T1& a, const T2& b, const std::string& message)
    {
        std::ostringstream ss;
        output_value(ss, b)
            << " must be equal to \n";

        output_value(ss, a)
            << ".";

        StreamUtilities::throw_exception(ss, message);
    }

    template <typename T1, typename T2>
    void EqualThrowException(
        const T1& a, const T2& b, const std::string& message)
    {
        std::ostringstream ss;
        output_value(ss, b)
            << " must Not be equal to \n";

        output_value(ss, a)
            << ".";

        StreamUtilities::throw_exception(ss, message);
    }

    struct Assert final
    {
        Assert() = delete;

        template <typename T>
        static void IsNotNullPtr(const T* const a, const std::string& message)
        {
            if (nullptr == a)
            {
                std::ostringstream ss;
                ss << "Null returned, not null is expected.";

                StreamUtilities::throw_exception(ss, message);
            }
        }

        template <typename T>
        static void IsNullPtr(const T* const a, const std::string& message)
        {
            if (nullptr != a)
            {
                std::ostringstream ss;
                ss << "Null is expected, but returned not null:\n";

                output_value(ss, *a)
                    << ".";

                StreamUtilities::throw_exception(ss, message);
            }
        }

        template <typename TException, typename TFunc = void(*)(void) >
        static void ExpectException(
            const TFunc function1,
            const std::string& expectedMessage,
            const std::string& message)
        {
            try
            {
                function1();
            }
            catch (const TException& ex)
            {
                const char* actual = ex.what();
                const std::string_view actualMesage = actual;
                if (expectedMessage == actualMesage)
                    return;

                std::ostringstream ss;
                ss << "Expected\n" << expectedMessage
                    << "\n but got\n" << actualMesage;

                StreamUtilities::throw_exception(ss, message);
            }

            {
                std::ostringstream ss;
                ss << "Did not get the expected message '" << expectedMessage
                    << "'.";

                StreamUtilities::throw_exception(ss, message);
            }
        }

        //The "T" must implement both: operator << AND operator ==
        template <typename T1, typename T2>
        static void AreEqualByPtr(
            const T1* const a, const T2* const b, const std::string& message)
        {
            if (a == b)
            {
                return;
            }

            if (nullptr == a
                //Pacify the code analyzer.
                && nullptr != b)
            {
                std::ostringstream ss;
                output_value(ss, *b)
                    << "\n must be equal to \nnullptr.";

                StreamUtilities::throw_exception(ss, message);
            }

            if (nullptr == b
                //Pacify the code analyzer.
                && nullptr != a)
            {
                std::ostringstream ss;
                ss << "The nullptr must be equal to \n";
                output_value(ss, *a);

                StreamUtilities::throw_exception(ss, message);
            }

            AreEqual<T1, T2>(*a, *b, message);
        }

        template <typename T1, typename T2>
        static void AreEqual(const T1& a, const T2& b, const std::string& message)
        {
            const auto messageC = message.c_str();
            AreEqual<T1, T2>(a, b, messageC);
        }

        template <typename T1, typename T2>
        static void AreEqual(const T1& a, const T2& b, const char* message)
        {
            if (!(a == b))
            {
                NotEqualThrowException<T1, T2>(a, b, message);
            }
        }

        template <typename T1, typename T2>
        static void Greater(const T1& greater, const T2& smaller, const char* message)
        {
            if (!(smaller < greater))
            {
                std::ostringstream ss;
                output_value(ss, greater)
                    << "\n must be greater than \n";

                output_value(ss, smaller);

                StreamUtilities::throw_exception(ss, message);
            }
        }

        template <typename T1, typename T2>
        static void Greater(
            const T1& greater, const T2& smaller, const std::string& message)
        {
            const char* const messageC = message.empty() ? nullptr : message.c_str();
            Greater<T1, T2>(greater, smaller, messageC);
        }

        template <typename T1, typename T2>
        static void GreaterOrEqual(
            const T1& greater, const T2& smaller, const std::string& message)
        {
            const char* const messageC = message.empty() ? nullptr : message.c_str();
            GreaterOrEqual<T1, T2>(greater, smaller, messageC);
        }

        template <typename T1, typename T2>
        static void GreaterOrEqual(
            const T1& greater, const T2& smaller, const char* message)
        {
            if (greater < smaller)
            {
                std::ostringstream ss;
                output_value(ss, greater)
                    << "\n must be greater than or equal to \n";

                output_value(ss, smaller);

                StreamUtilities::throw_exception(ss, message);
            }
        }

        template <typename Number1, typename Number2>
        static void AreEqualWithEpsilon(
            const Number1& a, const Number1& b,
            bool (*compareFunction)(const Number1& x, const Number2& y),
            const char* message)
        {
#ifdef _DEBUG
            if (nullptr == compareFunction)
            {
                throw std::runtime_error("nullptr == compareFunction");
            }
#endif
            if (!(compareFunction(a, b)))
            {
                NotEqualThrowException<Number1, Number2>(a, b, message);
            }
        }

        static void AreEqualWithEpsilon(const double& a, const double& b, const char* message, const double epsilon = 1.e-9);

        template <class a_t, class b_t>
        requires container<a_t>&& container<b_t>
            static void AreEqualWithEpsilon(const a_t& a, const b_t& b, const std::string& message, const double epsilon = 1.e-9)
        {
            const auto size = a.size();
            AreEqual(size, b.size(), message + " size");

            size_t i = 0;
            try
            {
                for (; i < size; i++)
                    AreEqualWithEpsilon(a[i], b[i], message.c_str(), epsilon);
            }
            catch (const std::exception& e)
            {
                const auto err = "Error at index " + std::to_string(i) + ". " + e.what();
                throw std::runtime_error(err);
            }
        }

        template <typename T1, typename T2>
        static void NotEqual(const T1& a, const T2& b, const std::string& message)
        {
            const char* messageC = message.c_str();
            NotEqual<T1, T2>(a, b, messageC);
        }

        template <typename T1, typename T2>
        static void NotEqual(const T1& a, const T2& b, const char* message)
        {
            if (a == b)
            {
                EqualThrowException<T1, T2>(a, b, message);
            }
        }

        template <typename TItem>
        static void Empty(
            const std::vector<TItem>& data,
            const std::string& message)
        {
            Empty(data, message.c_str());
        }

        template <typename TItem>
        static void Empty(
            const std::vector<TItem>& data,
            const char* const  message)
        {
            if (!StreamUtilities::HasMessage(message))
            {
                throw std::runtime_error("The 'message' must be not empty string.");
            }

            if (!data.empty())
            {
                std::ostringstream ss;
                ss << "The actual result must be empty. First item:\n";
                output_value(ss, data[0]);

                StreamUtilities::throw_exception(ss, message);
            }
        }

        template <typename TItem>
        static void NotEmpty(
            const std::vector<TItem>& data,
            const std::string& message)
        {
            NotEmpty<TItem>(data, message.c_str());
        }

        template <typename TItem>
        static void NotEmpty(
            const std::vector<TItem>& data,
            const char* const  message)
        {
            if (!StreamUtilities::HasMessage(message))
            {
                throw std::runtime_error("The 'message' must be not empty string.");
            }

            if (data.empty())
            {
                std::ostringstream ss;
                ss << "The actual result must be not empty.";

                StreamUtilities::throw_exception(ss, message);
            }
        }

        template <typename T1, typename T2>
        static void AreEqual(
            const std::vector<std::vector<T1>>& expectedVector,
            const std::vector<std::vector<T2>>& actualVector,
            const std::string& message)
        {
            const auto c_str = message.c_str();
            AreEqual<T1, T2>(expectedVector, actualVector, c_str);
        }

        template <typename T1, typename T2>
        static void AreEqual(
            const std::vector<std::vector<T1>>& expectedVector,
            const std::vector<std::vector<T2>>& actualVector,
            const char* const  message)
        {
            const size_t expectedSize = expectedVector.size();
            const size_t actualSize = actualVector.size();

            if (expectedSize != actualSize)
            {
                std::ostringstream ss;
                ss <<
                    "Expect \n" << expectedSize << " item(s), but got \n"
                    << actualSize << " item(s).";

                StreamUtilities::throw_exception(ss, message);
            }

            size_t i = 0;
            try
            {
                for (; i < expectedSize; ++i)
                {
                    const std::vector<T1>& expectedLine = expectedVector[i];
                    const std::vector<T2>& actualLine = actualVector[i];

                    AreEqual<T1, T2>(expectedLine, actualLine, message);
                }
            }
            catch (const std::exception& ex)
            {
                std::ostringstream ss;
                ss << "Vectors of vectors mismatch at index=" << i
                    << ". " << ex.what();

                StreamUtilities::throw_exception(ss);
            }
        }

        static inline void AreEqual(
            const std::vector<bool>& expectedVector,
            const std::vector<bool>& actualVector,
            const std::string& message)
        {
            const auto c_str = message.c_str();
            AreEqual(expectedVector, actualVector, c_str);
        }

        template <typename T1, typename T2>
        static void AreEqual(
            const std::vector<T1>& expectedVector,
            const std::vector<T2>& actualVector,
            const std::string& message)
        {
            const auto c_str = message.c_str();
            AreEqual<T1, T2>(expectedVector, actualVector, c_str);
        }

        template <typename T1, typename T2>
        static void AreEqual(
            const std::vector<T1>& expectedVector,
            const std::vector<T2>& actualVector,
            const char* const message)
        {
            T1* expected = const_cast<T1*>(expectedVector.data());
            const size_t expectedSize = expectedVector.size();

            T2* actual = const_cast<T2*>(actualVector.data());
            const size_t actualSize = actualVector.size();

            AreEqual<T1, T2>(
                expected, expectedSize,
                actual, actualSize,
                message);
        }

        template <typename T1, typename T2>
        static void AreEqual(
            const T1* expected, const size_t expectedSize,
            const std::vector<T2>& actualVector,
            const char* const  message)
        {
            T2* actual = const_cast<T2*>(actualVector.data());
            const size_t actualSize = actualVector.size();

            AreEqual<T1, T2>(
                expected, expectedSize,
                actual, actualSize,
                message);
        }

        template <typename T1, typename T2>
        static void AreEqual(
            const T1* expected, const size_t expectedSize,
            const T2* actual, const size_t actualSize,
            //Can be nullptr, but not recommended.
            const char* const  message)
        {
            AreEqual<T1, T2>(
                expected, expectedSize,
                actual, actualSize,
                message,
                ComparePtr<T1, T2>);
        }

        template <typename T1, typename T2, typename Comparer >
        static void AreEqual(
            const std::vector<T1>& expectedVector,
            const std::vector<T2>& actualVector,
            //Can be nullptr, but not recommended.
            const char* const  message,
            Comparer comparer)
        {
            const T1* expected = const_cast<const T1*>(expectedVector.data());
            const size_t expectedSize = expectedVector.size();

            const T2* actual = const_cast<const T2*>(actualVector.data());
            const size_t actualSize = actualVector.size();

            AreEqual<T1, T2, Comparer >(
                expected, expectedSize,
                actual, actualSize,
                message,
                comparer);
        }

        //The original one for arrays.
        //
        //To solve the error C2679: binary '<<' :
        // no operator found which takes a right-hand operand of type
        // please implement the "operator <<".
        template <typename T1, typename T2, typename Comparer>
        static void AreEqual(
            const T1* expected, const size_t expectedSize,
            const T2* actual, const size_t actualSize,
            //Can be nullptr, but not recommended.
            const char* const  message,
            Comparer comparer)
        {
            ThrowIfNull(comparer, "comparer");

            if (expectedSize == actualSize
                && (0 == expectedSize || (void*)expected == (void*)actual))
            {
                return;
            }

            if (nullptr == expected || 0 == expectedSize)
            {
                std::ostringstream ss;
                ss <<
                    "Empty result is 'expected', but got "
                    << (nullptr == actual ? 0 : actualSize)
                    << " item(s). actualSize=" << actualSize << ".";

                if (0 < actualSize && nullptr != actual)
                {
                    ss << " The first actual item is:\n";

                    output_value(ss, actual[0]);
                }

                throw StreamUtilities::throw_exception(ss, message);
            }

            if (expectedSize != actualSize || nullptr == actual)
            {
                std::ostringstream ss;
                ss <<
                    "Expect \n" << expectedSize << " item(s), but got \n"
                    << (nullptr == actual ? 0 : actualSize)
                    << " item(s). actualSize=" << actualSize << ".";

                if (0 < expectedSize && 0 == actualSize)
                {
                    ss << " The first item must be:\n";
                    output_value(ss, expected[0]);
                }

                throw StreamUtilities::throw_exception(ss, message);
            }

            const T1* expectedEnd = expected + expectedSize;
            const T2* actualEnd = actual + actualSize;

            size_t count = 0;
            for (;;)
            {
                const bool isEndOfExpected = expected == expectedEnd;
                const bool isEndOfActual = actual == actualEnd;
                if (isEndOfExpected || isEndOfActual)
                {
                    if (isEndOfExpected && isEndOfActual)
                    {
                        return;
                    }

                    std::ostringstream ss;
                    ss <<
                        (isEndOfExpected
                            ? "There is at least one more item in the actuals; expected count="
                            : "There is at least one more item in the expected; actual count=")
                        << count << ".";

                    StreamUtilities::throw_exception(ss, message);
                }

                if (comparer(expected, actual))
                {
                    ++count;
                    ++expected;
                    ++actual;
                }
                else
                {
                    std::ostringstream ss;
                    output_value(ss, *expected)
                        << "\n is expected, but actual is \n";

                    output_value(ss, *actual)
                        << "\n at position=" << count << ".";

                    StreamUtilities::throw_exception(ss, message);
                }
            }
        }

        static inline void AreEqual(
            const std::vector<bool>& expected,
            const std::vector<bool>& actual,
            const char* const  message)
        {
            const auto expectedSize = expected.size();
            const auto actualSize = actual.size();
            if (0 == expectedSize)
            {
                if (expectedSize == actualSize)
                    return;

                std::ostringstream ss;
                ss << "Empty result is 'expected', but actually got "
                    << actualSize << " item(s).";

                ss << " The first actual item is:\n";
                output_value(ss, actual[0]);

                throw StreamUtilities::throw_exception(ss, message);
            }

            if (expectedSize != actualSize)
            {
                std::ostringstream ss;
                ss << "Expect \n" << expectedSize << " item(s), but actually got \n"
                    << actualSize << " item(s).";

                if (0 == actualSize)
                {
                    ss << " The first item must be:\n";
                    output_value(ss, expected[0]);
                }

                throw StreamUtilities::throw_exception(ss, message);
            }

            for (size_t i = 0; i < expectedSize; i++)
            {
                // todo. p2. make eq faster by 64 bit compares.
                if (expected[i] != actual[i])
                {
                    std::ostringstream ss;
                    output_value(ss, expected[i])
                        << "\n is expected, but actual is \n";
                    output_value(ss, actual[i])
                        << "\n at position=" << i << ".";

                    StreamUtilities::throw_exception(ss, message);
                }
            }
        }

        template<class c1, class c2>
        static void AreEquals(
            const std::set<c1>& expected,
            const std::set<c2>& actual,
            const std::string& message)
        {
            const auto expectedSize = expected.size();
            const auto actualSize = actual.size();
            if (0 == expectedSize)
            {
                if (expectedSize == actualSize)
                    return;

                std::ostringstream ss;
                ss << "Empty result is 'expected', but actually got "
                    << actualSize << " item(s).";

                ss << " The first actual item is:\n";
                output_value(ss, *actual.begin());

                throw StreamUtilities::throw_exception(ss, message);
            }

            if (expectedSize != actualSize)
            {
                std::ostringstream ss;
                ss << "Expect \n" << expectedSize << " item(s), but actually got \n"
                    << actualSize << " item(s).";

                if (0 == actualSize)
                {
                    ss << " The first item must be:\n";
                    output_value(ss, *expected.begin());
                }

                throw StreamUtilities::throw_exception(ss, message);
            }

            auto it_expected = expected.begin();
            auto it_actual = actual.begin();
            auto i = 0;
            for (;;)
            {
                AreEqual(*it_expected, *it_actual, message + " at " + std::to_string(i));
                if (++it_expected == expected.end())
                    break;

                ++it_actual, ++i;
            }
        }
    };
}