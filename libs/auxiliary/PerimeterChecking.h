//
// Created by Michał Zmyślony on 27/09/2021.
//

#ifndef VECTOR_SLICER_PERIMETERCHECKING_H
#define VECTOR_SLICER_PERIMETERCHECKING_H

#include <vector>
#include <tuple>
#include <valarray>

std::valarray<double> getRepulsion(std::vector<std::vector<int>> &filledTable,
                                   std::vector<std::valarray<int>> &checkedArea,
                                   std::valarray<int> &startPositions,
                                   std::valarray<int> &sizes, double repulsionCoefficient);

bool isPerimeterFree(std::vector<std::vector<int>> &filledTable, std::vector<std::vector<int>> &shapeTable,
                     std::vector<std::valarray<int>> &perimeterList, std::valarray<int> &startPositions,
                     std::valarray<int> &sizes);

bool isOnEdge(const std::vector<std::vector<int>> &shapeTable, const std::valarray<int> &startPositions,
              const std::valarray<int> &sizes);

std::vector<std::valarray<int>>
findAllPerimeters(const std::vector<std::vector<int>> &shapeMatrix, const std::valarray<int> &sizes);

std::vector<std::valarray<int>>
findSortedPerimeters(const std::vector<std::vector<int>> &shapeMatrix, const std::valarray<int> &sizes);

#endif //VECTOR_SLICER_PERIMETERCHECKING_H
