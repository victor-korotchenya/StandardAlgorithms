#pragma once
#include <string>
#include <ostream>

namespace Privet::Algorithms::Tests
{
    // Base class for a test case.
    class base_test_case
    {
        std::string Name;

    protected:
        explicit base_test_case(std::string&& name);

    public:
        virtual ~base_test_case() = default;

        const std::string& get_Name() const;

        void set_Name(std::string&& name);

        virtual void Validate() const;

        virtual void Print(std::ostream& str) const;
    };

    std::ostream& operator << (std::ostream& str, base_test_case const& t);
}