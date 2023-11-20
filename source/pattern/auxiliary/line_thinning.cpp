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

#include "geometry.h"
#include "vector_operations.h"

/// Based on https://rosettacode.org/wiki/Zhang-Suen_thinning_algorithm Step 1
bool isRemovedEastSouth(const std::set<veci> &shape, const veci &coordinate) {
    bool P1 = shape.find(coordinate) != shape.end();
    bool P2 = shape.find(add(coordinate, {1, 0})) != shape.end();
    bool P3 = shape.find(add(coordinate, {1, 1})) != shape.end();
    bool P4 = shape.find(add(coordinate, {0, 1})) != shape.end();
    bool P5 = shape.find(add(coordinate, {-1, 1})) != shape.end();
    bool P6 = shape.find(add(coordinate, {-1, 0})) != shape.end();
    bool P7 = shape.find(add(coordinate, {-1, -1})) != shape.end();
    bool P8 = shape.find(add(coordinate, {0, -1})) != shape.end();
    bool P9 = shape.find(add(coordinate, {1, -1})) != shape.end();

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
bool isRemovedNorthWest(const std::set<veci> &shape, const veci &coordinate) {
    bool P1 = shape.find(coordinate) != shape.end();
    bool P2 = shape.find(add(coordinate, {1, 0})) != shape.end();
    bool P3 = shape.find(add(coordinate, {1, 1})) != shape.end();
    bool P4 = shape.find(add(coordinate, {0, 1})) != shape.end();
    bool P5 = shape.find(add(coordinate, {-1, 1})) != shape.end();
    bool P6 = shape.find(add(coordinate, {-1, 0})) != shape.end();
    bool P7 = shape.find(add(coordinate, {-1, -1})) != shape.end();
    bool P8 = shape.find(add(coordinate, {0, -1})) != shape.end();
    bool P9 = shape.find(add(coordinate, {1, -1})) != shape.end();

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

std::set<veci> fill_in_gaps(std::set<veci> shape) {
    bool is_algorithm_in_progress = true;
    std::vector<veci> neighbour_displacements = {{1,  0},
                                                 {0,  1},
                                                 {-1, 0},
                                                 {0,  -1}};
    while (is_algorithm_in_progress) {
        std::set<veci> coordinates_to_add;
        for (auto &filled_element: shape) {
            for (auto &displacement: neighbour_displacements) {
                veci neighbour = add(filled_element, displacement);
                if (isSurroundedByFilledElements(shape, neighbour)) {
                    coordinates_to_add.insert(neighbour);
                }
            }
        }

        if (coordinates_to_add.empty()) {
            is_algorithm_in_progress = false;
        } else {
            for (auto &element: coordinates_to_add) {
                shape.insert(element);
            }
        }
    }
    return shape;
}

std::set<veci> grow_pattern(const std::set<veci> &shape, double radius) {
    std::vector<vali> circle = findPointsInCircle(radius);
    std::set<veci> grown_pattern;
    for (auto &element: shape) {
        for (auto &displacement: circle) {
            grown_pattern.insert(add(element, valtovec(displacement)));
        }
    }
    return grown_pattern;
}

std::set<veci> skeletonize(std::set<veci> shape, int grow_size, int threads) {
    shape = grow_pattern(shape, grow_size);
    std::ofstream line_density_minima_file("/home/mlz22/OneDrive/Projects/Slicer/Notebooks/grown_line_density_minima.csv");
    if (line_density_minima_file.is_open()) {
        for (auto &line: shape) {
            line_density_minima_file << line[0] << "," << line[1] << std::endl;
        }
        line_density_minima_file.close();
    }

    bool is_any_pixel_removed_in_step = true;
    while (is_any_pixel_removed_in_step) {
        is_any_pixel_removed_in_step = false;

        std::set<veci> coordinates_to_remove;
        for (auto &coordinate: shape) {
            if (isRemovedEastSouth(shape, coordinate)) {
                coordinates_to_remove.insert(coordinate);
                is_any_pixel_removed_in_step = true;
            }
        }

        for (auto &coordinate: coordinates_to_remove) {
            shape.erase(coordinate);
        }

        std::set<veci> removed_coordinates_north_west;
        for (auto &coordinate: shape) {
            if (isRemovedNorthWest(shape, coordinate)) {
                removed_coordinates_north_west.insert(coordinate);
                is_any_pixel_removed_in_step = true;
            }
        }

        for (auto &coordinate: removed_coordinates_north_west) {
            shape.erase(coordinate);
        }
    }
    return shape;
}
