// Copyright (c) 2021-2023, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Vector Slicer.
// If not, see <https://www.gnu.org/licenses/>.

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

std::vector<vali> findPointsToFill(vald corner_first, vald corner_second, vald corner_third,
                                   vald corner_fourth, bool is_exclusive);

vald normalisedResultant(const vald &primary_vector, const vald &secondary_vector);

std::vector<vali> findPointsInCircle(double radius);

std::vector<vali> findPointsInCircle(int radius);

bool isLeftOfEdge(const vald &point, const vald &edge_point_first, const vald &edge_point_second, bool is_exclusive);

std::vector<vali>
findHalfCircleCentres(const vali &last_point, const vali &previous_point, double radius, bool is_last_point_filled,
                      const vald &last_director);

/// Finds coordinates in half-circle based on position of the centre-point and two edges of previous path segment.
std::vector<vali>
findHalfCircleEdges(const vali &centre_position, vald corner_one, vald corner_two, double radius,
                    bool is_last_point_filled, const vald &last_move_direction);

std::vector<vali> generateLineDisplacements(const vald &tangent, double radius);

#endif //VECTOR_SLICER_GEOMETRY_H
