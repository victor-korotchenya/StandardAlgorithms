#include <array>
#include <string>
#include "double_dispatch_tests.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std::literals;
using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    struct Ser
    {
        virtual ~Ser() = default;
        virtual void append(const class Cat& a, string& report, const bool has_before) const = 0;
        virtual void append(const class Dog& a, string& report, const bool has_before) const = 0;
    };
    struct Json : Ser
    {
        void append(const class Cat& a, string& report, const bool has_before) const override;
        void append(const class Dog& a, string& report, const bool has_before) const override;
    };
    struct Xml : Ser
    {
        void append(const class Cat& a, string& report, const bool has_before) const override;
        void append(const class Dog& a, string& report, const bool has_before) const override;
    };

    struct Animal
    {
        virtual ~Animal() = default;

        const string& get_name() const
        {
            return name;
        }

        virtual void save(const class Ser& ser, string& report, const bool has_before) const = 0;

    protected:
        Animal(const string& name) : name(name)
        {
        }

    private:
        string name;
    };
    struct Cat : Animal
    {
        Cat(const string& name, const string& kind)
            : Animal(name), kind(kind)
        {}

        const string& get_kind() const
        {
            return kind;
        }

        void save(const class Ser& ser, string& report, const bool has_before) const override
        {
            ser.append(*this, report, has_before);
        }

    private:
        string kind;
    };
    struct Dog : Animal
    {
        Dog(const string& name) : Animal(name)
        {}

        void save(const class Ser& ser, string& report, const bool has_before) const override
        {
            ser.append(*this, report, has_before);
        }
    };

    void Json::append(const Cat& a, string& report, const bool has_before) const
    {
        if (has_before)
            report += ",";

        report += R"deli({"type"="cat","name"=")deli";
        report += a.get_name();
        report += R"deli(","kind"=")deli";
        report += a.get_kind();
        report += R"deli("})deli";
    }
    void Json::append(const Dog& a, string& report, const bool has_before) const
    {
        if (has_before)
            report += ",";

        report += R"deli({"type"="dog","name"=")deli";
        report += a.get_name();
        report += R"deli("})deli";
    }

    void Xml::append(const Cat& a, string& report, const bool) const
    {
        report += R"deli(<animal type="cat" name=")deli";
        report += a.get_name();
        report += R"deli(" kind=")deli";
        report += a.get_kind();
        report += R"deli("/>)deli";
    }

    void Xml::append(const Dog& a, string& report, const bool) const
    {
        report += R"deli(<animal type="dog" name=")deli";
        report += a.get_name();
        report += R"deli("/>)deli";
    }
}

void Privet::Algorithms::Strings::Tests::double_dispatch_tests()
{
    const Cat cat("Was", "home");
    const Dog dog("Dee");
    const array<const Animal*, 2> animals{ &cat, &dog };

    const Json json;
    const Xml xml;
    const array<pair<const Ser*, string>, 2> ser_reps{
        make_pair(&json, R"deli({"type"="cat","name"="Was","kind"="home"},{"type"="dog","name"="Dee"})deli"),
        make_pair(&xml, R"deli(<animal type="cat" name="Was" kind="home"/><animal type="dog" name="Dee"/>)deli") };

    string report;

    for (const auto& p : ser_reps)
    {
        report.clear();

        const auto& ser = *(p.first);
        auto has_before = false;

        for (const auto& animal : animals)
        {
            animal->save(ser, report, has_before);
            has_before = true;
        }

        Assert::AreEqual(p.second, report, "report");
    }
}