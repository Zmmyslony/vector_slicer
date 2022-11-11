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
// Created by Michał Zmyślony on 29/09/2021.
//

#include "QuantifyPattern.h"
#include <utility>
#include <cmath>

QuantifyPattern::QuantifyPattern(FilledPattern pattern):
        pattern(std::move(pattern)) {
    emptySpots = calculateEmptySpots();
    averageOverlap = calculateAverageOverlap();
    directorDisagreement = calculateDirectorDisagreement();
    numberOfPaths = calculateNumberOfPaths();
}

double QuantifyPattern::calculateEmptySpots() {
    int numberOfEmptySpots = 0;
    int xSize = pattern.desiredPattern.getDimensions()[0];
    int ySize = pattern.desiredPattern.getDimensions()[1];
    int numberOfElements = 0;

    for (int i = 0; i < xSize; i++) {
        for (int j = 0; j < ySize; j++) {
            if (pattern.desiredPattern.getShapeMatrix()[i][j] == 1) {
                numberOfElements++;
                if (pattern.numberOfTimesFilled[i][j] == 0) {
                    numberOfEmptySpots++;
                }
            }
        }
    }
    return (double) numberOfEmptySpots / (double) numberOfElements;
}

double QuantifyPattern::calculateAverageOverlap() {
    int numberOfFilledTimes = 0;
    int xSize = pattern.desiredPattern.getDimensions()[0];
    int ySize = pattern.desiredPattern.getDimensions()[1];
    int numberOfElements = 0;

    for (int i = 0; i < xSize; i++) {
        for (int j = 0; j < ySize; j++) {
            numberOfFilledTimes += pattern.numberOfTimesFilled[i][j];
            numberOfElements += pattern.desiredPattern.getShapeMatrix()[i][j];
        }
    }
    return (double) numberOfFilledTimes / (double) numberOfElements - 1 + emptySpots;
}


double QuantifyPattern::calculateDirectorDisagreement() {
    double directorAgreement = 0;
    int xSize = pattern.desiredPattern.getDimensions()[0];
    int ySize = pattern.desiredPattern.getDimensions()[1];
    int numberOfFilledElements = 0;

    for (int i = 0; i < xSize; i++) {
        for (int j = 0; j < ySize; j++) {
            if (pattern.numberOfTimesFilled[i][j] > 0) {
                double filledDirectorNorm = sqrt(
                        pow(pattern.xFieldFilled[i][j], 2) + pow(pattern.yFieldFilled[i][j], 2));
                double desiredDirectorNorm = sqrt(pow(pattern.desiredPattern.getXFieldPreferred()[i][j], 2) +
                                                  pow(pattern.desiredPattern.getYFieldPreferred()[i][j], 2));
                double xDirectionAgreement = pattern.xFieldFilled[i][j] * pattern.desiredPattern.getXFieldPreferred()[i][j];
                double yDirectionAgreement = pattern.yFieldFilled[i][j] * pattern.desiredPattern.getYFieldPreferred()[i][j];
                if (desiredDirectorNorm != 0 && filledDirectorNorm != 0) {
                    directorAgreement +=
                            abs(xDirectionAgreement + yDirectionAgreement) / (filledDirectorNorm * desiredDirectorNorm);
                    numberOfFilledElements++;
                }
            }
        }
    }
    return 1 - (double) directorAgreement / (double) numberOfFilledElements;
}


double QuantifyPattern::calculateNumberOfPaths() {
    unsigned int paths = pattern.getSequenceOfPaths().size();
    auto perimeterLength = (unsigned int) fmax(pattern.desiredPattern.getDimensions()[0],
                                               pattern.desiredPattern.getDimensions()[1]);
    return (double) paths / (double) perimeterLength;
}

double QuantifyPattern::calculateCorrectness(double emptySpotWeight, double overlapWeight, double directorWeight,
                                             double pathWeight,
                                             double emptySpotExponent, double overLapExponent, double directorExponent,
                                             double pathExponent) const {
    return emptySpotWeight * pow(emptySpots, emptySpotExponent) + overlapWeight * pow(averageOverlap, overLapExponent) +
           directorWeight * pow(directorDisagreement, directorExponent) + pathWeight * pow(numberOfPaths, pathExponent);
}

double QuantifyPattern::calculateCorrectness(double emptySpotWeight, double overlapWeight, double directorWeight,
                                             double pathWeight) const {
    return calculateCorrectness(emptySpotWeight, overlapWeight, directorWeight, pathWeight, 1, 1, 1, 1);
}

