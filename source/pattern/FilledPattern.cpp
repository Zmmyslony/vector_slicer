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
// You should have received a copy of the GNU General Public License along with Vector Slicer. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 21/09/2021.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc51-cpp"

#include "FilledPattern.h"

#include "../importing_and_exporting/Exporting.h"
#include "../auxiliary/Geometry.h"
#include "../auxiliary/Perimeter.h"
#include "../auxiliary/SimpleMathOperations.h"
#include "../auxiliary/ValarrayOperations.h"
#include "../auxiliary/vector_operations.h"


FilledPattern::FilledPattern(const DesiredPattern &new_desired_pattern, FillingConfig new_config) :
        desired_pattern(std::cref(new_desired_pattern)),
        FillingConfig(new_config) {
    int x_dim = desired_pattern.get().getDimensions()[0];
    int y_dim = desired_pattern.get().getDimensions()[1];

    number_of_times_filled = std::vector<std::vector<int >>(x_dim, std::vector<int>(y_dim));
    x_field_filled = std::vector<std::vector<double >>(x_dim, std::vector<double>(y_dim));
    y_field_filled = std::vector<std::vector<double >>(x_dim, std::vector<double>(y_dim));
    x_distribution = std::uniform_int_distribution<int>(0, x_dim - 1);
    y_distribution = std::uniform_int_distribution<int>(0, y_dim - 1);
    setup();
}

void FilledPattern::setup() {
    print_circle = findPointsInCircle(getPrintRadius());
    repulsion_circle = findPointsInCircle(getPrintRadius() + getRepulsionRadius());
    fillable_points = findStartingRootPoints(getInitialFillingMethod());
    collision_list = generatePerimeterList(getCollisionRadius());
    random_engine = std::mt19937(getSeed());
    unsigned int number_of_fillable_points = fillable_points.size();
    distribution = std::uniform_int_distribution<unsigned int>(0, number_of_fillable_points - 1);
}


FilledPattern::FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius,
                             int step_length, unsigned int seed) :
        FilledPattern(desired_pattern,
                      FillingConfig(Perimeter, collision_radius, 2 * print_radius, 1.0, step_length, print_radius,
                                    0,
                                    seed)) {
}


FilledPattern::FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius,
                             int step_length)
        :
        FilledPattern::FilledPattern(desired_pattern, print_radius, collision_radius, step_length, 0) {}


std::vector<vali> FilledPattern::findAllFillablePoints() const {
    std::vector<vali> new_points_to_fill;
    for (int i = 0; i < desired_pattern.get().getDimensions()[0]; i++) {
        for (int j = 0; j < desired_pattern.get().getDimensions()[1]; j++) {
            vali current_pos = {i, j};
            if (isFillable(current_pos)) {
                new_points_to_fill.emplace_back(current_pos);
            }
        }
    }
    return new_points_to_fill;
}


std::vector<vali> FilledPattern::findStartingRootPoints(fillingMethod method) {
    std::vector<vali> stem_points = {};

    switch (method) {
        case Perimeter:
            stem_points = getSpacedLine(getStartingPointSeparation(), desired_pattern.get().getPerimeterList());
            binned_fillable_points = binBySplay(fillable_points, 100);
            break;
        case Dual:
            break;
    }
    return stem_points;
}

void FilledPattern::updateFillablePoints() {
    fillable_points = findAllFillablePoints();
    if (!fillable_points.empty()) {
        binned_fillable_points = binBySplay(fillable_points, 100);
    }
    search_stage = RandomPointSelection;
}


vald normalizeDirection(const vali &previous_step) {
    vald normalized_direction = normalize(previous_step);
    if (previous_step[0] > 0 || previous_step[0] == 0 && previous_step[1] > 0) {
        return normalized_direction;
    } else {
        return -normalized_direction;
    }
}


void FilledPattern::fillPoint(const vali &point, const vald &normalized_direction, int value) {
    number_of_times_filled[point[0]][point[1]] += value;
    x_field_filled[point[0]][point[1]] += normalized_direction[0] * value;
    y_field_filled[point[0]][point[1]] += normalized_direction[1] * value;
}


