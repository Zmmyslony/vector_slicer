#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc51-cpp"
//
// Created by Michał Zmyślony on 21/09/2021.
//

#include <iostream>
#include <utility>

#include "FilledPattern.h"

#include "../auxiliary/Exporting.h"
#include "../auxiliary/Geometry.h"
#include "../auxiliary/PerimeterGeneration.h"
#include "../auxiliary/PerimeterChecking.h"
#include "../auxiliary/SimpleMathOperations.h"
#include "../auxiliary/ValarrayOperations.h"


FilledPattern::FilledPattern(const DesiredPattern &newDesiredPattern, FillingConfig newConfig) :
        desiredPattern(newDesiredPattern),
        config(newConfig) {
    numberOfTimesFilled = std::vector<std::vector<int>>(newDesiredPattern.dimensions[0],
                                                        std::vector<int>(newDesiredPattern.dimensions[1]));
    xFieldFilled = std::vector<std::vector<double>>(newDesiredPattern.dimensions[0],
                                                    std::vector<double>(newDesiredPattern.dimensions[1]));
    yFieldFilled = std::vector<std::vector<double>>(newDesiredPattern.dimensions[0],
                                                    std::vector<double>(newDesiredPattern.dimensions[1]));
    collisionList = generatePerimeterList(config.getCollisionRadius());
    randomEngine = std::mt19937(config.getSeed());
    pointsInCircle = findPointsInCircle(config.getPrintRadius());
    pointsToFill = findInitialStartingPoints(config.getInitialFillingMethod());
    unsigned int numberOfFillablePoints = pointsToFill.size();
    distribution = std::uniform_int_distribution<unsigned int>(0, numberOfFillablePoints - 1);
}


FilledPattern::FilledPattern(const DesiredPattern &desiredPattern, int printRadius, int collisionRadius, int stepLength,
                             unsigned int seed) :
        FilledPattern(desiredPattern,FillingConfig(RandomPerimeter, collisionRadius, 2 * printRadius, 1.0, stepLength, printRadius,
                                    seed)) {
}

FilledPattern::FilledPattern(const DesiredPattern &desiredPattern, int printRadius, int collisionRadius, int stepLength) :
        FilledPattern::FilledPattern(desiredPattern, printRadius, collisionRadius, stepLength, 0) {}


std::vector<std::valarray<int>> FilledPattern::findAllFillablePoints() {
    std::vector<std::valarray<int>> newPointsToFill;
    for (int i = 0; i < desiredPattern.dimensions[0]; i++) {
        for (int j = 0; j < desiredPattern.dimensions[1]; j++) {
            std::valarray<int> currentPos = {i, j};
            if (isPerimeterFree(numberOfTimesFilled, desiredPattern.shapeMatrix, collisionList,
                                currentPos, desiredPattern.dimensions)) {
                newPointsToFill.push_back(currentPos);
            }
        }
    }
    return newPointsToFill;
}

std::vector<std::valarray<int>>
FilledPattern::findRemainingFillablePointsInList(std::vector<std::valarray<int>> listOfPoints) {
    std::vector<std::valarray<int>> fillablePointsList;
    for (auto &point: listOfPoints) {
        if (isPerimeterFree(numberOfTimesFilled, desiredPattern.shapeMatrix, collisionList,
                            point, desiredPattern.dimensions)) {
            fillablePointsList.push_back(point);
        }
    }
    return fillablePointsList;
}

void FilledPattern::findRemainingFillablePoints() {
    if (isPerimeterSearchOn) {
        pointsToFill = findRemainingFillablePointsInList(pointsToFill);

        if (pointsToFill.empty()) {
            isPerimeterSearchOn = false;
            pointsToFill = findAllFillablePoints();
        }
    } else {
        pointsToFill = findRemainingFillablePointsInList(pointsToFill);
    }
    unsigned int numberOfFillablePoints = pointsToFill.size();
    distribution = std::uniform_int_distribution<unsigned int>(0, numberOfFillablePoints - 1);
}

