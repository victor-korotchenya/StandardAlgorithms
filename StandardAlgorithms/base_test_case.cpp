#include <exception>
#include <stdexcept>
#include "base_test_case.h"
#include "Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

base_test_case::base_test_case(std::string&& name)
    : Name(forward<std::string>(name))
{
}

const std::string& base_test_case::get_Name() const
{
    return Name;
}

void base_test_case::set_Name(std::string&& name)
{
    Name = std::forward<std::string>(name);
}

void base_test_case::Validate() const
{
    if (this->Name.empty())
    {
        throw runtime_error("Test case Name must be non-empty.");
    }
}

std::ostream& Privet::Algorithms::Tests::operator<<(std::ostream& str, base_test_case const& t)
{
    t.Print(str);
    return str;
}

void base_test_case::Print(ostream& str) const
{
    PrintValue(str, "Name", Name);
}