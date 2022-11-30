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
// Created by Michał Zmyślony on 22/09/2021.
//

#include <vector>
#include <valarray>
#include <algorithm>
#include <iostream>

#include "Geometry.h"
#include "ValarrayOperations.h"


bool isLeftOfEdgeInclusive(const vali &point, const vald &edge_point_first, const vald &edge_point_second) {
    double cross_product = cross(edge_point_second - edge_point_first, itod(point) - edge_point_first);
    return (cross_product >= 0);
}


bool isLeftOfEdgeExclusive(const vali &point, const vald &edge_point_first, const vald &edge_point_second) {
    double cross_product = cross(edge_point_second - edge_point_first, itod(point) - edge_point_first);
    return (cross_product > 0);
}


bool isLeftOfEdge(const vali &point, const vald &edge_point_first, const vald &edge_point_second, bool is_exclusive) {
    if (is_exclusive) {
        return isLeftOfEdgeExclusive(point, edge_point_first, edge_point_second);
    } else {
        return isLeftOfEdgeInclusive(point, edge_point_first, edge_point_second);
    }
}


bool isInRectangle(const vali &point, const vald &corner_first, const vald &corner_second, const vald &corner_third,
                   const vald &corner_fourth, bool is_exclusive) {
    bool is_left_of_first_edge = isLeftOfEdge(point, corner_first, corner_second, false);
    bool is_left_of_second_edge = isLeftOfEdge(point, corner_second, corner_third, false);

    bool is_left_of_third_edge = isLeftOfEdge(point, corner_third, corner_fourth, false);
    bool is_left_of_fourth_edge = isLeftOfEdge(point, corner_fourth, corner_first, is_exclusive);

    return (is_left_of_first_edge && is_left_of_second_edge && is_left_of_third_edge && is_left_of_fourth_edge);
}


double minValue(const std::vector<double> &values) {
    return *std::min_element(values.begin(), values.end());
}

double maxValue(const std::vector<double> &values) {
    return *std::max_element(values.begin(), values.end());
}


std::vector<vali> findPointsToFill(const vald &corner_first, const vald &corner_second, const vald &corner_third,
                                   const vald &corner_fourth, bool is_exclusive) {
    std::vector<double> x_coordinates = {corner_first[0], corner_second[0], corner_third[0], corner_fourth[0]};
    std::vector<double> y_coordinates = {corner_first[1], corner_second[1], corner_third[1], corner_fourth[1]};

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

std::vector<vali>
findPointsToFill(const vali &point_current, const vali &point_next, double radius, bool is_first_point_filled) {
    vald tangent = normalize(point_next - point_current);
    vald normal = perpendicular(tangent) * radius;

    vald corner_first = itod(point_current) + normal;
    vald corner_second = itod(point_current) - normal;
    vald corner_third = itod(point_next) - normal;
    vald corner_fourth = itod(point_next) + normal;

    return findPointsToFill(corner_first, corner_second, corner_third, corner_fourth, is_first_point_filled);
}

std::vector<vali>
findPointsToFill(const vali &point_previous, const vali &point_current, const vali &point_next, double radius,
                 bool is_first_point_filled) {
    vald previous_tangent = normalize(point_current - point_previous);
    vald previous_normal = perpendicular(previous_tangent) * radius;

    vald current_tangent = normalize(point_next - point_current);
    vald current_normal = perpendicular(current_tangent) * radius;

    vald corner_first = itod(point_current) + previous_normal;
    vald corner_second = itod(point_current) - previous_normal;
    vald corner_third = itod(point_next) - current_normal;
    vald corner_fourth = itod(point_next) + current_normal;

    return findPointsToFill(corner_first, corner_second, corner_third, corner_fourth, is_first_point_filled);
}


std::vector<vali>
findHalfCircle(const vali &last_point, const vali &previous_point, double radius, bool is_last_point_filled) {

    vald tangent = normalize(last_point - previous_point);
    vald normal = perpendicular(tangent) * radius;

    std::vector<vali> points_to_fill;
    int range = (int) radius + 1;
    for (int x_displacement = -range; x_displacement <= range; x_displacement++) {
        for (int y_displacement = -range; y_displacement <= range; y_displacement++) {
            vali displacement = {x_displacement, y_displacement};
            bool is_on_correct_side = isLeftOfEdge(displacement, normal, -normal, is_last_point_filled);

            if (norm(displacement) <= radius && is_on_correct_side) {
                points_to_fill.push_back(displacement + last_point);
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
            if (norm(displacement) < radius) {
                points_in_circle.push_back(displacement);
            }
        }
    }
    return points_in_circle;
}


std::vector<vali> findPointsInCircle(int radius) {
    return findPointsInCircle((double) radius);
}