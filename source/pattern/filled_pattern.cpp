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
// Created by Michał Zmyślony on 21/09/2021.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc51-cpp"

#include "filled_pattern.h"

#include "importing_and_exporting/exporting.h"
#include "auxiliary/geometry.h"
#include "auxiliary/perimeter.h"
#include "auxiliary/repulsion.h"
#include "auxiliary/valarray_operations.h"
#include "auxiliary/vector_operations.h"

#define INVALID_POSITION {-1, -1}
#define DIRECTOR_DISCONTINUITY_THRESHOLD 0.7

FilledPattern::FilledPattern(const DesiredPattern &new_desired_pattern, FillingConfig new_config) :
        desired_pattern(std::cref(new_desired_pattern)),
        FillingConfig(new_config) {
    desired_pattern.get().isPatternUpdated();
    int x_dim = desired_pattern.get().getDimensions()[0];
    int y_dim = desired_pattern.get().getDimensions()[1];

    number_of_times_filled = std::vector<std::vector<int >>(x_dim, std::vector<int>(y_dim));
    x_field_filled = std::vector<std::vector<double >>(x_dim, std::vector<double>(y_dim));
    y_field_filled = std::vector<std::vector<double >>(x_dim, std::vector<double>(y_dim));
    setup();
}

/**
 * Takes in a list of seed lines which are sorted by adjacency (ordered), shuffles the order of lines, separates in
 * equidistant seeds and returns shuffled seeds.
 * @param list_of_lines
 * @param line_index starting line index
 * @return 2D array of Seed objects, shuffled so that they can be popped back
 */
std::vector<std::vector<SeedPoint>>
FilledPattern::separateLines(std::vector<std::vector<vali>> list_of_lines, int line_index) {
    std::vector<std::vector<SeedPoint>> separated_lines;
    std::shuffle(list_of_lines.begin(), list_of_lines.end(), random_engine);
    for (auto &line: list_of_lines) {
        std::vector<SeedPoint> spaced_line = getSpacedLine(getSeedSpacing(), line, line_index);
        std::shuffle(spaced_line.begin(), spaced_line.end(), random_engine);
        separated_lines.emplace_back(spaced_line);
        line_index++;
    }
    return separated_lines;
}

void FilledPattern::setup() {
    print_circle = findPointsInCircle(getPrintRadius());
    collision_list = generatePerimeterList(getTerminationRadius());
    random_engine = std::mt19937(getSeed());

    zero_splay_seeds = separateLines(desired_pattern.get().getLineDensityMinima(), 0);
    perimeter_seeds = separateLines(desired_pattern.get().getPerimeterList(), zero_splay_seeds.size());
    seed_lines = perimeter_seeds.size() + zero_splay_seeds.size();

    switch (getInitialSeedingMethod()) {
        case Splay:
            if (desired_pattern.get().isSplayProvided()) {
                search_stage = SplayFilling;
            } else {
                search_stage = PerimeterFilling;
            }
            break;
        case Perimeter:
            search_stage = PerimeterFilling;
            break;
        case Dual:
            search_stage = RemainingFilling;
            break;
    }
    setupRootPoints();
    updateSeedPoints();
}


FilledPattern::FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius,
                             int step_length, unsigned int seed) :
        FilledPattern(desired_pattern,
                      FillingConfig(Perimeter, collision_radius, 2 * print_radius, 1.0, step_length, print_radius,
                                    0, seed)) {
}


FilledPattern::FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius,
                             int step_length) :
        FilledPattern::FilledPattern(desired_pattern, print_radius, collision_radius, step_length, 0) {}


/**
 * Contains all points that can be used as seeds, sorted depending on their value of splay, as areas of low splay
 * ought to be more suitable for rooting a seed line.
 */
void FilledPattern::setupRootPoints() {
    std::vector<std::vector<vali>> root_points = desired_pattern.get().getSplaySortedEmptySpots();
    if (root_points.empty()) {
        return;
    }

    for (auto &bin: root_points) {
        if (!bin.empty()) {
            std::shuffle(bin.begin(), bin.end(), random_engine);
        }
    }
    binned_root_points = root_points;
}

