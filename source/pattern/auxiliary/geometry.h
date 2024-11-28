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
#include <algorithm>

using vecd = std::vector<double>;
using veci = std::vector<int>;

std::vector<veci> findPointsToFill(vecd corner_first, vecd corner_second, vecd corner_third,
                                   vecd corner_fourth, bool is_exclusive);

vecd normalisedResultant(const vecd &primary_vector, const vecd &secondary_vector);

std::vector<veci> findPointsInCircle(double radius);

std::vector<veci> findPointsInCircle(int radius);

bool isLeftOfEdge(const vecd &point, const vecd &edge_point_first, const vecd &edge_point_second, bool is_exclusive);

std::vector<veci>
findHalfCircleCentres(const veci &last_point, const veci &previous_point, double radius, bool is_last_point_filled,
                      const vecd &last_director);

/// Finds coordinates in half-circle based on position of the centre-point and two edges of previous path segment.
std::vector<veci>
findHalfCircleEdges(const veci &centre_position, vecd corner_one, vecd corner_two, double radius,
                    bool is_last_point_filled, const vecd &last_move_direction);

std::vector<veci> generateLineDisplacements(const vecd &tangent, double radius);

#endif //VECTOR_SLICER_GEOMETRY_H
