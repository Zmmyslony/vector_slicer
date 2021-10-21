//
// Created by Michał Zmyślony on 21/09/2021.
//

#include <cstdlib>
#include <iostream>

#include "FilledPattern.h"

#include "../auxiliary/PerimeterGeneration.h"
#include "../auxiliary/PerimeterChecking.h"
#include "../auxiliary/SimpleMathOperations.h"
#include "../auxiliary/Geometry.h"
#include "../auxiliary/ValarrayConversion.h"
#include "../auxiliary/Exporting.h"
#include "../auxiliary/ValarrayOperations.h"


FilledPattern::FilledPattern(DesiredPattern &desiredPattern, int printRadius, int collisionRadius, int stepLength, unsigned int seed):
        desiredPattern(desiredPattern),
        numberOfTimesFilled(desiredPattern.dimensions[0], std::vector<int>(desiredPattern.dimensions[1])),
        xFieldFilled(desiredPattern.dimensions[0], std::vector<double>(desiredPattern.dimensions[1])),
        yFieldFilled(desiredPattern.dimensions[0], std::vector<double>(desiredPattern.dimensions[1])),
        printRadius(printRadius),
        collisionList(generatePerimeterList(collisionRadius)),
        stepLength(stepLength) {
    srand(seed);
    pointsInCircle = findPointsInCircle(printRadius);
}


FilledPattern::FilledPattern(DesiredPattern &desiredPattern, int printRadius, int collisionRadius, int stepLength):
        FilledPattern::FilledPattern(desiredPattern, printRadius, collisionRadius, stepLength, 0) { }


std::vector<std::valarray<int>> FilledPattern::searchWholeGridForFillablePoints() {
    std::vector<std::valarray<int>> newPointsToFill;
    for (int i = 0; i < desiredPattern.dimensions[0]; i++) {
        for (int j = 0; j < desiredPattern.dimensions[1]; j++) {
            std::valarray<int> currentPos = {i, j};
            if (isPerimeterFree(numberOfTimesFilled, desiredPattern.shapeMatrix,collisionList,
                                currentPos,desiredPattern.dimensions)) {
                newPointsToFill.push_back(currentPos);
            }
        }
    }
    return newPointsToFill;
}


std::vector<std::valarray<int>> FilledPattern::searchForRemainingFillablePoints() {
    std::vector<std::valarray<int>> newPointsToFill;
    for (auto & currentPos : pointsToFill) {
        if (isPerimeterFree(numberOfTimesFilled, desiredPattern.shapeMatrix,collisionList, currentPos,
                            desiredPattern.dimensions)) {
            newPointsToFill.push_back(currentPos);
        }
    }
    return newPointsToFill;
}


void FilledPattern::findRemainingFillablePoints() {
    isRandomSearchOn = false;

    std::vector<std::valarray<int>> newPointsToFill;
    if (pointsToFill.empty()) {
        pointsToFill = searchWholeGridForFillablePoints();
    }
    else {
        pointsToFill = searchForRemainingFillablePoints();
    }
}


std::valarray<double> normalizeDirection(const std::valarray<int>& previousStep) {
    std::valarray<double> normalizedDirection  = normalize(previousStep);
    if (previousStep[0] > 0 || previousStep[0] == 0 && previousStep[1] > 0) {
        return normalizedDirection;
    }
    else {
        return -normalizedDirection;
    }
}


void FilledPattern::fillPoint(const std::valarray<int>& point, const std::valarray<double>& normalizedDirection) {
    numberOfTimesFilled[point[0]][point[1]] += 1;
    xFieldFilled[point[0]][point[1]] += normalizedDirection[0];
    yFieldFilled[point[0]][point[1]] += normalizedDirection[1];
}


void FilledPattern::fillPointsFromList(const std::vector<std::valarray<int>>& listOfPoints,
                                       const std::valarray<int>& previousStep) {
    std::valarray<double> normalizedDirection = normalizeDirection(previousStep);
    for (auto & point : listOfPoints) {
        fillPoint(point, normalizedDirection);
    }
}


