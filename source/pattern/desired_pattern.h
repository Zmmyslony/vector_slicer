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
#include <set>

using vald = std::valarray<double>;
using vali = std::valarray<int>;

using vecd = std::vector<double>;
using veci = std::vector<int>;

/// \brief Contains the information about the desired vector field such as its shape and local preferred direction, together
/// with the information about its continuous edges
class DesiredPattern {
    vali dimensions;
    /// Each element is one continuous edge of the pattern
    std::vector<std::vector<vali>> perimeter_list;
    std::vector<veci> shape_matrix;
    std::vector<vecd> x_field_preferred;
    std::vector<vecd> y_field_preferred;
    std::vector<vecd> splay_array;
    std::vector<std::vector<std::valarray<double>>> splay_vector_array;
    std::vector<std::vector<vali>> splay_sorted_empty_spots;
    std::vector<std::vector<vali>> lines_of_minimal_density;
    double last_bin_splay = 0;
    bool is_vector_filled = false;
    bool is_vector_sorted = false;
    bool is_splay_provided = false;

    [[nodiscard]] std::vector<std::vector<vali>> binBySplay(unsigned int bins);

    void findLineDensityMinima();

    [[nodiscard]] vecd preferredDirection(const vecd &position, double distance) const;

    [[nodiscard]] vecd getSplayDirection(const vecd &position, double length) const;

    veci findPointOfMinimumDensity(std::set<veci> &candidate_set, bool &is_valid, vecd current_coordinates);

    [[nodiscard]] double splay(const vecd &position) const;

    std::set<veci> findSplaySingularities();

public:

    DesiredPattern();

    DesiredPattern(const std::string &shape_filename, const std::string &x_field_filename,
                   const std::string &y_field_filename);

    DesiredPattern(const std::string &shape_filename, const std::string &theta_field_filename);

    DesiredPattern(std::vector<veci> shape_field, std::vector<vecd> x_field,
                   std::vector<vecd> y_field);

    [[nodiscard]] const std::vector<veci> &getShapeMatrix() const;

    [[nodiscard]] const std::vector<vecd> &getXFieldPreferred() const;

    [[nodiscard]] const std::vector<vecd> &getYFieldPreferred() const;

    [[nodiscard]] const vali &getDimensions() const;

    [[nodiscard]] const std::vector<std::vector<vali>> &getSplaySortedEmptySpots() const;

    [[nodiscard]] const std::vector<std::vector<std::valarray<int>>> &getPerimeterList() const;

    [[nodiscard]] double getSplay(const vali &point) const;

    [[nodiscard]] bool isVectorFilled() const;

    [[nodiscard]] bool isVectorSorted() const;

    [[nodiscard]] vali preferredDirection(const vali &position, int distance) const;

    [[nodiscard]] vald preferredDirection(const vald &position, double distance) const;

    [[nodiscard]] bool isInShape(const vali &position) const;

    [[nodiscard]] bool isInShape(const vald &position) const;

    [[nodiscard]] bool isSplayProvided() const;

    void setSplayVector(const std::string &path);

    [[nodiscard]] bool isLowSplay(const vald &coordinates) const;

    [[nodiscard]] const std::vector<std::vector<vali>> &getLineDensityMinima() const;

    void updateProperties();
};


#endif //VECTOR_SLICER_DESIRED_PATTERN_H
