#pragma once
#include <ostream>
#include <vector>

class NumberSplitterTests final
{
    NumberSplitterTests() = delete;

    static const unsigned _MaxSeparatorCounter = 100;

public:
    static void Test();

    inline static unsigned MaxSeparatorCounter()
    {
        return _MaxSeparatorCounter;
    }

private:
    using Number = short;

public:

    struct TestCase final
    {
        std::string Name;
        inline const std::string& get_Name() const
        {
            return Name;
        }
        std::vector< Number > Numbers;

        std::vector< Number > ExpectedNumbers;

        friend std::ostream& operator <<
            (std::ostream& str, TestCase const& a);
    };

private:
    static void GenerateTestCases(std::vector<TestCase>& testCases);
};