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

#define _USE_MATH_DEFINES

#include "desired_pattern.h"

#include <utility>
#include <algorithm>
#include <set>
#include <iostream>
#include <math.h>
#include <cfloat>
#include <iterator>

#include "importing_and_exporting/table_reading.h"
#include "auxiliary/simple_math_operations.h"
#include "auxiliary/perimeter.h"
#include "auxiliary/line_operations.h"
#include "auxiliary/line_thinning.h"
#include "auxiliary/valarray_operations.h"
#include "auxiliary/vector_operations.h"
#include "simulation/configuration_reading.h"
#include "vector_slicer_config.h"


DesiredPattern::DesiredPattern() = default;


DesiredPattern::DesiredPattern(std::vector<veci> shape_field, std::vector<vecd> x_field, std::vector<vecd> y_field,
                               bool is_splay_filling_enabled, int threads) :
        shape_matrix(std::move(shape_field)),
        x_field_preferred(std::move(x_field)),
        y_field_preferred(std::move(y_field)),
        dimensions(getTableDimensions(shape_field)),
        is_splay_filling_enabled(is_splay_filling_enabled) {

    is_vector_filled = readKeyBool(FILLING_CONFIG, "is_vector_filling_enabled");
    is_vector_sorted = readKeyBool(FILLING_CONFIG, "is_vector_sorting_enabled");
    sorting_method = readKeyInt(FILLING_CONFIG, "sorting_method");
}


DesiredPattern::DesiredPattern(const std::string &shape_filename, const std::string &x_field_filename,
                               const std::string &y_field_filename, bool is_splay_filling_enabled, int threads) :
        DesiredPattern(readFileToTableInt(shape_filename), readFileToTableDouble(x_field_filename),
                       readFileToTableDouble(y_field_filename), is_splay_filling_enabled, threads) {}


DesiredPattern::DesiredPattern(const std::string &shape_filename, const std::string &theta_field_filename,
                               bool is_splay_filling_enabled, int threads) {
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
    *this = DesiredPattern(readFileToTableInt(shape_filename), x_field, y_field, is_splay_filling_enabled, threads);
}

void DesiredPattern::updateProperties() {
    if (!isSplayProvided()) {
        splay_vector_array = splayVector(x_field_preferred, y_field_preferred, threads);
        splay_array = vectorArrayNorm(splay_vector_array, threads);
        if (shape_matrix.size() != splay_array.size()) {
            throw std::runtime_error("Incompatible x-size of splay array and shape array.");
        } else if (shape_matrix.front().size() != splay_array.front().size()) {
            throw std::runtime_error("Incompatible y-size of splay array and shape array.");
        }
    }
    adjustMargins();
    if (is_splay_filling_enabled) {
        findLineDensityMinima();
    }
    int bin_number = std::min(shape_matrix.size(), shape_matrix[0].size()) / 10;
    splay_sorted_empty_spots = binBySplay(bin_number);
    perimeter_list = findSeparatedPerimeters(shape_matrix, dimensions, splay_vector_array);
    is_pattern_updated = true;
}

void DesiredPattern::isPatternUpdated() const {
    if (!is_pattern_updated) {
        throw std::runtime_error("Desired pattern has not been updated prior to using it for filling.");
    }
}


void DesiredPattern::adjustMargins() {
    veci null_rows = findNullRows(shape_matrix);
    veci null_columns = findNullColumns(shape_matrix);

    adjustRowsAndColumns(shape_matrix, null_rows, null_columns);
    adjustRowsAndColumns(x_field_preferred, null_rows, null_columns);
    adjustRowsAndColumns(y_field_preferred, null_rows, null_columns);
    adjustRowsAndColumns(splay_vector_array, null_rows, null_columns);
    adjustRowsAndColumns(splay_array, null_rows, null_columns);

    dimensions = getTableDimensions(shape_matrix);
}


