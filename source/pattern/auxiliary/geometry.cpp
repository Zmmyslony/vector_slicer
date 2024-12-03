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
// Created by Michał Zmyślony on 22/09/2021.
//

#include <vector>
#include <algorithm>
#include <iostream>

#include "geometry.h"
#include "valarray_operations.h"



bool
isLeftOfEdge(const coord &point, const coord_d &edge_point_first, const coord_d &edge_point_second, bool is_exclusive) {
    double cross_product =
            (edge_point_second.x - edge_point_first.x) * (point.y - edge_point_first.y) -
            (edge_point_second.y - edge_point_first.y) * (point.x - edge_point_first.x);

    if (is_exclusive) {
        return cross_product > 0;
    } else {
        return cross_product >= 0;
    }
}

bool isLeftOfEdge(const coord_d &point, const coord_d &edge_point_first, const coord_d &edge_point_second,
                  bool is_exclusive) {
    double cross_product =
            (edge_point_second.x - edge_point_first.x) * (point.y - edge_point_first.y) -
            (edge_point_second.y - edge_point_first.y) * (point.x - edge_point_first.x);

    if (is_exclusive) {
        return cross_product > 0;
    } else {
        return cross_product >= 0;
    }
}

bool isInRectangle(const coord_d &point, const coord_d &corner_first, const coord_d &corner_second,
                   const coord_d &corner_third,
                   const coord_d &corner_fourth, bool is_exclusive) {

    return (isLeftOfEdge(point, corner_first, corner_second, is_exclusive) &&
            isLeftOfEdge(point, corner_second, corner_third, false) &&
            isLeftOfEdge(point, corner_third, corner_fourth, false) &&
            isLeftOfEdge(point, corner_fourth, corner_first, false));
}

bool isInRectangle(const coord &point, const coord_d &corner_first, const coord_d &corner_second,
                   const coord_d &corner_third,
                   const coord_d &corner_fourth, bool is_exclusive) {

    return (isLeftOfEdge(point, corner_first, corner_second, is_exclusive) &&
            isLeftOfEdge(point, corner_second, corner_third, false) &&
            isLeftOfEdge(point, corner_third, corner_fourth, false) &&
            isLeftOfEdge(point, corner_fourth, corner_first, false));
}


double minValue(const std::vector<double> &values) {
    return *std::min_element(values.begin(), values.end());
}

double maxValue(const std::vector<double> &values) {
    return *std::max_element(values.begin(), values.end());
}


std::vector<coord> findPointsToFill(coord_d corner_first, coord_d corner_second, coord_d corner_third,
                                    coord_d corner_fourth, bool is_exclusive) {
    std::vector<double> x_coordinates = {corner_first.x, corner_second.x, corner_third.x,
                                         corner_fourth.x};
    std::vector<double> y_coordinates = {corner_first.y, corner_second.y, corner_third.y,
                                         corner_fourth.y};
    int x_min = (int) minValue(x_coordinates);
    int x_max = (int) maxValue(x_coordinates) + 1;
    int y_min = (int) minValue(y_coordinates);
    int y_max = (int) maxValue(y_coordinates) + 1;

    coord_d centre_point = (corner_first + corner_second + corner_third + corner_fourth) / 4;

    // Error 1: Edge 1-2 is flipped - swap corners 1 & 2.
    if (!isLeftOfEdge(centre_point, corner_first, corner_second, false)) {
        std::swap(corner_first, corner_second);
    }

    // Error 2: Edge 3-4 is flipped - swap corners 3 & 4.
    if (!isLeftOfEdge(centre_point, corner_third, corner_fourth, false)) {
        std::swap(corner_third, corner_fourth);
    }

    // Error 3: Corner 4 is right of 1-2 - replace 4 with midpoint between 1 and 3
    if (!isLeftOfEdge(corner_fourth, corner_first, corner_second, false)) {
        corner_fourth = (corner_third + corner_first) / 2;
    }

    // Error 4: Corner 3 is right of 1-2 - replace 3 with midpoint between 2 and 4
    if (!isLeftOfEdge(corner_third, corner_first, corner_second, false)) {
        corner_third = (corner_fourth + corner_second) / 2;
    }

    std::vector<coord> coords_to_fill;
    for (int x_curr = x_min; x_curr <= x_max; x_curr++) {
        for (int y_curr = y_min; y_curr <= y_max; y_curr++) {
            coord pos ={x_curr, y_curr};
            if (isInRectangle(pos, corner_first, corner_second, corner_third, corner_fourth, is_exclusive)) {
                coords_to_fill.push_back(pos);
            }
        }
    }
    return coords_to_fill;
}

/// Adds to the primary vector the orientation corrected secondary vector, and resizes it so that the projection
/// onto primary vector is equal 1.
coord_d normalisedResultant(const coord_d &primary_vector, const coord_d &secondary_vector) {
    coord_d resultant;
    if (dot(primary_vector, secondary_vector) < 0) {
        resultant = normalized(normalized(primary_vector) - normalized(secondary_vector));
    } else {
        resultant = normalized(normalized(primary_vector) + normalized(secondary_vector));
    }
    double projection = dot(resultant, normalized(primary_vector));
    return resultant / projection;
}


std::vector<coord>
findHalfCircleCentres(const coord &last_point, const coord &previous_point, double radius, bool is_last_point_filled,
                      const coord_d &last_director) {

    coord_d displacements = normalized(last_point - previous_point);
    coord_d tangent = normalisedResultant(displacements, last_director);
    coord_d normal = perpendicular(tangent) * radius;

    coord_d opposite_normal = normal * (-1);

    std::vector<coord> points_to_fill;
    int range = (int) radius + 1;
    for (int x_displacement = -range; x_displacement <= range; x_displacement++) {
        for (int y_displacement = -range; y_displacement <= range; y_displacement++) {
            coord displacement = {x_displacement, y_displacement};
            bool is_on_correct_side = isLeftOfEdge(displacement, normal, opposite_normal, is_last_point_filled);

            if (norm(displacement) <= radius && is_on_correct_side) {
                points_to_fill.emplace_back(last_point + displacement);
            }
        }
    }
    return points_to_fill;
}

std::vector<coord>
findHalfCircleEdges(const coord_d &centre_position, coord_d corner_one, coord_d corner_two, double radius,
                    bool is_last_point_filled, const coord_d &last_move_direction) {
    if (!isLeftOfEdge(centre_position + last_move_direction, corner_one, corner_two, false)) { std::swap(corner_one, corner_two); }

    std::vector<coord> points_to_fill;
    int range = (int) radius + 1;
    auto centre = coord(centre_position);
    coord position;
    for (int x_displacement = -range; x_displacement <= range; x_displacement++) {
        for (int y_displacement = -range; y_displacement <= range; y_displacement++) {
            position = centre + coord{x_displacement, y_displacement};
            bool is_on_correct_side = isLeftOfEdge(position, corner_one, corner_two,
                                                   is_last_point_filled);

            if (is_on_correct_side && (x_displacement * x_displacement + y_displacement * y_displacement) <= (radius * radius)) {
                points_to_fill.emplace_back(position);
            }
        }
    }
    return points_to_fill;
}


std::vector<coord> findPointsInDisk(double radius) {
    std::vector<coord> disk_displacements;
    int range = (int) radius;
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            coord displacement = {i, j};
            if (norm(displacement) <= radius) {
                disk_displacements.push_back(displacement);
            }
        }
    }
    return disk_displacements;
}
