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

#ifndef VECTOR_SLICER_STARTINGPOINT_H
#define VECTOR_SLICER_STARTINGPOINT_H

#include "FilledPattern.h"
#include <valarray>


class StartingPoint {
    bool is_starting_point_found = false;
    bool is_there_fillable_points_remaining = true;
    int tries;
    int previously_found_point = 0;
    std::vector<vali> stem_points = {};

    void updateListOfPoints(FilledPattern &pattern);

    void lookForAPoint(FilledPattern &pattern);

    void trySearchingForAPoint(FilledPattern &pattern);

    void findStartPointFullyRandomly(FilledPattern &pattern);

    void findStartPointConsecutively(FilledPattern &pattern);

    void findStartPointSemiRandomly(FilledPattern &pattern);

public:
    explicit StartingPoint();

    void refresh();

    vali positions;

    vali findStartPointLegacy(FilledPattern &pattern);

    vali findRootPoint(FilledPattern &pattern);

    void findStemPoints(FilledPattern &pattern, const vali &root_point);

    vali findStartPoint(FilledPattern &pattern);
};


#endif //VECTOR_SLICER_STARTINGPOINT_H