void FilledPattern::fillPointsFromDisplacement(const std::valarray<int>& startingPosition,
                                               const std::vector<std::valarray<int>>& listOfDisplacements,
                                               const std::valarray<int>& previousStep) {
    std::valarray<double> normalizedDirection = normalizeDirection(previousStep);
    for (auto & displacement : listOfDisplacements) {
        std::valarray<int> point = startingPosition + displacement;
        if (point[0] >= 0 && point[0] < desiredPattern.dimensions[0] && point[1] >= 0 && point[1] < desiredPattern.dimensions[1]){
            fillPoint(point, normalizedDirection);
        }
    }
}


std::valarray<double> FilledPattern::getNewStep(std::valarray<double>& positions, int& length,
                                                std::valarray<double>& previousMove) {
    std::valarray<double> newMove = desiredPattern.preferredDirection(positions, length);
    int isOppositeToPreviousStep = sgn(newMove[0] * previousMove[0] + newMove[1] * previousMove[1]);
    if (isOppositeToPreviousStep < 0) {
        newMove = -newMove;
    }
    return newMove;
}


bool FilledPattern::tryGeneratingPathWithLength(Path& currentPath, std::valarray<double>& positions,
                                                std::valarray<double>& previousStep, int length) {
    std::valarray<int> currentCoordinates = dtoiArray(positions);
    std::valarray<double> newStep = getNewStep(positions, length, previousStep);
    std::valarray<double> newPositions = positions + newStep;
    std::valarray<int> newCoordinates = dtoiArray(newPositions);
    std::valarray<double> repulsion = getRepulsion(numberOfTimesFilled, pointsInCircle, newCoordinates,
                                                   desiredPattern.dimensions, 0.7);
    newPositions -= repulsion;
    newCoordinates = dtoiArray(newPositions);
//    printf("Current coordinates %i, %i, new coordinates %i, %i, repulsion %.2f, %.2f, new step %.2f, %.2f \n",
//           currentCoordinates[0], currentCoordinates[1], newCoordinates[0], newCoordinates[1],
//           repulsion[0], repulsion[1], newStep[0], newStep[1]);

    if (isPerimeterFree(numberOfTimesFilled, desiredPattern.shapeMatrix, collisionList, newCoordinates, desiredPattern.dimensions)) {
        std::vector<std::valarray<int>> currentPointsToFill = findPointsToFill(currentCoordinates, newCoordinates, printRadius);
//        std::valarray<int> newStepInt = dtoiArray(newStep);
        std::valarray<int> newStepInt = newCoordinates - currentCoordinates;
        fillPointsFromList(currentPointsToFill, newStepInt);
        currentPath.addPointToForwardArray(newCoordinates);

        newStep = getNewStep(newPositions, length, newStep);
        positions = newPositions;
//        printf("\t New position created.");
        return true;
    }
    return false;
}


Path FilledPattern::generateNewPathForDirection(std::valarray<int>& startingCoordinates, const std::valarray<int>& startingStep) {
    Path newPath(startingCoordinates);
    bool wasLineCreated = false;
    std::valarray<double> currentPositions = itodArray(startingCoordinates);
    std::valarray<double> currentStep = itodArray(startingStep);
    for (int length = stepLength; length >= printRadius; length--) {
        while (tryGeneratingPathWithLength(newPath, currentPositions, currentStep, length)) {
            wasLineCreated = true;
        }
    }
    if (!wasLineCreated) {
        fillPointsFromDisplacement(startingCoordinates, pointsInCircle, startingStep);
    }
    return newPath;
}



void FilledPattern::exportToDirectory(std::string& directory) const {
    std::string filledFilename = directory + "\\number_of_times_filled.csv";
    std::string xFieldFilename = directory + "\\x_field.csv";
    std::string yFieldFilename = directory + "\\y_field.csv";
    exportVectorTableToFile(numberOfTimesFilled, filledFilename);
}


std::vector<Path> FilledPattern::getSequenceOfPaths() {
    return sequenceOfPaths;
}


void FilledPattern::addNewPath(Path& newPath) {
    sequenceOfPaths.push_back(newPath);
}