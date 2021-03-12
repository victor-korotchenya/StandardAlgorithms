#include"double_dispatch_tests.h"
#include"../Utilities/test_utilities.h"
#include<array>

namespace
{
    struct cat;
    struct dog;

    struct ser
    {
        constexpr virtual ~ser() noexcept = default; // pure virt methods can have body in Java.

        constexpr ser() = default;
        constexpr ser(const ser &) = default;
        constexpr auto operator= (const ser &) & -> ser & = default;
        constexpr ser(ser &&) noexcept = default;
        constexpr auto operator= (ser &&) &noexcept -> ser & = default;

        constexpr virtual void append(const cat &anim, std::string &report, bool has_before) const = 0;
        constexpr virtual void append(const dog &anim, std::string &report, bool has_before) const = 0;
    };

    struct json : ser
    {
        constexpr void append(const cat &anim, std::string &report, bool has_before) const override;
        constexpr void append(const dog &anim, std::string &report, bool has_before) const override;
    };

    struct xml : ser
    {
        constexpr void append(const cat &anim, std::string &report, bool has_before) const override;
        constexpr void append(const dog &anim, std::string &report, bool has_before) const override;
    };

    struct animal
    {
        constexpr virtual ~animal() noexcept = default;

        constexpr animal() = default;
        constexpr animal(const animal &) = default;
        constexpr auto operator= (const animal &) & -> animal & = default;
        constexpr animal(animal &&) noexcept = default;
        constexpr auto operator= (animal &&) &noexcept -> animal & = default;

        [[nodiscard]] constexpr auto name() const &noexcept -> const std::string &
        {
            return Name;
        }

        constexpr virtual void save(const ser &ser, std::string &report, bool has_before) const = 0;

protected:
        constexpr explicit animal(std::string &&name)
            : Name(std::move(name))
        {
        }

private:
        std::string Name;
    };

    struct cat : animal
    {
        constexpr cat(std::string &&name, std::string &&kind)
            : animal(std::move(name))
            , Kind(std::move(kind))
        {
        }

        [[nodiscard]] constexpr auto kind() const &noexcept -> const std::string &
        {
            return Kind;
        }

        constexpr void save(const ser &ser, std::string &report, const bool has_before) const override
        {// 'this' is of type Cat*.
            ser.append(*this, report, has_before);
        }

private:
        std::string Kind;
    };

    struct dog : animal
    {
        constexpr explicit dog(std::string &&name)
            : animal(std::move(name))
        {
        }

        constexpr void save(const ser &ser, std::string &report, const bool has_before) const override
        {// And 'this' is of type Dog*.
            ser.append(*this, report, has_before);
        }
    };

    constexpr void json::append(const cat &anim, std::string &report, const bool has_before) const
    {
        if (has_before)
        {
            report += ",";
        }

        report += R"deli({"type"="cat","name"=")deli";
        report += anim.name();
        report += R"deli(","kind"=")deli";
        report += anim.kind();
        report += R"deli("})deli";
    }

    constexpr void json::append(const dog &anim, std::string &report, const bool has_before) const
    {
        if (has_before)
        {
            report += ",";
        }

        report += R"deli({"type"="dog","name"=")deli";
        report += anim.name();
        report += R"deli("})deli";
    }

    constexpr void xml::append(const cat &anim, std::string &report, const bool has_before) const
    {
        // NOLINTNEXTLINE NOLINT
        if (has_before)
        {
        }

        report += R"deli(<animal type="cat" name=")deli";
        report += anim.name();
        report += R"deli(" kind=")deli";
        report += anim.kind();
        report += R"deli("/>)deli";
    }

    constexpr void xml::append(const dog &anim, std::string &report, const bool has_before) const
    {
        // NOLINTNEXTLINE NOLINT
        if (has_before)
        {
        }

        report += R"deli(<animal type="dog" name=")deli";
        report += anim.name();
        report += R"deli("/>)deli";
    }
} // namespace

void Standard::Algorithms::Strings::Tests::double_dispatch_tests()
{
    const cat cat("Was", "home");
    const dog dog("Muddy");
    const std::array<const animal *, 2> animals{ &cat, &dog };

    const json json;
    const xml xml;
    const std::array<std::pair<const ser *, std::string>, 2> ser_reps{
        std::make_pair(&json, R"deli({"type"="cat","name"="Was","kind"="home"},{"type"="dog","name"="Muddy"})deli"),
        std::make_pair(&xml, R"deli(<animal type="cat" name="Was" kind="home"/><animal type="dog" name="Muddy"/>)deli")
    };

    std::string report;

    for (const auto &par : ser_reps)
    {
        report.clear();

        const auto &ser = *(par.first);
        auto has_before = false;

        for (const auto &animal : animals)
        {
            animal->save(ser, report, has_before);
            has_before = true;
        }

        ::Standard::Algorithms::ert::are_equal(par.second, report, "report");
    }
}
