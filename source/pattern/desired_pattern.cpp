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

#define _USE_MATH_DEFINES

#include "desired_pattern.h"

#include <utility>
#include <algorithm>
#include <set>
#include <iostream>
#include <math.h>
#include <cfloat>
#include <fstream>
#include <iterator>
#include <random>

#include "importing_and_exporting/table_reading.h"
#include "auxiliary/simple_math_operations.h"
#include "auxiliary/perimeter.h"
#include "auxiliary/line_operations.h"
#include "auxiliary/line_thinning.h"
#include "auxiliary/valarray_operations.h"
#include "auxiliary/vector_operations.h"
#include "simulation/filling_method_config.h"

#include "vector_slicer_config.h"

/// Numerical threshold for splay, indicating which values of it ought to be treated as zero.
#define ZERO_SPLAY_THRESHOLD 1e-6


DesiredPattern::DesiredPattern() = default;


DesiredPattern::DesiredPattern(std::vector<std::vector<uint8_t>> shape_field, std::vector<std::vector<double>> x_field,
                               std::vector<std::vector<double>> y_field,
                               bool is_splay_filling_enabled, int threads, const FillingMethodConfig &filling) :
        shape_matrix(std::move(shape_field)),
        x_field_preferred(std::move(x_field)),
        y_field_preferred(std::move(y_field)),
        dimensions(getTableDimensions(shape_field)),
        is_splay_filling_enabled(is_splay_filling_enabled),
        threads(threads) {

    is_vector_filled = filling.isVectorFillingEnabled();
    is_vector_sorted = filling.isVectorSortingEnabled();
    sorting_method = filling.getSortingMethod();
    double angular_discontinuity_threshold = filling.getDiscontinuityThreshold();
    discontinuity_threshold_cos = cos(angular_discontinuity_threshold * M_PI / 180);
    discontinuity_behaviour = filling.getDiscontinuityBehaviour();
    minimal_line_length = filling.getMinimalLineLength();
    is_points_removed = filling.isPointsRemoved();
    splay_line_behaviour = filling.getSplayLineBehaviour();
}


DesiredPattern::DesiredPattern(const std::string &shape_filename, const std::string &x_field_filename,
                               const std::string &y_field_filename, bool is_splay_filling_enabled, int threads,
                               const FillingMethodConfig &filling) :
        DesiredPattern(readFileToTableInt(shape_filename), readFileToTableDouble(x_field_filename),
                       readFileToTableDouble(y_field_filename), is_splay_filling_enabled, threads, filling) {}


DesiredPattern::DesiredPattern(const std::string &shape_filename, const std::string &theta_field_filename,
                               bool is_splay_filling_enabled, int threads, const FillingMethodConfig &filling) {
    std::vector<vecd> theta_field = readFileToTableDouble(theta_field_filename);
    std::vector<vecd> x_field;
    std::vector<vecd> y_field;
    for (const auto &theta_row: theta_field) {
        vecd x_row;
        vecd y_row;
        for (const auto &theta: theta_row) {
            x_row.push_back(cos(theta));
            y_row.push_back(sin(theta));
        }
        x_field.push_back(x_row);
        y_field.push_back(y_row);
    }
    *this = DesiredPattern(readFileToTableInt(shape_filename), x_field, y_field, is_splay_filling_enabled,
                           threads, filling);
}


