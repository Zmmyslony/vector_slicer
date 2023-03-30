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
#include <iostream>

StartingPoint::StartingPoint() : stem_points({}) {}


vali StartingPoint::findRootPoint(FilledPattern &pattern) {
    while (pattern.isFillablePointLeft()) {
        vali test_point = pattern.getFillablePoint();
        if (pattern.isFillable(test_point)) {
            return test_point;
        }
        if (!pattern.isFillablePointLeft() && pattern.search_stage == PerimeterSearch) {
            pattern.updateRootPoints();
        }
    }
    return {-1, -1};
}

void StartingPoint::findStemPoints(FilledPattern &pattern, const vali &root_point) {
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

