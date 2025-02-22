// Copyright (c) 2021-2025, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 27/09/2021.
//

#include "filling_patterns.h"
#include "simulation/configuration_reading.h"
#include "vector_slicer_config.h"

bool tryGeneratingNewPath(FilledPattern &pattern) {
    SeedPoint seed_point = pattern.findSeedPoint();
    if (seed_point.isInvalid()) {
        return false;
    } else {
        Path new_path = pattern.generateNewPath(seed_point);

        if (new_path.size() == 0) {}
        else if (new_path.size() == 1) {
            pattern.fillPointsInCircle(seed_point.getCoordinates());
        } else {
            pattern.fillPointsInHalfCircle(new_path, 1, true);
            pattern.fillPointsInHalfCircle(new_path, 1, false);
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
    if (pattern.desired_pattern.get().isPointsRemoved()) {
        pattern.removePoints();
    }
    double short_line_coefficient = pattern.desired_pattern.get().getMinimalLineLength();
    if (short_line_coefficient > 0) {
        pattern.removeShortLines(short_line_coefficient);
    }
}