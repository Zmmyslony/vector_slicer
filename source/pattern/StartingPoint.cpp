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
// Created by Michał Zmyślony on 23/09/2021.
//

#include "StartingPoint.h"
#include "../auxiliary/Perimeter.h"
#include "../auxiliary/ValarrayOperations.h"

int const MAX_RANDOM_SEARCH_TRIES = 1000;

StartingPoint::StartingPoint() :
        tries(0) {
}

void StartingPoint::findStartPointFullyRandomly(FilledPattern &pattern) {
    tries++;
    positions = pattern.findPointInShape();

    if (isPerimeterFree(pattern.number_of_times_filled, pattern.desired_pattern.get().getShapeMatrix(),
                        pattern.collision_list, positions, pattern.desired_pattern.get().getDimensions())) {
        is_starting_point_found = true;
    }
}

bool isPerimeterFree(const FilledPattern &pattern, const std::valarray<int> &positions) {
    return isPerimeterFree(pattern.number_of_times_filled, pattern.desired_pattern.get().getShapeMatrix(),
                           pattern.collision_list, positions, pattern.desired_pattern.get().getDimensions());
}

void StartingPoint::findStartPointSemiRandomly(FilledPattern &pattern) {
    tries++;
    unsigned int element = pattern.getNewElement();
    positions = pattern.points_to_fill[element];

    if (isPerimeterFree(pattern, positions)) {
        is_starting_point_found = true;
    }
}


void StartingPoint::findStartPointConsecutively(FilledPattern &pattern) {
    for (int i = previously_found_point; i < pattern.points_to_fill.size(); i++) {
        positions = pattern.points_to_fill[i];
        if (pattern.desired_pattern.get().getShapeMatrix()[positions[0]][positions[1]] &&
            isPerimeterFree(pattern.number_of_times_filled, pattern.desired_pattern.get().getShapeMatrix(),
                            pattern.collision_list, positions, pattern.desired_pattern.get().getDimensions())) {
            is_starting_point_found = true;
            previously_found_point = i;
            return;
        }
    }
    tries = MAX_RANDOM_SEARCH_TRIES;
}


void StartingPoint::lookForAPoint(FilledPattern &pattern) {
    if (pattern.search_stage == FullyRandomPointSelection) {
        findStartPointFullyRandomly(pattern);
    } else if (pattern.is_filling_method_random) {
        findStartPointSemiRandomly(pattern);
    } else {
        findStartPointConsecutively(pattern);
    }
}


void StartingPoint::updateListOfPoints(FilledPattern &pattern) {
    tries = 0;
    previously_found_point = 0;
    is_there_fillable_points_remaining = true;

    unsigned int previous_number_of_fillable_points = pattern.points_to_fill.size();
    pattern.updateSearchStageAndFillablePoints();

    if (pattern.search_stage != FullyRandomPointSelection) {
        is_there_fillable_points_remaining = !pattern.points_to_fill.empty();
        if (previous_number_of_fillable_points == pattern.points_to_fill.size()) {
            is_there_fillable_points_remaining = false;
        }
    }
}


void StartingPoint::trySearchingForAPoint(FilledPattern &pattern) {
    if (tries < MAX_RANDOM_SEARCH_TRIES) {
        lookForAPoint(pattern);
    } else if (tries == MAX_RANDOM_SEARCH_TRIES) {
        updateListOfPoints(pattern);
    }
}


std::valarray<int> StartingPoint::findStartPoint(FilledPattern &pattern) {
    while (!is_starting_point_found) {
        if (is_there_fillable_points_remaining) {
            trySearchingForAPoint(pattern);
        } else {
            positions = {-1, -1};
            return positions;
        }
    }
    return positions;
}


void StartingPoint::refresh() {
    tries = 0;
    is_starting_point_found = false;
    is_there_fillable_points_remaining = true;
}