vald DesiredPattern::getDirector(vali positions) const {
    while (positions[0] < 0) {
        positions[0]++;
    }
    while (positions[0] >= dimensions[0]) {
        positions[0]--;
    }
    while (positions[1] < 0) {
        positions[1]++;
    }
    while (positions[1] >= dimensions[1]) {
        positions[1]--;
    }
    return vald({x_field_preferred[positions[0]][positions[1]],
                 y_field_preferred[positions[0]][positions[1]]});
}


vald DesiredPattern::getDirector(const vald &positions) const {
    int x_base = (int) positions[0];
    int y_base = (int) positions[1];
    double x_fraction = 1 - (positions[0] - floor(positions[0]));
    double y_fraction = 1 - (positions[1] - floor(positions[1]));

    vald director = {0, 0};
    director += x_fraction * y_fraction * getDirector(vali{x_base, y_base});
    if (x_fraction < 1) {
        director += (1 - x_fraction) * y_fraction * getDirector(vali{x_base + 1, y_base});
    }
    if (x_fraction < 1 && y_fraction < 1) {
        director += (1 - x_fraction) * (1 - y_fraction) * getDirector(vali{x_base + 1, y_base + 1});
    }
    if (y_fraction < 1) {
        director += x_fraction * (1 - y_fraction) * getDirector(vali{x_base, y_base + 1});
    }

    return director;
}


bool DesiredPattern::isInShape(const std::valarray<int> &position) const {
    return 0 <= position[0] && position[0] < dimensions[0] &&
           0 <= position[1] && position[1] < dimensions[1] &&
           shape_matrix[position[0]][position[1]];
}


bool DesiredPattern::isInShape(const std::valarray<double> &position) const {
    return isInShape(dtoi(position));
}


const std::valarray<int> &DesiredPattern::getDimensions() const {
    return dimensions;
}


const std::vector<std::vector<std::valarray<int>>> &DesiredPattern::getPerimeterList() const {
    return perimeter_list;
}


const std::vector<veci> &DesiredPattern::getShapeMatrix() const {
    return shape_matrix;
}


const std::vector<vecd> &DesiredPattern::getXFieldPreferred() const {
    return x_field_preferred;
}


const std::vector<vecd> &DesiredPattern::getYFieldPreferred() const {
    return y_field_preferred;
}


double DesiredPattern::getSplay(const vali &point) const {
    return splay_array[point[0]][point[1]];
}

std::vector<vali> findFillableCells(const std::vector<veci> &shape_matrix) {
    std::vector<vali> fillable_cells;
    for (int i = 0; i < shape_matrix.size(); i++) {
        for (int j = 0; j < shape_matrix[i].size(); j++) {
            if (shape_matrix[i][j]) {
                fillable_cells.push_back({i, j});
            }
        }
    }
    return fillable_cells;
}


std::vector<std::vector<vali>> DesiredPattern::binBySplay(unsigned int bins) {
    std::vector<vali> unsorted_coordinates = findFillableCells(shape_matrix);
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

    std::vector<std::vector<vali>> binned_coordinates(bins);
    last_bin_splay = max_splay / (bins - 1);
    for (int i = 0; i < unsorted_coordinates.size(); i++) {
        unsigned int bin = (double) (bins - 1) * (coordinates_splay[i] - min_splay) / (max_splay - min_splay);
        binned_coordinates[bin].push_back(unsorted_coordinates[i]);
    }
    std::reverse(binned_coordinates.begin(), binned_coordinates.end());
    return binned_coordinates;
}

