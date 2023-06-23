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

std::vector<std::vector<vali>>
separateIntoLines(std::vector<vali> &unsorted_perimeters, vali starting_coordinates, double separation_distance) {
    vali current_element = findClosestNeighbour(unsorted_perimeters, starting_coordinates);
    std::vector<std::vector<vali>> forwards_paths;
    std::vector<std::vector<vali>> backwards_paths;

    std::vector<vali> current_path = {current_element};

    bool is_backwards_path_filled = false;
    while (!unsorted_perimeters.empty()) {
        current_element = findClosestNeighbour(unsorted_perimeters, current_element);
        double distance = norm(current_element - current_path.back());
        if (distance > separation_distance) {
            if (is_backwards_path_filled) {
                is_backwards_path_filled = false;
                backwards_paths.push_back(current_path);
            } else {
                is_backwards_path_filled = true;
                forwards_paths.push_back(current_path);
                current_element = current_path.front();
            }
            current_path.clear();
        }
        current_path.push_back(current_element);
    }
    backwards_paths.push_back(current_path);
    std::vector<std::vector<vali>> separated_paths;
    for (int i = 0; i < forwards_paths.size(); i++) {
        std::vector<vali> joined_path ({forwards_paths[i].begin() + 1, forwards_paths[i].end()});
        std::reverse(joined_path.begin(), joined_path.end());
        joined_path.insert(joined_path.end(), backwards_paths[i].begin(), backwards_paths[i].end());
        if (joined_path.size() > 20) {
            separated_paths.emplace_back(joined_path);
        }
    }
    return separated_paths;
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
//    if (!current_subpath.empty()) {
    if (current_subpath.size() > 20) {
        separated_perimeters.emplace_back(current_subpath);
    }
    return separated_perimeters;
}
