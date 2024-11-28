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
// Created by Michał Zmyślony on 19/06/2023.
//

#include "line_operations.h"

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "valarray_operations.h"
#include "simple_math_operations.h"

void removeElement(std::vector<veci> &array, int index) {
    array.erase(array.begin() + index);
}

veci findClosestNeighbour(std::vector<veci> &array, veci &element) {
    veci closest_element;
    auto closest_distance = DBL_MAX;

    int i_min = 0;
    for (int i = 0; i < array.size(); i++) {
        double distance = norm(subtract(array[i], element));
        if (distance < closest_distance) {
            closest_element = array[i];
            i_min = i;
            closest_distance = distance;
        }
    }
    removeElement(array, i_min);
    return closest_element;
}

/// Checks if the line forms a loop.
bool isLooped(const std::vector<veci> &line) {
    veci first = line.front();
    veci last = line.back();
    return norm(subtract(first, last)) <= 2;
}

std::vector<std::vector<veci>>
separateIntoLines(std::vector<veci> &unsorted_perimeters, veci starting_coordinates, double separation_distance) {
    veci current_element = findClosestNeighbour(unsorted_perimeters, starting_coordinates);
    std::vector<std::vector<veci>> forwards_paths;
    std::vector<std::vector<veci>> backwards_paths;

    std::vector<veci> current_path = {current_element};

    bool is_backwards_path_filled = false;
    while (!unsorted_perimeters.empty()) {
        current_element = findClosestNeighbour(unsorted_perimeters, current_element);
        double distance = norm(subtract(current_element, current_path.back()));
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
    if (is_backwards_path_filled) {
        backwards_paths.push_back(current_path);
    } else {
        forwards_paths.push_back(current_path);
        backwards_paths.emplace_back();
    }

    std::vector<std::vector<veci>> separated_paths;
    for (int i = 0; i < forwards_paths.size(); i++) {
        std::vector<veci> joined_path({forwards_paths[i].begin() + 1, forwards_paths[i].end()});
        std::reverse(joined_path.begin(), joined_path.end());
        joined_path.insert(joined_path.end(), backwards_paths[i].begin(), backwards_paths[i].end());
        if (joined_path.size() > 20) {
            separated_paths.emplace_back(joined_path);
        }
    }
    return separated_paths;
}

std::vector<std::vector<veci>> separateLines(std::vector<veci> &sorted_perimeters, double separation_distance) {
    std::vector<std::vector<veci>> separated_perimeters;
    std::vector<veci> current_subpath = {sorted_perimeters.front()};
    for (int i = 1; i < sorted_perimeters.size(); i++) {
        veci displacement_vector = subtract(sorted_perimeters[i], sorted_perimeters[i - 1]);
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

/// Creates pixel representation of a line - Bresenham's line algorithm. Works only when dx >= dy and dx > 0.
std::vector<veci> pixeliseLineBase(const vecd &line) {
    int dx = line[0];
    int dy = line[1];
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = 2 * dy - dx;
    int y = 0;

    std::vector<veci> coordinates;

    for (int x = 0; x <= dx; x++) {
        coordinates.emplace_back(veci{x, y});
        if (D > 0) {
            y += yi;
            D += 2 * dy - 2 * dx;
        } else {
            D += 2 * dy;
        }
    }

    return coordinates;
}

std::vector<veci> x_y_swap(std::vector<veci> vec) {
    for (int i = 0; i < vec.size(); i++) {
        std::swap(vec[i][0], vec[i][1]);
    }
    return vec;
}

std::vector<veci> x_invert_sign(std::vector<veci> vec) {
    for (int i = 0; i < vec.size(); i++) {
        vec[i][0] *= -1;
    }
    return vec;
}

std::vector<veci> pixeliseLine(const vecd &line) {
    double dx = line[0];
    double dy = line[1];
    if (fabs(dx) >= fabs(dy)) {
        if (dx >= 0) {
            return pixeliseLineBase(line);
        } else {
            std::vector<veci> line = pixeliseLineBase({-dx, dy});
            return x_invert_sign(line);
        }
    } else {
        if (dy >= 0) {
            std::vector<veci> line = pixeliseLineBase({dy, dx});
            return x_y_swap(line);
        } else {
            std::vector<veci> line = pixeliseLineBase({-dy, dx});
            return x_y_swap(x_invert_sign(line));
        }
    }
}