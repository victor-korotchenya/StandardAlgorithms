#include"get_sub_test_semaphore.h"
#include"../Numbers/arithmetic.h"
#include"compute_core_count.h"
#include<limits>

namespace
{
    // All tests must lock - a global variable makes such code simpler.
    // NOLINTNEXTLINE
    std::counting_semaphore<> sem(Standard::Algorithms::Numbers::add_signed(1,
        static_cast<std::int32_t>(
            Standard::Algorithms::Utilities::compute_core_count(std::numeric_limits<std::size_t>::max()))));
} // namespace

[[nodiscard]] auto Standard::Algorithms::Tests::get_sub_test_semaphore() -> std::counting_semaphore<> &
{
    // NOLINTNEXTLINE
    return sem;
}
