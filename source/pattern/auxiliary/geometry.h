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
#include <cstdint>
#include <algorithm>

#include "../coord.h"
#include "vector_operations.h"

using vecd = std::vector<double>;
using veci = std::vector<int>;

std::vector<coord> findPointsToFill(coord_d corner_first, coord_d corner_second, coord_d corner_third,
                                   coord_d corner_fourth, bool is_exclusive);

coord_d normalisedResultant(const coord_d &primary_vector, const coord_d &secondary_vector);

std::vector<coord> findPointsInDisk(double radius);

bool isLeftOfEdge(const coord &point, const coord_d &edge_point_first, const coord_d &edge_point_second, bool is_exclusive);

bool isLeftOfEdge(const coord_d &point, const vecd &edge_point_first, const vecd &edge_point_second, bool is_exclusive);

std::vector<coord>
findHalfCircleCentres(const coord &last_point, const coord &previous_point, double radius, bool is_last_point_filled,
                      const coord_d &last_director);

/// Finds coordinates in half-circle based on position of the centre-point and two edges of previous path segment.
std::vector<coord>
findHalfCircleEdges(const coord_d &centre_position, coord_d corner_one, coord_d corner_two, double radius,
                    bool is_last_point_filled, const coord_d &last_move_direction);

std::vector<coord> generateLineDisplacements(const coord_d &tangent, double radius);

#endif //VECTOR_SLICER_GEOMETRY_H
