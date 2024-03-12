// Copyright (c) 2024, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Vector Slicer.
// If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 11/03/2024.
//

#ifndef VECTOR_SLICER_SEED_LINE_SORTING_H
#define VECTOR_SLICER_SEED_LINE_SORTING_H

#include <vector>
#include "../seed_point.h"
#include "../filled_pattern.h"

/// Pair containing a seed point and the corresponding index in path sequence.
using SeedPair = std::pair<SeedPoint, int>;

/// Sorted vector of pairs(seed point, index_in_paths) corresponding to the same seed line.
using SeedLine = std::vector<SeedPair>;

/// Sorts paths according to their
class SeedLineSorting {
    std::vector<Path> unsorted_paths;
    /// Vector of all seed lines within the pattern, sorted based on the order of use by the slicer.
    std::vector<SeedLine> seed_lines;
    /// Indicates whether a given seed line is closed or open.
    std::vector<bool> is_seed_line_closed;
    bool is_vector_filled = false;

public:
    explicit SeedLineSorting(const FilledPattern &pattern);

    std::vector<vali> getSortedPaths() const;

};


#endif //VECTOR_SLICER_SEED_LINE_SORTING_H
