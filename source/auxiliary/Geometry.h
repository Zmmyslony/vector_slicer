//
// Created by Michał Zmyślony on 27/09/2021.
//

#ifndef VECTOR_SLICER_GEOMETRY_H
#define VECTOR_SLICER_GEOMETRY_H

#include <utility>
#include <vector>
#include <valarray>
#include <algorithm>

std::vector<std::valarray<int>> findPointsToFill(const std::valarray<int> &pointFirst,
                                                 const std::valarray<int> &pointSecond, double radius);

std::vector<std::valarray<int>> findPointsInCircle(double radius);

std::vector<std::valarray<int>> findPointsInCircle(int radius);

std::vector<std::valarray<int>> findHalfCircle(const std::valarray<int> &lastPoint,
                                               const std::valarray<int> &previousPoint, double radius);

#endif //VECTOR_SLICER_GEOMETRY_H