const std::vector<std::vector<vali>> &DesiredPattern::getSplaySortedEmptySpots() const {
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


vecd DesiredPattern::getDirector(const vecd &position, double distance) const {
    vald direction = getDirector(vald(position.data(), position.size())) * distance;
    return {std::begin(direction), std::end(direction)};
}


veci DesiredPattern::findInnerPointsOfMinimumDensity(std::set<veci> &candidate_set, bool &is_minimum_density,
                                                     vecd current_coordinates) {
    vecd current_displacement = getSplayDirection(current_coordinates, 1);
    is_minimum_density = false;
    veci current_coordinates_i;
    while (true) {
        current_coordinates_i = dtoi(current_coordinates);
        candidate_set.erase(current_coordinates_i);
        current_coordinates = add(current_coordinates, current_displacement);

        if (!isInShape(vectoval(current_coordinates))) {
            is_minimum_density = false;
            break;
        }

        vecd previous_displacement = current_displacement;
        double current_splay = getSplay(current_coordinates);
        current_displacement = getSplayDirection(current_coordinates, 1);
        // In order to avoid numerical errors, we arbitrarily set the threshold for zero splay
        if (current_splay < 1e-10) {
            current_displacement = getDirector(current_coordinates, 1);
            if (dot(current_displacement, previous_displacement) < 0) {
                current_displacement = scale(current_displacement, -1);
            }
        }
        double dot_product = dot(previous_displacement, current_displacement);
        if (dot_product < 0) {
            is_minimum_density = true;
            break;
        }
        if (current_splay > 1e-3) {
            break;
        }
    }

    return current_coordinates_i;
}


std::set<veci> DesiredPattern::fillablePointsSet() {
    std::set<veci> candidate_set;
    for (int i = 0; i < shape_matrix.size(); i++) {
        for (int j = 0; j < shape_matrix[i].size(); j++) {
            vali coordinates = {i, j};
            if (isInShape(coordinates)) {
                candidate_set.insert(valtovec(coordinates));
            }
        }
    }
    return candidate_set;
}


void DesiredPattern::findLineDensityMinima() {
    std::set<veci> candidate_set = fillablePointsSet();

    std::set<veci> solution_set;
    while (!candidate_set.empty()) {
        veci first_coordinate = *candidate_set.begin();
        candidate_set.erase(first_coordinate);
        bool is_valid = true;

        vecd current_coordinates = itod(first_coordinate);
        vecd previous_displacement = getDirector(current_coordinates, 1);

        veci point_of_minimum_density = findInnerPointsOfMinimumDensity(candidate_set, is_valid, current_coordinates);

        if (is_valid) {
            solution_set.insert(point_of_minimum_density);
        }
    }
    std::cout << "Search for points of minimum line density complete." << std::endl;

    solution_set = skeletonize(solution_set, 10, 1);
    std::cout << "Skeletonisation complete." << std::endl;
    std::vector<veci> line_density_minima_vectors(solution_set.begin(), solution_set.end());
    std::vector<vali> line_density_minima_local;
    for (auto &vector: line_density_minima_vectors) {
        line_density_minima_local.emplace_back(vectoval(vector));
    }

    if (line_density_minima_local.empty()) {
        return;
    }
    std::vector<std::vector<vali>> separated_lines_of_minimal_density = separateIntoLines(line_density_minima_local,
                                                                                          {0, 0}, sqrt(2));
    std::cout << " \t" << separated_lines_of_minimal_density.size() << " separate lines of divergent origin found."
              << std::endl;
    lines_of_minimal_density = separated_lines_of_minimal_density;
}

vecd DesiredPattern::getSplayDirection(const vecd &position, double length) const {
    vald splay_val = splay_vector_array[(int) position[0]][(int) position[1]];
    vecd splay_vec = normalize(valtovec(splay_val));
    return scale(splay_vec, length);
}

double DesiredPattern::getSplay(const vecd &position) const {
    return splay_array[(int) position[0]][(int) position[1]];
}

bool DesiredPattern::isLowSplay(const vald &coordinates) const {
    return getSplay(dtoi(coordinates)) <= last_bin_splay;
}

const std::vector<std::vector<vali>> &DesiredPattern::getLineDensityMinima() const {
    return lines_of_minimal_density;
}

int DesiredPattern::getSortingMethod() const {
    return sorting_method;
}
