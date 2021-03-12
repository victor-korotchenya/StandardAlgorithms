#include "TestNameUtilitiesTests.h"
#include <vector>
#include "../Utilities/TestNameUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    template <typename Type>
    static void TestInner(const string& name)
    {
        vector<Type> data;
        data.push_back(Type("Name1"));
        data.push_back(Type("Name2"));

        {
            const bool actual = TestNameUtilities::RequireUniqueNames(data, name, false);
            Assert::AreEqual(false, actual, name + " no duplicates");
        }
        data.push_back(data[0]);
        {
            const bool actual = TestNameUtilities::RequireUniqueNames(data, name, false);
            Assert::AreEqual(true, actual, name + " with duplicates");

            const auto expectedMessage = "The item \'Name1\' at index=2 is repeating. " + name,
                message = "throw " + name + " with duplicates";

            Assert::ExpectException<exception >(
                [&]() -> void { TestNameUtilities::RequireUniqueNames(data, name, true); },
                expectedMessage,
                message);
        }
    }

    struct FieldStruct final : base_test_case
    {
        using Number = int;
        Number Id;

        FieldStruct(string&& name = "", Number id = 0) : base_test_case(forward<string>(name)),
            Id(id)
        {
        }
    };

    struct MethodStruct final
    {
        using IdType = short int;

    private:
        IdType _Id;

        string _Name;

    public:
        MethodStruct(const string& name = "", IdType id = IdType(0)) : _Id(id), _Name(name)
        {
        }

        const string& get_Name() const
        {
            return _Name;
        }

        void set_Name(const string& name)
        {
            _Name = name;
        }
    };
}

void TestNameUtilitiesTests()
{
    TestInner< FieldStruct >("FieldStruct");
    TestInner< MethodStruct >("MethodStruct");
}