void FilledPattern::fillPointsFromList(const std::vector<vali> &list_of_points, const vali &direction, int value) {
    vald normalized_direction = normalizeDirection(direction);
    for (auto &point: list_of_points) {
        fillPoint(point, normalized_direction, value);
    }
}

void FilledPattern::fillPointsFromDisplacement(const vali &starting_position,
                                               const std::vector<vali> &list_of_displacements,
                                               const vali &previous_step, int value) {
    vald normalized_direction = normalizeDirection(previous_step);
    for (auto &displacement: list_of_displacements) {
        vali point = starting_position + displacement;
        if (point[0] >= 0 && point[0] < desired_pattern.get().getDimensions()[0] && point[1] >= 0 &&
            point[1] < desired_pattern.get().getDimensions()[1]) {
            fillPoint(point, normalized_direction, value);
        }
    }
}


void FilledPattern::fillPointsFromDisplacement(const vali &starting_position,
                                               const std::vector<vali> &list_of_displacements,
                                               const vali &previous_step) {
    fillPointsFromDisplacement(starting_position, list_of_displacements, previous_step, 1);
}


vald FilledPattern::getNewStep(vald &real_coordinates, int &length, vald &previous_move) const {
    vald new_move = desired_pattern.get().preferredDirection(real_coordinates, length);
    int is_opposite_to_previous_step = sgn(new_move[0] * previous_move[0] + new_move[1] * previous_move[1]);
    if (is_opposite_to_previous_step < 0) {
        new_move = -new_move;
    }
    return new_move;
}

bool FilledPattern::tryGeneratingPathWithLength(Path &current_path, vald &positions, vald &previous_step, int length) {
    vali current_coordinates = dtoi(positions);
    vald new_step = getNewStep(positions, length, previous_step);
    vald new_positions = positions + new_step;
    vali new_coordinates = dtoi(new_positions);
    vald repulsion = {0, 0};
    if (getRepulsion() != 0) {
        repulsion = getRepulsionValue(desired_pattern.get().getShapeMatrix(), number_of_times_filled, repulsion_circle,
                                      new_coordinates, desired_pattern.get().getDimensions(), getRepulsion());
    }

    new_positions -= repulsion;
    new_coordinates = dtoi(new_positions);
    vald real_step = new_step - repulsion;

    if (new_coordinates[0] == current_coordinates[0] && new_coordinates[1] == current_coordinates[1] ||
        dot(real_step, previous_step) <= 0 || norm(real_step) <= 2) {
        return false;
    }

    if (isFillable(new_coordinates)) {
        std::vector<vali> current_points_to_fill;
        if (current_path.size() >= 2) {
            current_points_to_fill = findPointsToFill(current_path.secondToLast(), current_coordinates,
                                                      new_coordinates, getPrintRadius(), isFilled(current_coordinates));
        } else {
            current_points_to_fill = findPointsToFill(current_coordinates, new_coordinates, getPrintRadius(),
                                                      isFilled(current_coordinates));
        }
        vali new_step_int = new_coordinates - current_coordinates;
        fillPointsFromList(current_points_to_fill, new_step_int, 1);
        current_path.addPoint(new_coordinates);

        new_step = getNewStep(new_positions, length, new_step);
        positions = new_positions;
        previous_step = new_step;
        return true;
    }
    return false;
}

Path FilledPattern::generateNewPathForDirection(vali &starting_coordinates, const vali &starting_step) {
    Path new_path(starting_coordinates);
    vald current_positions = itod(starting_coordinates);
    vald current_step = itod(starting_step);

    for (int length = getStepLength(); length >= getPrintRadius(); length--) {
        while (tryGeneratingPathWithLength(new_path, current_positions, current_step, length)) {}
    }
    return new_path;
}

void FilledPattern::fillPointsInCircle(const vali &starting_coordinates) {
    fillPointsFromDisplacement(starting_coordinates, print_circle, {1, 0});
    list_of_points.push_back(starting_coordinates);
}


void FilledPattern::removePoints() {
    for (auto &position: list_of_points) {
        fillPointsFromDisplacement(position, print_circle, {1, 0}, -1);
    }
    list_of_points.clear();
}


