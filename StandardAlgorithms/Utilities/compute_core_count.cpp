#include"compute_core_count.h"
#include<algorithm>
#include<thread>

[[nodiscard]] auto Standard::Algorithms::Utilities::compute_core_count(
    const std::size_t input_size, std::size_t cpu_count) noexcept -> std::size_t
{
    std::size_t result = 1;

    if (1U < input_size)
    {
        if (cpu_count == 0U)
        {
            cpu_count = std::thread::hardware_concurrency();
        }

        if (cpu_count == 0U)
        {
            cpu_count = 1;
        }

        // Do not waste CPU cores.
        result = std::min(input_size, cpu_count);
    }

    return result;
}
