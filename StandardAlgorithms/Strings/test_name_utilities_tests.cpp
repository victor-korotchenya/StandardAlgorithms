#include"test_name_utilities_tests.h"
#include"../Utilities/test_name_utilities.h"
#include"../Utilities/test_utilities.h"
#include<cstdint>
#include<vector>

namespace
{
    template<class typ>
    // todo(p3): [[nodiscard]]
    constexpr auto test_inner(const std::string &name) -> bool
    {
        std::vector<typ> data{};
        data.emplace_back("Name1", 0);
        data.emplace_back("Name2", 0);

        {
            const auto actual =
                Standard::Algorithms::Utilities::test_name_utilities::require_unique_names(data, name, false);

            ::Standard::Algorithms::ert::are_equal(false, actual, name + " no duplicates");
        }

        data.push_back(data.at(0));
        {
            const auto actual =
                Standard::Algorithms::Utilities::test_name_utilities::require_unique_names(data, name, false);

            ::Standard::Algorithms::ert::are_equal(true, actual, name + " with duplicates");

            const auto expected_message = "The item \'Name1\' at index 2 is repeating. " + name;

            const auto message = "throw " + name + " with duplicates";

            ::Standard::Algorithms::ert::expect_exception<std::exception>(
                expected_message,
                [&]() -> void
                {
                    Standard::Algorithms::Utilities::test_name_utilities::require_unique_names(data, name, true);
                },
                message);
        }

        return true;
    }

    struct field_struct final : Standard::Algorithms::Tests::base_test_case
    {
        using id_type = std::uint32_t;

        constexpr field_struct(std::string &&name, id_type id1) noexcept
            : base_test_case(std::move(name))
            , Ide(id1)
        {
        }

        [[nodiscard]] constexpr auto id() const &noexcept -> const id_type &
        {
            return Ide;
        }

private:
        id_type Ide;
    };

    struct method_struct final
    {
        using id_type = std::int16_t;

        constexpr method_struct(
            // Test of copy.
            // NOLINTNEXTLINE
            const std::string &name, id_type id1)
            : Ide(id1)
            , Name(name)
        {
        }

        [[nodiscard]] constexpr auto id() const &noexcept -> const id_type &
        {
            return Ide;
        }

        [[nodiscard]] constexpr auto name() const &noexcept -> const std::string &
        {
            return Name;
        }

        constexpr void name(std::string &&name) noexcept
        {
            Name = std::move(name);
        }

private:
        id_type Ide;
        std::string Name;
    };
} // namespace

void Standard::Algorithms::Strings::Tests::test_name_utilities_tests()
{
    // todo(p3): static_assert
    (test_inner<field_struct>("Field struct"));
    // todo(p3): static_assert
    (test_inner<method_struct>("Method struct"));
}
