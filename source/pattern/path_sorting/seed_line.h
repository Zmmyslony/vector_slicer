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
// Created by Michał Zmyślony on 12/03/2024.
//

#ifndef VECTOR_SLICER_SEED_LINE_H
#define VECTOR_SLICER_SEED_LINE_H

#include <vector>
#include "../path.h"
#include "../filled_pattern.h"

class SeedLine {
    std::vector<Path> paths;
    /// Only used for open seed-lines - decides which direction to traverse the seed line
    bool is_reversed = false;

    /// Is looped
    bool is_closed = false;
    /// Index of closest element to previous point
    int i_closest = 0;
    double loopDistance(const vali &point, bool is_vector_sorted);

public:

    SeedLine(const std::vector<Path> &unsorted_seed_pairs, double distance_threshold);

    /// Returns last coordinate of the last path (taking into the account ordering) within a seed line.
    vali endPoint() const;

    /// Calculates the closest distance between the line and a point.
    double distance(const vali &point, bool is_vector_sorted);

    /// Returns sorted paths, starting from the path with lowest distance from distance function.
    std::vector<Path> getOrderedPaths(bool is_vector_filled);
};


std::vector<Path> seedLineSort(const FilledPattern &pattern, vali &starting_point);

#endif //VECTOR_SLICER_SEED_LINE_H
