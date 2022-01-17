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
#include "../auxiliary/Perimeter.h"
#include "../auxiliary/SimpleMathOperations.h"
#include "../auxiliary/ValarrayOperations.h"


FilledPattern::FilledPattern(const DesiredPattern &newDesiredPattern, FillingConfig newConfig) :
        desiredPattern(newDesiredPattern),
        config(newConfig) {
    numberOfTimesFilled = std::vector<std::vector<int>>(newDesiredPattern.getDimensions()[0],
                                                        std::vector<int>(newDesiredPattern.getDimensions()[1]));
    xFieldFilled = std::vector<std::vector<double>>(newDesiredPattern.getDimensions()[0],
                                                    std::vector<double>(newDesiredPattern.getDimensions()[1]));
    yFieldFilled = std::vector<std::vector<double>>(newDesiredPattern.getDimensions()[0],
                                                    std::vector<double>(newDesiredPattern.getDimensions()[1]));
    collisionList = generatePerimeterList(config.getCollisionRadius());
    randomEngine = std::mt19937(config.getSeed());
    pointsInCircle = findPointsInCircle(config.getPrintRadius());
    pointsToFill = findInitialStartingPoints(config.getInitialFillingMethod());
    unsigned int numberOfFillablePoints = pointsToFill.size();
    distribution = std::uniform_int_distribution<unsigned int>(0, numberOfFillablePoints - 1);
}


FilledPattern::FilledPattern(const DesiredPattern &desiredPattern, int printRadius, int collisionRadius, int stepLength,
                             unsigned int seed) :
        FilledPattern(desiredPattern,
                      FillingConfig(RandomPerimeter, collisionRadius, 2 * printRadius, 1.0, stepLength, printRadius,
                                    seed)) {
}


FilledPattern::FilledPattern(const DesiredPattern &desiredPattern, int printRadius, int collisionRadius, int stepLength)
        :
        FilledPattern::FilledPattern(desiredPattern, printRadius, collisionRadius, stepLength, 0) {}




std::vector<std::valarray<int>> FilledPattern::findAllFillablePoints() {
    std::vector<std::valarray<int>> newPointsToFill;
    for (int i = 0; i < desiredPattern.getDimensions()[0]; i++) {
        for (int j = 0; j < desiredPattern.getDimensions()[1]; j++) {
            std::valarray<int> currentPos = {i, j};
            if (isPerimeterFree(numberOfTimesFilled, desiredPattern.getShapeMatrix(), collisionList,
                                currentPos, desiredPattern.getDimensions())) {
                newPointsToFill.push_back(currentPos);
            }
        }
    }
    return newPointsToFill;
}


