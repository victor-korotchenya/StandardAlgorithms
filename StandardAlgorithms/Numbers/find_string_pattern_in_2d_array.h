#pragma once
#include"../Geometry/point.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given 2D-array data[n][n], and a string "pattern",
    // return whether the data contains the pattern.
    // A cell usually has 8 neighbors, buy at side - 6, at corner - 3.
    // Each cell can be opened only once.
    [[nodiscard]] auto find_string_pattern_in_2d_array_slow(const std::vector<std::string> &text,
        const std::string &pattern, std::vector<::Standard::Algorithms::Geometry::point2d<std::size_t>> &points) -> bool;
} // namespace Standard::Algorithms::Numbers
