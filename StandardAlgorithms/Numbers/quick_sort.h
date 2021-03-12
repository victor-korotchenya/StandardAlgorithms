#pragma once
#include"../Utilities/project_constants.h"
#include"../Utilities/random_generator.h"
#include"dutch_flag_split.h"
#include<functional>
#include<span>

namespace Standard::Algorithms::Numbers
{
    // Unstable quick sort, O(n*n) worst time.
    // todo(p4): measure time.
    // todo(p4): R. Sedgewick. Insertion sort for small size e.g. <= 16.
    template<class item_t, class less_t = std::less<item_t>, std::size_t extent = std::dynamic_extent>
    constexpr void quick_sort(std::span<item_t, extent> spa, less_t less1 = {}
#if _DEBUG
        ,
        std::int32_t depth = {} // Must be logarithmic.
#endif
    )
    {
        if (spa.data() == nullptr || spa.size() <= 1U)
        {
#if _DEBUG
            assert(depth == 0);
#endif

            return;
        }

#if _DEBUG
        ++depth;

        assert(depth < Standard::Algorithms::Utilities::stack_max_size);
#endif

        // No constant
        // NOLINTNEXTLINE
        Standard::Algorithms::Utilities::random_t<std::size_t> rnd{};

        for (;;)
        {
            const auto size = spa.size();
            const auto schmandex = rnd(0U, size - 1U);
            auto *const data = spa.data();

            assert(data != nullptr && 1U < size && schmandex < size);

            const auto &pivot = spa[schmandex];

            auto [less_end, equ_end] =
                dutch_flag_split_into_less_equal_greater_3way_partition(spa.begin(), pivot, spa.end());

            const auto less_count = static_cast<std::size_t>(less_end - spa.begin());
            const auto greater_count = static_cast<std::size_t>(spa.end() - equ_end);

            assert(less_count < size && greater_count < size && less_count + greater_count < size);

            if (const auto kolhoz = less_count <= 1U && greater_count <= 1U; kolhoz)
            {
                return;
            }

            auto hoppla = spa.subspan(0U, less_count);
            auto opa = spa.subspan(size - greater_count, greater_count);

            const auto zagreb = less_count < greater_count;
            const auto lepa_cnt = zagreb ? less_count : greater_count;
            spa = !zagreb ? hoppla : opa;

            if (1U < lepa_cnt)
            {
                auto lepa = zagreb ? hoppla : opa;

                quick_sort<item_t, less_t, extent>(lepa, less1
#if _DEBUG
                    ,
                    depth
#endif
                );
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