std::vector<std::valarray<int>>
FilledPattern::findRemainingFillablePointsInList(std::vector<std::valarray<int>> &listOfPoints) const {
    std::vector<std::valarray<int>> fillablePointsList;
    for (auto &point: listOfPoints) {
        if (isPerimeterFree(numberOfTimesFilled, desiredPattern.getShapeMatrix(), collisionList,
                            point, desiredPattern.getDimensions())) {
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
        if (point[0] >= 0 && point[0] < desiredPattern.getDimensions()[0] && point[1] >= 0 &&
            point[1] < desiredPattern.getDimensions()[1]) {
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
    std::valarray<double> repulsion = {0, 0};
    if (config.getRepulsion() != 0) {
        repulsion = getRepulsion(numberOfTimesFilled, pointsInCircle, newCoordinates,
                                 desiredPattern.getDimensions(), config.getRepulsion());
    }

    newPositions -= repulsion;
    newCoordinates = dtoiArray(newPositions);
    std::valarray<double> realStep = newStep - repulsion;

    if (newCoordinates[0] == currentCoordinates[0] && newCoordinates[1] == currentCoordinates[1] ||
        dot(realStep, previousStep) <= 0 || norm(realStep) <= 2) {
        return false;
    }

    if (isPerimeterFree(numberOfTimesFilled, desiredPattern.getShapeMatrix(), collisionList, newCoordinates,
                        desiredPattern.getDimensions())) {
        std::vector<std::valarray<int>> currentPointsToFill = findPointsToFill(currentCoordinates, newCoordinates,
                                                                               config.getPrintRadius());
        std::valarray<int> newStepInt = newCoordinates - currentCoordinates;
        fillPointsFromList(currentPointsToFill, newStepInt);
        currentPath.addPoint(newCoordinates);

        newStep = getNewStep(newPositions, length, newStep);
        positions = newPositions;
        previousStep = newStep;
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
        while (tryGeneratingPathWithLength(newPath, currentPositions, currentStep, length)) { }
    }
    return newPath;
}

void FilledPattern::fillPointsInCircle(const std::valarray<int> &startingCoordinates) {
    fillPointsFromDisplacement(startingCoordinates, pointsInCircle, {1, 0});
}

void FilledPattern::fillPointsInHalfCircle(const std::valarray<int> &lastPoint, const std::valarray<int> &previousPoint) {
    std::vector<std::valarray<int>> halfCirclePoints = findHalfCircle(lastPoint, previousPoint, config.getPrintRadius());
    fillPointsFromList(halfCirclePoints, previousPoint - lastPoint);
}

void FilledPattern::exportToDirectory(const std::string &directory) const {
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


std::vector<std::valarray<int>> reshuffle(const std::vector<std::valarray<int>> &initialVector, std::mt19937 &randomEngine) {
    std::vector<std::valarray<int>> newVector(initialVector.size());
    std::uniform_int_distribution<unsigned int> distribution(0, initialVector.size() - 1);
    unsigned int elementsToPush = distribution(randomEngine);
    for (int i = 0; i < initialVector.size(); i++) {
        newVector[i] = initialVector[(i + elementsToPush) % initialVector.size()];
    }
    return newVector;
}


std::valarray<double> FilledPattern::getDirector(const std::valarray<int> &positions) {
    return std::valarray<double>({desiredPattern.getXFieldPreferred()[positions[0]][positions[1]],
                                  desiredPattern.getYFieldPreferred()[positions[0]][positions[1]]});
}


std::valarray<double> FilledPattern::getDirector(const std::valarray<double> &positions) {
    int xBase = (int) positions[0];
    int yBase = (int) positions[1];
    double xFraction = positions[0] - floor(positions[0]);
    double yFraction = positions[1] - floor(positions[1]);

    std::valarray<double> director = {0, 0};
    director += xFraction * yFraction * getDirector(std::valarray<int>{xBase, yBase});
    director += (1 - xFraction) * yFraction * getDirector(std::valarray<int>{xBase + 1, yBase});
    director += (1 - xFraction) * (1 - yFraction) * getDirector(std::valarray<int>{xBase + 1, yBase + 1});
    director += xFraction * (1 - yFraction) * getDirector(std::valarray<int>{xBase, yBase + 1});

    return director;
}


std::valarray<double> normalizedDualVector(const std::valarray<double> &vector) {
    return normalize(perpendicular(vector));
}


std::vector<std::valarray<int>>
FilledPattern::findDualLineOneDirection(std::valarray<double> coordinates, std::valarray<double> previousDualDirector) {
    std::vector<std::valarray<int>> line;
    while (desiredPattern.isInShape(coordinates)) {
        line.push_back(dtoiArray(coordinates));
        std::valarray<double> director = getDirector(coordinates);
        std::valarray<double> dualDirector = normalizedDualVector(director);
        if (dot(dualDirector, previousDualDirector) < 0) {
            dualDirector *= -1;
        }
        coordinates += dualDirector;
        previousDualDirector = dualDirector;
    }
    return line;
}

std::vector<std::valarray<int>>
stitchTwoVectors(std::vector<std::valarray<int>> backwardsVector, std::vector<std::valarray<int>> forwardsVector) {
    std::reverse(backwardsVector.begin(), backwardsVector.end());
    if (!backwardsVector.empty()) {
        backwardsVector.pop_back();
    }
    std::vector<std::valarray<int>> dualLine = backwardsVector;
    dualLine.reserve(backwardsVector.size() + forwardsVector.size());
    dualLine.insert(dualLine.end(), forwardsVector.begin(), forwardsVector.end());
    return dualLine;
}


std::vector<std::valarray<int>> FilledPattern::findDualLine(const std::valarray<int> &start) {
    std::valarray<double> realCoordinates = itodArray(start);
    std::valarray<double> previousDirector = getDirector(realCoordinates);
    std::valarray<double> initialDualDirector = normalizedDualVector(previousDirector);

    std::vector<std::valarray<int>> pointsInDualLineForward;
    std::vector<std::valarray<int>> pointsInDualLineBackward;

    pointsInDualLineForward = findDualLineOneDirection(realCoordinates, initialDualDirector);
    pointsInDualLineBackward = findDualLineOneDirection(realCoordinates, -initialDualDirector);

    return stitchTwoVectors(pointsInDualLineBackward, pointsInDualLineForward);
}


std::vector<std::valarray<int>>
FilledPattern::getSpacedLine(const double &distance, const std::vector<std::valarray<int>> &line) {
    std::vector<std::valarray<int>> reshuffledStartingPoints = reshuffle(line, randomEngine);

    std::valarray<int> previousPosition = reshuffledStartingPoints[0];
    std::valarray<double> previousDirector = getDirector(previousPosition);
    std::valarray<double> previousDoubleDirector = normalizedDualVector(previousDirector);

    std::vector<std::valarray<int>> separatedStartingPoints;
    separatedStartingPoints.push_back(previousPosition);

    for (auto &currentPosition: reshuffledStartingPoints) {
        double currentDistance = abs(dot(itodArray(currentPosition - previousPosition), previousDoubleDirector));
        if (currentDistance > distance) {
            previousPosition = currentPosition;
            previousDirector = getDirector(currentPosition);
            previousDoubleDirector = normalizedDualVector(previousDirector);
            separatedStartingPoints.push_back(previousPosition);
        }
    }
    return separatedStartingPoints;
}


std::valarray<int> FilledPattern::findPointInShape() {
    std::uniform_int_distribution<int> xDistribution(0, desiredPattern.getDimensions()[0] - 1);
    std::uniform_int_distribution<int> yDistribution(0, desiredPattern.getDimensions()[1] - 1);

    int xStart = xDistribution(randomEngine);
    int yStart = yDistribution(randomEngine);
    while (!desiredPattern.getShapeMatrix()[xStart][yStart]) {
        xStart = xDistribution(randomEngine);
        yStart = yDistribution(randomEngine);
    }
    return std::valarray<int>{xStart, yStart};
}


std::vector<std::valarray<int>> FilledPattern::findInitialStartingPoints(FillingMethod method) {
    std::vector<std::valarray<int>> startingPoints;
    std::vector<std::valarray<int>> tempStartingPoints;
    std::vector<std::valarray<int>> dualLine;

    switch (method) {
        case RandomPerimeter:
            startingPoints = getSpacedLine(config.getStartingPointSeparation(), desiredPattern.getPerimeterList());
            isFillingMethodRandom = true;
            break;
        case ConsecutivePerimeter:
            startingPoints = getSpacedLine(config.getStartingPointSeparation(), desiredPattern.getPerimeterList());
            isFillingMethodRandom = false;
            break;
        case RandomRadial:
            dualLine = findDualLine(findPointInShape());
            startingPoints = getSpacedLine(config.getStartingPointSeparation(), dualLine);
            isFillingMethodRandom = true;
            break;
        case ConsecutiveRadial:
            dualLine = findDualLine(findPointInShape());
            startingPoints = getSpacedLine(config.getStartingPointSeparation(), dualLine);
            isFillingMethodRandom = false;
            break;
    }
    return startingPoints;
}


#pragma clang diagnostic pop