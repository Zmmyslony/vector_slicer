//
// Created by Michał Zmyślony on 17/09/2021.
//

#include "DesiredPattern.h"
#include "../auxiliary/TableReading.h"
#include "../auxiliary/SimpleMathOperations.h"
#include "../auxiliary/Perimeter.h"
#include "../auxiliary/ValarrayOperations.h"

DesiredPattern::DesiredPattern(std::string &shapeFilename, std::string &xVectorFieldFilename,
                               std::string &yVectorFieldFilename) :
        shapeMatrix(readFileToTableInt(shapeFilename)),
        xFieldPreferred(readFileToTableDouble(xVectorFieldFilename)),
        yFieldPreferred(readFileToTableDouble(yVectorFieldFilename)),
        dimensions(getTableDimensions(shapeFilename)) {
    perimeterList = findSortedPerimeters(shapeMatrix, dimensions);
}


std::valarray<int> DesiredPattern::preferredDirection(const std::valarray<int> &position, int distance) const {
    return std::valarray<int>{roundUp(distance * xFieldPreferred[position[0]][position[1]]),
                              roundUp(distance * yFieldPreferred[position[0]][position[1]])};
}


std::valarray<double> DesiredPattern::preferredDirection(const std::valarray<double> &position, int distance) const {
    double xPositionFraction = decimalPart(position[0]);
    double yPositionFraction = decimalPart(position[1]);
    unsigned int xPosition = (int) floor(position[0]);
    unsigned int yPosition = (int) floor(position[1]);
    double xField = (xPositionFraction * yPositionFraction * xFieldPreferred[xPosition][yPosition] +
                     (1 - xPositionFraction) * yPositionFraction * xFieldPreferred[xPosition + 1][yPosition] +
                     (1 - xPositionFraction) * (1 - yPositionFraction) * xFieldPreferred[xPosition + 1][yPosition + 1] +
                     xPositionFraction * (1 - yPositionFraction) * xFieldPreferred[xPosition][yPosition + 1]);

    double yField = (xPositionFraction * yPositionFraction * yFieldPreferred[xPosition][yPosition] +
                     (1 - xPositionFraction) * yPositionFraction * yFieldPreferred[xPosition + 1][yPosition] +
                     (1 - xPositionFraction) * (1 - yPositionFraction) * yFieldPreferred[xPosition + 1][yPosition + 1] +
                     xPositionFraction * (1 - yPositionFraction) * yFieldPreferred[xPosition][yPosition + 1]);

    if (xField == 0 && yField == 0) {
        return std::valarray<double>{xFieldPreferred[xPosition - 1][yPosition],
                                     yFieldPreferred[xPosition - 1][yPosition]};
    }
    std::valarray<double> newStep = {xField, yField};
    newStep = distance * normalize(newStep);
    return newStep;
}


bool DesiredPattern::isInShape(const std::valarray<int> &position) const {
    return shapeMatrix[position[0]][position[1]];
}


bool DesiredPattern::isInShape(const std::valarray<double> &position) const {
    return isInShape(dtoiArray(position));
}

const std::valarray<int> &DesiredPattern::getDimensions() const {
    return dimensions;
}

const std::vector<std::valarray<int>> &DesiredPattern::getPerimeterList() const {
    return perimeterList;
}

const std::vector<std::vector<int>> &DesiredPattern::getShapeMatrix() const {
    return shapeMatrix;
}

const std::vector<std::vector<double>> &DesiredPattern::getXFieldPreferred() const {
    return xFieldPreferred;
}

const std::vector<std::vector<double>> &DesiredPattern::getYFieldPreferred() const {
    return yFieldPreferred;
}
