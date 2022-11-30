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
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 21/09/2021.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc51-cpp"

#include <utility>

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

    number_of_times_filled = std::vector<std::vector<int>>(x_dim, std::vector<int>(y_dim));
    x_field_filled = std::vector<std::vector<double>>(x_dim, std::vector<double>(y_dim));
    y_field_filled = std::vector<std::vector<double>>(x_dim, std::vector<double>(y_dim));
    x_distribution = std::uniform_int_distribution<int>(0, x_dim - 1);
    y_distribution = std::uniform_int_distribution<int>(0, y_dim - 1);
    setup();
}

void FilledPattern::setup() {
    print_circle = findPointsInCircle(getPrintRadius());
    repulsion_circle = findPointsInCircle(getPrintRadius() + getRepulsionRadius());
    points_to_fill = findInitialStartingPoints(getInitialFillingMethod());
    collision_list = generatePerimeterList(getCollisionRadius());
    random_engine = std::mt19937(getSeed());
    unsigned int number_of_fillable_points = points_to_fill.size();
    distribution = std::uniform_int_distribution<unsigned int>(0, number_of_fillable_points - 1);
}


FilledPattern::FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius,
                             int step_length, unsigned int seed) :
        FilledPattern(desired_pattern,
                      FillingConfig(RandomPerimeter, collision_radius, 2 * print_radius, 1.0, step_length, print_radius,
                                    0,
                                    seed)) {
}


FilledPattern::FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius,
                             int step_length)
        :
        FilledPattern::FilledPattern(desired_pattern, print_radius, collision_radius, step_length, 0) {}


std::vector<std::valarray<int>> FilledPattern::findAllFillablePoints() const {
    std::vector<std::valarray<int>> new_points_to_fill;
    for (int i = 0; i < desired_pattern.get().getDimensions()[0]; i++) {
        for (int j = 0; j < desired_pattern.get().getDimensions()[1]; j++) {
            std::valarray<int> current_pos = {i, j};
            if (isPerimeterFree(number_of_times_filled, desired_pattern.get().getShapeMatrix(), collision_list,
                                current_pos, desired_pattern.get().getDimensions())) {
                new_points_to_fill.emplace_back(current_pos);
            }
        }
    }
    return new_points_to_fill;
}


std::vector<std::valarray<int>>
FilledPattern::findRemainingFillablePointsInList(std::vector<std::valarray<int>> &list_of_points) const {
    std::vector<std::valarray<int>> fillable_points_list;
    for (auto &point: list_of_points) {
        if (isPerimeterFree(number_of_times_filled, desired_pattern.get().getShapeMatrix(), collision_list,
                            point, desired_pattern.get().getDimensions())) {
            fillable_points_list.push_back(point);
        }
    }
    return fillable_points_list;
}


void FilledPattern::updateSearchStageAndFillablePoints() {
    switch (search_stage) {
        case PerimeterSearch:
            points_to_fill = findRemainingFillablePointsInList(points_to_fill);
            if (points_to_fill.empty()) {
                search_stage = FullyRandomPointSelection;
            }
            break;
        case FullyRandomPointSelection:
            points_to_fill = findAllFillablePoints();
            search_stage = EmptySpotRandomSelection;
            break;
        case EmptySpotRandomSelection:
            points_to_fill = findRemainingFillablePointsInList(points_to_fill);
            break;
    }

    unsigned int number_of_fillable_points = points_to_fill.size();
    distribution = std::uniform_int_distribution<unsigned int>(0, number_of_fillable_points - 1);
}


std::valarray<double> normalizeDirection(const std::valarray<int> &previous_step) {
    std::valarray<double> normalized_direction = normalize(previous_step);
    if (previous_step[0] > 0 || previous_step[0] == 0 && previous_step[1] > 0) {
        return normalized_direction;
    } else {
        return -normalized_direction;
    }
}


void FilledPattern::fillPoint(const std::valarray<int> &point, const std::valarray<double> &normalized_direction) {
    number_of_times_filled[point[0]][point[1]] += 1;
    x_field_filled[point[0]][point[1]] += normalized_direction[0];
    y_field_filled[point[0]][point[1]] += normalized_direction[1];
}


void FilledPattern::fillPointsFromList(const std::vector<std::valarray<int>> &list_of_points,
                                       const std::valarray<int> &direction) {
    std::valarray<double> normalized_direction = normalizeDirection(direction);
    for (auto &point: list_of_points) {
        fillPoint(point, normalized_direction);
    }
}


void FilledPattern::fillPointsFromDisplacement(const std::valarray<int> &starting_position,
                                               const std::vector<std::valarray<int>> &list_of_displacements,
                                               const std::valarray<int> &previous_step) {
    std::valarray<double> normalized_direction = normalizeDirection(previous_step);
    for (auto &displacement: list_of_displacements) {
        std::valarray<int> point = starting_position + displacement;
        if (point[0] >= 0 && point[0] < desired_pattern.get().getDimensions()[0] && point[1] >= 0 &&
            point[1] < desired_pattern.get().getDimensions()[1]) {
            fillPoint(point, normalized_direction);
        }
    }
}


