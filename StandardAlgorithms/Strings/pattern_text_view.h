#pragma once

namespace Standard::Algorithms::Strings
{
    template<class pattern_t1, class text_t1>
    struct pattern_text_view final
    {
        using pattern_t = pattern_t1;
        using text_t = text_t1;

        pattern_t pattern{};
        text_t text{};
    };

    // Deduction guide.
    template<class pattern_t, class text_t>
    pattern_text_view(pattern_t, text_t) -> pattern_text_view<pattern_t, text_t>;
} // namespace Standard::Algorithms::Strings
