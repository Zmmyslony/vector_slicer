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

#ifndef VECTOR_SLICER_GEOMETRY_H
#define VECTOR_SLICER_GEOMETRY_H

#include <utility>
#include <vector>
#include <valarray>
#include <algorithm>

using vald = std::valarray<double>;
using vali = std::valarray<int>;

std::vector<vali> findPointsToFill(const vali &point_current, const vali &point_next, double radius, bool is_first_point_filled);

std::vector<vali>
findPointsToFill(const vali &point_previous, const vali &point_current, const vali &point_next, double radius,
                 bool is_first_point_filled);

std::vector<vali> findPointsInCircle(double radius);

std::vector<vali> findPointsInCircle(int radius);

std::vector<vali>
findHalfCircle(const vali &last_point, const vali &previous_point, double radius, bool is_last_point_filled);

#endif //VECTOR_SLICER_GEOMETRY_H