void DesiredPattern::updateProperties() {
#ifdef TIMING
    auto t1 = std::chrono::high_resolution_clock::now();
#endif
    adjustMargins();
    if (!isSplayProvided()) {
        splay_vector_array = splayVector(x_field_preferred, y_field_preferred, threads);
        splay_array = vectorArrayNorm(splay_vector_array, threads);

        if (shape_matrix.size() != splay_array.size()) {
            throw std::runtime_error("Incompatible x-size of splay array and shape array.");
        } else if (shape_matrix.front().size() != splay_array.front().size()) {
            throw std::runtime_error("Incompatible y-size of splay array and shape array.");
        }
    }

    std::cout << "Pattern dimensions: " << dimensions[0] << "x" << dimensions[1] << std::endl;
    if (is_splay_filling_enabled) {
        findLineDensityMinima();
    }
    int bin_number = std::min(shape_matrix.size(), shape_matrix[0].size()) / 10;

    splay_sorted_empty_spots = binBySplay(bin_number);
    perimeter_list = findSeparatedPerimeters(shape_matrix, dimensions, splay_vector_array);
    is_pattern_updated = true;
#ifdef TIMING
    time_t end_time;
    time(&end_time);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = t2 - t1;
    double ns_per_pix =
            1000000 * ms.count() / (dimensions[0] * dimensions[1]);
    std::cout << "Initialisation duration: " << ms.count() << "ms (" << ns_per_pix << " ns/pix)" << std::endl;
#endif
}

void DesiredPattern::isPatternUpdated() const {
    if (!is_pattern_updated) {
        throw std::runtime_error("Desired pattern has not been updated prior to using it for filling.");
    }
}


void DesiredPattern::adjustMargins() {
    std::vector<int> null_rows = findNullRows(shape_matrix, 10);
    std::vector<int> null_columns = findNullColumns(shape_matrix, 10);

    adjustRowsAndColumns(shape_matrix, null_rows, null_columns);
    adjustRowsAndColumns(x_field_preferred, null_rows, null_columns);
    adjustRowsAndColumns(y_field_preferred, null_rows, null_columns);
    adjustRowsAndColumns(splay_vector_array, null_rows, null_columns);
    adjustRowsAndColumns(splay_array, null_rows, null_columns);

    dimensions = getTableDimensions(shape_matrix);
}


coord_d DesiredPattern::getDirector(const coord &positions) const {
    if (positions.x < 0 || positions.x >= dimensions[0] ||
        positions.y < 0 || positions.y >= dimensions[1]) {
        return {0, 0};
    }
    return coord_d({x_field_preferred[positions.x][positions.y],
                    y_field_preferred[positions.x][positions.y]});
}

double DesiredPattern::getDirectorX(int x, int y) const {
    if (x < 0 || x >= dimensions[0] ||
        y < 0 || y >= dimensions[1]) {
        return 0;
    }
    return x_field_preferred[x][y];
}

double DesiredPattern::getDirectorY(int x, int y) const {
    if (x < 0 || x >= dimensions[0] ||
        y < 0 || y >= dimensions[1]) {
        return 0;
    }
    return y_field_preferred[x][y];
}


coord_d DesiredPattern::getDirector(const coord_d &positions) const {
    int x_base = floor(positions.x);
    int y_base = floor(positions.y);
    double x_fraction = 1 - (positions.x - floor(positions.x));
    double y_fraction = 1 - (positions.y - floor(positions.y));

    double x_director =  x_fraction * y_fraction * getDirectorX(x_base, y_base);
    x_director += (1 - x_fraction) * y_fraction * getDirectorX(x_base + 1, y_base);
    x_director += (1 - x_fraction) * (1 - y_fraction) * getDirectorX(x_base + 1, y_base + 1);
    x_director += x_fraction * (1 - y_fraction) * getDirectorX(x_base, y_base + 1);

    double y_director =  x_fraction * y_fraction * getDirectorY(x_base, y_base);
    y_director += (1 - x_fraction) * y_fraction * getDirectorY(x_base + 1, y_base);
    y_director += (1 - x_fraction) * (1 - y_fraction) * getDirectorY(x_base + 1, y_base + 1);
    y_director += x_fraction * (1 - y_fraction) * getDirectorY(x_base, y_base + 1);

    return {x_director, y_director};
//    coord_d director = {0, 0};
//    director += x_fraction * y_fraction * getDirector(coord{x_base, y_base});
//    director += (1 - x_fraction) * y_fraction * getDirector(coord{x_base + 1, y_base});
//    director += (1 - x_fraction) * (1 - y_fraction) * getDirector(coord{x_base + 1, y_base + 1});
//    director += x_fraction * (1 - y_fraction) * getDirector(coord{x_base, y_base + 1});

//    return director;
}

