#include"generatorin_tests.h"
#include"ert.h"
#include"generatorin.h"
#include<vector>

namespace
{
    template<std::unsigned_integral int_t>
    [[nodiscard]] auto generator(const int_t maxi) -> Standard::Algorithms::Utilities::generatorin<int_t>
    {
        for (int_t index{}; index < maxi; ++index)
        {
            co_yield index;
        }
    }

    void corout_test()
    {
        using int_t = std::uint64_t;

        constexpr int_t maxi{ 5U };

        int_t index{};

        for (auto gen = generator(maxi); gen.has_more();)
        {
            Standard::Algorithms::require_greater(maxi, index, "Watch one's back to prevent an endless cycle.");

            const auto actual = gen.value();
            Standard::Algorithms::ert::are_equal(index, actual, "intermediate index");
            ++index;
        }

        Standard::Algorithms::ert::are_equal(maxi, index, "final index");
    }
} // namespace

void Standard::Algorithms::Utilities::Tests::generatorin_tests()
{
    corout_test();
}
