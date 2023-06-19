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
// Created by Michał Zmyślony on 19/06/2023.
//

#include "line_operations.h"

#include <cfloat>

#include "valarray_operations.h"

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

std::vector<vali> sortPoints(std::vector<vali> &unsorted_perimeters, vali starting_coordinates) {
    vali current_element = findClosestNeighbour(unsorted_perimeters, starting_coordinates);
    std::vector<vali> sorted_perimeters = {current_element};

    while (!unsorted_perimeters.empty()) {
        current_element = findClosestNeighbour(unsorted_perimeters, current_element);
        sorted_perimeters.push_back(current_element);
    }
    return sorted_perimeters;
}

std::vector<std::vector<vali>> separateLines(std::vector<vali> &sorted_perimeters, double separation_distance) {
    std::vector<std::vector<vali>> separated_perimeters;
    std::vector<vali> current_subpath = {sorted_perimeters.front()};
    for (int i = 1; i < sorted_perimeters.size(); i++) {
        vali displacement_vector = sorted_perimeters[i] - sorted_perimeters[i - 1];
        if (norm(displacement_vector) > separation_distance && !current_subpath.empty()) {
            separated_perimeters.emplace_back(current_subpath);
            current_subpath.clear();
        } else {
            current_subpath.emplace_back(sorted_perimeters[i]);
        }
    }
    if (!current_subpath.empty()) {
        separated_perimeters.emplace_back(current_subpath);
    }
    return separated_perimeters;
}