bool DesiredPattern::isInRange(const coord &coordinate) const {
    return 0 <= coordinate.x && coordinate.x < dimensions[0] &&
           0 <= coordinate.y && coordinate.y < dimensions[1];
}


bool DesiredPattern::isInShape(const coord &coordinate) const {
    return isInRange(coordinate) && shape_matrix[coordinate.x][coordinate.y];
}

bool DesiredPattern::isInShape(const coord_d &coordinate) const {
    return isInShape(coord(coordinate));
}


const std::vector<int> &DesiredPattern::getDimensions() const {
    return dimensions;
}


const std::vector<std::vector<coord>> &DesiredPattern::getPerimeterList() const {
    return perimeter_list;
}


const std::vector<std::vector<uint8_t>> &DesiredPattern::getShapeMatrix() const {
    return shape_matrix;
}


const std::vector<vecd> &DesiredPattern::getXFieldPreferred() const {
    return x_field_preferred;
}


const std::vector<vecd> &DesiredPattern::getYFieldPreferred() const {
    return y_field_preferred;
}

double DesiredPattern::getSplay(const coord &point) const {
    return splay_array[point.x][point.y];
}


std::vector<coord> findFillableCells(const std::vector<std::vector<uint8_t>> &shape_matrix) {
    std::vector<coord> fillable_cells;
    for (int i = 0; i < shape_matrix.size(); i++) {
        for (int j = 0; j < shape_matrix[i].size(); j++) {
            if (shape_matrix[i][j]) {
                fillable_cells.emplace_back(i, j);
            }
        }
    }
    return fillable_cells;
}


std::vector<std::vector<coord>> DesiredPattern::binBySplay(unsigned int bins) {
    std::vector<coord> unsorted_coordinates = findFillableCells(shape_matrix);
    if (unsorted_coordinates.empty()) {
        return {};
    }
    vecd coordinates_splay;
    for (auto &point: unsorted_coordinates) {
        double current_splay = getSplay(point);
        coordinates_splay.push_back(current_splay);
    }
    double max_splay = *std::max_element(coordinates_splay.begin(), coordinates_splay.end());
    double min_splay = *std::min_element(coordinates_splay.begin(), coordinates_splay.end());
    if (max_splay == min_splay) {
        return {unsorted_coordinates};
    }

    std::vector<std::vector<coord>> binned_coordinates(bins);
    for (int i = 0; i < unsorted_coordinates.size(); i++) {
        unsigned int bin = (double) (bins - 1) * (coordinates_splay[i] - min_splay) / (max_splay - min_splay);
        binned_coordinates[bin].push_back(unsorted_coordinates[i]);
    }
    std::reverse(binned_coordinates.begin(), binned_coordinates.end());
    return binned_coordinates;
}

const std::vector<std::vector<coord>> &DesiredPattern::getSplaySortedEmptySpots() const {
    return splay_sorted_empty_spots;
}

bool DesiredPattern::isVectorFilled() const {
    return is_vector_filled;
}

bool DesiredPattern::isVectorSorted() const {
    return is_vector_sorted;
}

bool DesiredPattern::isSplayProvided() const {
    return is_splay_provided;
}


void DesiredPattern::setSplayVector(const std::string &path) {
    splay_vector_array = readFileToTableDoubleVector(path);

    if (shape_matrix.size() != splay_vector_array.size()) {
        std::cout << "Incompatible x-size of splay array and shape array. Defaulting to numerical calculation."
                  << std::endl;
    } else if (shape_matrix.front().size() != splay_vector_array.front().size()) {
        std::cout << "Incompatible y-size of splay array and shape array. Defaulting to numerical calculation."
                  << std::endl;
    } else {
        splay_array = vectorArrayNorm(splay_vector_array, threads);
        is_splay_provided = true;
    }
}


/// Returns vector along the director in the same direction as previous displacement
coord_d DesiredPattern::getMove(const coord_d &position, const coord_d &displacement) const {
    coord_d undirected_move = getDirector(position);
    if (dot(undirected_move, displacement) >= 0) {
        return undirected_move;
    } else {
        return -1.0 * undirected_move;
    }
}

