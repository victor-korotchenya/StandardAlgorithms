#include "Random.h"

using namespace std;

void FillRandom(vector<bool>& data, const size_t size)
{
    data.resize(size);
    if (!size)
        return;

    constexpr auto bits_per_int = sizeof(unsigned int) << 3;
    random_device rd;
    size_t index = 0;

    for (;;)
    {
        const auto raw = rd();
        auto bit = 0u;
        do
        {
            data[index] = 0 != ((raw >> bit) & 1u);

            if (size <= ++index)
                return;
        } while (++bit < bits_per_int);
    }
}