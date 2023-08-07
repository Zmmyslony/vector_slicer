// Copyright (c) 2021-2023, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 22/09/2021.
//

#include "perimeter.h"
#include <iostream>

#include "line_operations.h"
#include "geometry.h"
#include "valarray_operations.h"


std::vector<vali> generatePerimeterList(double radius) {
    std::vector<vali> perimeter_list;
    int range = (int) radius + 1;
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            if (floor(sqrt(i * i + j * j) - radius) == 0) {
                perimeter_list.push_back({i, j});
            }
        }
    }

    return perimeter_list;
}


bool isInRange(const vali &position, const vali &dimensions) {
    return (0 <= position[0] && position[0] < dimensions[0] &&
            0 <= position[1] && position[1] < dimensions[1]);
}


bool isEmpty(const vali &position, const std::vector<std::vector<int>> &table) {
    return (table[position[0]][position[1]] == 0);
}


bool
isPerimeterFree(const std::vector<std::vector<int>> &filled_table, const std::vector<std::vector<int>> &shape_table,
                const std::vector<vali> &perimeter_displacements_list, const vali &coordinates, const vali &sizes) {
    if (!isInRange(coordinates, sizes) ||
        isEmpty(coordinates, shape_table) ||
        !isEmpty(coordinates, filled_table)) {
        return false;
    }
    for (auto &perimeter_displacement: perimeter_displacements_list) {
        vali perimeter = perimeter_displacement + coordinates;
        if (isInRange(perimeter, sizes) && !isEmpty(perimeter, filled_table)) {
            return false;
        }
    }
    return true;
}


bool isOnEdge(const std::vector<std::vector<int>> &shape_table, const vali &coordinates, const vali &sizes) {
    std::vector<vali> neighbour_displacements_list = {{-1, 0},
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
        vali neighbour_positions = neighbour_displacement + coordinates;
        if (!isInRange(neighbour_positions, sizes) ||
            isEmpty(neighbour_positions, shape_table)) {
            return true;
        }
    }
    return false;
}

vald
getOutwardPointingVector(const vali &positions, const std::vector<std::vector<int>> &shape_matrix, const vali &sizes,
                         const std::vector<vali> &tested_circle) {
    int number_of_empty_points = 0;
    vald sum_of_empty_point_displacements = {0, 0};
    for (auto &displacement: tested_circle) {
        vali offset_positions = displacement + positions;
        if (!isInRange(offset_positions, sizes) ||
            isEmpty(offset_positions, shape_matrix)) {
            number_of_empty_points++;
            sum_of_empty_point_displacements += itod(displacement);
        }
    }
    if (number_of_empty_points > 0) {
        return sum_of_empty_point_displacements / (double) number_of_empty_points;
    } else {
        return sum_of_empty_point_displacements;
    }

}


bool isValidPerimeterPoint(const vali &positions, const std::vector<std::vector<int>> &shape_matrix, const vali &sizes,
                           const std::vector<vali> &tested_circle, const std::vector<std::vector<vald>> &splay_array) {
    if (!isOnEdge(shape_matrix, positions, sizes)) {
        return false;
    }

    vald outward_pointing_vector = getOutwardPointingVector(positions, shape_matrix, sizes, tested_circle);
    vald current_splay = splay_array[positions[0]][positions[1]];
    if (dot(outward_pointing_vector, current_splay) < 0) {
        return false;
    } else {
        return true;
    }
}

std::vector<vali> findValidPerimeterPoints(const std::vector<std::vector<int>> &shape_matrix, const vali &sizes,
                                           const std::vector<std::vector<vald>> &splay_array) {
    std::vector<vali> unsorted_perimeters;
    // It is set to constant radius, maybe add a control over it?
    std::vector<vali> tested_circle = generatePerimeterList(4);
    for (int i = 0; i < sizes[0]; i++) {
        for (int j = 0; j < sizes[1]; j++) {
            vali current_position = {i, j};
            if (isValidPerimeterPoint({i, j}, shape_matrix, sizes, tested_circle, splay_array)) {
                unsorted_perimeters.push_back(current_position);
            }
        }
    }
    return unsorted_perimeters;
}

std::vector<vali> findGeometricalPerimeter(const std::vector<std::vector<int>> &shape_matrix, const vali &sizes) {
    std::vector<vali> unsorted_perimeters;
    // It is set to constant radius, maybe add a control over it?
    std::vector<vali> tested_circle = generatePerimeterList(4);
    for (int i = 0; i < sizes[0]; i++) {
        for (int j = 0; j < sizes[1]; j++) {
            vali current_position = {i, j};
            if (isOnEdge(shape_matrix, {i, j}, sizes)) {
                unsorted_perimeters.push_back(current_position);
            }
        }
    }
    return unsorted_perimeters;
}


std::vector<std::vector<vali>>
findSeparatedPerimeters(const std::vector<std::vector<int>> &shape_matrix, const vali &sizes,
                        const std::vector<std::vector<vald>> &splay_array) {
    std::vector<vali> unsorted_perimeters = findValidPerimeterPoints(shape_matrix, sizes, splay_array);
    std::vector<std::vector<vali>> separated_perimeters = separateIntoLines(unsorted_perimeters, {0, 0}, sqrt(2));
    // If using the splay approach for selecting splay-valid perimeter points yields single points that are unconnected
    // then separation into perimeters will not detect any lines. Therefore, we revert to the simple geometrical
    // definition of perimeter, where point within the pattern that neighbours one that is outside is counted as perimeter.
    if (separated_perimeters.empty()) {
        unsorted_perimeters = findGeometricalPerimeter(shape_matrix, sizes);
        separated_perimeters = separateIntoLines(unsorted_perimeters, {0, 0}, sqrt(2));
    }
    return separated_perimeters;
}