//
// Created by Michał Zmyślony on 23/09/2021.
//

#include "StartingPoint.h"
#include <utility>
#include "../auxiliary/PerimeterChecking.h"
#include <iostream>
#include <cstdlib>


int const MAX_RANDOM_SEARCH_TRIES = 100;

StartingPoint::StartingPoint():
        tries(0) {
}


void StartingPoint::findSuitableStartPointSemiRandomly(FilledPattern &pattern) {
    tries++;
//    unsigned int element = rand() % pattern.pointsToFill.size();
    unsigned int element = pattern.getNewElement();
    positions = pattern.pointsToFill[element];

    if (isPerimeterFree(pattern.numberOfTimesFilled, pattern.desiredPattern.shapeMatrix,
                        pattern.collisionList, positions, pattern.desiredPattern.dimensions)) {
        isStartingPointFound = true;
    }
}


std::valarray<int> StartingPoint::findSuitableStartPoint(FilledPattern &pattern) {
    while(!isStartingPointFound) {
        if (tries < MAX_RANDOM_SEARCH_TRIES && areThereFillablePointsRemaining) {
            findSuitableStartPointSemiRandomly(pattern);
        }
        else if (tries == MAX_RANDOM_SEARCH_TRIES && areThereFillablePointsRemaining) {
            tries = 0;
            unsigned int previousNumberOfFillablePoints = pattern.pointsToFill.size();
            pattern.findRemainingFillablePoints();
            areThereFillablePointsRemaining = !pattern.pointsToFill.empty();
            if (previousNumberOfFillablePoints == pattern.pointsToFill.size()) {
                positions = {-1, -1};
                isStartingPointFound = true;
            }
        }
        else {
            positions = {-1, -1};
            isStartingPointFound = true;
        }
    }
    return positions;
}
