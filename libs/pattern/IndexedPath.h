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
