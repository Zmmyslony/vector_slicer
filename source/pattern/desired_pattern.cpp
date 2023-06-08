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
    splay_vector_array = splayVector(x_field_preferred, y_field_preferred);
    splay_array = normalizeVectorArray(splay_vector_array);
    int maximal_size = std::max(shape_matrix.size(), shape_matrix[0].size());
    splay_sorted_empty_spots = binBySplay(maximal_size);
//    splay_gradient = gradient(splay_array);
    is_vector_filled = readKeyBool(FILLING_CONFIG, "is_vector_filling_enabled");
    is_vector_sorted = readKeyBool(FILLING_CONFIG, "is_vector_sorting_enabled");
    maximal_repulsion_angle = readKeyDouble(FILLING_CONFIG, "maximum_repulsion_angle");
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
    if (max_splay == 0) {
        return {unsorted_coordinates};
    }

    std::vector<std::vector<vali>> binned_coordinates(bins);
    first_bin_splay = max_splay / bins;
    for (int i = 0; i < unsorted_coordinates.size(); i++) {
        auto bin = (unsigned int) ((double) bins * (max_splay - coordinates_splay[i]) / max_splay);
        if (bin == bins) {
            bin = bins - 1;
        }
        binned_coordinates[bin].push_back(unsorted_coordinates[i]);
    }
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
    findLineDensityMinima();
}

void DesiredPattern::setSplayGradient(const std::string &path) {
    splay_gradient = readFileToTableDoubleVector(path);
    is_splay_gradient_provided = true;
}

vecd DesiredPattern::preferredDirection(const vecd &position, int distance) const {
    vald direction = preferredDirection(vald(position.data(), position.size()), distance);
    return {std::begin(direction), std::end(direction)};
}

std::set<veci>
DesiredPattern::findLineDensityInDirection(std::set<veci> &candidate_set, bool &is_valid, vecd current_coordinates,
                                           vecd previous_displacement, std::set<veci> solution_set,
                                           std::set<veci> incorrect_set) {
    std::set<veci> current_set = {dtoi(current_coordinates)};

    while (true) {
        vecd displacement = preferredDirection(current_coordinates, 1);
        if (dot(displacement, previous_displacement) < 0) {
            displacement = scale(displacement, -1);
        }
        previous_displacement = displacement;
        vecd previous_coordinates = current_coordinates;
        current_coordinates = add(current_coordinates, displacement);

        if (splay(current_coordinates) > first_bin_splay) {
            vecd splay_vec = getSplayDirection(current_coordinates, 1);
            if (dot(splay_vec, displacement) < 0) {
                is_valid = true;
            } else {
                is_valid = false;
            }
            break;
        } else if (solution_set.find(dtoi(current_coordinates)) != solution_set.end()) {
            is_valid = true;
            break;
        } else if (incorrect_set.find(dtoi(current_coordinates)) != incorrect_set.end()) {
            is_valid = false;
            break;
        } else if (current_set.find(dtoi(current_coordinates)) != current_set.end() &&
                   current_coordinates != previous_coordinates) {
            is_valid = true;
            break;
        } else if (!isInShape(vectoval(current_coordinates))) {
            is_valid = true;
            break;
        }
        current_set.insert(dtoi(current_coordinates));
        candidate_set.erase(dtoi(current_coordinates));
    }
    return current_set;
}


void DesiredPattern::findLineDensityMinima() {
    std::vector<vali> candidate_coordinates = splay_sorted_empty_spots.back();
    std::set<veci> candidate_set;
    for (auto &vali_coordinates: candidate_coordinates) {
        candidate_set.insert(veci(std::begin(vali_coordinates), std::end(vali_coordinates)));
    }

    std::set<veci> solution_set;
    std::set<veci> incorrect_set;
    while (!candidate_set.empty()) {
        veci first_coordinate = *candidate_set.begin();
        candidate_set.erase(first_coordinate);
        bool is_forward_path_valid = true;
        bool is_backward_path_valid = true;

        vecd current_coordinates = itod(first_coordinate);
        vecd previous_displacement = preferredDirection(current_coordinates, 1);

        std::set<veci> forward_set = findLineDensityInDirection(candidate_set, is_forward_path_valid,
                                                                current_coordinates, previous_displacement,
                                                                solution_set, incorrect_set);

        std::set<veci> backward_set = findLineDensityInDirection(candidate_set, is_forward_path_valid,
                                                                 current_coordinates, scale(previous_displacement, -1),
                                                                 solution_set, incorrect_set);


        bool is_valid_candidate = is_forward_path_valid && is_backward_path_valid;
        if (is_valid_candidate) {
            solution_set.insert(forward_set.begin(), forward_set.end());
            solution_set.insert(backward_set.begin(), backward_set.end());
        } else {
            incorrect_set.insert(forward_set.begin(), forward_set.end());
            incorrect_set.insert(backward_set.begin(), backward_set.end());
        }
    }
    std::vector<veci> line_density_minima_vectors(solution_set.begin(), solution_set.end());
    std::vector<vali> line_density_minima_local;
    for (auto &vector: line_density_minima_vectors) {
        line_density_minima_local.emplace_back(vectoval(vector));
    }
    line_density_minima = line_density_minima_local;
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
     return splay_array[(int)coordinates[0]][(int)coordinates[1]] < first_bin_splay;
}

const std::vector<vali> &DesiredPattern::getLineDensityMinima() const {
    return line_density_minima;
}

void DesiredPattern::updateSplayGradient() {
    if (!isSplayProvided()) {
        splay_gradient = gradient(splay_array);
    }
}
