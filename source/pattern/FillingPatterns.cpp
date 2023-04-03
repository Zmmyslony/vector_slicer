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
// Created by Michał Zmyślony on 27/09/2021.
//

#include "FillingPatterns.h"
#include "../auxiliary/configuration_reading.h"
#include "vector_slicer_config.h"

bool tryGeneratingNewPath(FilledPattern &pattern) {
    std::valarray<int> starting_coordinates = pattern.findStartPoint();
    if (starting_coordinates[0] == -1 || starting_coordinates[1] == -1) {
        return false;
    } else {
        Path forwards_path = pattern.generateNewPathForDirection(starting_coordinates,
                                                                 pattern.desired_pattern.get().preferredDirection(
                                                                         starting_coordinates,
                                                                         pattern.getStepLength()));
        Path backwards_path = pattern.generateNewPathForDirection(starting_coordinates,
                                                                  -pattern.desired_pattern.get().preferredDirection(
                                                                          starting_coordinates,
                                                                          pattern.getStepLength()));

        Path new_path(forwards_path, backwards_path);

        if (new_path.size() == 1) {
            pattern.fillPointsInCircle(starting_coordinates);
        } else {
            pattern.fillPointsInHalfCircle(new_path.first(), new_path.second(), 1);
            pattern.fillPointsInHalfCircle(new_path.last(), new_path.secondToLast(), 1);
            pattern.addNewPath(new_path);
        }

        return true;
    }
}

void fillWithPaths(FilledPattern &pattern) {
    bool is_there_any_spot_fillable = true;
    while (is_there_any_spot_fillable) {
        is_there_any_spot_fillable = tryGeneratingNewPath(pattern);
    }
    if (readKeyBool(FILLING_CONFIG, "is_points_removed")) {
        pattern.removePoints();
    }
    double short_line_coefficient = readKeyDouble(FILLING_CONFIG, "minimal_line_length");
    if (short_line_coefficient > 0) {
        pattern.removeShortLines(short_line_coefficient);
    }
}