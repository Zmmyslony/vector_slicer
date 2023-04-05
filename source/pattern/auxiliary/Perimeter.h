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
// You should have received a copy of the GNU General Public License along with Vector Slicer. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 27/09/2021.
//

#ifndef VECTOR_SLICER_PERIMETER_H
#define VECTOR_SLICER_PERIMETER_H

#include <vector>
#include <tuple>
#include <valarray>

using vald = std::valarray<double>;
using vali = std::valarray<int>;

std::vector<vali> generatePerimeterList(double radius);

vald getRepulsionValue(const std::vector<std::vector<int>> &filled_table,
                       const std::vector<vali> &checked_area,
                       const vali &start_positions,
                       const vali &sizes, double repulsion_coefficient);

vald
getRepulsionValue(const std::vector<std::vector<int>> &empty_spots, const std::vector<std::vector<int>> &filled_table,
                  const std::vector<vali> &checked_area, const vald &coordinates, const vali &sizes,
                  double repulsion_coefficient);

bool
isPerimeterFree(const std::vector<std::vector<int>> &filled_table, const std::vector<std::vector<int>> &shape_table,
                const std::vector<vali> &perimeter_displacements_list, const vali &coordinates,
                const vali &sizes);

bool isOnEdge(const std::vector<std::vector<int>> &shape_table, const vali &coordinates,
              const vali &sizes);

std::vector<vali>
findAllPerimeters(const std::vector<std::vector<int>> &shape_matrix, const vali &sizes);

std::vector<vali>
findSortedPerimeters(const std::vector<std::vector<int>> &shape_matrix, const vali &sizes);

#endif //VECTOR_SLICER_PERIMETER_H
