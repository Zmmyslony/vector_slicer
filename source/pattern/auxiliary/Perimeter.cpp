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
// You should have received a copy of the GNU General Public License along with Vector Slicer. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 22/09/2021.
//

#include "Perimeter.h"
#include "ValarrayOperations.h"
#include "Geometry.h"

#include <cfloat>
#include <iostream>


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
    return (0 <= position[0] && position[0] < dimensions[0] && 0 <= position[1] && position[1] < dimensions[1]);
}


bool isEmpty(const vali &position, const std::vector<std::vector<int>> &table) {
    return (table[position[0]][position[1]] == 0);
}


vald
getRepulsionFromDisplacement(const vald &coordinates, const std::vector<vali> &current_displacements, const vali &sizes,
                             const std::vector<std::vector<int>> &shape_matrix,
                             const std::vector<std::vector<int>> &filled_table) {
    int number_of_repulsing_coordinates = 0;
    vald attraction = {0, 0};

    for (auto &displacement: current_displacements) {
        vali neighbour = dtoi(coordinates) + displacement;
        if (isInRange(neighbour, sizes) &&
            !isEmpty(neighbour, shape_matrix) &&
            isEmpty(neighbour, filled_table)) {

            attraction += itod(displacement);
            number_of_repulsing_coordinates++;
        }
    }
    if (number_of_repulsing_coordinates == 0) {
        return {0, 0};
    }
    else{
        vald repulsion_vector = attraction / number_of_repulsing_coordinates;
        return repulsion_vector;
    }
}


vald
getLineBasedRepulsion(const std::vector<std::vector<int>> &shape_matrix,
                      const std::vector<std::vector<int>> &filled_table, const vald &tangent, double radius,
                      const vald &coordinates, const vali &sizes, double repulsion_coefficient,
                      double maximal_repulsion_angle) {


    std::vector<vali> current_displacements = generateLineDisplacements(tangent, radius);
    vald maximal_repulsion_vector = repulsion_coefficient *
                                    getRepulsionFromDisplacement(coordinates, current_displacements, sizes,
                                                                 shape_matrix, filled_table);

    double maximal_repulsion_length = norm(maximal_repulsion_vector);
    if (maximal_repulsion_length < 1) {
        return maximal_repulsion_vector;
    }
    vali maximal_repulsion_vector_i = dtoi(maximal_repulsion_vector);
    int maximal_repulsion_length_i = std::max(std::abs(maximal_repulsion_vector_i[0]),
                                              std::abs(maximal_repulsion_vector_i[1]));

    vald previous_displacement = {0, 0};
    for (int i = 1; i <= maximal_repulsion_length_i; i++) {
        vald local_displacement = maximal_repulsion_vector * (double) i / (double) maximal_repulsion_length_i;
        vald local_repulsion = repulsion_coefficient *
                               getRepulsionFromDisplacement(coordinates + local_displacement, current_displacements,
                                                            sizes, shape_matrix, filled_table);

        if (dot(local_repulsion, maximal_repulsion_vector) <= 0 ||
            dot(tangent, tangent + local_repulsion) < cos(maximal_repulsion_angle)) {
            return previous_displacement;
        }
        previous_displacement = local_displacement;
    }

    return maximal_repulsion_vector;
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
        if (isInRange(neighbour_positions, sizes) &&
            isEmpty(neighbour_positions, shape_table)) {
            return true;
        }
    }
    return false;
}


std::vector<vali> findUnsortedPerimeters(const std::vector<std::vector<int>> &shape_matrix, const vali &sizes) {
    std::vector<vali> unsorted_perimeters;
    for (int i = 0; i < sizes[0]; i++) {
        for (int j = 0; j < sizes[1]; j++) {
            vali current_position = {i, j};
            if (isOnEdge(shape_matrix, current_position, sizes)) {
                unsorted_perimeters.push_back(current_position);
            }
        }
    }
    return unsorted_perimeters;
}


void removeElement(std::vector<vali> &array, int index) {
    array.erase(array.begin() + index);
}


vali findClosestNeighbour(std::vector<vali> &array, vali &element) {
    vali closest_element;
    auto closest_distance = DBL_MAX;

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


std::vector<vali> sortPerimeters(std::vector<vali> &unsorted_perimeters, int starting_index) {
    vali current_element = unsorted_perimeters[starting_index];
    removeElement(unsorted_perimeters, starting_index);

    std::vector<vali> sorted_perimeters;
    sorted_perimeters.push_back(current_element);
    while (!unsorted_perimeters.empty()) {
        current_element = findClosestNeighbour(unsorted_perimeters, current_element);
        sorted_perimeters.push_back(current_element);
    }
    return sorted_perimeters;
}


std::vector<vali> findSortedPerimeters(const std::vector<std::vector<int>> &shape_matrix, const vali &sizes) {
    std::vector<vali> unsorted_perimeters = findUnsortedPerimeters(shape_matrix, sizes);
    std::vector<vali> sorted_perimeters = sortPerimeters(unsorted_perimeters, 0);
    return sorted_perimeters;
}