/// Returns vector along the director in the same direction as previous displacement
coord_d DesiredPattern::getMove(const coord &position, const coord_d &displacement) const {
    coord_d undirected_move = getDirector(position);
    if (dot(undirected_move, displacement) >= 0) {
        return 1.0 * undirected_move;
    } else {
        return -1.0 * undirected_move;
    }
}


double distance(const coord &first, const coord &second) {
    return norm(first - second);
}


void DesiredPattern::updateIntegralCurveInDirection(coord current_coord, coord_d current_position,
                                                    coord_d current_travel_direction) {

    auto start = std::chrono::system_clock::now();
    is_coordinate_used[current_coord.x][current_coord.y] = 0;

    while (
            isInShape(current_coord) &&
            (!is_coordinate_in_curve[current_coord.x][current_coord.y] ||
             (integral_curve_coords.back() == current_coord))
            ) {
        is_coordinate_in_curve[current_coord.x][current_coord.y] = 1;
        // Avoid doubling the entries.
        if (integral_curve_coords.empty() || current_coord != integral_curve_coords.back()) {
            integral_curve_coords.emplace_back(current_coord);
        }
        is_coordinate_used[current_coord.x][current_coord.y] = 0;
        current_travel_direction = getMove(current_position, current_travel_direction);
        current_position = current_position + current_travel_direction;
        current_coord = coord(current_position);
    }
}


void DesiredPattern::updateIntegralCurve(const coord &starting_coordinate) {
    coord_d current_position = to_coord_d(starting_coordinate);
    coord_d current_travel_direction = getDirector(current_position);

    coord current_coord = starting_coordinate;

    updateIntegralCurveInDirection(current_coord, current_position, current_travel_direction);
    if (!integral_curve_coords.empty()) {
        std::reverse(integral_curve_coords.begin(), integral_curve_coords.end());
    }

    updateIntegralCurveInDirection(current_coord, current_position, -1 * current_travel_direction);

    for (auto &coord: integral_curve_coords) {
        is_coordinate_in_curve[coord.x][coord.y] = 0;
    }
}

coord_d DesiredPattern::getSplayVector(const coord &coordinate) {
    return splay_vector_array[coordinate.x][coordinate.y];
}

/// The magnitude splay in the direction from back to front.
std::vector<double> DesiredPattern::directedSplayMagnitude(const coord_vector &integral_curve) {
    std::vector<double> directed_splay(integral_curve.size());

    coord_d displacement = to_coord_d(integral_curve[0] - integral_curve[1]);

    displacement = getMove(integral_curve.front(), displacement);
    coord_d splay = getSplayVector(integral_curve.front());
    directed_splay[0] = dot(splay, displacement);

    for (int i = 1; i < integral_curve.size(); i++) {
        displacement = getMove(integral_curve[i], displacement);
        splay = getSplayVector(integral_curve[i]);

        directed_splay[i] = dot(displacement, splay);
    }
    return directed_splay;
}

/// Checks whether any of the neighbours of the last element exists in the coordinate line in the first 3 coordinates
/// if the length of the curve is longer than 6 coordiantes.
bool isLoopedAnywhere(const coord_vector &coordinate_line) {
    if (coordinate_line.size() <= 6) {
        return false;
    }
    coord_set front_set(coordinate_line.begin(), coordinate_line.begin() + 3);
    coord last_coord = coordinate_line.back();
    int x = coordinate_line.back().x;
    int y = coordinate_line.back().y;
    coord_vector neighbours = {
            last_coord + coord{1, 0},
            last_coord + coord{1, 1},
            last_coord + coord{0, 1},
            last_coord + coord{-1, 1},
            last_coord + coord{-1, 0},
            last_coord + coord{-1, -1},
            last_coord + coord{0, -1},
            last_coord + coord{1, -1},
    };
    bool is_looped = std::any_of(neighbours.begin(), neighbours.end(),
                                 [front_set](const coord &coordinate) {
                                     return front_set.find(coordinate) != front_set.end();
                                 });
    return is_looped;
}

