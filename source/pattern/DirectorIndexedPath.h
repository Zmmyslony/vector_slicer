// Copyright (c) 2022-2023, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 07/01/2022.
//

#ifndef VECTOR_SLICER_DIRECTORINDEXEDPATH_H
#define VECTOR_SLICER_DIRECTORINDEXEDPATH_H

#include "Path.h"
#include "FilledPattern.h"
#include <valarray>
#include <vector>
#include <climits>
#include <cfloat>

using vali = std::valarray<int>;
using vald = std::valarray<double>;

/// Helper class for sorting the vector of Path based on the nearest neighbour approach
class DirectorIndexedPath {
private:
    bool reversed_path;
    vali start_coordinates;
    vali end_coordinates;
    int index;
    int x_min = INT_MAX;
    int x_max = INT_MIN;
    int y_min = INT_MAX;
    int y_max = INT_MIN;
    double angle = DBL_MAX;

public:
    DirectorIndexedPath();

    DirectorIndexedPath(int index, bool is_path_reversed, vali start_coordinates, vali end_coordinates);

    DirectorIndexedPath(int index, bool is_path_reversed, const std::vector<vali> &path, const vali &dimensions);

    const vali &getEndCoordinates() const;

    int getIndex() const;

    bool isPathReversed() const;

    const vali &getStartCoordinates() const;

    int getXMin() const;

    int getXMax() const;

    int getYMin() const;

    int getYMax() const;

    double getAngle() const;

};

std::vector<std::vector<vali>> getDirectorSortedPaths(FilledPattern &filled_pattern, int starting_point_number);

#endif //VECTOR_SLICER_DIRECTORINDEXEDPATH_H
