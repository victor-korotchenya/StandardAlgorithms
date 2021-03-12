#pragma once
#include"stdream.h"
#include<ostream>
#include<stdexcept>
#include<string>

namespace Standard::Algorithms::Tests
{
    // Base class for a test case.
    struct base_test_case
    {
protected:
        inline constexpr explicit base_test_case(std::string &&name) noexcept
            : Name(std::move(name))
        {
        }

public:
        constexpr base_test_case(const base_test_case &) = default;
        constexpr auto operator= (const base_test_case &) & -> base_test_case & = default;
        constexpr base_test_case(base_test_case &&) noexcept = default;
        constexpr auto operator= (base_test_case &&) &noexcept -> base_test_case & = default;

        constexpr virtual ~base_test_case() noexcept = default;

        [[nodiscard]] inline constexpr auto name() const &noexcept -> const std::string &
        {
            return Name;
        }

        inline constexpr void name(std::string &&name) noexcept
        {
            Name = std::move(name);
        }

        inline constexpr virtual void validate() const
        {
            if (Name.empty()) [[unlikely]]
            {
                throw std::runtime_error("The test case Name must be a non-empty string.");
            }
        }

private:
        virtual inline constexpr void print([[maybe_unused]] stdream &str) const {}

        virtual inline constexpr void print([[maybe_unused]] std::ostream &str) const {}

        friend constexpr auto operator<< (stdream &str, const base_test_case &test) -> stdream &;

        friend auto operator<< (std::ostream &str, const base_test_case &test) -> std::ostream &;

        std::string Name;
    };

    auto operator<< (std::ostream &str, const base_test_case &test) -> std::ostream &;

    inline constexpr auto operator<< (stdream &str, const base_test_case &test) -> stdream &
    {
        // todo(p4): ::Standard::Algorithms::print_value("Name", str, test.name());
        str << " Name '" << test.name() << "'";
        test.print(str);
        return str;
    }
} // namespace Standard::Algorithms::Tests