bool isValidSplayFreeLineStart(bool is_boundary_last_in_curve, double splay) {
    // When this element is the last element in the integral curve and is at a boundary, this means that the integral
    // curve ends at a boundary for which condition is different.
    if (is_boundary_last_in_curve) {
        return splay < ZERO_SPLAY_THRESHOLD;
    } else {
        return splay > ZERO_SPLAY_THRESHOLD;
    }
}

bool isValidSplayFreeLineInterior(double splay) {
    // If the absolute value of splay is lesser than the zero splay threshold, we assume it is a valid inner point.
    return fabs(splay) < ZERO_SPLAY_THRESHOLD;
}

bool isValidSplayFreeLineEnd(bool is_boundary_last_in_curve, double splay) {
    if (is_boundary_last_in_curve) {
        return splay > -ZERO_SPLAY_THRESHOLD;
    } else {
        return splay < -ZERO_SPLAY_THRESHOLD;
    }
}

bool DesiredPattern::isBoundary(const coord &coordinate) {
    coord coord_i = {coordinate.x, coordinate.y};
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            coord displacement = {i, j};
            bool is_out_of_bounds = !isInShape(coord_i + displacement);
            if (is_out_of_bounds) {
                return true;
            }
        }
    }
    return false;
}

coord_set DesiredPattern::findPointsOfZeroSplay(const coord &starting_coordinate) {
    updateIntegralCurve(starting_coordinate);
    if (integral_curve_coords.empty()) {
        return {};
    }

    /// The splay magnitude in the direction from back to front.
    std::vector<double> directed_splay = directedSplayMagnitude(integral_curve_coords);

    coord_vector current_splay_free_line;
    coord_set valid_coords_set;
    coord_set zero_splay_boundary_nodes;

    bool is_integral_curve_looped = isLoopedAnywhere(integral_curve_coords);
    bool is_last_in_curve = !is_integral_curve_looped;

    while (!integral_curve_coords.empty()) {
        coord coordinate = integral_curve_coords.back();
        is_coordinate_in_curve[coordinate.x][coordinate.y] = 0;
        integral_curve_coords.pop_back();
        if (integral_curve_coords.empty()) { is_last_in_curve = !is_integral_curve_looped; }
        double splay = directed_splay.back();
        directed_splay.pop_back();
        bool is_current_boundary = isBoundary(coordinate);
        if (is_current_boundary) { zero_splay_boundary_nodes.insert(coordinate); }

        if (isValidSplayFreeLineEnd(is_current_boundary && is_last_in_curve, splay) &&
            !current_splay_free_line.empty()) {
            current_splay_free_line.emplace_back(coordinate);
            bool is_looped = isLoopedAnywhere(current_splay_free_line);

            if (!is_looped) {
                if (zero_splay_boundary_nodes.empty() || splay_line_behaviour == SPLAY_LINE_CENTRES) {
                    valid_coords_set.insert(current_splay_free_line[current_splay_free_line.size() / 2]);
                } else {
                    valid_coords_set.insert(zero_splay_boundary_nodes.begin(), zero_splay_boundary_nodes.end());
                }
            }
            zero_splay_boundary_nodes.clear();
            current_splay_free_line.clear();
        }
        if (isValidSplayFreeLineInterior(splay) && !current_splay_free_line.empty()) {
            current_splay_free_line.emplace_back(coordinate);
        }
        if (isValidSplayFreeLineStart(is_current_boundary && is_last_in_curve, splay)) {
            current_splay_free_line = {coordinate};
            if (is_current_boundary) {
                zero_splay_boundary_nodes = {coordinate};
            } else {
                zero_splay_boundary_nodes.clear();
            }
        }
        is_last_in_curve = false;
    }
    return valid_coords_set;
}


coord_vector shape_coordinates_vector(const std::vector<std::vector<uint8_t>> &shape_matrix) {
    coord_vector candidate_set;
    for (int i = 0; i < shape_matrix.size(); i++) {
        for (int j = 0; j < shape_matrix[i].size(); j++) {
            coord current = {i, j};
            if (shape_matrix[i][j]) {
                candidate_set.emplace_back(current);
            }
        }
    }
    return candidate_set;
}


