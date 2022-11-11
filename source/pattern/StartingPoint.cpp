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
// Created by Michał Zmyślony on 23/09/2021.
//

#include "StartingPoint.h"
#include "../auxiliary/Perimeter.h"
#include "../auxiliary/ValarrayOperations.h"
#include <iostream>

int const MAX_RANDOM_SEARCH_TRIES = 1000;

StartingPoint::StartingPoint() :
        tries(0) {
}

void StartingPoint::findStartPointTotallyRandomly(FilledPattern &pattern) {
    tries++;
    positions = pattern.findPointInShape();

    if (isPerimeterFree(pattern.numberOfTimesFilled, pattern.desiredPattern.getShapeMatrix(),
                        pattern.collisionList, positions, pattern.desiredPattern.getDimensions())) {
        isStartingPointFound = true;
    }
}


void StartingPoint::findStartPointSemiRandomly(FilledPattern &pattern) {
    tries++;
    unsigned int element = pattern.getNewElement();
    positions = pattern.pointsToFill[element];

    if (isPerimeterFree(pattern.numberOfTimesFilled, pattern.desiredPattern.getShapeMatrix(),
                        pattern.collisionList, positions, pattern.desiredPattern.getDimensions())) {
        isStartingPointFound = true;
    }
}


void StartingPoint::findStartPointConsecutively(FilledPattern &pattern) {
    for (int i = previouslyFoundPoint; i < pattern.pointsToFill.size(); i++) {
        positions = pattern.pointsToFill[i];
        if (pattern.desiredPattern.getShapeMatrix()[positions[0]][positions[1]] &&
            isPerimeterFree(pattern.numberOfTimesFilled, pattern.desiredPattern.getShapeMatrix(),
                            pattern.collisionList, positions, pattern.desiredPattern.getDimensions())) {
            isStartingPointFound = true;
            previouslyFoundPoint = i;
            return;
        }
    }
    tries = MAX_RANDOM_SEARCH_TRIES;
}


void StartingPoint::lookForAPoint(FilledPattern &pattern) {
    if (pattern.searchStage == TotallyRandomPointSelection) {
        findStartPointTotallyRandomly(pattern);
    }
    else if (pattern.isFillingMethodRandom) {
        findStartPointSemiRandomly(pattern);
    } else {
        findStartPointConsecutively(pattern);
    }
}


void StartingPoint::updateListOfPoints(FilledPattern &pattern) {
    tries = 0;
    previouslyFoundPoint = 0;
    areThereFillablePointsRemaining = true;

    unsigned int previousNumberOfFillablePoints = pattern.pointsToFill.size();
    pattern.updateSearchStageAndFillablePoints();

    if (pattern.searchStage != TotallyRandomPointSelection) {
        areThereFillablePointsRemaining = !pattern.pointsToFill.empty();
        if (previousNumberOfFillablePoints == pattern.pointsToFill.size()) {
            areThereFillablePointsRemaining = false;
        }
    }
}


void StartingPoint::trySearchingForAPoint(FilledPattern &pattern) {
    if (tries < MAX_RANDOM_SEARCH_TRIES) {
        lookForAPoint(pattern);
    } else if (tries == MAX_RANDOM_SEARCH_TRIES) {
        updateListOfPoints(pattern);
    }
}


std::valarray<int> StartingPoint::findStartPoint(FilledPattern &pattern) {
    while (!isStartingPointFound) {
        if (areThereFillablePointsRemaining) {
            trySearchingForAPoint(pattern);
        }
        else {
            positions = {-1, -1};
            return positions;
        }
    }
    return positions;
}


void StartingPoint::refresh() {
    tries = 0;
    isStartingPointFound = false;
    areThereFillablePointsRemaining = true;
}