std::valarray<double> normalizeDirection(const std::valarray<int> &previousStep) {
    std::valarray<double> normalizedDirection = normalize(previousStep);
    if (previousStep[0] > 0 || previousStep[0] == 0 && previousStep[1] > 0) {
        return normalizedDirection;
    } else {
        return -normalizedDirection;
    }
}

void FilledPattern::fillPoint(const std::valarray<int> &point, const std::valarray<double> &normalizedDirection) {
    numberOfTimesFilled[point[0]][point[1]] += 1;
    xFieldFilled[point[0]][point[1]] += normalizedDirection[0];
    yFieldFilled[point[0]][point[1]] += normalizedDirection[1];
}

void FilledPattern::fillPointsFromList(const std::vector<std::valarray<int>> &listOfPoints,
                                       const std::valarray<int> &direction) {
    std::valarray<double> normalizedDirection = normalizeDirection(direction);
    for (auto &point: listOfPoints) {
        fillPoint(point, normalizedDirection);
    }
}

void FilledPattern::fillPointsFromDisplacement(const std::valarray<int> &startingPosition,
                                               const std::vector<std::valarray<int>> &listOfDisplacements,
                                               const std::valarray<int> &previousStep) {
    std::valarray<double> normalizedDirection = normalizeDirection(previousStep);
    for (auto &displacement: listOfDisplacements) {
        std::valarray<int> point = startingPosition + displacement;
        if (point[0] >= 0 && point[0] < desiredPattern.dimensions[0] && point[1] >= 0 &&
            point[1] < desiredPattern.dimensions[1]) {
            fillPoint(point, normalizedDirection);
        }
    }
}

std::valarray<double> FilledPattern::getNewStep(std::valarray<double> &positions, int &length,
                                                std::valarray<double> &previousMove) {
    std::valarray<double> newMove = desiredPattern.preferredDirection(positions, length);
    int isOppositeToPreviousStep = sgn(newMove[0] * previousMove[0] + newMove[1] * previousMove[1]);
    if (isOppositeToPreviousStep < 0) {
        newMove = -newMove;
    }
    return newMove;
}

bool FilledPattern::tryGeneratingPathWithLength(Path &currentPath, std::valarray<double> &positions,
                                                std::valarray<double> &previousStep, int length) {
    std::valarray<int> currentCoordinates = dtoiArray(positions);
    std::valarray<double> newStep = getNewStep(positions, length, previousStep);
    std::valarray<double> newPositions = positions + newStep;
    std::valarray<int> newCoordinates = dtoiArray(newPositions);
    std::valarray<double> repulsion = getRepulsion(numberOfTimesFilled, pointsInCircle, newCoordinates,
                                                   desiredPattern.dimensions, config.getRepulsion());
    newPositions -= repulsion;
    newCoordinates = dtoiArray(newPositions);

    if (isPerimeterFree(numberOfTimesFilled, desiredPattern.shapeMatrix, collisionList, newCoordinates,
                        desiredPattern.dimensions)) {
        std::vector<std::valarray<int>> currentPointsToFill = findPointsToFill(currentCoordinates, newCoordinates,
                                                                               config.getPrintRadius());
        std::valarray<int> newStepInt = newCoordinates - currentCoordinates;
        fillPointsFromList(currentPointsToFill, newStepInt);
        currentPath.addPoint(newCoordinates);

        newStep = getNewStep(newPositions, length, newStep);
        positions = newPositions;
        return true;
    }
    return false;
}

Path FilledPattern::generateNewPathForDirection(std::valarray<int> &startingCoordinates,
                                                const std::valarray<int> &startingStep) {
    Path newPath(startingCoordinates);
    std::valarray<double> currentPositions = itodArray(startingCoordinates);
    std::valarray<double> currentStep = itodArray(startingStep);
    for (int length = config.getStepLength(); length >= config.getPrintRadius(); length--) {
        while (tryGeneratingPathWithLength(newPath, currentPositions, currentStep, length)) {}
    }
    return newPath;
}

