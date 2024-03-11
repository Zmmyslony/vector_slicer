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

//
// Created by Michał Zmyślony on 03/04/2023.
//

#include <algorithm>
#include <cfloat>

#include "vector_sorted_paths.h"

double distance(const Path &first, const Path &second) {
    return norm(first.last() - second.first());
}

double distance(const std::valarray<int> &point, const Path &path) {
    return norm(point - path.first());
}

std::vector<std::vector<vali>> getVectorSortedPaths(std::vector<Path> unsorted_paths, const vali& starting_point) {
    auto closest_distance = DBL_MAX;
    Path *closest_path;
    unsigned int closest_index;
    unsigned int current_index = 0;
    for (auto &path: unsorted_paths) {
        double current_distance = distance(starting_point, path);
        if (current_distance < closest_distance) {
            closest_distance = current_distance;
            closest_path = &path;
            closest_index = current_index;
        }
        current_index++;
    }

    std::vector<Path> sorted_paths({*closest_path});
    unsorted_paths.erase(unsorted_paths.begin() + closest_index);

    while (!unsorted_paths.empty()) {
        closest_distance = DBL_MAX;
        current_index = 0;
        for (auto &path: unsorted_paths) {
            double current_distance = distance(starting_point, path);
            if (current_distance < closest_distance) {
                closest_distance = current_distance;
                closest_path = &path;
                closest_index = current_index;
            }
            current_index++;
        }
        sorted_paths.push_back(*closest_path);
        unsorted_paths.erase(unsorted_paths.begin() + closest_index);
    }

    std::vector<std::vector<vali>> sorted_vector_paths;
    for (auto &path : sorted_paths) {
        sorted_vector_paths.push_back(path.sequence_of_positions);
    }

    return sorted_vector_paths;
}