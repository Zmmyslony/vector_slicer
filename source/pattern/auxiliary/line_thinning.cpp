// Copyright (c) 2023, Michał Zmyślony, mlz22@cam.ac.uk.
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

// 2023, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Vector Slicer. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 23/06/2023.
//

#include "line_thinning.h"

#include <iostream>
#include <omp.h>
#include <fstream>
#include <unordered_set>

#include "geometry.h"
#include "vector_operations.h"


/// Based on https://rosettacode.org/wiki/Zhang-Suen_thinning_algorithm Step 1
bool isRemovedEastSouth(const coord_set &shape, const coord &coordinate) {
    /// true -> black, false -> white
    bool P1 = shape.find(coordinate) != shape.end();
    bool P2 = shape.find(coordinate + coord{1, 0}) != shape.end();
    bool P3 = shape.find(coordinate + coord{1, 1}) != shape.end();
    bool P4 = shape.find(coordinate + coord{0, 1}) != shape.end();
    bool P5 = shape.find(coordinate + coord{-1, 1}) != shape.end();
    bool P6 = shape.find(coordinate + coord{-1, 0}) != shape.end();
    bool P7 = shape.find(coordinate + coord{-1, -1}) != shape.end();
    bool P8 = shape.find(coordinate + coord{0, -1}) != shape.end();
    bool P9 = shape.find(coordinate + coord{1, -1}) != shape.end();

    int filled_neighbours = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;
    int number_of_colour_transitions = (!P2 && P3) +
                                       (!P3 && P4) +
                                       (!P4 && P5) +
                                       (!P5 && P6) +
                                       (!P6 && P7) +
                                       (!P7 && P8) +
                                       (!P8 && P9) +
                                       (!P9 && P2);

    bool first_condition = !P2 || !P4 || !P6;
    bool second_condition = !P4 || !P6 || !P8;

    return (P1 &&
            2 <= filled_neighbours && filled_neighbours <= 6 &&
            number_of_colour_transitions == 1 &&
            first_condition &&
            second_condition);
}

/// Based on https://rosettacode.org/wiki/Zhang-Suen_thinning_algorithm Step 1
bool isRemovedNorthWest(const coord_set &shape, const coord &coordinate) {
    bool P1 = shape.find(coordinate) != shape.end();
    bool P2 = shape.find(coordinate + coord{1, 0}) != shape.end();
    bool P3 = shape.find(coordinate + coord{1, 1}) != shape.end();
    bool P4 = shape.find(coordinate + coord{0, 1}) != shape.end();
    bool P5 = shape.find(coordinate + coord{-1, 1}) != shape.end();
    bool P6 = shape.find(coordinate + coord{-1, 0}) != shape.end();
    bool P7 = shape.find(coordinate + coord{-1, -1}) != shape.end();
    bool P8 = shape.find(coordinate + coord{0, -1}) != shape.end();
    bool P9 = shape.find(coordinate + coord{1, -1}) != shape.end();

    int filled_neighbours = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;
    int number_of_colour_transitions = (!P2 && P3) +
                                       (!P3 && P4) +
                                       (!P4 && P5) +
                                       (!P5 && P6) +
                                       (!P6 && P7) +
                                       (!P7 && P8) +
                                       (!P8 && P9) +
                                       (!P9 && P2);

    bool first_condition = !P2 || !P4 || !P8;
    bool second_condition = !P2 || !P6 || !P8;

    return (P1 &&
            2 <= filled_neighbours && filled_neighbours <= 6 &&
            number_of_colour_transitions == 1 &&
            first_condition &&
            second_condition);
}


bool isSurroundedByFilledElements(const std::set<veci> &shape, const veci &coordinate) {
    bool is_this_filled = shape.find(coordinate) != shape.end();

    bool north = shape.find(add(coordinate, {1, 0})) != shape.end();
    bool east = shape.find(add(coordinate, {0, 1})) != shape.end();
    bool south = shape.find(add(coordinate, {-1, 0})) != shape.end();
    bool west = shape.find(add(coordinate, {0, -1})) != shape.end();

    int neighbour_sum = north + east + south + west;
    return neighbour_sum >= 3 && !is_this_filled;
}


coord_set grow_pattern(const coord_set &shape, double radius, const std::vector<std::vector<uint8_t>> &shape_matrix) {
    std::vector<coord> circle = findPointsInDisk(radius);
    coord_set grown_pattern;
    for (auto &element: shape) {
        for (auto &displacement: circle) {
            coord current = element + displacement;
            if (shape_matrix[current.x][current.y]) {
                grown_pattern.insert(current);
            }
        }
    }
    return grown_pattern;
}


coord_set skeletonize(coord_set shape, int grow_size, const std::vector<std::vector<uint8_t>> &shape_matrix) {
    shape = grow_pattern(shape, grow_size, shape_matrix);
    /// Instead of doing the skeletonisation until convergence, we only aim to reduce what was added by the growth.
    for (int i = 0; i <= grow_size * 2; i++) {
        coord_vector coordinates_to_remove_stage_one;
        for (auto &coordinate: shape) {
            if (isRemovedEastSouth(shape, coordinate)) {
                coordinates_to_remove_stage_one.emplace_back(coordinate);
            }
        }

        for (auto &coordinate: coordinates_to_remove_stage_one) { shape.erase(coordinate); }

        coord_vector coordinates_to_remove_stage_two;
        for (auto &coordinate: shape) {
            if (isRemovedNorthWest(shape, coordinate)) {
                coordinates_to_remove_stage_two.emplace_back(coordinate);
            }
        }

        for (auto &coordinate: coordinates_to_remove_stage_two) { shape.erase(coordinate); }
    }
    return shape;
}
