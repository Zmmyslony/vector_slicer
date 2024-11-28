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
#include "simple_math_operations.h"



bool isLeftOfEdge(const vecd &point, const vecd &edge_point_first, const vecd &edge_point_second, bool is_exclusive) {
    double cross_product = (edge_point_second[0] - edge_point_first[0]) * (point[1] - edge_point_first[1]) -
                           (edge_point_second[1] - edge_point_first[1]) * (point[0] - edge_point_first[0]);
    if (is_exclusive) {
        return cross_product > 0;
    } else {
        return cross_product >= 0;
    }
}

bool isLeftOfEdge(const veci &point, const vecd &edge_point_first, const vecd &edge_point_second, bool is_exclusive) {
    return isLeftOfEdge(itod(point), edge_point_first, edge_point_second, is_exclusive);
}


coord_d to_coord(const vecd &vec) {
    return coord_d({vec[0], vec[1]});
}

coord_d to_coord(const veci &vec) {
    return coord_d({vec[0], vec[1]});
}

bool isLeftOfEdge(const coord_d &point, const coord_d &edge_point_first, const coord_d &edge_point_second, bool is_exclusive) {
    double cross_product = (edge_point_second.first - edge_point_first.first) * (point.second - edge_point_first.second) -
                           (edge_point_second.second - edge_point_first.second) * (point.first - edge_point_first.first);

    if (is_exclusive) {
        return cross_product > 0;
    } else {
        return cross_product >= 0;
    }
}