void FilledPattern::removeLine(Path path) {
    std::vector<vali> current_points_to_fill;
    vali current_coordinates = path.sequence_of_positions[1];
    vali previous_coordinates = path.sequence_of_positions[0];
    current_points_to_fill = findPointsToFill(previous_coordinates, current_coordinates, getPrintRadius(),
                                              !isFilled(previous_coordinates));

    vali new_step_int = current_coordinates - previous_coordinates;
    fillPointsFromList(current_points_to_fill, new_step_int, -1);

    for (int i = 2; i < path.size(); i++) {
        current_coordinates = path.sequence_of_positions[i];
        previous_coordinates = path.sequence_of_positions[i - 1];
        vali second_previous_coordinates = path.sequence_of_positions[i - 2];
        current_points_to_fill = findPointsToFill(second_previous_coordinates, previous_coordinates,
                                                  current_coordinates, getPrintRadius(),
                                                  !isFilled(previous_coordinates));

        new_step_int = current_coordinates - previous_coordinates;
        fillPointsFromList(current_points_to_fill, new_step_int, -1);
    }
    fillPointsInHalfCircle(path.sequence_of_positions[0], path.sequence_of_positions[1], -1);
    fillPointsInHalfCircle(path.last(), path.secondToLast(), -1);
}


void FilledPattern::removeShortLines() {
    double minimal_length = 10 * getPrintRadius();
    for (auto &path: sequence_of_paths) {
        if (path.getLength() < minimal_length) {
            removeLine(path);
        }
    }
}

void
FilledPattern::fillPointsInHalfCircle(const vali &last_point, const vali &previous_point, int value) {
    std::vector<vali> half_circle_points = findHalfCircle(last_point, previous_point, getPrintRadius(),
                                                          isFilled(last_point));
    fillPointsFromList(half_circle_points, previous_point - last_point, value);
}

void FilledPattern::exportFilledMatrix(const fs::path &directory) const {
    fs::path filled_filename = directory / "number_of_times_filled.csv";
    exportVectorTableToFile(number_of_times_filled, filled_filename);
}

std::vector<Path> FilledPattern::getSequenceOfPaths() {
    return sequence_of_paths;
}


void FilledPattern::addNewPath(Path &new_path) {
    sequence_of_paths.push_back(new_path);
}


std::vector<vali> reshuffle(const std::vector<vali> &initial_vector, std::mt19937 &random_engine) {
    std::vector<vali> new_vector(initial_vector.size());
    std::uniform_int_distribution<unsigned int> distribution(0, initial_vector.size() - 1);
    unsigned int elements_to_push = distribution(random_engine);
    for (int i = 0; i < initial_vector.size(); i++) {
        new_vector[i] = initial_vector[(i + elements_to_push) % initial_vector.size()];
    }
    return new_vector;
}


vald FilledPattern::getDirector(const vali &positions) const {
    return vald({desired_pattern.get().getXFieldPreferred()[positions[0]][positions[1]],
                 desired_pattern.get().getYFieldPreferred()[positions[0]][positions[1]]});
}


vald FilledPattern::getDirector(const vald &positions) {
    int x_base = (int) positions[0];
    int y_base = (int) positions[1];
    double x_fraction = positions[0] - floor(positions[0]);
    double y_fraction = positions[1] - floor(positions[1]);

    vald director = {0, 0};
    director += x_fraction * y_fraction * getDirector(vali{x_base, y_base});
    director += (1 - x_fraction) * y_fraction * getDirector(vali{x_base + 1, y_base});
    director += (1 - x_fraction) * (1 - y_fraction) * getDirector(vali{x_base + 1, y_base + 1});
    director += x_fraction * (1 - y_fraction) * getDirector(vali{x_base, y_base + 1});

    return director;
}

vald normalizedDualVector(const vald &vector) {
    return normalize(perpendicular(vector));
}


