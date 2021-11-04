//
// Created by Michał Zmyślony on 21/10/2021.
//

#include "GcodeGenerator.h"
#include "../auxiliary/ValarrayOperations.h"
#include "../auxiliary/ValarrayConversion.h"
#include <iostream>
#include <fstream>


std::vector<std::valarray<int>> getStartAndEndPositions(std::vector<Path> &sequenceOfPaths) {
    std::vector<std::valarray<int>> startEndPositions;
    int iterator = 0;
    for (auto &path : sequenceOfPaths) {
        std::valarray<int> newElementFront = {path.first()[0], path.first()[1], iterator};
        std::valarray<int> newElementBack = {path.last()[0], path.last()[1], iterator};
        startEndPositions.push_back(newElementFront);
        startEndPositions.push_back(newElementBack);
        iterator++;
    }
    return startEndPositions;
}


int findClosestElementInList(std::valarray<int> &previousPosition, std::vector<std::valarray<int>> &startEndPositions) {
    double distance = DBL_MAX;
    int index = 0;
    for (int i = 0; i < startEndPositions.size(); i++) {
        std::valarray<int> currentPosition = {startEndPositions[i][0], startEndPositions[i][1]};
        double currentDistance = norm(itodArray(currentPosition - previousPosition));
        if(currentDistance < distance && currentDistance > 0) {
            distance = currentDistance;
            index = i;
        }
    }
    return index;
}


GcodeGenerator::GcodeGenerator(FilledPattern pattern):
        sequenceOfPaths(pattern.getSequenceOfPaths()) {
    startAndEndPositions = getStartAndEndPositions(sequenceOfPaths);
}


void GcodeGenerator::startGeneratingPrintPathsFrom(std::valarray<int> startingPoint) {
    std::valarray<int> currentPoint = startingPoint;
    std::vector<std::valarray<int>> localStartAndEndPositions = startAndEndPositions;
    orderingOfPaths.clear();
    isPathInCorrectDirection.clear();

    while (!localStartAndEndPositions.empty()) {
        int closestIndex = findClosestElementInList(currentPoint, localStartAndEndPositions);
        int closestPathIndex = localStartAndEndPositions[closestIndex][2];
        bool isItEndingPoint = closestIndex % 2;
        orderingOfPaths.push_back(closestPathIndex);
        isPathInCorrectDirection.push_back(!isItEndingPoint);

        if (!isItEndingPoint) {
            currentPoint = {localStartAndEndPositions[closestIndex + 1][0], localStartAndEndPositions[closestIndex + 1][1]};
            localStartAndEndPositions.erase(localStartAndEndPositions.begin() + closestIndex);
            localStartAndEndPositions.erase(localStartAndEndPositions.begin() + closestIndex);
        }
        else {
            currentPoint = {localStartAndEndPositions[closestIndex - 1][0], localStartAndEndPositions[closestIndex - 1][1]};
            localStartAndEndPositions.erase(localStartAndEndPositions.begin() + closestIndex - 1);
            localStartAndEndPositions.erase(localStartAndEndPositions.begin() + closestIndex - 1);
        }
    }
}


double GcodeGenerator::getMovedDistance() {
    double movedDistance = 0;
    std::valarray<int> previousPosition;
    int currentIndex = orderingOfPaths[0];

    if (isPathInCorrectDirection[0]) {
        previousPosition = sequenceOfPaths[currentIndex].last();
    }
    else {
        previousPosition = sequenceOfPaths[currentIndex].first();
    }

    for (int i = 1; i < orderingOfPaths.size(); i++) {
        currentIndex = orderingOfPaths[i];
        if (isPathInCorrectDirection[i]) {
            std::valarray<int> currentPosition = sequenceOfPaths[currentIndex].first();
            movedDistance += norm(currentPosition - previousPosition);
            previousPosition = sequenceOfPaths[currentIndex].last();
        }
        else {
            std::valarray<int> currentPosition = sequenceOfPaths[currentIndex].last();
            movedDistance += norm(currentPosition - previousPosition);
            previousPosition = sequenceOfPaths[currentIndex].first();
        }
    }
    return movedDistance;
}


std::valarray<int> GcodeGenerator::findBestStartingPoints() {
    std::valarray<int> bestStartingPoint;
    double lowestMovedDistance = DBL_MAX;
    for (auto &startingPoint : startAndEndPositions) {
        startGeneratingPrintPathsFrom(startingPoint);
        double currentMovedDistance = getMovedDistance();
//        printf("Current starting position: %i, %i. \tMoved distance: %.2f\n", startingPoint[0], startingPoint[1],
//               currentMovedDistance);
        if (currentMovedDistance < lowestMovedDistance) {
            lowestMovedDistance = currentMovedDistance;
            bestStartingPoint = startingPoint;
        }
    }
//    printf("Best starting position: %i, %i. Moved distance: %.2f", bestStartingPoint[0], bestStartingPoint[1],
//           lowestMovedDistance);
    return bestStartingPoint;
}


void GcodeGenerator::exportToPath(std::string filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << header << "\n";
        file << printPathsGcode << "\n";
        file << footer;
        file.close();
    }
}