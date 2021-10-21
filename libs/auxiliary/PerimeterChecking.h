//
// Created by Michał Zmyślony on 27/09/2021.
//

#ifndef VECTOR_SLICER_PERIMETERCHECKING_H
#define VECTOR_SLICER_PERIMETERCHECKING_H

#include <vector>
#include <tuple>
#include <valarray>

std::valarray<double> getRepulsion(std::vector<std::vector<int>>& filledTable,
                                   std::vector<std::valarray<int>>& checkedArea,
                                   std::valarray<int>& startPositions,
                                   std::valarray<int>& sizes, double repulsionCoefficient);

bool isPerimeterFree(std::vector<std::vector<int>>& filledTable, std::vector<std::vector<int>>& shapeTable,
                     std::vector<std::valarray<int>>& perimeterList, std::valarray<int>& startPositions,
                     std::valarray<int>& sizes);

#endif //VECTOR_SLICER_PERIMETERCHECKING_H