void FilledPattern::updateSeedPoints() {
    if (search_stage == SplayFilling) {
        if (zero_splay_seeds.empty()) {
            search_stage = PerimeterFilling;
        } else {
            seed_points = zero_splay_seeds.back();
            zero_splay_seeds.pop_back();
            return;
        }
    }

    if (search_stage == PerimeterFilling) {
        if (perimeter_seeds.empty()) {
            search_stage = RemainingFilling;
        } else {
            seed_points = perimeter_seeds.back();
            perimeter_seeds.pop_back();
            return;
        }
    }

    vali root_point = findRemainingRootPoint();
    if (root_point[0] == -1) {
        seed_points = {INVALID_SEED};
        return;
    }
    std::vector<vali> dual_line = findDualLine(root_point);
    std::vector<SeedPoint> spaced_dual_line = getSpacedLine(getSeedSpacing(), dual_line, seed_lines);
    seed_lines++;
    std::shuffle(spaced_dual_line.begin(), spaced_dual_line.end(), random_engine);
    seed_points = spaced_dual_line;
}


vali FilledPattern::findRemainingRootPoint() {
    while (isFillablePointLeft()) {
        vali test_point = getFillablePoint();
        if (isFillable(test_point)) {
            return test_point;
        }
    }
    // If there are no remaining root points, return invalid position.
    return INVALID_POSITION;
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
    if (isInRange(point)) {
        number_of_times_filled[point[0]][point[1]] += value;
        x_field_filled[point[0]][point[1]] += normalized_direction[0] * value;
        y_field_filled[point[0]][point[1]] += normalized_direction[1] * value;
    }
}


