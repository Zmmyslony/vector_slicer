// Copyright (c) 2024-2025, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 12/03/2024.
//

#define _USE_MATH_DEFINES

#include "nearest_neighbour.h"
#include <cfloat>
#include <iostream>

std::vector<Path> nearestNeighbourSort(const FilledPattern &pattern, const coord &starting_coordinates) {
    bool is_vector_sorted = pattern.desired_pattern.get().isVectorSorted();
    std::vector<Path> unsorted_paths = pattern.getSequenceOfPaths();
    std::vector<Path> sorted_paths;
    coord_d previous_end = to_coord_d(starting_coordinates);

    while (!unsorted_paths.empty()) {
        int i_nearest = 0;
        auto minimal_distance = DBL_MAX;
        for (int i = 0; i < unsorted_paths.size(); i++) {
            double current_distance = unsorted_paths[i].distance(previous_end, is_vector_sorted);
            if (current_distance < minimal_distance) {
                i_nearest = i;
                minimal_distance = current_distance;
            }
        }
        Path nearest_path = unsorted_paths[i_nearest];
        sorted_paths.emplace_back(nearest_path);
        previous_end = nearest_path.endPoint();
        unsorted_paths.erase(unsorted_paths.begin() + i_nearest);
    }
    return sorted_paths;
}