bool isInRectangle(const coord_d &point, const coord_d &corner_first, const coord_d &corner_second, const coord_d &corner_third,
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


std::vector<veci> findPointsToFill(vecd corner_first, vecd corner_second, vecd corner_third,
                                   vecd corner_fourth, bool is_exclusive) {
    std::vector<double> x_coordinates = {corner_first[0], corner_second[0], corner_third[0], corner_fourth[0]};
    std::vector<double> y_coordinates = {corner_first[1], corner_second[1], corner_third[1], corner_fourth[1]};

    vecd midpoint_first = multiply(add(corner_second, corner_first), 1. / 2);
    vecd midpoint_second = multiply(add(corner_third, corner_fourth), 1. / 2);

    // Error 1: Edge 3-4 is flipped - swap corners 3 & 4.
    if (!isLeftOfEdge(midpoint_first, corner_third, corner_fourth, false)) {
        std::swap(corner_third, corner_fourth);
    }

    // Error 2: Edge 1-2 is flipped - swap corners 1 & 2.
    if (!isLeftOfEdge(midpoint_second, corner_first, corner_second, false)) {
        std::swap(corner_first, corner_second);
    }

    // Error 3: Corner 4 is right of 1-2 - replace 4 with midpoint between 1 and 3
    if (!isLeftOfEdge(corner_fourth, corner_first, corner_second, false)) {
        corner_fourth = multiply(add(corner_third, corner_first), 1. / 2);
    }

    // Error 4: Corner 3 is right of 1-2 - replace 3 with midpoint between 2 and 4
    if (!isLeftOfEdge(corner_third, corner_first, corner_second, false)) {
        corner_third = multiply(add(corner_fourth, corner_second), 1. / 2);
    }

    int x_min = (int) minValue(x_coordinates);
    int x_max = (int) maxValue(x_coordinates) + 1;
    int y_min = (int) minValue(y_coordinates);
    int y_max = (int) maxValue(y_coordinates) + 1;

    coord_d corner_first_c = to_coord(corner_first);
    coord_d corner_second_c = to_coord(corner_second);
    coord_d corner_third_c = to_coord(corner_third);
    coord_d corner_fourth_c = to_coord(corner_fourth);

    std::vector<coord> coords_to_fill;
    coords_to_fill.reserve((x_max - x_min) * (y_max - y_min));

    for (int x_curr = x_min; x_curr <= x_max; x_curr++) {
        for (int y_curr = y_min; y_curr <= y_max; y_curr++) {
            coord pos({x_curr, y_curr});
            if (isInRectangle(pos, corner_first_c, corner_second_c, corner_third_c, corner_fourth_c, is_exclusive)) {
                coords_to_fill.push_back(pos);
            }
        }
    }
    std::vector<veci> points_to_fill;
    points_to_fill.reserve(coords_to_fill.size());
    for (coord & coord : coords_to_fill) {
        points_to_fill.emplace_back(veci({coord.first, coord.second}));
    }

    return points_to_fill;
}

/// Adds to the primary vector the orientation corrected secondary vector, and resizes it so that the projection
/// onto primary vector is equal 1.
vecd normalisedResultant(const vecd &primary_vector, const vecd &secondary_vector) {
    vecd resultant;
    if (dot(primary_vector, secondary_vector) < 0) {
        resultant = normalize(subtract(normalize(primary_vector), normalize(secondary_vector)));
    } else {
        resultant = normalize(add(normalize(primary_vector), normalize(secondary_vector)));
    }
    double projection = dot(resultant, normalize(primary_vector));

    return multiply(resultant, 1. / projection);
}


std::vector<veci>
findHalfCircleCentres(const veci &last_point, const veci &previous_point, double radius, bool is_last_point_filled,
                      const vecd &last_director) {

    vecd displacements = normalize(subtract(last_point, previous_point));
    vecd tangent = normalisedResultant(displacements, last_director);
    vecd normal = multiply(perpendicular(tangent), radius);
    coord_d normal_c = {normal[0], normal[1]};
    coord_d normal_opposite_c = {-normal[0], -normal[1]};

    std::vector<veci> points_to_fill;
    int range = (int) radius + 1;
    for (int x_displacement = -range; x_displacement <= range; x_displacement++) {
        for (int y_displacement = -range; y_displacement <= range; y_displacement++) {
            veci displacement = {x_displacement, y_displacement};
            bool is_on_correct_side = isLeftOfEdge({x_displacement, y_displacement}, normal_c, normal_opposite_c, is_last_point_filled);

            if (norm(displacement) <= radius && is_on_correct_side) {
                points_to_fill.emplace_back(add(displacement, last_point));
            }
        }
    }
    return points_to_fill;
}

std::vector<veci>
findHalfCircleEdges(const veci &centre_position, vecd corner_one, vecd corner_two, double radius,
                    bool is_last_point_filled, const vecd &last_move_direction) {
    vecd normal = perpendicular(last_move_direction);
    corner_one = subtract(corner_one, itod(centre_position));
    corner_two = subtract(corner_two, itod(centre_position));

    if (dot(normal, corner_one) < 0) {
        vecd tmp = corner_one;
        corner_one = corner_two;
        corner_two = tmp;
    }
    coord_d corner_one_c = {corner_one[0], corner_one[1]};
    coord_d corner_two_c = {corner_two[0], corner_two[1]};

    std::vector<veci> points_to_fill;
    int range = (int) radius + 1;
    for (int x_displacement = -range; x_displacement <= range; x_displacement++) {
        for (int y_displacement = -range; y_displacement <= range; y_displacement++) {
            veci displacement = {x_displacement, y_displacement};
            bool is_on_correct_side = isLeftOfEdge({x_displacement, y_displacement}, corner_one_c, corner_two_c, is_last_point_filled);

            if (norm(displacement) <= radius && is_on_correct_side) {
                points_to_fill.emplace_back(add(displacement, centre_position));
            }
        }
    }
    return points_to_fill;
}


std::vector<veci> findPointsInCircle(double radius) {
    std::vector<veci> points_in_circle;
    int range = (int) radius;
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            veci displacement = {i, j};
            if (norm(displacement) <= radius) {
                points_in_circle.push_back(displacement);
            }
        }
    }
    return points_in_circle;
}


std::vector<veci> findPointsInCircle(int radius) {
    return findPointsInCircle((double) radius);
}
