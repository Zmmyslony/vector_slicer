//
// Created by Michał Zmyślony on 27/09/2021.
//

#include "FillingPatterns.h"
#include "StartingPoint.h"
#include <iostream>

bool tryGeneratingNewPath(FilledPattern& pattern) {
    StartingPoint startingPoint;
    std::valarray<int> startingCoordinates = startingPoint.findSuitableStartPoint(pattern);
    if (startingPoint.positions[0] == -1 || startingPoint.positions[1] == -1) {
        return false;
    }
    else {
        Path newPathForward = pattern.generateNewPathForDirection(startingPoint.positions, pattern.desiredPattern.preferredDirection(startingPoint.positions, pattern.stepLength));
        Path newPathBackwards = pattern.generateNewPathForDirection(startingPoint.positions, -pattern.desiredPattern.preferredDirection(startingPoint.positions, pattern.stepLength));

        Path newPath(newPathForward.positionsAlongDirector, newPathBackwards.positionsOppositeToDirector);

        pattern.addNewPath(newPath);
        return true;
    }
}

void fillWithPaths(FilledPattern& pattern) {
    bool isThereAnySpotFillable = true;
    while (isThereAnySpotFillable) {
        isThereAnySpotFillable = tryGeneratingNewPath(pattern);
    }
}