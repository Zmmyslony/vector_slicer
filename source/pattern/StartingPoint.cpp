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
    is_starting_point_found = pattern.isPointPerimeterFree(positions);
}

void StartingPoint::findStartPointSemiRandomly(FilledPattern &pattern) {
    tries++;
    unsigned int element = pattern.getNewElement();
    positions = pattern.fillable_points[element];
    is_starting_point_found = pattern.isPointPerimeterFree(positions);
}


void StartingPoint::findStartPointConsecutively(FilledPattern &pattern) {
    for (int i = previously_found_point; i < pattern.fillable_points.size(); i++) {
        positions = pattern.fillable_points[i];
        if (pattern.isPointInShape(positions) &&
            pattern.isPointPerimeterFree(positions)) {
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

    unsigned int previous_number_of_fillable_points = pattern.fillable_points.size();
    pattern.updateSearchStageAndFillablePoints();

    if (pattern.search_stage != FullyRandomPointSelection) {
        is_there_fillable_points_remaining = !pattern.fillable_points.empty();
        if (previous_number_of_fillable_points == pattern.fillable_points.size()) {
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
