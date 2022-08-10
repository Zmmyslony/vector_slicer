//
// Created by Michał Zmyślony on 22/09/2021.
//

#include <utility>
#include <vector>
#include <valarray>
#include <algorithm>
#include "ValarrayOperations.h"
#include <cfloat>


bool isOnTheLeftSideOfEdge(std::valarray<int> point, std::valarray<double> edgeFirst,
                           std::valarray<double> edgeSecond) {
    int sign = (int) ((edgeSecond[0] - edgeFirst[0]) * (point[1] - edgeFirst[1]) -
                      (point[0] - edgeFirst[0]) * (edgeSecond[1] - edgeFirst[1]));
    if (sign > 0) {
        return true;
    } else {
        return false;
    }
}


bool isInRectangle(std::valarray<int> &point, std::valarray<double> &edgeFirst,
                   std::valarray<double> &edgeSecond, std::valarray<double> &edgeThird,
                   std::valarray<double> &edgeFourth) {
    bool isOnLeftOfFirstEdge = isOnTheLeftSideOfEdge(point, edgeFirst, edgeSecond);
    bool isOnLeftOfSecondEdge = isOnTheLeftSideOfEdge(point, edgeSecond, edgeThird);
    bool isOnLeftOfThirdEdge = isOnTheLeftSideOfEdge(point, edgeThird, edgeFourth);
    bool isOnLeftOfFourthEdge = isOnTheLeftSideOfEdge(point, edgeFourth, edgeFirst);

    if (isOnLeftOfFirstEdge && isOnLeftOfSecondEdge && isOnLeftOfThirdEdge && isOnLeftOfFourthEdge) {
        return true;
    } else {
        return false;
    }
}


double minValue(const std::vector<double> &values) {
    double minValue = DBL_MAX;
    for (auto &currentValue : values) {
        if (currentValue < minValue) {
            minValue = currentValue;
        }
    }
    return minValue;
}

double maxValue(const std::vector<double> &values) {
    double maxValue = DBL_MIN;
    for (auto &currentValue : values) {
        if (currentValue > maxValue) {
            maxValue = currentValue;
        }
    }
    return maxValue;
}


std::vector<std::valarray<int>> findPointsToFill(const std::valarray<int> &pointFirst,
                                                 const std::valarray<int> &pointSecond, double radius) {
    std::valarray<double> pointFirstDouble = itodArray(pointFirst);
    std::valarray<double> pointSecondDouble = itodArray(pointSecond);

    std::valarray<double> connectingVector = normalize(pointSecondDouble - pointFirstDouble);
    std::valarray<double> perpendicularVector = {connectingVector[1] * radius, -connectingVector[0] * radius};

    std::valarray<double> firstEdge = pointFirstDouble + perpendicularVector - 0.5 * connectingVector;
    std::valarray<double> secondEdge = pointSecondDouble + perpendicularVector;
    std::valarray<double> thirdEdge = pointSecondDouble - perpendicularVector;
    std::valarray<double> fourthEdge = pointFirstDouble - perpendicularVector - 0.5 * connectingVector;


    int xMin = (int)minValue({firstEdge[0], secondEdge[0], thirdEdge[0], fourthEdge[0]});
    int xMax = (int)maxValue({firstEdge[0], secondEdge[0], thirdEdge[0], fourthEdge[0]}) + 1;
    int yMax = (int)maxValue({firstEdge[1], secondEdge[1], thirdEdge[1], fourthEdge[1]}) + 1;
    int yMin = (int)minValue({firstEdge[1], secondEdge[1], thirdEdge[1], fourthEdge[1]});

    std::vector<std::valarray<int>> pointsToFill;

    int yCurr = yMax;
    for (int xCurr = xMin; xCurr <= xMax; xCurr++) {
        std::valarray<int> topPoint = {xCurr, yCurr};
        bool searchingForTopPoint = true;
        while (searchingForTopPoint) {
            if (isInRectangle(topPoint, firstEdge, secondEdge, thirdEdge, fourthEdge) &&
                topPoint[1] >= yCurr && topPoint[1] <= yMax) {
                topPoint[1]++;
            } else if (!isInRectangle(topPoint, firstEdge, secondEdge, thirdEdge, fourthEdge) &&
                       topPoint[1] <= yCurr && topPoint[1] >= yMin) {
                topPoint[1]--;
            } else {
                searchingForTopPoint = false;
            }
        }

        while (isInRectangle(topPoint, firstEdge, secondEdge, thirdEdge, fourthEdge) && topPoint[1] >= 0) {
            pointsToFill.push_back(topPoint);
            topPoint[1]--;
        }
    }
    return pointsToFill;
}


std::vector<std::valarray<int>> findPointsInCircle(double radius) {
    std::vector<std::valarray<int>> pointsInCircle;
    int range = (int)radius + 1;
    for (int i = -range ; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            if (i * i + j * j <= radius * radius) {
                pointsInCircle.push_back({i, j});
            }
        }
    }
    return pointsInCircle;
}


std::vector<std::valarray<int>> findHalfCircle(const std::valarray<int> &lastPoint,
                                               const std::valarray<int> &previousPoint, double radius) {
    std::valarray<double> lastPointDouble = itodArray(lastPoint);
    std::valarray<double> pointSecondDouble = itodArray(previousPoint);

    std::valarray<double> connectingVector = normalize(pointSecondDouble - lastPointDouble);
    std::valarray<double> perpendicularVector = {connectingVector[1] * radius, -connectingVector[0] * radius};

    std::valarray<double> firstCorner = perpendicularVector - 0.5 * connectingVector;
    std::valarray<double> secondCorner = -perpendicularVector - 0.5 * connectingVector;

    std::vector<std::valarray<int>> pointsToFill;

    int intRadius = (int)radius + 1;
    for (int xDisplacement = -intRadius; xDisplacement < intRadius; xDisplacement++) {
        for (int yDisplacement = -intRadius; yDisplacement < intRadius; yDisplacement++) {
            std::valarray<int> displacement = {xDisplacement, yDisplacement};
            double distance = norm(displacement);

            bool isOnCorrectSide = isOnTheLeftSideOfEdge(displacement, firstCorner, secondCorner);

            if (distance <= radius && isOnCorrectSide) {
                pointsToFill.push_back(displacement + lastPoint);
            }
        }
    }
    return pointsToFill;
}


std::vector<std::valarray<int>> findPointsInCircle(int radius) {
    return findPointsInCircle((double)radius);
}