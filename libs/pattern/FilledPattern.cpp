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
#include "../auxiliary/ValarrayConversion.h"
#include "../auxiliary/ValarrayOperations.h"


FilledPattern::FilledPattern(DesiredPattern desiredPattern, int printRadius, int collisionRadius, int stepLength,
                             unsigned int seed) :
        desiredPattern(desiredPattern),
        numberOfTimesFilled(desiredPattern.dimensions[0], std::vector<int>(desiredPattern.dimensions[1])),
        xFieldFilled(desiredPattern.dimensions[0], std::vector<double>(desiredPattern.dimensions[1])),
        yFieldFilled(desiredPattern.dimensions[0], std::vector<double>(desiredPattern.dimensions[1])),
        collisionList(generatePerimeterList(collisionRadius)),
        config(FillingConfig(RandomPerimeter, collisionRadius, stepLength, 1.0, stepLength, printRadius)) {

    randomEngine = std::mt19937(seed);
    pointsInCircle = findPointsInCircle(config.getPrintRadius());
    pointsToFill = findInitialStartingPoints(config.getInitialFillingMethod());
    unsigned int numberOfFillablePoints = pointsToFill.size();
    distribution = std::uniform_int_distribution<unsigned int>(0, numberOfFillablePoints - 1);
}


FilledPattern::FilledPattern(DesiredPattern desiredPattern, FillingConfig config, int seed) :
        desiredPattern(std::move(desiredPattern)),
        config(config) {
    randomEngine = std::mt19937(seed);
}


FilledPattern::FilledPattern(DesiredPattern desiredPattern, int printRadius, int collisionRadius, int stepLength) :
        FilledPattern::FilledPattern(std::move(desiredPattern), printRadius, collisionRadius, stepLength, 0) {}


std::valarray<int> FilledPattern::findFirstPointOnPerimeter() {
    std::vector<std::valarray<int>> perimeterList;
    for (int i = 0; i < desiredPattern.dimensions[0]; i++) {
        for (int j = 0; j < desiredPattern.dimensions[1]; j++) {
            std::valarray<int> currentPos = {i, j};
            if (isOnEdge(desiredPattern.shapeMatrix, currentPos, desiredPattern.dimensions)) {
                return (currentPos);
            }
        }
    }
    return {-1, -1};
}

std::valarray<int> FilledPattern::findNextPointOnPerimeter(std::valarray<int> &currentPoint,
                                                           std::valarray<int> &previousDirection,
                                                           std::vector<std::valarray<int>> &perimeterList) {
    for (auto &displacement: perimeterList) {
        std::valarray<int> newPoint = currentPoint + displacement;
        if (0 <= newPoint[0] && newPoint[0] < desiredPattern.dimensions[0] &&
            0 <= newPoint[1] && newPoint[1] < desiredPattern.dimensions[1]) {
            double direction = dot(previousDirection, displacement);
            if (direction >= 0 && isOnEdge(desiredPattern.shapeMatrix, newPoint, desiredPattern.dimensions)) {
                return newPoint;
            }
        }
    }
    return currentPoint;
}


std::vector<std::valarray<int>> FilledPattern::findPerimeterOfTheShape() {
    std::vector<std::valarray<int>> listOfPerimeters;
    std::valarray<int> firstPointOnPerimeter = findFirstPointOnPerimeter();
    listOfPerimeters.push_back(firstPointOnPerimeter);

    std::valarray<int> previousDirection = {1, 0};
    std::vector<std::valarray<int>> perimeterSlightlyLargerThanPrint = generatePerimeterList(
            2 * config.getPrintRadius());

    std::valarray<int> previousPoint = firstPointOnPerimeter;
    std::valarray<int> currentPoint = findNextPointOnPerimeter(firstPointOnPerimeter, previousDirection,
                                                               perimeterSlightlyLargerThanPrint);


    listOfPerimeters.push_back(currentPoint);
    while (norm(itodArray(firstPointOnPerimeter - currentPoint)) > config.getPrintRadius()) {
        previousDirection = currentPoint - previousPoint;
        previousPoint = currentPoint;
        currentPoint = findNextPointOnPerimeter(currentPoint, previousDirection, perimeterSlightlyLargerThanPrint);
        listOfPerimeters.push_back(currentPoint);
    }
    return listOfPerimeters;
}


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


std::vector<std::valarray<int>> FilledPattern::findLineThroughShape() {
    std::vector<std::valarray<int>> listOfStartingPoints;

    int xCoordinate = desiredPattern.dimensions[0] / 2;
    int yCoordinateOfPreviousPoint = 0;
    for (int yCoordinate = 0; yCoordinate < desiredPattern.dimensions[1]; yCoordinate++) {

        if (desiredPattern.shapeMatrix[xCoordinate][yCoordinate] > 0 &&
            yCoordinate - yCoordinateOfPreviousPoint >= config.getPrintRadius()) {
            std::valarray<int> currentCoordinates = {xCoordinate, yCoordinate};
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


std::vector<std::valarray<int>> FilledPattern::getSpacedPerimeter(const double &distance) {
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
    switch (method) {
        case RandomPerimeter:
//            startingPoints = findPerimeterOfTheShape();
            startingPoints = getSpacedPerimeter(2 * config.getPrintRadius());
            isFillingMethodRandom = true;
            break;
        case ConsecutivePerimeter:
//            startingPoints = reshuffle(findPerimeterOfTheShape(), randomEngine);
            startingPoints = getSpacedPerimeter(2 * config.getPrintRadius());
            isFillingMethodRandom = false;
            break;
        case RandomRadial:
            startingPoints = findLineThroughShape();
            isFillingMethodRandom = true;
            break;
        case ConsecutiveRadial:
            startingPoints = reshuffle(findLineThroughShape(), randomEngine);
            isFillingMethodRandom = false;
            break;
    }
    return startingPoints;
}


#pragma clang diagnostic pop