std::valarray<double> FilledPattern::getNewStep(std::valarray<double> &real_coordinates, int &length,
                                                std::valarray<double> &previous_move) const {
    std::valarray<double> new_move = desired_pattern.get().preferredDirection(real_coordinates, length);
    int is_opposite_to_previous_step = sgn(new_move[0] * previous_move[0] + new_move[1] * previous_move[1]);
    if (is_opposite_to_previous_step < 0) {
        new_move = -new_move;
    }
    return new_move;
}


bool FilledPattern::tryGeneratingPathWithLength(Path &current_path, std::valarray<double> &positions,
                                                std::valarray<double> &previous_step, int length) {
    std::valarray<int> current_coordinates = dtoi(positions);
    std::valarray<double> new_step = getNewStep(positions, length, previous_step);
    std::valarray<double> new_positions = positions + new_step;
    std::valarray<int> new_coordinates = dtoi(new_positions);
    std::valarray<double> repulsion = {0, 0};
    if (getRepulsion() != 0) {
        repulsion = getRepulsionValue(number_of_times_filled, repulsion_circle, new_coordinates,
                                      desired_pattern.get().getDimensions(), getRepulsion());
    }

    new_positions -= repulsion;
    new_coordinates = dtoi(new_positions);
    std::valarray<double> real_step = new_step - repulsion;

    if (new_coordinates[0] == current_coordinates[0] && new_coordinates[1] == current_coordinates[1] ||
        dot(real_step, previous_step) <= 0 || norm(real_step) <= 2) {
        return false;
    }

    if (isPerimeterFree(number_of_times_filled, desired_pattern.get().getShapeMatrix(), collision_list, new_coordinates,
                        desired_pattern.get().getDimensions())) {
        std::vector<std::valarray<int>> current_points_to_fill;
        if (current_path.size() >= 2) {
            current_points_to_fill = findPointsToFill(current_path.secondToLast(), current_coordinates,
                                                      new_coordinates, getPrintRadius(), isFilled(current_coordinates));
        } else {
            current_points_to_fill = findPointsToFill(current_coordinates, new_coordinates, getPrintRadius(),
                                                      isFilled(current_coordinates));
        }
        std::valarray<int> new_step_int = new_coordinates - current_coordinates;
        fillPointsFromList(current_points_to_fill, new_step_int);
        current_path.addPoint(new_coordinates);

        new_step = getNewStep(new_positions, length, new_step);
        positions = new_positions;
        previous_step = new_step;
        return true;
    }
    return false;
}

Path FilledPattern::generateNewPathForDirection(std::valarray<int> &starting_coordinates,
                                                const std::valarray<int> &starting_step) {
    Path new_path(starting_coordinates);
    std::valarray<double> current_positions = itod(starting_coordinates);
    std::valarray<double> current_step = itod(starting_step);

    for (int length = getStepLength(); length >= getPrintRadius(); length--) {
        while (tryGeneratingPathWithLength(new_path, current_positions, current_step, length)) {}
    }
    return new_path;
}

void FilledPattern::fillPointsInCircle(const std::valarray<int> &starting_coordinates) {
    fillPointsFromDisplacement(starting_coordinates, print_circle, {1, 0});
}

void
FilledPattern::fillPointsInHalfCircle(const std::valarray<int> &last_point, const std::valarray<int> &previous_point) {
    std::vector<std::valarray<int>> half_circle_points = findHalfCircle(last_point, previous_point, getPrintRadius(),
                                                                        isFilled(last_point));
    fillPointsFromList(half_circle_points, previous_point - last_point);
}

void FilledPattern::exportToDirectory(const fs::path &directory) const {
    fs::path filled_filename = directory / "number_of_times_filled.csv";
    fs::path x_field_filename = directory / "x_field.csv";
    fs::path y_field_filename = directory / "y_field.csv";
    exportVectorTableToFile(number_of_times_filled, filled_filename);
}

std::vector<Path> FilledPattern::getSequenceOfPaths() {
    return sequence_of_paths;
}

void FilledPattern::addNewPath(Path &new_path) {
    sequence_of_paths.push_back(new_path);
}

unsigned int FilledPattern::getNewElement() {
    return distribution(random_engine);
}


std::vector<std::valarray<int>>
reshuffle(const std::vector<std::valarray<int>> &initial_vector, std::mt19937 &random_engine) {
    std::vector<std::valarray<int>> new_vector(initial_vector.size());
    std::uniform_int_distribution<unsigned int> distribution(0, initial_vector.size() - 1);
    unsigned int elements_to_push = distribution(random_engine);
    for (int i = 0; i < initial_vector.size(); i++) {
        new_vector[i] = initial_vector[(i + elements_to_push) % initial_vector.size()];
    }
    return new_vector;
}


std::valarray<double> FilledPattern::getDirector(const std::valarray<int> &positions) const {
    return std::valarray<double>({desired_pattern.get().getXFieldPreferred()[positions[0]][positions[1]],
                                  desired_pattern.get().getYFieldPreferred()[positions[0]][positions[1]]});
}


