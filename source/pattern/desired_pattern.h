// Copyright (c) 2021-2025, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite following publication if you use any part of this code in work you publish or distribute:
// [1] Michał Zmyślony M., Klaudia Dradrach, John S. Biggins,
//    Slicing vector fields into tool paths for additive manufacturing of nematic elastomers,
//    Additive Manufacturing, Volume 97, 2025, 104604, ISSN 2214-8604, https://doi.org/10.1016/j.addma.2024.104604.
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
#include <set>
#include "simulation/filling_method_config.h"
#include "auxiliary/line_thinning.h"

#define SORT_NEAREST_NEIGHBOUR 0
#define SORT_SEED_LINE 1
//#define SORT_TRAVELLING_SALESMAN 2

#define SPLAY_LINE_CENTRES 0
#define SPLAY_LINE_BOUNDARIES 1

using vecd = std::vector<double>;

/// \brief Contains the information about the desired vector field such as its shape and local preferred direction, together
/// with the information about its continuous edges
class DesiredPattern {
    std::vector<int> dimensions;
    /// Each element is one continuous edge of the pattern
    std::vector<std::vector<coord>> perimeter_list;
    std::vector<std::vector<uint8_t>> shape_matrix;
    std::vector<std::vector<coord_d>> splay_vector_array;
    std::vector<std::vector<double>> x_field_preferred;
    std::vector<std::vector<double>> y_field_preferred;
    std::vector<std::vector<double>> splay_array;
    std::vector<std::vector<coord>> splay_sorted_empty_spots;
    std::vector<std::vector<coord>> lines_of_minimal_density;

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

    /// Maximal angle for the director to be counted as discontinuous.
    double discontinuity_threshold_cos = 0;

    int splay_line_behaviour = SPLAY_LINE_CENTRES;

    /// Splay seeding: Initialised as original shape matrix and 1's are replaced with 0's once they are considered by the algorithm.
    std::vector<std::vector<uint8_t>> is_coordinate_used;
    /// Splay seeding: 2D matrix initialised with 0's, which are replaced with 1's if used in the current integral curve.
    std::vector<std::vector<uint8_t>> is_coordinate_in_curve;
    /// Splay seeding: List of coordinates within the current integral curve.
    coord_vector integral_curve_coords;
    /// Splay seeding: List of all initial coords in the shape.
    coord_vector coord_in_shape;

    [[nodiscard]] std::vector<std::vector<coord>> binBySplay(unsigned int bins);


    coord_set findPointsOfZeroSplay(const coord &starting_coordinate);

    void adjustMargins();


    [[nodiscard]] coord_d getMove(const coord_d &position, const coord_d &displacement) const;

    void updateIntegralCurve(const coord &starting_coordinate);

    void updateIntegralCurveInDirection(coord current_coord, coord_d current_position,
                                        coord_d current_travel_direction);

    coord_d getMove(const coord &position, const coord_d &displacement) const;

    coord_d getSplayVector(const coord &coordinate);

    std::vector<double> directedSplayMagnitude(const coord_vector &integral_curve);

    void initialiseSplaySeeding();

    bool isCoordinateViable(const coord &coordinate);

    bool isBoundary(const coord &coordinate);

    double getDirectorY(int x, int y) const;

    double getDirectorX(int x, int y) const;

public:

    DesiredPattern();

    DesiredPattern(const std::string &shape_filename, const std::string &x_field_filename,
                   const std::string &y_field_filename, bool is_splay_filling_enabled, int threads,
                   const FillingMethodConfig &filling);

    DesiredPattern(const std::string &shape_filename, const std::string &theta_field_filename,
                   bool is_splay_filling_enabled, int threads, const FillingMethodConfig &filling);

    DesiredPattern(std::vector<std::vector<uint8_t>> shape_field, std::vector<std::vector<double>> x_field,
                   std::vector<std::vector<double>> y_field,
                   bool is_splay_filling_enabled, int threads, const FillingMethodConfig &filling);

    [[nodiscard]] const std::vector<std::vector<uint8_t>> &getShapeMatrix() const;

    [[nodiscard]] const std::vector<vecd> &getXFieldPreferred() const;

    [[nodiscard]] const std::vector<vecd> &getYFieldPreferred() const;

    [[nodiscard]] const std::vector<int> &getDimensions() const;

    [[nodiscard]] const std::vector<std::vector<coord>> &getSplaySortedEmptySpots() const;

    [[nodiscard]] const std::vector<std::vector<coord>> &getPerimeterList() const;

    [[nodiscard]] double getSplay(const coord &point) const;

    [[nodiscard]] bool isVectorFilled() const;

    [[nodiscard]] bool isVectorSorted() const;

    [[nodiscard]] coord_d getDirector(const coord &positions) const;

    [[nodiscard]] coord_d getDirector(const coord_d &positions) const;

    [[nodiscard]] bool isInShape(const coord &position) const;

    [[nodiscard]] bool isSplayProvided() const;

    void setSplayVector(const std::string &path);

    [[nodiscard]] const std::vector<std::vector<coord>> &getLineDensityMinima() const;

    void updateProperties();

    void findLineDensityMinima();

    void isPatternUpdated() const;

    [[nodiscard]] int getSortingMethod() const;

    [[nodiscard]] int getDiscontinuityBehaviour() const;

    [[nodiscard]] double getDiscontinuityThresholdCos() const;

    [[nodiscard]] double getMinimalLineLength() const;

    [[nodiscard]] bool isPointsRemoved() const;

    bool isInShape(const coord_d &coordinate) const;

    bool isInRange(const coord &coordinate) const;
};


#endif //VECTOR_SLICER_DESIRED_PATTERN_H
