// Copyright (c) 2021-2025, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite following publication if you use any part of this code in work you publish or distribute:
// [1] Michał Zmyślony M., Klaudia Dradrach, John S. Biggins,
//    Slicing vector fields into tool paths for additive manufacturing of nematic elastomers,
//    Additive Manufacturing, Volume 97, 2025, 104604, ISSN 2214-8604, https://doi.org/10.1016/j.addma.2024.104604.
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
// Created by Michał Zmyślony on 22/09/2021.
//

#include "perimeter.h"
#include <iostream>
#include <cmath>

#include "line_operations.h"
#include "geometry.h"
#include "valarray_operations.h"
#include "simple_math_operations.h"


std::vector<coord> circleDisplacements(double radius) {
    std::vector<coord> perimeter_list;
    int range = (int) radius + 1;
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            if (ceil(sqrt(i * i + j * j) - radius) == 0) {
                perimeter_list.emplace_back(i, j);
            }
        }
    }

    return perimeter_list;
}


bool isInRange(const coord &position, const veci &dimensions) {
    return (0 <= position.x && position.x < dimensions[0] &&
            0 <= position.y && position.y < dimensions[1]);
}


bool isEmpty(const coord &position, const std::vector<std::vector<uint8_t>> &table) {
    return (table[position.x][position.y] == 0);
}


bool
isPerimeterFree(const std::vector<std::vector<uint8_t>> &filled_table, const std::vector<std::vector<uint8_t>> &shape_table,
                const std::vector<coord> &perimeter_displacements_list, const coord &coordinates, const veci &sizes) {
    if (!isInRange(coordinates, sizes) ||
        isEmpty(coordinates, shape_table) ||
        !isEmpty(coordinates, filled_table)) {
        return false;
    }
    for (auto &perimeter_displacement: perimeter_displacements_list) {
        coord perimeter = coordinates + perimeter_displacement;
        if (isInRange(perimeter, sizes) && !isEmpty(perimeter, filled_table)) {
            return false;
        }
    }
    return true;
}


bool isOnEdge(const std::vector<std::vector<uint8_t>> &shape_table, const coord &coordinates, const veci &sizes) {
    std::vector<coord> neighbour_displacements_list = {{-1, 0},
                                                      {-1, 1},
                                                      {0,  1},
                                                      {1,  1},
                                                      {1,  0},
                                                      {1,  -1},
                                                      {0,  -1},
                                                      {-1, -1}};

    if (isEmpty(coordinates, shape_table)) {
        return false;
    }


    for (auto &neighbour_displacement: neighbour_displacements_list) {
        coord neighbour_positions = coordinates + neighbour_displacement;
        if (!isInRange(neighbour_positions, sizes) ||
            isEmpty(neighbour_positions, shape_table)) {
            return true;
        }
    }
    return false;
}

coord_d
getOutwardPointingVector(const coord &current_position, const std::vector<std::vector<uint8_t>> &shape_matrix,
                         const veci &sizes,
                         const std::vector<coord> &tested_circle) {
    int number_of_empty_points = 0;
    coord_d sum_of_empty_point_displacements = {0, 0};
    for (auto &displacement: tested_circle) {
        coord coordinates = current_position + displacement;
        if (!isInRange(coordinates, sizes) ||
            isEmpty(coordinates, shape_matrix)) {
            number_of_empty_points++;
            sum_of_empty_point_displacements += to_coord_d(displacement);
        }
    }
    if (number_of_empty_points > 0) {
        return sum_of_empty_point_displacements / (double) number_of_empty_points;
    } else {
        return {0, 0};
    }

}


bool isValidPerimeterPoint(const coord &positions, const std::vector<std::vector<uint8_t>> &shape_matrix, const veci &sizes,
                           const std::vector<coord> &tested_circle, const std::vector<std::vector<coord_d>> &splay_array) {
    if (!isOnEdge(shape_matrix, positions, sizes)) {
        return false;
    }

    coord_d outward_pointing_vector = normalized(getOutwardPointingVector(positions, shape_matrix, sizes, tested_circle));
    coord_d current_splay = splay_array[positions.x][positions.y];

    // Threshold is set slightly below zero to improve stability for numerically calculated splay
    double zero_splay_threshold = -1e-10;
    return dot(outward_pointing_vector, current_splay) > zero_splay_threshold;
}

coord_set findValidPerimeterPoints(const std::vector<std::vector<uint8_t>> &shape_matrix, const veci &sizes,
                                   const std::vector<std::vector<coord_d>> &splay_array) {
    coord_set unsorted_perimeters;
    std::vector<coord> tested_circle = circleDisplacements(4);
    for (int i = 0; i < sizes[0]; i++) {
        for (int j = 0; j < sizes[1]; j++) {
            coord current_position = {i, j};
            if (isValidPerimeterPoint({i, j}, shape_matrix, sizes, tested_circle, splay_array)) {
                unsorted_perimeters.insert(current_position);
            }
        }
    }
    return unsorted_perimeters;
}

coord_set findGeometricalPerimeter(const std::vector<std::vector<uint8_t>> &shape_matrix, const veci &sizes) {
    coord_set unsorted_perimeters;
    // It is set to constant radius, maybe add a control over it?
    std::vector<coord> tested_circle = circleDisplacements(4);
    for (int i = 0; i < sizes[0]; i++) {
        for (int j = 0; j < sizes[1]; j++) {
            coord current_position = {i, j};
            if (isOnEdge(shape_matrix, {i, j}, sizes)) {
                unsorted_perimeters.insert(current_position);
            }
        }
    }
    return unsorted_perimeters;
}


std::vector<std::vector<coord>>
findSeparatedPerimeters(const std::vector<std::vector<uint8_t>> &shape_matrix, const veci &sizes,
                        const std::vector<std::vector<coord_d>> &splay_array) {
    coord_set unsorted_perimeters = findValidPerimeterPoints(shape_matrix, sizes, splay_array);
    std::vector<std::vector<coord>> separated_perimeters = separateIntoLines(unsorted_perimeters, {0, 0}, 2);
    // If using the splay approach for selecting splay-valid perimeter points yields single points that are unconnected
    // then separation into perimeters will not detect any lines. Therefore, we revert to the simple geometrical
    // definition of perimeter, where point within the pattern that neighbours one that is outside is counted as perimeter.
    if (separated_perimeters.empty()) {
        unsorted_perimeters = findGeometricalPerimeter(shape_matrix, sizes);
        separated_perimeters = separateIntoLines(unsorted_perimeters, {0, 0}, 2);
    }
    return separated_perimeters;
}