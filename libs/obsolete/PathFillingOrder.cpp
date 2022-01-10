////
//// Created by Michał Zmyślony on 21/10/2021.
////
//
//#include "PathFillingOrder.h"
//#include "../auxiliary/ValarrayOperations.h"
//#include <iostream>
//#include <fstream>
//#include "../auxiliary//Exporting.h"
//
//
//std::vector<std::valarray<int>> getStartAndEndPositions(std::vector<Path> &sequenceOfPaths) {
//    std::vector<std::valarray<int>> startEndPositions;
//    int iterator = 0;
//    for (auto &path: sequenceOfPaths) {
//        std::valarray<int> newElementFront = {path.first()[0], path.first()[1]};
//        std::valarray<int> newElementBack = {path.last()[0], path.last()[1]};
//        startEndPositions.push_back(newElementFront);
//        startEndPositions.push_back(newElementBack);
//        iterator++;
//    }
//    return startEndPositions;
//}
//
//
//int findClosestElementInList(std::valarray<int> &previousPosition, std::vector<std::valarray<int>> &startEndPositions) {
//    double distance = DBL_MAX;
//    int index = 0;
//    for (int i = 0; i < startEndPositions.size(); i++) {
//        std::valarray<int> currentPosition = startEndPositions[i];
//        double currentDistance = norm(itodArray(currentPosition - previousPosition));
//        if (currentDistance < distance && currentDistance > 0) {
//            distance = currentDistance;
//            index = i;
//        }
//    }
//    return index;
//}
//
//
//PathFillingOrder::PathFillingOrder(FilledPattern &pattern) :
//        sequenceOfPaths(pattern.getSequenceOfPaths()),
//        dimensions(pattern.desiredPattern.dimensions) {
//    startAndEndPositions = getStartAndEndPositions(sequenceOfPaths);
//}
//
//
//void PathFillingOrder::startGeneratingPrintPathsFrom(std::valarray<int> startingPoint) {
//    std::valarray<int> currentPoint = std::move(startingPoint);
//    std::vector<std::valarray<int>> localStartAndEndPositions = startAndEndPositions;
//    orderingOfPaths.clear();
//    isPathInCorrectDirection.clear();
//
//    while (!localStartAndEndPositions.empty()) {
//        int closestIndex = findClosestElementInList(currentPoint, localStartAndEndPositions);
//        int closestPathIndex = closestIndex / 2;
//        bool isItEndingPoint = closestIndex % 2;
//        orderingOfPaths.push_back(closestPathIndex);
//        isPathInCorrectDirection.push_back(!isItEndingPoint);
//
//        if (isItEndingPoint) {
//            currentPoint = localStartAndEndPositions[closestIndex - 1];
//            localStartAndEndPositions.erase(localStartAndEndPositions.begin() + closestIndex - 1);
//            localStartAndEndPositions.erase(localStartAndEndPositions.begin() + closestIndex - 1);
//        } else {
//            currentPoint = localStartAndEndPositions[closestIndex + 1][0];
//            localStartAndEndPositions.erase(localStartAndEndPositions.begin() + closestIndex);
//            localStartAndEndPositions.erase(localStartAndEndPositions.begin() + closestIndex);
//        }
//    }
//}
//
//
//double PathFillingOrder::getMovedDistance() {
//    double movedDistance = 0;
//    std::valarray<int> previousPosition;
//    int currentIndex = orderingOfPaths[0];
//
//    if (isPathInCorrectDirection[0]) {
//        previousPosition = sequenceOfPaths[currentIndex].last();
//    } else {
//        previousPosition = sequenceOfPaths[currentIndex].first();
//    }
//
//    for (int i = 1; i < orderingOfPaths.size(); i++) {
//        currentIndex = orderingOfPaths[i];
//        if (isPathInCorrectDirection[i]) {
//            std::valarray<int> currentPosition = sequenceOfPaths[currentIndex].first();
//            movedDistance += norm(currentPosition - previousPosition);
//            previousPosition = sequenceOfPaths[currentIndex].last();
//        } else {
//            std::valarray<int> currentPosition = sequenceOfPaths[currentIndex].last();
//            movedDistance += norm(currentPosition - previousPosition);
//            previousPosition = sequenceOfPaths[currentIndex].first();
//        }
//    }
//    return movedDistance;
//}
//
//
//std::vector<std::valarray<int>> generateStartingPoints(const std::valarray<int> &dimensions, int numberOfSteps) {
//    std::vector<std::valarray<int>> startingPoints = {{0, 0}, {dimensions[0], 0}, dimensions, {0, dimensions[1]}};
//
//    double xStep = (double) dimensions[0] / numberOfSteps;
//    double yStep = (double) dimensions[1] / numberOfSteps;
//    for (int step = 0; step < numberOfSteps; step++) {
//        startingPoints.push_back({(int) (xStep * step), 0});
//        startingPoints.push_back({dimensions[0], (int) (yStep * step)});
//        startingPoints.push_back({dimensions[0] - (int) (xStep * step), dimensions[1]});
//        startingPoints.push_back({dimensions[0], dimensions[1] - (int) (yStep * step)});
//    }
//    return startingPoints;
//}
//
//
//std::valarray<int> PathFillingOrder::findBestStartingPoints() {
//    std::valarray<int> bestStartingPoint;
//    std::vector<std::valarray<int>> startingPoints = generateStartingPoints(dimensions, 5);
//
//    double lowestMovedDistance = DBL_MAX;
//    for (auto &startingPoint: startingPoints) {
//        startGeneratingPrintPathsFrom(startingPoint);
//        double currentMovedDistance = getMovedDistance();
//        if (currentMovedDistance < lowestMovedDistance) {
//            lowestMovedDistance = currentMovedDistance;
//            bestStartingPoint = startingPoint;
//        }
//    }
//    return bestStartingPoint;
//}
//
//
//void printVecVal(const std::vector<std::valarray<int>> &array) {
//    for (auto &element: array) {
//        std::cout << "(";
//        for (auto &subelement: element) {
//             std::cout << subelement << " " ;
//        }
//        std::cout << ")" << std::endl;
//    }
//}
//
//
//std::vector<std::vector<std::valarray<int>>> sortedSequenceOfPaths(const std::vector<Path> &pathSequence,
//                                                                   const std::vector<int> &ordering,
//                                                                   const std::vector<bool> &isPathInCorrectDirection) {
//
//    std::vector<std::vector<std::valarray<int>>> sortedSequence;
//    for (int i = 0; i < ordering.size(); i++) {
//        std::vector<std::valarray<int>> newPath = (pathSequence[ordering[i]]).sequenceOfPositions;
//        std::cout << ordering[i] << std::endl;
//
//        if (!isPathInCorrectDirection[i]) {
//            std::reverse(newPath.begin(), newPath.end());
//        }
//        sortedSequence.emplace_back(newPath);
//    }
//    return sortedSequence;
//}
//
//
//void PathFillingOrder::generateBestFillingOrderAndExportToPath(const std::string &path) {
//    std::valarray<int> bestStartingPoint = findBestStartingPoints();
//
//    startGeneratingPrintPathsFrom(bestStartingPoint);
//    std::vector<std::vector<std::valarray<int>>> sortedPaths = sortedSequenceOfPaths(sequenceOfPaths, orderingOfPaths,
//                                                                                     isPathInCorrectDirection);
//    export3DVectorToFile(sortedPaths, path, "best_paths");
//}
//
//
