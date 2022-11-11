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
// Created by Michał Zmyślony on 07/01/2022.
//

#ifndef VECTOR_SLICER_INDEXEDPATH_H
#define VECTOR_SLICER_INDEXEDPATH_H

#include "Path.h"
#include "FilledPattern.h"
#include <valarray>
#include <vector>
#include <climits>
#include <cfloat>

class IndexedPath {
private:
    bool reversedPath;
    std::valarray<int> startCoordinates;
    std::valarray<int> endCoordinates;
    int index;
    int xMin = INT_MAX;
    int xMax = INT_MIN;
    int yMin = INT_MAX;
    int yMax = INT_MIN;
    double angle = DBL_MAX;

public:

    const std::valarray<int> &getEndCoordinates() const;

    int getIndex() const;

    bool isPathReversed() const;

    const std::valarray<int> &getStartCoordinates() const;

    int getXMin() const;

    int getXMax() const;

    int getYMin() const;

    int getYMax() const;

    double getAngle() const;

    IndexedPath(int index, bool isPathReversed, std::valarray<int> startCoordinates,
                std::valarray<int> endCoordinates);

    IndexedPath(int index, bool isPathReversed, const std::vector<std::valarray<int>> &path,
                const std::valarray<int> &dimensions);

    IndexedPath();
};

std::vector<std::vector<std::valarray<int>>> getSortedPaths(FilledPattern &filledPattern, int startingPointNumber);

#endif //VECTOR_SLICER_INDEXEDPATH_H
