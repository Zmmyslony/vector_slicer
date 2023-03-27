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
    is_starting_point_found = pattern.isFillable(positions);
}

void StartingPoint::findStartPointSemiRandomly(FilledPattern &pattern) {
    tries++;
    unsigned int element = pattern.getNewElement();
    positions = pattern.fillable_points[element];
    is_starting_point_found = pattern.isFillable(positions);
}


void StartingPoint::findStartPointConsecutively(FilledPattern &pattern) {
    for (int i = previously_found_point; i < pattern.fillable_points.size(); i++) {
        positions = pattern.fillable_points[i];
        if (pattern.isPointInShape(positions) &&
                pattern.isFillable(positions)) {
            is_starting_point_found = true;
            previously_found_point = i;
            return;
        }
    }
    tries = MAX_RANDOM_SEARCH_TRIES;
}


void StartingPoint::lookForAPoint(FilledPattern &pattern) {
    if (pattern.search_stage == RandomPointSelection) {
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

    if (pattern.search_stage != RandomPointSelection) {
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


std::valarray<int> StartingPoint::findStartPointLegacy(FilledPattern &pattern) {
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


vali StartingPoint::findRootPoint(FilledPattern &pattern){
    while (pattern.isFillablePointLeft()) {
        vali test_point = pattern.getFillablePoint();
        if (pattern.isFillable(test_point)) {
            return test_point;
        }
        if (!pattern.isFillablePointLeft() && pattern.search_stage == PerimeterSearch) {
            pattern.updateFillablePoints();
        }
    }
    return {-1, -1};
}

void StartingPoint::findStemPoints(FilledPattern &pattern, const vali& root_point) {
    std::vector<vali> dual_line = pattern.findDualLine(root_point);
    stem_points = pattern.getSpacedLine(pattern.getStartingPointSeparation(), dual_line);
}


vali StartingPoint::findStartPoint(FilledPattern &pattern) {
    if (stem_points.empty()) {
        vali root_point = findRootPoint(pattern);
        if (root_point[0] == -1) {
            return root_point;
        }
        findStemPoints(pattern, root_point);
    }
    std::uniform_int_distribution<unsigned int> bin_distribution(0, stem_points.size() - 1);
    unsigned int random_index = bin_distribution(pattern.random_engine);
    vali random_element = stem_points[random_index];
    stem_points.erase(stem_points.begin() + random_index);

    return random_element;
}