void DesiredPattern::initialiseSplaySeeding() {
    is_coordinate_used = std::vector<std::vector<uint8_t>>(shape_matrix.size());
    is_coordinate_in_curve = std::vector<std::vector<uint8_t>>(shape_matrix.size());
    for (int i = 0; i < shape_matrix.size(); i++) {
        is_coordinate_used[i].insert(is_coordinate_used[i].end(), shape_matrix[i].begin(), shape_matrix[i].end());
        is_coordinate_in_curve[i] = std::vector<uint8_t>(shape_matrix[i].size(), 0);
    }
    coord_in_shape = shape_coordinates_vector(shape_matrix);
    // We are reshuffling in order to analyse the coordinates semi-randomly.
    std::shuffle(coord_in_shape.begin(), coord_in_shape.end(), std::mt19937(0));
}

bool DesiredPattern::isCoordinateViable(const coord &coordinate) {
    return is_coordinate_used[coordinate.x][coordinate.y];
}

void exportCoord(const coord_set &vec, const fs::path &output) {
    std::ofstream file(output.string());
    if (file.is_open()) {
        for (auto &el: vec) {
            file << el.x << "," << el.y << std::endl;
        }
    }
    file.close();
}

void DesiredPattern::findLineDensityMinima() {
    auto t0 = std::chrono::system_clock::now();
    std::cout << "Beginning search for seeding lines." << std::endl;
    initialiseSplaySeeding();
    size_t fillable_point_count = coord_in_shape.size();

    coord_set solution_set;
    while (!coord_in_shape.empty()) {
        coord starting_coordinate = coord_in_shape.back();
        coord_in_shape.pop_back();
        if (!isCoordinateViable(starting_coordinate)) {
            continue;
        }

        double progress = (1 - (double) coord_in_shape.size() / (double) fillable_point_count) * 100;
        printf("\r%.2f%% coordinates analysed       ", progress);
        fflush(stdout);
        coord_set point_of_minimum_density = findPointsOfZeroSplay(starting_coordinate);
        solution_set.insert(point_of_minimum_density.begin(), point_of_minimum_density.end());
    }

    std::cout << "\rSearch for seeding lines complete." << std::endl;
    solution_set = skeletonize(solution_set, 10, shape_matrix);
//    exportCoord(solution_set, "/home/mlz22/OneDrive/tmp/splay_lines.csv");
    std::vector<coord> line_density_minima_local;
    for (auto &vector: solution_set) {
        line_density_minima_local.emplace_back(coord{vector.x, vector.y});
    }

    if (line_density_minima_local.empty()) {
        std::cout << "No splay seeding lines found, which indicates a pattern being composed of +1 defects. "
                     "Proceeding with dual seeding. " << std::endl;
        return;
    }
    std::vector<std::vector<coord>> separated_lines_of_minimal_density = separateIntoLines(line_density_minima_local,
                                                                                           {0, 0}, sqrt(2));
    if (separated_lines_of_minimal_density.size() > 1) {
        std::cout << " \t" << separated_lines_of_minimal_density.size() << " splay seeding lines found."
                  << std::endl;
    } else {
        std::cout << "\t 1 splay seeding line found." << std::endl;
    }
    lines_of_minimal_density = separated_lines_of_minimal_density;
}

const std::vector<std::vector<coord>> &DesiredPattern::getLineDensityMinima() const {
    return lines_of_minimal_density;
}

int DesiredPattern::getSortingMethod() const {
    return sorting_method;
}

int DesiredPattern::getDiscontinuityBehaviour() const {
    return discontinuity_behaviour;
}

double DesiredPattern::getDiscontinuityThresholdCos() const {
    return discontinuity_threshold_cos;
}

double DesiredPattern::getMinimalLineLength() const {
    return minimal_line_length;
}

bool DesiredPattern::isPointsRemoved() const {
    return is_points_removed;
}
