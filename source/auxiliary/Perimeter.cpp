// 2022, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 22/09/2021.
//

#include "Perimeter.h"
#include "ValarrayOperations.h"
#include <iostream>
#include <cfloat>

std::vector<std::valarray<int>> generatePerimeterList(double radius) {
    std::vector<std::valarray<int>> perimeter_list;
    int range = (int)radius + 1;
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            if (floor(sqrt(i * i + j * j) - radius) == 0) {
                perimeter_list.push_back({i, j});
            }
        }
    }

    return perimeter_list;
}


bool isInRange(const std::valarray<int> &position, const std::valarray<int> &dimensions) {
    return (0 <= position[0] && position[0] < dimensions[0] && 0 <= position[1] && position[1] < dimensions[1]);
}


bool isEmpty(const std::valarray<int> &position, const std::vector<std::vector<int>> &table) {
    return (table[position[0]][position[1]] == 0);
}


std::valarray<double>
getRepulsion(std::vector<std::vector<int>> &filled_table, std::vector<std::valarray<int>> &checked_area,
             const std::valarray<int> &start_positions, const std::valarray<int> &sizes, double repulsion_coefficient) {

    std::valarray<double> attraction = {0, 0};
    int number_of_empty_spots = 0;
    for (auto &direction: checked_area) {
        std::valarray<int> positions_new = direction + start_positions;
        if (isInRange(positions_new, sizes)) {
            if (isEmpty(positions_new, filled_table)) {
                attraction += itodArray(direction);
                number_of_empty_spots++;
            }
        }
    }

    return -repulsion_coefficient * attraction / number_of_empty_spots;
}

bool isPerimeterFree(const std::vector<std::vector<int>> &filled_table, const std::vector<std::vector<int>> &shape_table,
                     const std::vector<std::valarray<int>> &perimeter_list, const std::valarray<int> &start_positions,
                     const std::valarray<int> &sizes) {
    if (shape_table[start_positions[0]][start_positions[1]] == 0) {
        return false;
    }
    for (auto &perimeter: perimeter_list) {
        std::valarray<int> positions_new = perimeter + start_positions;
        if (isInRange(positions_new, sizes)) {
            if (!isEmpty(positions_new, filled_table)) {
                return false;
            }
        }
    }
    return true;
}


bool
isOnEdge(const std::vector<std::vector<int>> &shape_table, const std::valarray<int> &start_positions,
         const std::valarray<int> &sizes) {
    std::vector<std::valarray<int>> list_of_nearest_neighbours = {{-1, 0},
                                                                  {-1, 1},
                                                                  {0,  1},
                                                                  {1,  1},
                                                                  {1,  0},
                                                                  {1,  -1},
                                                                  {0,  -1},
                                                                  {-1, -1}};

    if (shape_table[start_positions[0]][start_positions[1]] == 0) {
        return false;
    }

    for (auto &neighbour: list_of_nearest_neighbours) {
        std::valarray<int> positions_new = neighbour + start_positions;
        if (isInRange(positions_new, sizes)) {
            if (isEmpty(positions_new, shape_table)) {
                return true;
            }
        }
    }
    return false;
}


std::vector<std::valarray<int>>
findUnsortedPerimeters(const std::vector<std::vector<int>> &shape_matrix, const std::valarray<int> &sizes) {
    std::vector<std::valarray<int>> unsorted_perimeters;
    for (int i = 0; i < sizes[0]; i++) {
        for (int j = 0; j < sizes[1]; j++) {
            std::valarray<int> current_position = {i, j};
            if (isOnEdge(shape_matrix, current_position, sizes)) {
                unsorted_perimeters.push_back(current_position);
            }
        }
    }
    return unsorted_perimeters;
}


void removeElement(std::vector<std::valarray<int>> &array, int index) {
    array.erase(array.begin() + index);
}


std::valarray<int> findClosestNeighbour(std::vector<std::valarray<int>> &array, std::valarray<int> &element) {
    std::valarray<int> closest_element;
    double closest_distance = DBL_MAX;

    int i_min = 0;
    for (int i = 0; i < array.size(); i++) {
        double distance = norm(array[i] - element);
        if (distance < closest_distance) {
            closest_element = array[i];
            i_min = i;
            closest_distance = distance;
        }
    }
    removeElement(array, i_min);
    return closest_element;
}


std::vector<std::valarray<int>> sortPerimeters(std::vector<std::valarray<int>> &unsorted_perimeters, int starting_index) {
    std::valarray<int> current_element = unsorted_perimeters[starting_index];
    removeElement(unsorted_perimeters, starting_index);

    std::vector<std::valarray<int>> sorted_perimeters;
    sorted_perimeters.push_back(current_element);
    while (!unsorted_perimeters.empty()) {
        current_element = findClosestNeighbour(unsorted_perimeters, current_element);
        sorted_perimeters.push_back(current_element);
    }
    return sorted_perimeters;
}


std::vector<std::valarray<int>>
findSortedPerimeters(const std::vector<std::vector<int>> &shape_matrix, const std::valarray<int> &sizes) {
    std::vector<std::valarray<int>> unsorted_perimeters = findUnsortedPerimeters(shape_matrix, sizes);
    std::vector<std::valarray<int>> sorted_perimeters = sortPerimeters(unsorted_perimeters, 0);
    return sorted_perimeters;
}