//
// Created by Michał Zmyślony on 22/09/2021.
//

#include <utility>
#include <vector>
#include <valarray>
#include <algorithm>
#include <iostream>


bool isOnTheLeftSideOfEdge(std::valarray<int> point, std::valarray<int> EdgeFirst,
                             std::valarray<int> EdgeSecond) {
    int sign = (int)((EdgeSecond[0] - EdgeFirst[0]) * (point[1] - EdgeFirst[1]) -
                     (point[0] - EdgeFirst[0]) * (EdgeSecond[1] - EdgeFirst[1]));
    if (sign > 0) {
        return true;
    }
    else {
        return false;
    }
}

bool isInRectangle(std::valarray<int>& point, std::valarray<int>& EdgeFirst,
                   std::valarray<int>& EdgeSecond, std::valarray<int>& EdgeThird,
                   std::valarray<int>& EdgeFourth) {
    bool isOnLeftOfFirstEdge = isOnTheLeftSideOfEdge(point,EdgeFirst,EdgeSecond);
    bool isOnLeftOfSecondEdge = isOnTheLeftSideOfEdge(point,EdgeSecond,EdgeThird);
    bool isOnLeftOfThirdEdge = isOnTheLeftSideOfEdge(point,EdgeThird,EdgeFourth);
    bool isOnLeftOfFourthEdge = isOnTheLeftSideOfEdge(point,EdgeFourth,EdgeFirst);

    if (isOnLeftOfFirstEdge && isOnLeftOfSecondEdge && isOnLeftOfThirdEdge && isOnLeftOfFourthEdge) {
//        printf("Current filled point coordinates: %i, %i\n", point[0], point[1]);
        return true;
    }
    else {
        return false;
    }
}

std::vector<std::valarray<int>> findPointsToFill(std::valarray<int> pointFirst,
                                                 std::valarray<int> pointSecond, int radius) {
    std::valarray<int> connectingVector = pointSecond - pointFirst;
    double norm = sqrt(connectingVector[0] * connectingVector[0] + connectingVector[1] * connectingVector[1]);
    std::valarray<int> perpendicularVector = {(int)trunc(connectingVector[1] / norm * (radius + 1)),
                                               (int)trunc(-connectingVector[0] / norm * (radius + 1))};

    std::valarray<int> firstEdge = pointFirst + perpendicularVector;
    std::valarray<int> secondEdge = pointSecond + perpendicularVector;
    std::valarray<int> thirdEdge = pointSecond - perpendicularVector;
    std::valarray<int> fourthEdge = pointFirst - perpendicularVector;

    int xMin = min({firstEdge[0], secondEdge[0], thirdEdge[0], fourthEdge[0]}, std::less<int>());
    int xMax = max({firstEdge[0], secondEdge[0], thirdEdge[0], fourthEdge[0]}, std::less<int>());
    int yMax = max({firstEdge[1], secondEdge[1], thirdEdge[1], fourthEdge[1]}, std::less<int>());
    int yMin = min({firstEdge[1], secondEdge[1], thirdEdge[1], fourthEdge[1]}, std::less<int>());

    std::vector<std::valarray<int>> pointsToFill;

    int yCurr = yMax;
    for (int xCurr = xMin; xCurr <= xMax; xCurr++) {
        std::valarray<int> topPoint = {xCurr, yCurr};
        bool searchingForTopPoint = true;
        while (searchingForTopPoint) {
            if (isInRectangle(topPoint, firstEdge, secondEdge, thirdEdge, fourthEdge) &&
                topPoint[1] >= yCurr && topPoint[1] <= yMax) {
                topPoint[1]++;
            }
            else if (!isInRectangle(topPoint, firstEdge, secondEdge, thirdEdge, fourthEdge) &&
                     topPoint[1] <= yCurr && topPoint[1] >= yMin) {
                topPoint[1]--;
            }
            else {
                searchingForTopPoint = false;
            }
        }

        while(isInRectangle(topPoint, firstEdge, secondEdge, thirdEdge, fourthEdge) && topPoint[1] >= 0) {
            pointsToFill.push_back(topPoint);
            topPoint[1]--;
        }
    }
    return pointsToFill;
}


std::vector<std::valarray<int>> findPointsToFill(std::valarray<int> point, int radius) {
    std::vector<std::valarray<int>> pointsToFill;
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++){
            if (i * i + j * j <= radius * radius){
                pointsToFill.push_back({point[0] + i, point[1] + j});
            }
        }
    }
    return pointsToFill;
}

std::vector<std::valarray<int>> findPointsInCircle(int radius) {
    std::vector<std::valarray<int>> pointsInCircle;
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++){
            if (i * i + j * j <= radius * radius){
                pointsInCircle.push_back({i, j});
            }
        }
    }
    return pointsInCircle;
}