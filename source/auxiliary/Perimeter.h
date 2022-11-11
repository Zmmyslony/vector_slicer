// 2022, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 27/09/2021.
//

#ifndef VECTOR_SLICER_PERIMETER_H
#define VECTOR_SLICER_PERIMETER_H

#include <vector>
#include <tuple>
#include <valarray>

std::vector<std::valarray<int>> generatePerimeterList(double radius);

std::valarray<double> getRepulsion(std::vector<std::vector<int>> &filledTable,
                                   std::vector<std::valarray<int>> &checkedArea,
                                   const std::valarray<int> &startPositions,
                                   const std::valarray<int> &sizes, double repulsionCoefficient);

bool isPerimeterFree(const std::vector<std::vector<int>> &filledTable, const std::vector<std::vector<int>> &shapeTable,
                     const std::vector<std::valarray<int>> &perimeterList, const std::valarray<int> &startPositions,
                     const std::valarray<int> &sizes);

bool isOnEdge(const std::vector<std::vector<int>> &shapeTable, const std::valarray<int> &startPositions,
              const std::valarray<int> &sizes);

std::vector<std::valarray<int>>
findAllPerimeters(const std::vector<std::vector<int>> &shapeMatrix, const std::valarray<int> &sizes);

std::vector<std::valarray<int>>
findSortedPerimeters(const std::vector<std::vector<int>> &shapeMatrix, const std::valarray<int> &sizes);

#endif //VECTOR_SLICER_PERIMETER_H
