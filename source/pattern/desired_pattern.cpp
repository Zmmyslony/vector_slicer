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

#include "desired_pattern.h"

#include <utility>
#include <algorithm>
#include <set>
#include <iostream>
#include <fstream>

#include "importing_and_exporting/table_reading.h"
#include "auxiliary/simple_math_operations.h"
#include "auxiliary/perimeter.h"
#include "auxiliary/valarray_operations.h"
#include "auxiliary/vector_operations.h"
#include "auxiliary/configuration_reading.h"
#include "vector_slicer_config.h"


DesiredPattern::DesiredPattern() = default;


DesiredPattern::DesiredPattern(std::vector<veci> shape_field, std::vector<vecd> x_field,
                               std::vector<vecd> y_field) :
        shape_matrix(std::move(shape_field)),
        x_field_preferred(std::move(x_field)),
        y_field_preferred(std::move(y_field)),
        dimensions(getTableDimensions(shape_field)) {

    perimeter_list = findSeparatedPerimeters(shape_matrix, dimensions);
    is_vector_filled = readKeyBool(FILLING_CONFIG, "is_vector_filling_enabled");
    is_vector_sorted = readKeyBool(FILLING_CONFIG, "is_vector_sorting_enabled");
    maximal_repulsion_angle = readKeyDouble(FILLING_CONFIG, "maximum_repulsion_angle");
}


