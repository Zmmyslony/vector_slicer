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

#include "IndexedPath.h"
#include "../auxiliary/ValarrayOperations.h"
#include <utility>
#include <cfloat>
#include <iostream>
#include <cmath>


IndexedPath::IndexedPath(int index, bool isPathReversed, std::valarray<int> startCoordinates,
                         std::valarray<int> endCoordinates) : index(
        index), reversedPath(isPathReversed), startCoordinates(std::move(startCoordinates)),
                                                              endCoordinates(std::move(endCoordinates)) {}

int IndexedPath::getIndex() const {
    return index;
}

bool IndexedPath::isPathReversed() const {
    return reversedPath;
}

const std::valarray<int> &IndexedPath::getStartCoordinates() const {
    return startCoordinates;
}

const std::valarray<int> &IndexedPath::getEndCoordinates() const {
    return endCoordinates;
}

IndexedPath::IndexedPath() : index(0), reversedPath(false), startCoordinates(std::valarray<int>({0, 0})),
                             endCoordinates(std::valarray<int>({0, 0})) {}

IndexedPath::IndexedPath(int index, bool isPathReversed, const std::vector<std::valarray<int>> &path,
                         const std::valarray<int> &dimensions) :
        index(index),
        reversedPath(isPathReversed) {
    if (isPathReversed) {
        startCoordinates = path.back();
        endCoordinates = path.front();
    } else {
        startCoordinates = path.front();
        endCoordinates = path.back();
    }
    double maxDistance = DBL_MIN;
    for (auto &point: path) {
        double xVector = point[0] - (double) dimensions[0] / 2;
        double yVector = point[1] - (double) dimensions[1] / 2;
        double distance = norm(point - dimensions / 2);
        double currentAngle = atan2(yVector, xVector);
        if (xMax < point[0]) { xMax = point[0]; }
        if (xMin > point[0]) { xMin = point[0]; }
        if (yMax < point[1]) { yMax = point[1]; }
        if (yMin > point[1]) { yMin = point[1]; }
        if (maxDistance < distance) {
            angle = currentAngle;
            maxDistance = distance;
        }
    }
}

int IndexedPath::getXMin() const {
    return xMin;
}

int IndexedPath::getXMax() const {
    return xMax;
}

int IndexedPath::getYMin() const {
    return yMin;
}

int IndexedPath::getYMax() const {
    return yMax;
}

double IndexedPath::getAngle() const {
    return angle;
}


std::vector<IndexedPath> indexPaths(const std::vector<Path> &sequenceOfPaths, const std::valarray<int> &dimensions) {
    std::vector<IndexedPath> indexedPaths;
    for (int i = 0; i < sequenceOfPaths.size(); i++) {
        Path currentPath = sequenceOfPaths[i];
//        indexedPaths.emplace_back(i, false, currentPath.first(), currentPath.last());
//        indexedPaths.emplace_back(i, true, currentPath.last(), currentPath.first());
        indexedPaths.emplace_back(i, false, currentPath.sequenceOfPositions, dimensions);
        indexedPaths.emplace_back(i, true, currentPath.sequenceOfPositions, dimensions);
    }
    return indexedPaths;
}


void removePathsWithIndex(std::vector<IndexedPath> &indexedPaths, int index) {
    std::vector<int> indicesToRemove;
    int i = 0;
    for (auto &element: indexedPaths) {
        if (element.getIndex() == index) {
            indicesToRemove.push_back(i);
        }
        i++;
    }
    while (!indicesToRemove.empty()) {
        int indexToRemove = indicesToRemove.back();
        indicesToRemove.pop_back();
        indexedPaths.erase(indexedPaths.begin() + indexToRemove);
    }
}


void removePathsWithSameIndex(std::vector<IndexedPath> &indexedPaths, const IndexedPath &path) {
    removePathsWithIndex(indexedPaths, path.getIndex());
}


double distanceBetweenPaths(const IndexedPath &currentPath, const IndexedPath &newPath) {
    std::valarray<int> connectingVector = currentPath.getEndCoordinates() - newPath.getStartCoordinates();
    return norm(connectingVector);
}


IndexedPath findClosestNeighbour(const std::vector<IndexedPath> &indexedPaths, const IndexedPath &currentPath) {
    double minimalDistance = DBL_MAX;
    IndexedPath closestPath = currentPath;
    for (auto &element: indexedPaths) {
        double currentDistance = distanceBetweenPaths(currentPath, element);
        if (currentDistance < minimalDistance) {
            minimalDistance = currentDistance;
            closestPath = element;
        }
    }
    return closestPath;
}

IndexedPath findClosestNeighbourLeft(const std::vector<IndexedPath> &indexedPaths, const IndexedPath &currentPath) {
    double minimalDistance = DBL_MAX;
    double leftMostPosition = DBL_MAX;
    IndexedPath closestPath = currentPath;

    for (auto &element: indexedPaths) {
        double currentPos = element.getXMin();
        if (currentPos < leftMostPosition) {
            leftMostPosition = currentPos;
            minimalDistance = distanceBetweenPaths(currentPath, element);
            closestPath = element;

        } else if (currentPos == leftMostPosition) {
            double currentDistance = distanceBetweenPaths(currentPath, element);
            if (currentDistance <= minimalDistance) {
                minimalDistance = currentDistance;
                closestPath = element;
            }
        }
    }
    return closestPath;
}

