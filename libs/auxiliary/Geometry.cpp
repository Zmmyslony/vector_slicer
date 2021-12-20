//
// Created by Michał Zmyślony on 22/09/2021.
//

#include <utility>
#include <vector>
#include <valarray>
#include <algorithm>
#include "ValarrayOperations.h"


bool isOnTheLeftSideOfEdge(std::valarray<int> point, std::valarray<double> EdgeFirst,
                           std::valarray<double> EdgeSecond) {
    int sign = (int) ((EdgeSecond[0] - EdgeFirst[0]) * (point[1] - EdgeFirst[1]) -
                      (point[0] - EdgeFirst[0]) * (EdgeSecond[1] - EdgeFirst[1]));
    if (sign > 0) {
        return true;
    } else {
        return false;
    }
}

bool isInRectangle(std::valarray<int> &point, std::valarray<double> &EdgeFirst,
                   std::valarray<double> &EdgeSecond, std::valarray<double> &EdgeThird,
                   std::valarray<double> &EdgeFourth) {
    bool isOnLeftOfFirstEdge = isOnTheLeftSideOfEdge(point, EdgeFirst, EdgeSecond);
    bool isOnLeftOfSecondEdge = isOnTheLeftSideOfEdge(point, EdgeSecond, EdgeThird);
    bool isOnLeftOfThirdEdge = isOnTheLeftSideOfEdge(point, EdgeThird, EdgeFourth);
    bool isOnLeftOfFourthEdge = isOnTheLeftSideOfEdge(point, EdgeFourth, EdgeFirst);

    if (isOnLeftOfFirstEdge && isOnLeftOfSecondEdge && isOnLeftOfThirdEdge && isOnLeftOfFourthEdge) {
        return true;
    } else {
        return false;
    }
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

    int xMin = (int)min({firstEdge[0], secondEdge[0], thirdEdge[0], fourthEdge[0]}, std::less<>());
    int xMax = (int)max({firstEdge[0], secondEdge[0], thirdEdge[0], fourthEdge[0]}, std::less<>()) + 1;
    int yMax = (int)max({firstEdge[1], secondEdge[1], thirdEdge[1], fourthEdge[1]}, std::less<>()) + 1;
    int yMin = (int)min({firstEdge[1], secondEdge[1], thirdEdge[1], fourthEdge[1]}, std::less<>());

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


//std::vector<std::valarray<int>> findPointsToFill(std::valarray<int> point, int radius) {
//    std::vector<std::valarray<int>> pointsToFill;
//    for (int i = -radius; i <= radius; i++) {
//        for (int j = -radius; j <= radius; j++) {
//            if (i * i + j * j <= radius * radius) {
//                pointsToFill.push_back({point[0] + i, point[1] + j});
//            }
//        }
//    }
//    return pointsToFill;
//}

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

std::vector<std::valarray<int>> findPointsInCircle(int radius) {
    return findPointsInCircle((double) radius);
}