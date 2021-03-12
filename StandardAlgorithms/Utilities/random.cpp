#include"random.h"

void Standard::Algorithms::Utilities::fill_random(std::vector<bool> &data, const std::size_t size)
{
    data.resize(size);

    if (size == 0U)
    {
        return;
    }

    using int_t = std::uint32_t;

    constexpr int_t bits_per_int = sizeof(int_t) << 3U;

    Standard::Algorithms::Utilities::random_t<int_t> rnd{};

    for (std::size_t index{};;)
    {
        const auto raw = rnd();
        auto bit = 0U;

        do
        {
            // todo(p4): do it 7 times faster on large inputs.
            data[index] = 0U != ((raw >> bit) & 1U);

            if (size == ++index)
            {
                return;
            }
        } while (++bit < bits_per_int);
    }
}

void Standard::Algorithms::Utilities::fill_random(
    const std::size_t rows, std::vector<std::vector<bool>> &data, const std::size_t columns)
{
    data.resize(rows);

    for (auto &line : data)
    {
        fill_random(line, columns);
    }
}
