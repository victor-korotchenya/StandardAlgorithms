#pragma once
#include <sstream>
#include <vector>

class ArithmeticTests final
{
    ArithmeticTests() = delete;

public:
    static void Test();

    using TUnsigned = unsigned short;
    using TSigned = signed short;

    template <typename T >
    struct Triple final
    {
        T A, B, Expected;

        friend std::ostream& operator << (std::ostream& str, Triple const& t)
        {
            t.Print(str);
            return str;
        }

        void Print(std::ostream& str) const;
    };

    template <typename T>
    using AddFunction = T(*)(T const a, T const b);

private:
    static void TestAddSigned();
    static void TestAddUnsigned();

    //TODO: p2. static void TestSubtractUnsigned();
    static void TestSubtractSigned();
    static void Test_numbers_having_reminder();

    template <typename T>
    static void Fail(
        const std::vector<Triple<T>>& testCases,
        const std::string& prefixName,
        const std::string& badPart1,
        const std::string& badPart2,
        AddFunction<T> addFunction);

    using TU3 = Triple<TUnsigned>;
    using TS3 = Triple<TSigned>;

    template <typename T>
    static void Success(
        const std::vector<Triple<T>>& testCases,
        const std::string& prefixName,
        AddFunction<T> aAddFunction);
};

template <typename T>
void ArithmeticTests::Triple<T>::Print(std::ostream& str) const
{
    str << "A=" << A
        << ", B=" << B
        << ", Expected=" << Expected;
}