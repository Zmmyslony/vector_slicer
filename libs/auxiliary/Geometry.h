//
// Created by Michał Zmyślony on 27/09/2021.
//

#ifndef VECTOR_SLICER_GEOMETRY_H
#define VECTOR_SLICER_GEOMETRY_H

#include <utility>
#include <vector>
#include <valarray>
#include <algorithm>

bool isOnTheLeftSideOfCorner(std::valarray<int> point, std::valarray<int> cornerFirst,
                             std::valarray<int> cornerSecond);

bool isInRectangle(std::valarray<int>& point, std::valarray<int>& cornerFirst,
                   std::valarray<int>& cornerSecond, std::valarray<int>& cornerThird,
                   std::valarray<int>& cornerFourth);

std::vector<std::valarray<int>> findPointsToFill(std::valarray<int> pointFirst,
                                                 std::valarray<int> pointSecond, int radius);

std::vector<std::valarray<int>> findPointsToFill(std::valarray<int> point, int radius);
std::vector<std::valarray<int>> findPointsInCircle(int radius);


#endif //VECTOR_SLICER_GEOMETRY_H
