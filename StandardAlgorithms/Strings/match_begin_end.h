#pragma once
#include<algorithm>
#include<cassert>
#include<cstddef>
#include<cstdint>
#include<tuple>
#include<utility>

namespace Standard::Algorithms::Strings
{
    // Given "beginABCend" and "beginDEFGHend", return { 5, 8, 10 }.
    [[nodiscard]] static constexpr auto match_begin_end(const auto &source, const auto &destination) noexcept
        -> std::tuple<std::size_t, std::size_t, std::size_t>
    {
        auto source_stop = source.size();
        auto destination_stop = destination.size();
        auto min_size = std::min(source_stop, destination_stop);
        std::size_t start{};

        while (start < min_size && source[start] == destination[start])
        {
            ++start;
        }

        min_size -= start;

        std::size_t ending = 1;

        while (ending <= min_size && source[source_stop - ending] == destination[destination_stop - ending])
        {
            ++ending;
        }

        --ending;
        source_stop -= ending;
        destination_stop -= ending;

        assert(start <= std::min(source_stop, destination_stop) && source_stop <= source.size());
        assert(destination_stop <= destination.size());

        return { start, source_stop, destination_stop };
    }
} // namespace Standard::Algorithms::Strings