IndexedPath findClosestNeighbourRadial(const std::vector<IndexedPath> &indexedPaths, const IndexedPath &currentPath) {
    double minimalDistance = DBL_MAX;
    double minimalAngle = DBL_MAX;
    IndexedPath closestPath = currentPath;

    for (auto &element: indexedPaths) {
        double currentAngle = element.getAngle();
        if (currentAngle < minimalAngle) {
            minimalAngle = currentAngle;
            minimalDistance = distanceBetweenPaths(currentPath, element);
            closestPath = element;

        } else if (currentAngle == minimalAngle) {
            double currentDistance = distanceBetweenPaths(currentPath, element);
            if (currentDistance <= minimalDistance) {
                minimalDistance = currentDistance;
                closestPath = element;
            }
        }
    }
    return closestPath;
}


std::vector<IndexedPath>
sortIndexedPaths(std::vector<IndexedPath> indexedPaths, const std::valarray<int> &startingPositions) {
    IndexedPath currentPath(0, false, startingPositions, startingPositions);
    std::vector<IndexedPath> sortedPaths;
    while (!indexedPaths.empty()) {
//        currentPath = findClosestNeighbour(indexedPaths, currentPath);
//        currentPath = findClosestNeighbourLeft(indexedPaths, currentPath);
        currentPath = findClosestNeighbourRadial(indexedPaths, currentPath);
        removePathsWithSameIndex(indexedPaths, currentPath);
        sortedPaths.emplace_back(currentPath);
    }
    return sortedPaths;
}


double getMoveDistance(const std::vector<IndexedPath> &sortedPaths) {
    double distance = 0;
    for (int i = 1; i < sortedPaths.size(); i++) {
        distance += distanceBetweenPaths(sortedPaths[i - 1], sortedPaths[i]);
    }
    return distance;
}


std::vector<IndexedPath> findBestSortingOfPathsFromStartingPoints(const std::vector<IndexedPath> &unsortedIndexedPaths,
                                                                  const std::vector<std::valarray<int>> &listOfStartingPositions) {
    double minimalDistance = DBL_MAX;
    std::vector<IndexedPath> bestSorting;
    for (auto &startingPosition: listOfStartingPositions) {
        std::vector<IndexedPath> currentSorting = sortIndexedPaths(unsortedIndexedPaths, startingPosition);
        if (getMoveDistance(currentSorting) < minimalDistance) {
            minimalDistance = getMoveDistance(currentSorting);
            bestSorting = currentSorting;
        }

    }
    return bestSorting;
}


std::vector<std::valarray<int>> generateStartingPoints(const std::valarray<int> &dimensions, int numberOfSteps) {
    std::vector<std::valarray<int>> startingPoints = {{0, 0}, {dimensions[0], 0}, dimensions, {0, dimensions[1]}};

    double xStep = (double) dimensions[0] / numberOfSteps;
    double yStep = (double) dimensions[1] / numberOfSteps;
    for (int step = 0; step < numberOfSteps; step++) {
        startingPoints.push_back({(int) (xStep * step), 0});
        startingPoints.push_back({dimensions[0], (int) (yStep * step)});
        startingPoints.push_back({dimensions[0] - (int) (xStep * step), dimensions[1]});
        startingPoints.push_back({0, dimensions[1] - (int) (yStep * step)});
    }
    return startingPoints;
}


std::vector<std::vector<std::valarray<int>>>
sortedSequenceOfPaths(const std::vector<Path> &paths, const std::vector<IndexedPath> &sortedIndexedPaths) {
    std::vector<std::vector<std::valarray<int>>> sortedSequence;
    for (auto &indexedPath: sortedIndexedPaths) {
        std::vector<std::valarray<int>> newPath = (paths[indexedPath.getIndex()]).sequenceOfPositions;

        if (indexedPath.isPathReversed()) {
            std::reverse(newPath.begin(), newPath.end());
        }
        sortedSequence.emplace_back(newPath);
    }
    return sortedSequence;
}


std::vector<IndexedPath> indexPaths(FilledPattern filledPattern, const std::valarray<int> &dimensions) {
    std::vector<Path> sequenceOfPaths = filledPattern.getSequenceOfPaths();
    return indexPaths(sequenceOfPaths, dimensions);
}


std::vector<std::vector<std::valarray<int>>> pathToVector(const std::vector<Path> &path) {
    std::vector<std::vector<std::valarray<int>>> resultingVector;
    resultingVector.reserve(path.size());
    for (auto &element: path) {
        resultingVector.push_back(element.sequenceOfPositions);
    }
    return resultingVector;
}


std::vector<std::vector<std::valarray<int>>> getSortedPaths(FilledPattern &filledPattern, int startingPointNumber) {
    std::valarray<int> dimensions = filledPattern.desiredPattern.getDimensions();
    std::vector<IndexedPath> unsortedIndices = indexPaths(filledPattern, dimensions);

    std::vector<std::valarray<int>> startingPoints = generateStartingPoints(dimensions, startingPointNumber);
    std::vector<IndexedPath> sortedIndices = findBestSortingOfPathsFromStartingPoints(unsortedIndices, startingPoints);

    std::vector<Path> unsortedPaths = filledPattern.getSequenceOfPaths();
    std::vector<std::vector<std::valarray<int>>> sortedPaths = sortedSequenceOfPaths(unsortedPaths, sortedIndices);

    return sortedPaths;
}