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
// Created by Michał Zmyślony on 27/09/2021.
//

#include "FillingPatterns.h"


bool tryGeneratingNewPath(FilledPattern &pattern, StartingPoint &startingPoint) {
    startingPoint.refresh();
    std::valarray<int> startingCoordinates = startingPoint.findStartPoint(pattern);
    if (startingPoint.positions[0] == -1 || startingPoint.positions[1] == -1) {
        return false;
    } else {
        Path newPathForwards = pattern.generateNewPathForDirection(startingPoint.positions,
                                                                   pattern.desiredPattern.preferredDirection(
                                                                           startingPoint.positions,
                                                                           pattern.config.getStepLength()));
        Path newPathBackwards = pattern.generateNewPathForDirection(startingPoint.positions,
                                                                    -pattern.desiredPattern.preferredDirection(
                                                                            startingPoint.positions,
                                                                            pattern.config.getStepLength()));

        Path newPath(newPathForwards, newPathBackwards);

        if (newPath.getSize() == 1) {
            pattern.fillPointsInCircle(newPath.sequenceOfPositions[0]);
        } else {
            pattern.fillPointsInHalfCircle(newPath.first(), newPath.second());
            pattern.fillPointsInHalfCircle(newPath.last(), newPath.previousToLast());
        }

        pattern.addNewPath(newPath);
        return true;
    }
}

void fillWithPaths(FilledPattern &pattern) {
    bool isThereAnySpotFillable = true;
    StartingPoint startingPoint;
    while (isThereAnySpotFillable) {
        isThereAnySpotFillable = tryGeneratingNewPath(pattern, startingPoint);
    }
}