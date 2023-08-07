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

#include "importing_and_exporting/table_reading.h"
#include "auxiliary/simple_math_operations.h"
#include "auxiliary/perimeter.h"
#include "auxiliary/line_operations.h"
#include "auxiliary/line_thinning.h"
#include "auxiliary/valarray_operations.h"
#include "auxiliary/vector_operations.h"
#include "auxiliary/configuration_reading.h"
#include "vector_slicer_config.h"

const double SPLAY_SINGULARITY_THRESHOLD = 0.005;

// TODO Write a preprocessing function that removes margins from the pattern
// TODO Make all functions out-of-bounds safe so that the margins are not required (OOB = unfilled)


DesiredPattern::DesiredPattern() = default;


DesiredPattern::DesiredPattern(std::vector<veci> shape_field, std::vector<vecd> x_field,
                               std::vector<vecd> y_field) :
        shape_matrix(std::move(shape_field)),
        x_field_preferred(std::move(x_field)),
        y_field_preferred(std::move(y_field)),
        dimensions(getTableDimensions(shape_field)) {

    is_vector_filled = readKeyBool(FILLING_CONFIG, "is_vector_filling_enabled");
    is_vector_sorted = readKeyBool(FILLING_CONFIG, "is_vector_sorting_enabled");
}


void DesiredPattern::updateProperties() {
    if (!isSplayProvided()) {
        splay_vector_array = splayVector(x_field_preferred, y_field_preferred);
        splay_array = vectorArrayNorm(splay_vector_array);
    }
    adjustMargins();
    int bin_number = std::min(shape_matrix.size(), shape_matrix[0].size());
    splay_sorted_empty_spots = binBySplay(100);
    perimeter_list = findSeparatedPerimeters(shape_matrix, dimensions, splay_vector_array);
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

DesiredPattern::DesiredPattern(const std::string &shape_filename, const std::string &x_field_filename,
                               const std::string &y_field_filename) :
        DesiredPattern(readFileToTableInt(shape_filename), readFileToTableDouble(x_field_filename),
                       readFileToTableDouble(y_field_filename)) {}


DesiredPattern::DesiredPattern(const std::string &shape_filename, const std::string &theta_field_filename) {
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
    *this = DesiredPattern(readFileToTableInt(shape_filename), x_field, y_field);
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
    splay_array = vectorArrayNorm(splay_vector_array);
    is_splay_provided = true;
}


vecd DesiredPattern::getDirector(const vecd &position, double distance) const {
    vald direction = getDirector(vald(position.data(), position.size())) * distance;
    return {std::begin(direction), std::end(direction)};
}

veci
DesiredPattern::findPointOfMinimumDensity(std::set<veci> &candidate_set, bool &is_valid, vecd current_coordinates) {
    vecd starting_coordinates = current_coordinates;
    vecd starting_vector = getDirector(current_coordinates, sqrt(2));

    std::set<veci> current_set;
    veci coordinate_splay_minimum = dtoi(current_coordinates);
    double minimum_splay = DBL_MAX;

    vecd current_displacement = starting_vector;
    bool is_forward_path_valid = true;
    while (true) {
        veci current_coordinates_i = dtoi(current_coordinates);
        current_set.insert(current_coordinates_i);
        candidate_set.erase(current_coordinates_i);
        current_coordinates = add(current_coordinates, current_displacement);

        double current_splay = getSplay(vectoval(current_coordinates_i));
        if (current_splay < minimum_splay) {
            minimum_splay = current_splay;
            coordinate_splay_minimum = current_coordinates_i;
        }

        if (current_splay > last_bin_splay) {
            vecd current_splay_vector = getSplayDirection(current_coordinates, 1);
            if (dot(current_splay_vector, current_displacement) > 0) {
                is_forward_path_valid = false;
            }
            break;
        } else if (!isInShape(vectoval(current_coordinates))) {
            is_forward_path_valid = false;
            break;
        }

        vecd previous_displacement = current_displacement;
        current_displacement = getDirector(current_coordinates, sqrt(2));
        if (dot(current_displacement, previous_displacement) < 0) {
            current_displacement = scale(current_displacement, -1);
        }
    }

    current_coordinates = starting_coordinates;
    current_displacement = scale(starting_vector, -1);
    bool is_backward_path_valid = true;
    while (true) {
        veci current_coordinates_i = dtoi(current_coordinates);
        current_set.insert(current_coordinates_i);
        candidate_set.erase(current_coordinates_i);
        current_coordinates = add(current_coordinates, current_displacement);

        double current_splay = getSplay(vectoval(current_coordinates_i));
        if (current_splay < minimum_splay) {
            minimum_splay = current_splay;
            coordinate_splay_minimum = current_coordinates_i;
        }

        if (current_splay > last_bin_splay) {
            vecd current_splay_vector = getSplayDirection(current_coordinates, 1);
            if (dot(current_splay_vector, current_displacement) > 0) {
                is_backward_path_valid = false;

            }
            break;
        } else if (!isInShape(vectoval(current_coordinates))) {
            is_backward_path_valid = false;
            break;
        }

        vecd previous_displacement = current_displacement;
        current_displacement = getDirector(current_coordinates, sqrt(2));
        if (dot(current_displacement, previous_displacement) < 0) {
            current_displacement = scale(current_displacement, -1);
        }
    }
    is_valid = is_backward_path_valid && is_forward_path_valid;
    return coordinate_splay_minimum;
}


std::set<veci> DesiredPattern::fillablePointsSet() {
    std::vector<std::vector<vald>> normalised_splay_vector = normalizeVectorArray(splay_vector_array);
    std::vector<std::vector<double>> normalised_splay_divergence = divergence(normalised_splay_vector);

    std::set<veci> candidate_set;

    for (int i = 0; i < normalised_splay_divergence.size(); i++) {
        for (int j = 0; j < normalised_splay_divergence[i].size(); j++) {
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

        veci point_of_minimum_density = findPointOfMinimumDensity(candidate_set, is_valid, current_coordinates);

        if (is_valid) {
            solution_set.insert(point_of_minimum_density);
        }
    }
    std::cout << "Search for points of minimum line density complete" << std::endl;

    solution_set = skeletonize(solution_set);
    std::cout << "Skeletonization complete" << std::endl;
    std::vector<veci> line_density_minima_vectors(solution_set.begin(), solution_set.end());
    std::vector<vali> line_density_minima_local;
    for (auto &vector: line_density_minima_vectors) {
        line_density_minima_local.emplace_back(vectoval(vector));
    }
    std::ofstream line_density_minima_file("/home/mlz22/OneDrive/Projects/Slicer/Notebooks/line_density_minima.csv");
    if (line_density_minima_file.is_open()) {
        for (auto &line: line_density_minima_local) {
            line_density_minima_file << line[0] << "," << line[1] << std::endl;
        }
        line_density_minima_file.close();
    }

    if (line_density_minima_local.empty()) {
        return;
    }
    std::vector<std::vector<vali>> separated_lines_of_minimal_density = separateIntoLines(line_density_minima_local,
                                                                                          {0, 0}, sqrt(2));
    std::cout << " \tNumber of separated divergence-lines: " << separated_lines_of_minimal_density.size() << std::endl;
    lines_of_minimal_density = separated_lines_of_minimal_density;
}

vecd DesiredPattern::getSplayDirection(const vecd &position, double length) const {
    vald splay_val = splay_vector_array[(int) position[0]][(int) position[1]];
    vecd splay_vec = normalize(valtovec(splay_val));
    return scale(splay_vec, length);
}

double DesiredPattern::splay(const vecd &position) const {
    return splay_array[(int) position[0]][(int) position[1]];
}

bool DesiredPattern::isLowSplay(const vald &coordinates) const {
    return getSplay(dtoi(coordinates)) <= last_bin_splay;
}

const std::vector<std::vector<vali>> &DesiredPattern::getLineDensityMinima() const {
    return lines_of_minimal_density;
}