void FilledPattern::fillPointsInCircle(std::valarray<int> &startingCoordinates) {
    fillPointsFromDisplacement(startingCoordinates, pointsInCircle, {1, 0});
}

void FilledPattern::exportToDirectory(std::string &directory) const {
    std::string filledFilename = directory + "\\number_of_times_filled.csv";
    std::string xFieldFilename = directory + "\\x_field.csv";
    std::string yFieldFilename = directory + "\\y_field.csv";
    exportVectorTableToFile(numberOfTimesFilled, filledFilename);
}

std::vector<Path> FilledPattern::getSequenceOfPaths() {
    return sequenceOfPaths;
}

void FilledPattern::addNewPath(Path &newPath) {
    sequenceOfPaths.push_back(newPath);
}

unsigned int FilledPattern::getNewElement() {

    return distribution(randomEngine);
}


std::vector<std::valarray<int>> FilledPattern::findDualLine(const std::valarray<int> &start) {
    std::vector<std::valarray<int>> pointsInDualLineForward;
    std::vector<std::valarray<int>> pointsInDualLineBackward;

    std::valarray<double> realCoordinates = itodArray(start);
    std::valarray<double> initialStep = perpendicular(desiredPattern.preferredDirection(realCoordinates, 1));
    std::valarray<double> previousStep = initialStep;
    while (desiredPattern.isInShape(realCoordinates)) {
        pointsInDualLineForward.push_back(dtoiArray(realCoordinates));
        std::valarray<double> newStep = perpendicular(desiredPattern.preferredDirection(realCoordinates, 1));
        if (dot(newStep, previousStep) < 0) {
            newStep *= -1;
        }
        realCoordinates += newStep;
        previousStep = newStep;
    }

    realCoordinates = itodArray(start);
    previousStep = -initialStep;
    while (desiredPattern.isInShape(realCoordinates)) {
        pointsInDualLineBackward.push_back(dtoiArray(realCoordinates));
        std::valarray<double> newStep = perpendicular(desiredPattern.preferredDirection(realCoordinates, 1));
        if (dot(newStep, previousStep) < 0) {
            newStep *= -1;
        }
        realCoordinates += newStep;
        previousStep = newStep;
    }


    std::reverse(pointsInDualLineBackward.begin(), pointsInDualLineBackward.end());
    if (!pointsInDualLineBackward.empty()) {
        pointsInDualLineBackward.pop_back();
    }
    std::vector<std::valarray<int>> dualLine = pointsInDualLineBackward;
    dualLine.reserve(pointsInDualLineBackward.size() + pointsInDualLineForward.size());
    dualLine.insert(dualLine.end(), pointsInDualLineForward.begin(), pointsInDualLineForward.end());

    return dualLine;
}


std::vector<std::valarray<int>> FilledPattern::findLineThroughShape() {
    std::vector<std::valarray<int>> listOfStartingPoints;

    int xCoordinate = desiredPattern.dimensions[0] / 2;
    int yCoordinateOfPreviousPoint = 0;
    for (int yCoordinate = 0; yCoordinate < desiredPattern.dimensions[1]; yCoordinate++) {
        std::valarray<int> currentCoordinates = {xCoordinate, yCoordinate};
        if (desiredPattern.isInShape(currentCoordinates) > 0 &&
            yCoordinate - yCoordinateOfPreviousPoint >= config.getPrintRadius()) {

            yCoordinateOfPreviousPoint = yCoordinate;
            listOfStartingPoints.push_back(currentCoordinates);
        }
    }
    return listOfStartingPoints;
}


