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
// Created by Michał Zmyślony on 17/09/2021.
//
#ifndef VECTOR_SLICER_DESIRED_PATTERN_H
#define VECTOR_SLICER_DESIRED_PATTERN_H

#define _USE_MATH_DEFINES

#include <cmath>
#include <math.h>
#include <string>
#include <vector>
#include <valarray>

using vald = std::valarray<double>;
using vali = std::valarray<int>;

/// \brief Contains the information about the desired vector field such as its shape and local preferred direction, together
/// with the information about its continuous edges
class DesiredPattern {
    vali dimensions;
    /// Each element is one continuous edge of the pattern
    std::vector<std::vector<vali>> perimeter_list;
    std::vector<std::vector<int>> shape_matrix;
    std::vector<std::vector<double>> x_field_preferred;
    std::vector<std::vector<double>> y_field_preferred;
    std::vector<std::vector<double>> splay_array;
    std::vector<std::vector<vali>> splay_sorted_empty_spots;
    bool is_vector_filled = false;
    bool is_vector_sorted = false;
    double maximal_repulsion_angle = M_PI;

    [[nodiscard]] std::vector<std::vector<vali>> binBySplay(unsigned int bins) const;

public:
    DesiredPattern(const std::string &shape_filename, const std::string &x_field_filename,
                   const std::string &y_field_filename);

    DesiredPattern(const std::string &shape_filename, const std::string &theta_field_filename);

    DesiredPattern(std::vector<std::vector<int>> shape_field, std::vector<std::vector<double>> x_field,
                   std::vector<std::vector<double>> y_field);

    [[nodiscard]] const std::vector<std::vector<int>> &getShapeMatrix() const;

    [[nodiscard]] const std::vector<std::vector<double>> &getXFieldPreferred() const;

    [[nodiscard]] const std::vector<std::vector<double>> &getYFieldPreferred() const;

    [[nodiscard]] const vali &getDimensions() const;

    [[nodiscard]] const std::vector<std::vector<vali>> &getSplaySortedEmptySpots() const;

    [[nodiscard]] const std::vector<std::vector<std::valarray<int>>> & getPerimeterList() const;

    [[nodiscard]] double getSplay(const vali &point) const;

    [[nodiscard]] bool isVectorFilled() const;

    [[nodiscard]] bool isVectorSorted() const;

    [[nodiscard]] vali preferredDirection(const vali &position, int distance) const;

    [[nodiscard]] vald preferredDirection(const vald &position, int distance) const;

    [[nodiscard]] bool isInShape(const vali &position) const;

    [[nodiscard]] bool isInShape(const vald &position) const;

    [[nodiscard]] double getMaximalRepulsionAngle() const;
};


#endif //VECTOR_SLICER_DESIRED_PATTERN_H
