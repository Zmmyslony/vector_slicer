//
// Created by Michał Zmyślony on 07/01/2022.
//

#ifndef VECTOR_SLICER_INDEXEDPATH_H
#define VECTOR_SLICER_INDEXEDPATH_H

#include "Path.h"
#include "FilledPattern.h"
#include <valarray>
#include <vector>


class IndexedPath {
private:
    bool reversedPath;
    std::valarray<int> startCoordinates;
    std::valarray<int> endCoordinates;
public:
    const std::valarray<int> &getEndCoordinates() const;

private:
    int index;
public:
    int getIndex() const;

    bool isPathReversed() const;

    const std::valarray<int> &getStartCoordinates() const;

public:
    IndexedPath(int index, bool isPathReversed, std::valarray<int> startCoordinates,
                std::valarray<int> endCoordinates);

    IndexedPath();
};

std::vector<std::vector<std::valarray<int>>> getSortedPaths(FilledPattern &filledPattern, int startingPointNumber);

#endif //VECTOR_SLICER_INDEXEDPATH_H