std::vector<std::valarray<int>> reshuffle(std::vector<std::valarray<int>> initialVector, std::mt19937 randomEngine) {
    std::vector<std::valarray<int>> newVector(initialVector.size());
    std::uniform_int_distribution<unsigned int> distribution(0, initialVector.size() - 1);
    unsigned int elementsToPush = distribution(randomEngine);
    for (int i = 0; i < initialVector.size(); i++) {
        newVector[i] = initialVector[(i + elementsToPush) % initialVector.size()];
    }
    return newVector;
}


std::valarray<double> FilledPattern::getDirector(const std::valarray<int> &positions) {
    return std::valarray<double>({desiredPattern.xFieldPreferred[positions[0]][positions[1]],
                                  desiredPattern.yFieldPreferred[positions[0]][positions[1]]});
}


std::valarray<double> doubleDirector(const std::valarray<double> &director) {
    return normalize(perpendicular(director));
}


std::vector<std::valarray<int>>
FilledPattern::getSpacedLine(const double &distance, const std::vector<std::valarray<int>> &line) {
    std::vector<std::valarray<int>> reshuffledStartingPoints = reshuffle(desiredPattern.perimeterList, randomEngine);

    std::valarray<int> previousPosition = reshuffledStartingPoints[0];
    std::valarray<double> previousDirector = getDirector(previousPosition);
    std::valarray<double> previousDoubleDirector = doubleDirector(previousDirector);

    std::vector<std::valarray<int>> separatedStartingPoints;
    separatedStartingPoints.push_back(previousPosition);

    for (auto &currentPosition: reshuffledStartingPoints) {
        double currentDistance = abs(dot(itodArray(currentPosition - previousPosition), previousDoubleDirector));
        if (currentDistance > distance) {
            previousPosition = currentPosition;
            previousDirector = getDirector(currentPosition);
            previousDoubleDirector = doubleDirector(previousDirector);
            separatedStartingPoints.push_back(previousPosition);
        }
    }
    return separatedStartingPoints;
}

std::vector<std::valarray<int>> FilledPattern::findInitialStartingPoints(FillingMethod method) {
    std::vector<std::valarray<int>> startingPoints;
    std::vector<std::valarray<int>> tempStartingPoints;
    int randomX;
    int randomY;
    std::vector<std::valarray<int>> dualLine;

    switch (method) {
        case RandomPerimeter:
            startingPoints = getSpacedLine(config.getStartingPointSeparation(), desiredPattern.perimeterList);
            isFillingMethodRandom = true;
            break;
        case ConsecutivePerimeter:
            startingPoints = getSpacedLine(config.getStartingPointSeparation(), desiredPattern.perimeterList);
            isFillingMethodRandom = false;
            break;
        case RandomRadial:
            randomX = rand() % desiredPattern.dimensions[0];
            randomY = rand() % desiredPattern.dimensions[1];
            while(!desiredPattern.shapeMatrix[randomX][randomY]) {
                randomX = rand() % desiredPattern.dimensions[0];
                randomY = rand() % desiredPattern.dimensions[1];
            }
            dualLine = findDualLine({randomX, randomY});
            startingPoints = getSpacedLine(config.getStartingPointSeparation(), dualLine);
            isFillingMethodRandom = true;
            break;
        case ConsecutiveRadial:
            randomX = rand() % desiredPattern.dimensions[0];
            randomY = rand() % desiredPattern.dimensions[1];
            while(!desiredPattern.shapeMatrix[randomX][randomY]) {
                randomX = rand() % desiredPattern.dimensions[0];
                randomY = rand() % desiredPattern.dimensions[1];
            }
            dualLine = findDualLine({randomX, randomY});
            startingPoints = getSpacedLine(config.getStartingPointSeparation(), dualLine);
//            startingPoints = reshuffle(findLineThroughShape(), randomEngine);
            isFillingMethodRandom = false;
            break;
    }
    return startingPoints;
}


#pragma clang diagnostic pop