void DesiredPattern::updateSplayProperties() {
    if (!isSplayProvided()) {
        splay_vector_array = splayVector(x_field_preferred, y_field_preferred);
        splay_array = normalizeVectorArray(splay_vector_array);
        splay_vector_array.clear();
    } else if (!isSplayGradientProvided()) {
        splay_gradient = gradient(splay_array);
    }
    int bin_number = std::min(shape_matrix.size(), shape_matrix[0].size());
    splay_sorted_empty_spots = binBySplay(100);
    if (isSplayProvided()) {
        findLineDensityMinima();
    }
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


std::valarray<int> DesiredPattern::preferredDirection(const std::valarray<int> &position, int distance) const {
    return std::valarray<int>{roundUp(distance * x_field_preferred[position[0]][position[1]]),
                              roundUp(distance * y_field_preferred[position[0]][position[1]])};
}


std::valarray<double> DesiredPattern::preferredDirection(const std::valarray<double> &position, int distance) const {
    double x_position_fraction = decimalPart(position[0]);
    double y_position_fraction = decimalPart(position[1]);
    unsigned int x_position = (int) floor(position[0]);
    unsigned int y_position = (int) floor(position[1]);
    double x_field = (x_position_fraction * y_position_fraction * x_field_preferred[x_position][y_position] +
                      (1 - x_position_fraction) * y_position_fraction * x_field_preferred[x_position + 1][y_position] +
                      (1 - x_position_fraction) * (1 - y_position_fraction) *
                      x_field_preferred[x_position + 1][y_position + 1] +
                      x_position_fraction * (1 - y_position_fraction) * x_field_preferred[x_position][y_position + 1]);

    double y_field = (x_position_fraction * y_position_fraction * y_field_preferred[x_position][y_position] +
                      (1 - x_position_fraction) * y_position_fraction * y_field_preferred[x_position + 1][y_position] +
                      (1 - x_position_fraction) * (1 - y_position_fraction) *
                      y_field_preferred[x_position + 1][y_position + 1] +
                      x_position_fraction * (1 - y_position_fraction) * y_field_preferred[x_position][y_position + 1]);

    if (x_field == 0 && y_field == 0) {
        return std::valarray<double>{x_field_preferred[x_position - 1][y_position],
                                     y_field_preferred[x_position - 1][y_position]};
    }
    std::valarray<double> new_step = {x_field, y_field};
    new_step = distance * normalize(new_step);
    return new_step;
}


bool DesiredPattern::isInShape(const std::valarray<int> &position) const {
    return 0 <= position[0] && 0 <= position[1] && shape_matrix[position[0]][position[1]];
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

double DesiredPattern::getMaximalRepulsionAngle() const {
    return maximal_repulsion_angle;
}

const vald &DesiredPattern::getSplayGradient(const vali &positions) const {
    return splay_gradient[positions[0]][positions[1]];
}

const vald &DesiredPattern::getSplayGradient(const vald &positions) const {
    return getSplayGradient(dtoi(positions));
}

bool DesiredPattern::isSplayProvided() const {
    return is_splay_provided;
}

bool DesiredPattern::isSplayGradientProvided() const {
    return is_splay_gradient_provided;
}

void DesiredPattern::setSplayVector(const std::string &path) {
    splay_vector_array = readFileToTableDoubleVector(path);
    splay_array = normalizeVectorArray(splay_vector_array);
    is_splay_provided = true;
}

void DesiredPattern::setSplayGradient(const std::string &path) {
    splay_gradient = readFileToTableDoubleVector(path);
    is_splay_gradient_provided = true;
}

vecd DesiredPattern::preferredDirection(const vecd &position, int distance) const {
    vald direction = preferredDirection(vald(position.data(), position.size()), distance);
    return {std::begin(direction), std::end(direction)};
}

veci
DesiredPattern::findPointOfMinimumDensity(std::set<veci> &candidate_set, bool &is_valid, vecd current_coordinates) {
    vecd previous_displacement = getSplayDirection(current_coordinates, 1);

    // If we start in the minimum of line density we need to start away from it, so that the later algorithm will work
    if (norm(previous_displacement) == 0) {
        previous_displacement = preferredDirection(current_coordinates, 2);
        current_coordinates = add(current_coordinates, previous_displacement);
        previous_displacement = preferredDirection(current_coordinates, 1);
    }

    veci line_minimum = dtoi(current_coordinates);
    vecd previous_coordinates = current_coordinates;

    while (true) {
        vecd displacement = getSplayDirection(current_coordinates, 1);
        if (dot(displacement, previous_displacement) < 0) {
            is_valid = true;
            line_minimum = dtoi(previous_coordinates);
            break;
        }
        previous_coordinates = current_coordinates;
        if (norm(displacement) > 0) {
            previous_displacement = displacement;
            current_coordinates = add(current_coordinates, displacement);
        } else {
            current_coordinates = add(current_coordinates, previous_displacement);
        }

        if (splay(current_coordinates) > last_bin_splay ||
            !isInShape(vectoval(current_coordinates))) {
            is_valid = false;
            break;
        }

        candidate_set.erase(dtoi(current_coordinates));
    }
    return line_minimum;
}

void DesiredPattern::findLineDensityMinima() {
    std::vector<vali> candidate_coordinates = splay_sorted_empty_spots.back();
    std::set<veci> candidate_set;
    for (auto &vali_coordinates: candidate_coordinates) {
        candidate_set.insert(veci(std::begin(vali_coordinates), std::end(vali_coordinates)));
    }

    std::set<veci> solution_set;
    while (!candidate_set.empty()) {
        veci first_coordinate = *candidate_set.begin();
        candidate_set.erase(first_coordinate);
        bool is_valid = true;

        vecd current_coordinates = itod(first_coordinate);
        vecd previous_displacement = preferredDirection(current_coordinates, 1);

        veci point_of_minimum_density = findPointOfMinimumDensity(candidate_set, is_valid, current_coordinates);

        if (is_valid) {
            solution_set.insert(point_of_minimum_density);
        }
    }
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
    std::vector<vali> sorted_lines_of_minimal_density = sortPoints(line_density_minima_local, {0, 0});
    std::vector<std::vector<vali>> separated_lines_of_minimal_density = separateLines(sorted_lines_of_minimal_density,sqrt(2));

    for (auto &line: separated_lines_of_minimal_density) {
        if (line.size() > 2) {
            lines_of_minimal_density.push_back(line);
        }
    }

}

vecd DesiredPattern::getSplayDirection(const vecd &position, double length) const {
    vald splay_val = splay_vector_array[(int) position[0]][(int) position[1]];
    vecd splay_vec = normalize(valtovac(splay_val));
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
