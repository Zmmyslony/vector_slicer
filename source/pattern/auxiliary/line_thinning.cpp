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

#include "geometry.h"
#include "vector_operations.h"

bool isRemovedEastSouth(const std::set<veci> &shape, const veci &coordinate) {
    bool north = shape.find(add(coordinate, {1, 0})) != shape.end();
    bool north_east = shape.find(add(coordinate, {1, 1})) != shape.end();
    bool east = shape.find(add(coordinate, {0, 1})) != shape.end();
    bool south_east = shape.find(add(coordinate, {-1, 1})) != shape.end();
    bool south = shape.find(add(coordinate, {-1, 0})) != shape.end();
    bool south_west = shape.find(add(coordinate, {-1, -1})) != shape.end();
    bool west = shape.find(add(coordinate, {0, -1})) != shape.end();
    bool north_west = shape.find(add(coordinate, {1, -1})) != shape.end();

    int filled_neighbours = north + north_east + east + south_east + south + south_west + west + north_west;
    int number_of_ordered_neighbours = (!north && north_east) +
                                       (!north_east && east) +
                                       (!east && south_east) +
                                       (!south_east && south) +
                                       (!south && south_west) +
                                       (!south_west && west) +
                                       (!west && north_west) +
                                       (!north_west && north);

    bool first_condition = north && east && south;
    bool second_condition = east && south && west;

    return (2 <= filled_neighbours && filled_neighbours <= 6 &&
            number_of_ordered_neighbours == 1 &&
            first_condition &&
            second_condition);
}

bool isRemovedNorthWest(const std::set<veci> &shape, const veci &coordinate) {
    bool north = shape.find(add(coordinate, {1, 0})) != shape.end();
    bool north_east = shape.find(add(coordinate, {1, 1})) != shape.end();
    bool east = shape.find(add(coordinate, {0, 1})) != shape.end();
    bool south_east = shape.find(add(coordinate, {-1, 1})) != shape.end();
    bool south = shape.find(add(coordinate, {-1, 0})) != shape.end();
    bool south_west = shape.find(add(coordinate, {-1, -1})) != shape.end();
    bool west = shape.find(add(coordinate, {0, -1})) != shape.end();
    bool north_west = shape.find(add(coordinate, {1, -1})) != shape.end();

    int filled_neighbours = north + north_east + east + south_east + south + south_west + west + north_west;
    int number_of_ordered_neighbours = (!north && north_east) +
                                       (!north_east && east) +
                                       (!east && south_east) +
                                       (!south_east && south) +
                                       (!south && south_west) +
                                       (!south_west && west) +
                                       (!west && north_west) +
                                       (!north_west && north);

    bool first_condition = !(north && east && west);
    bool second_condition = !(north && south && west);

    return (2 <= filled_neighbours && filled_neighbours <= 6 &&
            number_of_ordered_neighbours == 1 &&
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

std::set<veci> skeletonize(std::set<veci> shape) {
//    shape = fill_in_gaps(shape);
    shape = grow_pattern(shape, 3);

    bool is_algorithm_in_progress = true;
    while (is_algorithm_in_progress) {
        is_algorithm_in_progress = false;

        std::set<veci> removed_coordinates_east_south;
        for (auto &coordinate: shape) {
            if (isRemovedEastSouth(shape, coordinate)) {
                removed_coordinates_east_south.insert(coordinate);
                is_algorithm_in_progress = true;
            }
        }

        for (auto &coordinate: removed_coordinates_east_south) {
            shape.erase(coordinate);
        }

        std::set<veci> removed_coordinates_north_west;
        for (auto &coordinate: shape) {
            if (isRemovedNorthWest(shape, coordinate)) {
                removed_coordinates_north_west.insert(coordinate);
                is_algorithm_in_progress = true;
            }
        }

        for (auto &coordinate: removed_coordinates_north_west) {
            shape.erase(coordinate);
        }
    }
    return shape;
}
