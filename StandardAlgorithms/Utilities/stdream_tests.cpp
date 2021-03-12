#include"stdream_tests.h"
#include"stdream.h"
#include"test_utilities.h"
#include<cstdint>

namespace
{
    [[nodiscard]] constexpr auto opa_hopa()
    {
        const auto *const expect = "-1,000 is -11-101.\nDream! Is Dream!";

        Standard::Algorithms::stdream str{}; // 22 bytes might be reserved even for an empty string.
        str.reserve(std::string_view(expect).size());
        {
            constexpr auto num_1 = -1000LL;
            constexpr auto num_2 = -11;
            constexpr std::uint16_t num_3 = 101;

            str << num_1 << " is " << num_2 << '-' << num_3 << ".\n";
        }
        {
            const std::string line = "Dream!";
            const std::string_view view = line;
            str << line << std::string(" Is ") << view;
        }
        {
            const auto &actual = str.str();
            Standard::Algorithms::ert::are_equal(expect, actual, "stdream");
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Utilities::Tests::stdream_tests()
{
    static_assert(opa_hopa());
}
