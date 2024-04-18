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
#include "simulation/filling_method_config.h"
#include "auxiliary/line_thinning.h"

#define DISCONTINUITY_IGNORE 0
#define DISCONTINUITY_STICK 1
#define DISCONTINUITY_TERMINATE 2

#define SORT_NEAREST_NEIGHBOUR 0
#define SORT_SEED_LINE 1
//#define SORT_TRAVELLING_SALESMAN 2

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

    bool is_vector_filled = false;
    bool is_vector_sorted = false;
    bool is_splay_provided = false;
    bool is_splay_filling_enabled = false;
    bool is_pattern_updated = false;
    int threads = 1;
    int sorting_method = SORT_NEAREST_NEIGHBOUR;
    double minimal_line_length = 0;
    bool is_points_removed = true;

    /// Behaviour at discontinuities: 0 - ignoring, 1 - sticking, 2 - terminating.
    int discontinuity_behaviour = DISCONTINUITY_STICK;

    /// Termination
    double discontinuity_threshold_cos = -1;

    /// Splay seeding: Initialised as original shape matrix and 1's are replaced with 0's once they are considered by the algorithm.
    std::vector<std::vector<uint8_t>> is_coordinate_used;
    /// Splay seeding: 2D matrix initialised with 0's, which are replaced with 1's if used in the current integral curve.
    std::vector<std::vector<uint8_t>> is_coordinate_in_curve;
    /// Splay seeding: List of coordinates within the current integral curve.
    coord_vector integral_curve_coords;
    /// Splay seeding: List of all initial coords in the shape.
    coord_vector coord_in_shape;

    [[nodiscard]] std::vector<std::vector<vali>> binBySplay(unsigned int bins);


    coord_set findPointsOfZeroSplay(const coord &starting_coordinate);

    void adjustMargins();


    [[nodiscard]] vald getMove(const vald &position, double distance, const vald &displacement) const;

    void updateIntegralCurve(const coord &starting_coordinate);

    void updateIntegralCurveInDirection(coord current_coord, vald current_position,
                                        vald current_travel_direction);


    vald getSplayVector(const coord &coordinate);

    std::vector<double> directedSplayMagnitude(const coord_vector &integral_curve);

    void initialiseSplaySeeding();

    bool isCoordinateViable(const coord &coordinate);

    bool isBoundary(const coord &coordinate);

public:

    DesiredPattern();

    DesiredPattern(const std::string &shape_filename, const std::string &x_field_filename,
                   const std::string &y_field_filename, bool is_splay_filling_enabled, int threads,
                   const FillingMethodConfig &filling);

    DesiredPattern(const std::string &shape_filename, const std::string &theta_field_filename,
                   bool is_splay_filling_enabled, int threads, const FillingMethodConfig &filling);

    DesiredPattern(std::vector<veci> shape_field, std::vector<vecd> x_field, std::vector<vecd> y_field,
                   bool is_splay_filling_enabled, int threads, const FillingMethodConfig &filling);

    [[nodiscard]] const std::vector<veci> &getShapeMatrix() const;

    [[nodiscard]] const std::vector<vecd> &getXFieldPreferred() const;

    [[nodiscard]] const std::vector<vecd> &getYFieldPreferred() const;

    [[nodiscard]] const vali &getDimensions() const;

    [[nodiscard]] const std::vector<std::vector<vali>> &getSplaySortedEmptySpots() const;

    [[nodiscard]] const std::vector<std::vector<std::valarray<int>>> &getPerimeterList() const;

    [[nodiscard]] double getSplay(const vali &point) const;

    [[nodiscard]] bool isVectorFilled() const;

    [[nodiscard]] bool isVectorSorted() const;

    [[nodiscard]] vald getDirector(vali positions) const;

    [[nodiscard]] vald getDirector(const vald &positions) const;

    [[nodiscard]] bool isInShape(const vali &position) const;

    [[nodiscard]] bool isSplayProvided() const;

    void setSplayVector(const std::string &path);

    [[nodiscard]] const std::vector<std::vector<vali>> &getLineDensityMinima() const;

    void updateProperties();

    void findLineDensityMinima();

    void isPatternUpdated() const;

    [[nodiscard]] int getSortingMethod() const;

    [[nodiscard]] int getDiscontinuityBehaviour() const;

    [[nodiscard]] double getDiscontinuityThresholdCos() const;

    [[nodiscard]] double getMinimalLineLength() const;

    [[nodiscard]] bool isPointsRemoved() const;

    [[nodiscard]] bool isInShape(const coord &coordinate) const;

};


#endif //VECTOR_SLICER_DESIRED_PATTERN_H
