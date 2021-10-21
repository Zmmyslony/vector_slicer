//
// Created by Michał Zmyślony on 23/09/2021.
//

#include "StartingPoint.h"
#include <utility>
#include "../auxiliary/PerimeterChecking.h"
#include <iostream>

int const MAX_RANDOM_SEARCH_TRIES = 100;

StartingPoint::StartingPoint():
        tries(0) {
}


//void StartingPoint::findSuitableStartPointRandomly(FilledPattern& pattern) {
//    tries++;
//    positions = {rand() % pattern.desiredPattern.dimensions[0], rand() % pattern.desiredPattern.dimensions[1]};
//    if (pattern.desiredPattern.shapeMatrix[positions[0]][positions[1]]) {
//        if (isPerimeterFree(pattern.numberOfTimesFilled,pattern.desiredPattern.shapeMatrix,
//                            pattern.collisionList,positions, pattern.desiredPattern.dimensions)) {
//            isStartingPointFound = true;
//        }
//    }
//}


void StartingPoint::findSuitableStartPointSemiRandomly(FilledPattern& pattern) {
    tries++;
    unsigned int element = rand() % pattern.pointsToFill.size();
    positions = pattern.pointsToFill[element];

    if (isPerimeterFree(pattern.numberOfTimesFilled, pattern.desiredPattern.shapeMatrix,
                        pattern.collisionList, positions, pattern.desiredPattern.dimensions)) {
        isStartingPointFound = true;
    }
}


std::valarray<int> StartingPoint::findSuitableStartPoint(FilledPattern& pattern) {
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
//    if(pattern.isPerimeterSearchOn) {
//        printf("Position: %i, %i, tries: %i\n", positions[0], positions[1], tries);
//    }
    return positions;
}

//std::valarray<int> StartingPoint::findSuitableStartPoint(FilledPattern& pattern) {
//    while(!isStartingPointFound) {
//        if (tries < MAX_RANDOM_SEARCH_TRIES && pattern.isRandomSearchOn) {
//            findSuitableStartPointRandomly(pattern);
//        }
//        else if (tries < MAX_RANDOM_SEARCH_TRIES && !pattern.isRandomSearchOn && areThereFillablePointsRemaining) {
//            findSuitableStartPointSemiRandomly(pattern);
//        }
//        else if (tries == MAX_RANDOM_SEARCH_TRIES && areThereFillablePointsRemaining) {
//            tries = 0;
//            unsigned int previousNumberOfFillablePoints = pattern.pointsToFill.size();
//            pattern.findRemainingFillablePoints();
//            areThereFillablePointsRemaining = !pattern.pointsToFill.empty();
//            if (previousNumberOfFillablePoints == pattern.pointsToFill.size()) {
//                positions = {-1, -1};
//                isStartingPointFound = true;
//            }
//        }
//        else {
//            positions = {-1, -1};
//            isStartingPointFound = true;
//        }
//    }
//    return positions;
//}
