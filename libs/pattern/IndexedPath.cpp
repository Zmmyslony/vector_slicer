//
// Created by Michał Zmyślony on 07/01/2022.
//

#include "IndexedPath.h"
#include "../auxiliary/ValarrayOperations.h"
#include <iostream>
#include <utility>


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

std::vector<IndexedPath> indexPaths(const std::vector<Path> &sequenceOfPaths) {
    std::vector<IndexedPath> indexedPaths;
    for (int i = 0; i < sequenceOfPaths.size(); i++) {
        Path currentPath = sequenceOfPaths[i];
        indexedPaths.emplace_back(i, false, currentPath.first(), currentPath.last());
        indexedPaths.emplace_back(i, true, currentPath.last(), currentPath.first());
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


double distanceBetweenPaths(const IndexedPath &firstPath, const IndexedPath &secondPath) {
    std::valarray<int> connectingVector = firstPath.getEndCoordinates() - secondPath.getStartCoordinates();
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


std::vector<IndexedPath>
sortIndexedPaths(std::vector<IndexedPath> indexedPaths, const std::valarray<int> &startingPositions) {
    IndexedPath currentPath(0, false, startingPositions, startingPositions);
    std::vector<IndexedPath> sortedPaths;
    while (!indexedPaths.empty()) {
        currentPath = findClosestNeighbour(indexedPaths, currentPath);
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


std::vector<IndexedPath> indexPaths(FilledPattern &filledPattern) {
    std::vector<Path> sequenceOfPaths = filledPattern.getSequenceOfPaths();
    return indexPaths(sequenceOfPaths);
}


std::vector<std::vector<std::valarray<int>>> pathToVector(const std::vector<Path> &path) {
    std::vector<std::vector<std::valarray<int>>> resultingVector;
    for (auto &element: path) {
        resultingVector.push_back(element.sequenceOfPositions);
    }
    return resultingVector;
}


std::vector<std::vector<std::valarray<int>>> getSortedPaths(FilledPattern &filledPattern, int startingPointNumber) {
    std::vector<IndexedPath> unsortedIndices = indexPaths(filledPattern);
    std::valarray<int> dimensions = filledPattern.desiredPattern.dimensions;

    std::vector<std::valarray<int>> startingPoints = generateStartingPoints(dimensions, startingPointNumber);
    std::vector<IndexedPath> sortedIndices = findBestSortingOfPathsFromStartingPoints(unsortedIndices, startingPoints);

    std::vector<Path> unsortedPaths = filledPattern.getSequenceOfPaths();
    std::vector<std::vector<std::valarray<int>>> sortedPaths = sortedSequenceOfPaths(unsortedPaths, sortedIndices);

    return sortedPaths;
}