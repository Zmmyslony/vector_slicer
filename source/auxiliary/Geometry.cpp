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

#include <utility>
#include <vector>
#include <valarray>
#include <algorithm>
#include "ValarrayOperations.h"
#include <cfloat>


bool isOnTheLeftSideOfEdge(std::valarray<int> point, std::valarray<double> edge_first,
                           std::valarray<double> edge_second) {
    int sign = (int) ((edge_second[0] - edge_first[0]) * (point[1] - edge_first[1]) -
                      (point[0] - edge_first[0]) * (edge_second[1] - edge_first[1]));
    if (sign > 0) {
        return true;
    } else {
        return false;
    }
}


bool isInRectangle(std::valarray<int> &point, std::valarray<double> &edge_first,
                   std::valarray<double> &edge_second, std::valarray<double> &edge_third,
                   std::valarray<double> &edge_fourth) {
    bool is_on_left_of_first_edge = isOnTheLeftSideOfEdge(point, edge_first, edge_second);
    bool is_on_left_of_second_edge = isOnTheLeftSideOfEdge(point, edge_second, edge_third);
    bool is_on_left_of_third_edge = isOnTheLeftSideOfEdge(point, edge_third, edge_fourth);
    bool is_on_left_of_fourth_edge = isOnTheLeftSideOfEdge(point, edge_fourth, edge_first);

    if (is_on_left_of_first_edge && is_on_left_of_second_edge && is_on_left_of_third_edge && is_on_left_of_fourth_edge) {
        return true;
    } else {
        return false;
    }
}


double minValue(const std::vector<double> &values) {
    double min_value = DBL_MAX;
    for (auto &current_value : values) {
        if (current_value < min_value) {
            min_value = current_value;
        }
    }
    return min_value;
}

double maxValue(const std::vector<double> &values) {
    double max_value = DBL_MIN;
    for (auto &current_value : values) {
        if (current_value > max_value) {
            max_value = current_value;
        }
    }
    return max_value;
}


std::vector<std::valarray<int>> findPointsToFill(const std::valarray<int> &point_first,
                                                 const std::valarray<int> &point_second, double radius) {
    std::valarray<double> point_first_double = itodArray(point_first);
    std::valarray<double> point_second_double = itodArray(point_second);

    std::valarray<double> connecting_vector = normalize(point_second_double - point_first_double);
    std::valarray<double> perpendicular_vector = {connecting_vector[1] * radius, -connecting_vector[0] * radius};

    std::valarray<double> first_edge = point_first_double + perpendicular_vector - 0.5 * connecting_vector;
    std::valarray<double> second_edge = point_second_double + perpendicular_vector;
    std::valarray<double> third_edge = point_second_double - perpendicular_vector;
    std::valarray<double> fourth_edge = point_first_double - perpendicular_vector - 0.5 * connecting_vector;


    int x_min = (int)minValue({first_edge[0], second_edge[0], third_edge[0], fourth_edge[0]});
    int x_max = (int)maxValue({first_edge[0], second_edge[0], third_edge[0], fourth_edge[0]}) + 1;
    int y_max = (int)maxValue({first_edge[1], second_edge[1], third_edge[1], fourth_edge[1]}) + 1;
    int y_min = (int)minValue({first_edge[1], second_edge[1], third_edge[1], fourth_edge[1]});

    std::vector<std::valarray<int>> points_to_fill;

    int y_curr = y_max;
    for (int x_curr = x_min; x_curr <= x_max; x_curr++) {
        std::valarray<int> top_point = {x_curr, y_curr};
        bool searching_for_top_point = true;
        while (searching_for_top_point) {
            if (isInRectangle(top_point, first_edge, second_edge, third_edge, fourth_edge) &&
                top_point[1] >= y_curr && top_point[1] <= y_max) {
                top_point[1]++;
            } else if (!isInRectangle(top_point, first_edge, second_edge, third_edge, fourth_edge) &&
                       top_point[1] <= y_curr && top_point[1] >= y_min) {
                top_point[1]--;
            } else {
                searching_for_top_point = false;
            }
        }

        while (isInRectangle(top_point, first_edge, second_edge, third_edge, fourth_edge) && top_point[1] >= 0) {
            points_to_fill.push_back(top_point);
            top_point[1]--;
        }
    }
    return points_to_fill;
}


std::vector<std::valarray<int>> findPointsInCircle(double radius) {
    std::vector<std::valarray<int>> points_in_circle;
    int range = (int)radius + 1;
    for (int i = -range ; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            if (i * i + j * j <= radius * radius) {
                points_in_circle.push_back({i, j});
            }
        }
    }
    return points_in_circle;
}


std::vector<std::valarray<int>> findHalfCircle(const std::valarray<int> &last_point,
                                               const std::valarray<int> &previous_point, double radius) {
    std::valarray<double> last_point_double = itodArray(last_point);
    std::valarray<double> point_second_double = itodArray(previous_point);

    std::valarray<double> connecting_vector = normalize(point_second_double - last_point_double);
    std::valarray<double> perpendicular_vector = {connecting_vector[1] * radius, -connecting_vector[0] * radius};

    std::valarray<double> first_corner = perpendicular_vector - 0.5 * connecting_vector;
    std::valarray<double> second_corner = -perpendicular_vector - 0.5 * connecting_vector;

    std::vector<std::valarray<int>> points_to_fill;

    int int_radius = (int)radius + 1;
    for (int x_displacement = -int_radius; x_displacement < int_radius; x_displacement++) {
        for (int y_displacement = -int_radius; y_displacement < int_radius; y_displacement++) {
            std::valarray<int> displacement = {x_displacement, y_displacement};
            double distance = norm(displacement);

            bool is_on_correct_side = isOnTheLeftSideOfEdge(displacement, first_corner, second_corner);

            if (distance <= radius && is_on_correct_side) {
                points_to_fill.push_back(displacement + last_point);
            }
        }
    }
    return points_to_fill;
}


std::vector<std::valarray<int>> findPointsInCircle(int radius) {
    return findPointsInCircle((double)radius);
}