std::vector<vali> FilledPattern::findDualLineOneDirection(vald coordinates, vald previous_dual_director) {
    std::vector<vali> line;
    while (isFillable(dtoi(coordinates))) {
        line.push_back(dtoi(coordinates));
        vald director = getDirector(coordinates);
        vald dual_director = normalizedDualVector(director);
        if (dot(dual_director, previous_dual_director) < 0) {
            dual_director *= -1;
        }
        coordinates += dual_director;
        previous_dual_director = dual_director;
    }
    return line;
}


std::vector<vali> FilledPattern::findDualLine(const vali &start) {
    vald real_coordinates = itod(start);
    vald previous_director = getDirector(real_coordinates);
    vald initial_dual_director = normalizedDualVector(previous_director);

    std::vector<vali> points_in_dual_line_forward;
    std::vector<vali> points_in_dual_line_backward;

    points_in_dual_line_forward = findDualLineOneDirection(real_coordinates, initial_dual_director);
    points_in_dual_line_backward = findDualLineOneDirection(real_coordinates, -initial_dual_director);

    return stitchTwoVectors(points_in_dual_line_backward, points_in_dual_line_forward);
}


std::vector<vali> FilledPattern::getSpacedLine(const double &distance, const std::vector<vali> &line) {
    std::vector<vali> reshuffled_starting_points = reshuffle(line, random_engine);

    vali previous_position = reshuffled_starting_points[0];
    vald previous_director = getDirector(previous_position);
    vald previous_double_director = normalizedDualVector(previous_director);

    std::vector<vali> separated_starting_points;
    separated_starting_points.push_back(previous_position);

    for (auto &current_position: reshuffled_starting_points) {
        double current_distance = std::abs(dot(itod(current_position - previous_position), previous_double_director));
        if (current_distance > distance) {
            previous_position = current_position;
            previous_director = getDirector(current_position);
            previous_double_director = normalizedDualVector(previous_director);
            separated_starting_points.push_back(previous_position);
        }
    }
    return separated_starting_points;
}


bool FilledPattern::isFilled(const vali &coordinates) const {
    return number_of_times_filled[coordinates[0]][coordinates[1]];
}


bool FilledPattern::isFillable(const vali &point) const {
    return desired_pattern.get().isInShape(point) &&
           !isFilled(point) &&
           isPerimeterFree(number_of_times_filled, desired_pattern.get().getShapeMatrix(),
                           collision_list, point, desired_pattern.get().getDimensions());
}


// Bins fillable points by their splay
std::vector<std::vector<vali>>
FilledPattern::binBySplay(std::vector<vali> &unsorted_coordinates, unsigned int bins) const {
    if (unsorted_coordinates.empty()) {
        return {};
    }
    std::vector<double> coordinates_splay;
    for (auto &point: unsorted_coordinates) {
        double current_splay = desired_pattern.get().getSplay(point);
        coordinates_splay.push_back(current_splay);
    }
    double max_splay = *std::max_element(coordinates_splay.begin(), coordinates_splay.end());
    if (max_splay == 0) {
        std::shuffle(unsorted_coordinates.begin(), unsorted_coordinates.end(), std::mt19937());
        return {unsorted_coordinates};
    }

    std::vector<std::vector<vali>> binned_coordinates(bins);

    for (int i = 0; i < unsorted_coordinates.size(); i++) {
        auto bin = (unsigned int) ((double) bins * (max_splay - coordinates_splay[i]) / max_splay);
        if (bin == bins) {
            bin = bins - 1;
        }
        binned_coordinates[bin].push_back(unsorted_coordinates[i]);
    }

    for (auto &bin: binned_coordinates) {
        if (!bin.empty()) {
            std::shuffle(binned_coordinates.begin(), binned_coordinates.end(), std::mt19937());
        }

    }
    return binned_coordinates;
}


vali FilledPattern::getFillablePoint() {
    while (!binned_fillable_points.empty() && binned_fillable_points.back().empty()) {
        binned_fillable_points.pop_back();
    }
    if (binned_fillable_points.empty()) {
        return {-1, -1};
    }
    vali last_element = binned_fillable_points.back().back();
    binned_fillable_points.back().pop_back();
    return last_element;
}

bool FilledPattern::isFillablePointLeft() const {
    return !binned_fillable_points.empty();
}

#pragma clang diagnostic pop