std::valarray<double> FilledPattern::getDirector(const std::valarray<double> &positions) {
    int x_base = (int) positions[0];
    int y_base = (int) positions[1];
    double x_fraction = positions[0] - floor(positions[0]);
    double y_fraction = positions[1] - floor(positions[1]);

    std::valarray<double> director = {0, 0};
    director += x_fraction * y_fraction * getDirector(std::valarray<int>{x_base, y_base});
    director += (1 - x_fraction) * y_fraction * getDirector(std::valarray<int>{x_base + 1, y_base});
    director += (1 - x_fraction) * (1 - y_fraction) * getDirector(std::valarray<int>{x_base + 1, y_base + 1});
    director += x_fraction * (1 - y_fraction) * getDirector(std::valarray<int>{x_base, y_base + 1});

    return director;
}


std::valarray<double> normalizedDualVector(const std::valarray<double> &vector) {
    return normalize(perpendicular(vector));
}


std::vector<std::valarray<int>>
FilledPattern::findDualLineOneDirection(std::valarray<double> coordinates,
                                        std::valarray<double> previous_dual_director) {
    std::vector<std::valarray<int>> line;
    while (desired_pattern.get().isInShape(coordinates)) {
        line.push_back(dtoi(coordinates));
        std::valarray<double> director = getDirector(coordinates);
        std::valarray<double> dual_director = normalizedDualVector(director);
        if (dot(dual_director, previous_dual_director) < 0) {
            dual_director *= -1;
        }
        coordinates += dual_director;
        previous_dual_director = dual_director;
    }
    return line;
}

std::vector<std::valarray<int>> FilledPattern::findDualLine(const std::valarray<int> &start) {
    std::valarray<double> real_coordinates = itod(start);
    std::valarray<double> previous_director = getDirector(real_coordinates);
    std::valarray<double> initial_dual_director = normalizedDualVector(previous_director);

    std::vector<std::valarray<int>> points_in_dual_line_forward;
    std::vector<std::valarray<int>> points_in_dual_line_backward;

    points_in_dual_line_forward = findDualLineOneDirection(real_coordinates, initial_dual_director);
    points_in_dual_line_backward = findDualLineOneDirection(real_coordinates, -initial_dual_director);

    return stitchTwoVectors(points_in_dual_line_backward, points_in_dual_line_forward);
}


std::vector<std::valarray<int>>
FilledPattern::getSpacedLine(const double &distance, const std::vector<std::valarray<int>> &line) {
    std::vector<std::valarray<int>> reshuffled_starting_points = reshuffle(line, random_engine);

    std::valarray<int> previous_position = reshuffled_starting_points[0];
    std::valarray<double> previous_director = getDirector(previous_position);
    std::valarray<double> previous_double_director = normalizedDualVector(previous_director);

    std::vector<std::valarray<int>> separated_starting_points;
    separated_starting_points.push_back(previous_position);

    for (auto &current_position: reshuffled_starting_points) {
        double current_distance = abs(dot(itod(current_position - previous_position), previous_double_director));
        if (current_distance > distance) {
            previous_position = current_position;
            previous_director = getDirector(current_position);
            previous_double_director = normalizedDualVector(previous_director);
            separated_starting_points.push_back(previous_position);
        }
    }
    return separated_starting_points;
}


std::valarray<int> FilledPattern::findPointInShape() {
    int x_start = x_distribution(random_engine);
    int y_start = y_distribution(random_engine);
    while (!desired_pattern.get().getShapeMatrix()[x_start][y_start]) {
        x_start = x_distribution(random_engine);
        y_start = y_distribution(random_engine);
    }
    return std::valarray<int>{x_start, y_start};
}


std::vector<std::valarray<int>> FilledPattern::findInitialStartingPoints(fillingMethod method) {
    std::vector<std::valarray<int>> starting_points;
    std::vector<std::valarray<int>> temp_starting_points;
    std::vector<std::valarray<int>> dual_line;

    switch (method) {
        case RandomPerimeter:
            starting_points = getSpacedLine(getStartingPointSeparation(), desired_pattern.get().getPerimeterList());
            is_filling_method_random = true;
            break;
        case ConsecutivePerimeter:
            starting_points = getSpacedLine(getStartingPointSeparation(), desired_pattern.get().getPerimeterList());
            is_filling_method_random = false;
            break;
        case RandomRadial:
            dual_line = findDualLine(findPointInShape());
            starting_points = getSpacedLine(getStartingPointSeparation(), dual_line);
            is_filling_method_random = true;
            break;
        case ConsecutiveRadial:
            dual_line = findDualLine(findPointInShape());
            starting_points = getSpacedLine(getStartingPointSeparation(), dual_line);
            is_filling_method_random = false;
            break;
    }
    return starting_points;
}

bool FilledPattern::isFilled(const std::valarray<int> &coordinates) {
    return number_of_times_filled[coordinates[0]][coordinates[1]];
}


#pragma clang diagnostic pop