void FilledPattern::fillPointsFromList(const std::vector<vali> &points_to_fill, const vali &direction, int value) {
    vald normalized_direction = normalizeDirection(direction);
    for (auto &point: points_to_fill) {
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
    vald new_move = desired_pattern.get().getDirector(real_coordinates) * length;
    double is_opposite_to_previous_step = dot(new_move, previous_move);

    if (is_opposite_to_previous_step >= 0) {
        return new_move;
    } else {
        return -new_move;
    }
}

bool is_unchanged(const vali &first_position, const vali &second_position) {
    return first_position[0] == second_position[0] && first_position[1] == second_position[1];
}

bool is_reversed(const vald &first_step, const vald &second_step) {
    return dot(first_step, second_step) <= 0;
}

vald FilledPattern::calculateNextPosition(vald &positions, vald &previous_step, int length) {
    vali current_coordinates = dtoi(positions);
    vald new_step = getNewStep(positions, length, previous_step);
    vald new_positions = positions + new_step;

    if (getRepulsion() != 0) {
        vald repulsion = getLineBasedRepulsion(desired_pattern.get().getShapeMatrix(), number_of_times_filled,
                                               new_step, getPrintRadius(),
                                               new_positions, desired_pattern.get().getDimensions(),
                                               getRepulsion(), getRepulsionAngle());
        new_positions += repulsion;
        new_step += repulsion;
    }

    vali new_coordinates = dtoi(new_positions);

    // Check if newly generated position is valid
    if (isFillable(new_coordinates) &&
        isDirectorContinuous(current_coordinates, new_coordinates) &&
        !is_unchanged(current_coordinates, new_coordinates) &&
        !is_reversed(previous_step, new_step)) {
        return new_positions;
    } else {
        return INVALID_POSITION;
    }
}

bool FilledPattern::isDirectorContinuous(const vali &previous_coordinates, const vali &new_coordinates) const {
    vald previous_director = desired_pattern.get().getDirector(previous_coordinates);
    vald new_director = desired_pattern.get().getDirector(new_coordinates);
    double product = dot(previous_director, new_director) / (norm(previous_director) * norm(new_director));

    if (desired_pattern.get().isVectorFilled()) {
        return product > DIRECTOR_DISCONTINUITY_THRESHOLD;
    } else {
        return abs(product) > DIRECTOR_DISCONTINUITY_THRESHOLD;
    }
}

bool FilledPattern::isInRange(const vali &index) const {
    return ::isInRange(index, desired_pattern.get().getDimensions());
}

bool FilledPattern::isInRange(const vald &index) const {
    return ::isInRange(dtoi(index), desired_pattern.get().getDimensions());
}

bool FilledPattern::tryGeneratingPathWithLength(Path &current_path, vald &positions, vald &previous_step, int length) {
    vali current_coordinates = dtoi(positions);
    vald new_positions = calculateNextPosition(positions, previous_step, length);

    if (!isInRange(current_coordinates)) {
        return false;
    }
    // Try creating the longest possible step
    while (length > 0 && !isInRange(new_positions)) {
        new_positions = calculateNextPosition(positions, previous_step, length);
        length--;
    }

    // Check if new position is valid
    if (!isInRange(new_positions)) {
        return false;
    }
    previous_step = new_positions - positions;
    positions = new_positions;
    vali new_coordinates = dtoi(new_positions);

    std::vector<vali> current_points_to_fill;
    if (current_path.size() >= 2) {
        current_points_to_fill = findPointsToFill(current_path.secondToLast(), current_coordinates,
                                                  new_coordinates, getPrintRadius(),
                                                  isFilled(current_coordinates));
    } else {
        current_points_to_fill = findPointsToFill(current_coordinates, new_coordinates, getPrintRadius(),
                                                  isFilled(current_coordinates));
    }
    vali new_step_int = new_coordinates - current_coordinates;
    fillPointsFromList(current_points_to_fill, new_step_int, 1);
    current_path.addPoint(new_coordinates);
    return true;
}


Path FilledPattern::generateNewPathForDirection(const SeedPoint &seed_point, const vali &starting_step) {
    Path new_path(seed_point);
    vald current_positions = itod(seed_point.getCoordinates());
    vald current_step = itod(starting_step);

    for (int length = getStepLength(); length >= getPrintRadius(); length--) {
        while (tryGeneratingPathWithLength(new_path, current_positions, current_step, length)) {
            length = getStepLength();
        }
    }
    return new_path;
}


Path FilledPattern::generateNewPath(const SeedPoint &seed_point) {
    vali starting_step = dtoi(desired_pattern.get().getDirector(seed_point.getCoordinates()) * getStepLength());

    Path forward_path = generateNewPathForDirection(seed_point, starting_step);
    Path backward_path = generateNewPathForDirection(seed_point, -starting_step);

    return {forward_path, backward_path};
}


void FilledPattern::fillPointsInCircle(const vali &coordinates) {
    fillPointsFromDisplacement(coordinates, print_circle, {1, 0});
    list_of_points.push_back(coordinates);
}


void FilledPattern::removePoints() {
    for (auto &position: list_of_points) {
        fillPointsFromDisplacement(position, print_circle, {1, 0}, -1);
    }
    list_of_points.clear();
}


void FilledPattern::removeLine(Path path) {
    std::vector<vali> current_points_to_fill;
    vali current_coordinates = path.position(1);
    vali previous_coordinates = path.position(0);
    current_points_to_fill = findPointsToFill(previous_coordinates, current_coordinates, getPrintRadius(),
                                              !isFilled(previous_coordinates));

    vali new_step_int = current_coordinates - previous_coordinates;
    fillPointsFromList(current_points_to_fill, new_step_int, -1);

    for (int i = 2; i < path.size(); i++) {
        current_coordinates = path.position(i);
        previous_coordinates = path.position(i - 1);
        vali second_previous_coordinates = path.position(i - 2);
        current_points_to_fill = findPointsToFill(second_previous_coordinates, previous_coordinates,
                                                  current_coordinates, getPrintRadius(),
                                                  !isFilled(previous_coordinates));

        new_step_int = current_coordinates - previous_coordinates;
        fillPointsFromList(current_points_to_fill, new_step_int, -1);
    }
    fillPointsInHalfCircle(path.position(0), path.position(1), -1);
    fillPointsInHalfCircle(path.last(), path.secondToLast(), -1);
}


void FilledPattern::removeShortLines(double length_coefficient) {
    double minimal_length = length_coefficient * getPrintRadius();
    std::vector<Path> new_sequence_of_paths;

    for (auto &path: sequence_of_paths) {
        if (path.getLength() < minimal_length) {
            removeLine(path);
        } else {
            new_sequence_of_paths.push_back(path);
        }
    }
    sequence_of_paths = new_sequence_of_paths;
}


void
FilledPattern::fillPointsInHalfCircle(const vali &last_point, const vali &previous_point, int value) {
    std::vector<vali> half_circle_points = findHalfCircle(last_point, previous_point, getPrintRadius(),
                                                          isFilled(last_point));
    fillPointsFromList(half_circle_points, previous_point - last_point, value);
}


void FilledPattern::exportFilledMatrix(const fs::path &path) const {
    fs::path filled_filename = path;
    exportVectorTableToFile(number_of_times_filled, filled_filename);
}


std::vector<Path> FilledPattern::getSequenceOfPaths() const {
    return sequence_of_paths;
}


void FilledPattern::addNewPath(Path &new_path) {
    sequence_of_paths.push_back(new_path);
}


vald normalizedDualVector(const vald &vector) {
    return normalize(perpendicular(vector));
}

std::vector<vali> FilledPattern::findDualLineOneDirection(vald coordinates, vald previous_dual_director) {
    veci coordinates_i = valtovec(dtoi(coordinates));
    std::set<veci> line_set;
    vald dual_director = previous_dual_director;
    while (
            isFillable(dtoi(coordinates)) &&
            line_set.find(coordinates_i) == line_set.end() &&
            dot(previous_dual_director, dual_director) > 0
            ) {

        line_set.insert(coordinates_i);
        vald director = desired_pattern.get().getDirector(coordinates);
        dual_director = normalizedDualVector(director);
        if (dot(dual_director, previous_dual_director) < 0) {
            dual_director *= -1;
        }
        coordinates += dual_director;
        coordinates_i = valtovec(dtoi(coordinates));
        previous_dual_director = dual_director;
    }
    std::vector<vali> line;
    line.reserve(line_set.size());
    for (auto &element: line_set) {
        line.push_back(vectoval(element));
    }
    return line;
}


std::vector<vali>
FilledPattern::findLineGeneral(const vali &start, std::vector<vali> (FilledPattern::*line_propagation)(vald, vald)) {
    vald real_coordinates = itod(start);
    vald previous_director = desired_pattern.get().getDirector(real_coordinates);
    vald initial_dual_director = normalizedDualVector(previous_director);

    std::vector<vali> points_in_dual_line_forward;
    std::vector<vali> points_in_dual_line_backward;

    points_in_dual_line_forward = (this->*line_propagation)(real_coordinates, initial_dual_director);
    points_in_dual_line_backward = (this->*line_propagation)(real_coordinates, -initial_dual_director);
    return stitchTwoVectors(points_in_dual_line_backward, points_in_dual_line_forward);
}


std::vector<vali> FilledPattern::findDualLine(const vali &start) {
    return findLineGeneral(start, &FilledPattern::findDualLineOneDirection);
}


matrix_d FilledPattern::getDualTensor(const vali &coordinates) const {
    vald dual_director = normalizedDualVector(desired_pattern.get().getDirector(coordinates));
    return tensor(dual_director, dual_director);
}

double FilledPattern::distance(const vali &first_point, const vali &second_point) {
    vald connecting_vector = itod(second_point - first_point);
    matrix_d first_dual_tensor = getDualTensor(first_point);
    matrix_d second_dual_tensor = getDualTensor(second_point);
    double first_distance = sqrt(dot(connecting_vector, multiply(first_dual_tensor, connecting_vector)));
    double second_distance = sqrt(dot(connecting_vector, multiply(second_dual_tensor, connecting_vector)));

    double dist = 2 * first_distance * second_distance / (first_distance + second_distance);
    return std::max({first_distance, second_distance, dist});
}


void FilledPattern::tryAddingPointToSpacedLine(const vali &current_position, vali &previous_position,
                                               bool &is_filled_coordinate_encountered, double separation,
                                               std::vector<SeedPoint> &separated_starting_points, int line_index,
                                               int point_index) {
    double current_distance = distance(current_position, previous_position);
    if (!isInRange(current_position) ||
        isFilled(current_position)) {
        is_filled_coordinate_encountered = true;
        previous_position = current_position;
    } else if (!is_filled_coordinate_encountered && current_distance >= separation ||
               is_filled_coordinate_encountered && current_distance >= separation / 2) {
        separated_starting_points.emplace_back(current_position, line_index, point_index);
        previous_position = current_position;
        is_filled_coordinate_encountered = false;
    }
}

std::vector<SeedPoint>
FilledPattern::getSpacedLine(const double &separation, const std::vector<vali> &line, int line_index) {
    std::uniform_int_distribution<> index_distribution(0, line.size() - 1);
    int starting_index = index_distribution(random_engine);
    std::vector<SeedPoint> separated_starting_points = {{line[starting_index], line_index, starting_index}};

    bool is_filled_coordinate_encountered = false;

    vali previous_position = line[starting_index];
    for (int i = starting_index + 1; i < line.size(); i++) {
        tryAddingPointToSpacedLine(line[i], previous_position, is_filled_coordinate_encountered, separation,
                                   separated_starting_points, line_index, i);
    }

    is_filled_coordinate_encountered = false;
    previous_position = line[starting_index];
    for (int i = starting_index - 1; i >= 0; i--) {
        tryAddingPointToSpacedLine(line[i], previous_position, is_filled_coordinate_encountered, separation,
                                   separated_starting_points, line_index, i);
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


vali FilledPattern::getFillablePoint() {
    while (!binned_root_points.empty() && binned_root_points.back().empty()) {
        binned_root_points.pop_back();
    }
    if (binned_root_points.empty()) {
        return INVALID_POSITION;
    }
    vali last_element = binned_root_points.back().back();
    binned_root_points.back().pop_back();
    return last_element;
}


bool FilledPattern::isFillablePointLeft() const {
    return !binned_root_points.empty();
}


SeedPoint FilledPattern::findSeedPoint() {
    if (seed_points.empty()) {
        updateSeedPoints();
    }

    SeedPoint last_element = seed_points.back();
    seed_points.pop_back();

    return last_element;
}

#pragma clang diagnostic pop