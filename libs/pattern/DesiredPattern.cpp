//
// Created by Michał Zmyślony on 17/09/2021.
//

#include "DesiredPattern.h"
#include "../auxiliary/TableReading.h"
#include "../auxiliary/SimpleMathOperations.h"


DesiredPattern::DesiredPattern(std::string &shapeFilename, std::string &xVectorFieldFilename,
                               std::string &yVectorFieldFilename) :
        shapeMatrix(readFileToTableInt(shapeFilename)),
        xFieldPreferred(readFileToTableDouble(xVectorFieldFilename)),
        yFieldPreferred(readFileToTableDouble(yVectorFieldFilename)),
        dimensions(getTableDimensions(shapeFilename)) {}


std::valarray<int> DesiredPattern::preferredDirection(std::valarray<int> position, int distance) {
    return std::valarray<int>{roundUp(distance * xFieldPreferred[position[0]][position[1]]),
                              roundUp(distance * yFieldPreferred[position[0]][position[1]])};
}


std::valarray<double> DesiredPattern::preferredDirection(std::valarray<double> position, int distance) {
    double xPositionFraction = decimalPart(position[0]);
    double yPositionFraction = decimalPart(position[1]);
    unsigned int xPosition = (int) floor(position[0]);
    unsigned int yPosition = (int) floor(position[1]);
    double xField = distance * (xPositionFraction * yPositionFraction * xFieldPreferred[xPosition][yPosition] +
                                (1 - xPositionFraction) * yPositionFraction *
                                xFieldPreferred[xPosition + 1][yPosition] +
                                (1 - xPositionFraction) * (1 - yPositionFraction) *
                                xFieldPreferred[xPosition + 1][yPosition + 1] +
                                xPositionFraction * (1 - yPositionFraction) *
                                xFieldPreferred[xPosition][yPosition + 1]);

    double yField = distance * (xPositionFraction * yPositionFraction * yFieldPreferred[xPosition][yPosition] +
                                (1 - xPositionFraction) * yPositionFraction *
                                yFieldPreferred[xPosition + 1][yPosition] +
                                (1 - xPositionFraction) * (1 - yPositionFraction) *
                                yFieldPreferred[xPosition + 1][yPosition + 1] +
                                xPositionFraction * (1 - yPositionFraction) *
                                yFieldPreferred[xPosition][yPosition + 1]);

    if (xField == 0 && yField == 0) {
        return std::valarray<double>{xFieldPreferred[xPosition - 1][yPosition],
                                     yFieldPreferred[xPosition - 1][yPosition]};
    }
    return std::valarray<double>{xField, yField};
}