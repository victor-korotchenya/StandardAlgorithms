#include"work_critical_path.h"
#include"../Utilities/require_utilities.h"

void Standard::Algorithms::Graphs::Internal::restore_critical_path(std::size_t source,
    const std::vector<std::size_t> &parents, std::size_t destination, std::vector<std::size_t> &critical_path)
{
    const auto size = require_greater(parents.size(), 1U, "parents size");

    assert(1U < size && source != destination && source < size && destination < size);

    critical_path.clear();

    constexpr auto npos = std::size_t{} - static_cast<std::size_t>(1);

    for (auto cur = destination;;)
    {
        assert(cur != npos && cur < size);

        critical_path.push_back(cur);

        if (cur == source)
        {
            std::reverse(critical_path.begin(), critical_path.end());
            return;
        }

        cur = parents.at(cur);
    }
}
