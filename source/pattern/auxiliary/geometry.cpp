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
#include <valarray>
#include <algorithm>
#include <iostream>

#include "geometry.h"
#include "valarray_operations.h"


bool isLeftOfEdge(const vali &point, const vald &edge_point_first, const vald &edge_point_second, bool is_exclusive) {
    double cross_product = cross(edge_point_second - edge_point_first, itod(point) - edge_point_first);
    if (is_exclusive) {
        return cross_product > 0;
    } else {
        return cross_product >= 0;
    }
}

bool isLeftOfEdge(const vald &point, const vald &edge_point_first, const vald &edge_point_second, bool is_exclusive) {
    return isLeftOfEdge(dtoi(point), edge_point_first, edge_point_second, is_exclusive);
}

bool isInRectangle(const vali &point, const vald &corner_first, const vald &corner_second, const vald &corner_third,
                   const vald &corner_fourth, bool is_exclusive) {
    bool is_left_of_first_edge = isLeftOfEdge(point, corner_first, corner_second, is_exclusive);
    bool is_left_of_second_edge = isLeftOfEdge(point, corner_second, corner_third, false);

    bool is_left_of_third_edge = isLeftOfEdge(point, corner_third, corner_fourth, false);
    bool is_left_of_fourth_edge = isLeftOfEdge(point, corner_fourth, corner_first, false);

    return (is_left_of_first_edge && is_left_of_second_edge && is_left_of_third_edge && is_left_of_fourth_edge);
}


double minValue(const std::vector<double> &values) {
    return *std::min_element(values.begin(), values.end());
}

double maxValue(const std::vector<double> &values) {
    return *std::max_element(values.begin(), values.end());
}


std::vector<vali> findPointsToFill(vald corner_first, vald corner_second, vald corner_third,
                                   vald corner_fourth, bool is_exclusive) {
    std::vector<double> x_coordinates = {corner_first[0], corner_second[0], corner_third[0], corner_fourth[0]};
    std::vector<double> y_coordinates = {corner_first[1], corner_second[1], corner_third[1], corner_fourth[1]};

    vald midpoint_first = (corner_second + corner_first) / 2;
    vald midpoint_second = (corner_third + corner_fourth) / 2;

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
        corner_fourth = (corner_third + corner_first) / 2;
    }

    // Error 4: Corner 3 is right of 1-2 - replace 3 with midpoint between 2 and 4
    if (!isLeftOfEdge(corner_third, corner_first, corner_second, false)) {
        corner_third = (corner_fourth + corner_second) / 2;
    }

    int x_min = (int) minValue(x_coordinates);
    int x_max = (int) maxValue(x_coordinates) + 1;
    int y_min = (int) minValue(y_coordinates);
    int y_max = (int) maxValue(y_coordinates) + 1;

    std::vector<vali> points_to_fill;

    for (int x_curr = x_min; x_curr <= x_max; x_curr++) {
        for (int y_curr = y_min; y_curr <= y_max; y_curr++) {
            vali top_point = {x_curr, y_curr};
            if (isInRectangle(top_point, corner_first, corner_second, corner_third, corner_fourth, is_exclusive)) {
                points_to_fill.push_back(top_point);
            }
        }
    }
    return points_to_fill;
}

/// Adds to the primary vector the orientation corrected secondary vector, and resizes it so that the projection
/// onto primary vector is equal 1.
vald normalisedResultant(const vald &primary_vector, const vald &secondary_vector) {
    vald resultant;
    if (dot(primary_vector, secondary_vector) < 0) {
        resultant = normalize(normalize(primary_vector) - normalize(secondary_vector));
    } else {
        resultant = normalize(normalize(primary_vector) + normalize(secondary_vector));
    }
    double projection = dot(resultant, normalize(primary_vector));

    return resultant / projection;
}


std::vector<vali>
findHalfCircle(const vali &last_point, const vali &previous_point, double radius, bool is_last_point_filled,
               const vald &last_director) {

    vald displacements = normalize(last_point - previous_point);
    vald tangent = normalisedResultant(displacements, last_director);
    vald normal = perpendicular(tangent) * radius;

    std::vector<vali> points_to_fill;
    int range = (int) radius + 1;
    for (int x_displacement = -range; x_displacement <= range; x_displacement++) {
        for (int y_displacement = -range; y_displacement <= range; y_displacement++) {
            vali displacement = {x_displacement, y_displacement};
            bool is_on_correct_side = isLeftOfEdge(displacement, normal, -normal, is_last_point_filled);

            if (norm(displacement) <= radius && is_on_correct_side) {
                points_to_fill.emplace_back(displacement + last_point);
            }
        }
    }
    return points_to_fill;
}


std::vector<vali> findPointsInCircle(double radius) {
    std::vector<vali> points_in_circle;
    int range = (int) radius;
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            vali displacement = {i, j};
            if (norm(displacement) <= radius) {
                points_in_circle.push_back(displacement);
            }
        }
    }
    return points_in_circle;
}


std::vector<vali> findPointsInCircle(int radius) {
    return findPointsInCircle((double) radius);
}
