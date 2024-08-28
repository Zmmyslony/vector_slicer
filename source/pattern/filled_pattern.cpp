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

#include <iostream>
#include "filled_pattern.h"

#include "importing_and_exporting/exporting.h"
#include "auxiliary/geometry.h"
#include "auxiliary/perimeter.h"
#include "auxiliary/repulsion.h"
#include "auxiliary/valarray_operations.h"
#include "auxiliary/vector_operations.h"
#include "auxiliary/line_operations.h"

#define INVALID_POSITION {-1, -1}
/// Minimal value of cosine between current and previous director for it to be assumed as continuous.
#define DIRECTOR_DISCONTINUITY_THRESHOLD 0

bool isValid(const vali &positions) {
    return positions[0] >= 0 && positions[1] >= 0;
}

bool isValid(const vald &positions) {
    return positions[0] >= 0 && positions[1] >= 0;
}

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
        std::vector<SeedPoint> spaced_line = getSpacedLine(line, line_index, 0);
        if (is_random_filling_enabled) {
            std::shuffle(spaced_line.begin(), spaced_line.end(), random_engine);
        }
        separated_lines.emplace_back(spaced_line);
        line_index++;
    }
    return separated_lines;
}

/// Extends the seed lines by SeedSeparation in the dual direction, so there is an overlap between different seed lines
/// which will make the inter-seed-line spacing consistent.
void FilledPattern::extendSeedLines() {
    for (std::vector<vali> &seed_line: seed_lines) {
        if (isLooped(seed_line)) { continue; }

        vald front_dual = perpendicular(getDirector(seed_line.front()));
        vald previous_displacement = itod(seed_line.front() - seed_line[3]);
        if (dot(front_dual, previous_displacement) < 0) { front_dual *= -1; }

        vald back_dual = perpendicular(getDirector(seed_line.back()));
        previous_displacement = itod(seed_line.back() - seed_line[seed_line.size() - 4]);
        if (dot(back_dual, previous_displacement) < 0) { back_dual *= -1; }

        std::vector<vali> front_displacements = pixeliseLine(front_dual * getSeedSpacing());
        std::vector<vali> back_displacements = pixeliseLine(back_dual * getSeedSpacing());

        vali front = seed_line.front();
        for (auto &displacement: front_displacements) {
            vali current = front + displacement;
            if (desired_pattern.get().isInShape(current)) {
                seed_line.insert(seed_line.begin(), current);
            }
        }

        vali back = seed_line.back();
        for (auto &displacement: back_displacements) {
            vali current = back + displacement;
            if (desired_pattern.get().isInShape(current)) {
                seed_line.emplace_back(current);
            }
        }
    }
}


void FilledPattern::setup() {
    print_circle = findPointsInCircle(getPrintRadius());
    collision_list = circleDisplacements(getTerminationRadius());
    random_engine = std::mt19937(getSeed());

    switch (getInitialSeedingMethod()) {
        case Splay:
            if (desired_pattern.get().isSplayProvided()) {
                search_stage = SplayFilling;
                seed_lines = desired_pattern.get().getLineDensityMinima();
                extendSeedLines();
            } else {
                search_stage = RemainingFilling;
                seed_lines = desired_pattern.get().getPerimeterList();
            }
            break;
        case Perimeter:
            search_stage = PerimeterFilling;
            seed_lines = desired_pattern.get().getPerimeterList();
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

/// Finds index of overlapping seed line.
std::vector<unsigned int> FilledPattern::findOverlappingSeedLines() {
    std::vector<unsigned int> overlapping_indices;
    for (int i = 0; i < seed_lines.size(); i++) {
        bool is_overlapping = false;
        for (auto &coordinate: seed_lines[i]) {
            is_overlapping |= isFilled(coordinate);
        }
        if (is_overlapping) {
            overlapping_indices.emplace_back(i);
        }
    }
    return overlapping_indices;
}

std::vector<SeedPoint> FilledPattern::getSeedsFromRandomSeedLine() {
    std::uniform_int_distribution<> distribution(0, seed_lines.size() - 1);
    int i = distribution(random_engine);
    std::vector<vali> current_seed_line = seed_lines[i];
    seed_lines.erase(seed_lines.begin() + i);

    return getSpacedLineRandom(current_seed_line, current_seed_line_index);
}

std::vector<SeedPoint>
FilledPattern::getSeedsFromOverlappingSeedLine(const std::vector<unsigned int> &overlapping_indexes) {
    std::uniform_int_distribution<> distribution(0, overlapping_indexes.size() - 1);
    int i = overlapping_indexes[distribution(random_engine)];
    std::vector<vali> current_seed_line = seed_lines[i];
    seed_lines.erase(seed_lines.begin() + i);

    return getSpacedLineOverlapping(current_seed_line, current_seed_line_index);
}

void FilledPattern::updateSeedPoints() {
    if (!seed_lines.empty()) {
        std::vector<unsigned int> overlapping_seed_lines = findOverlappingSeedLines();
        if (overlapping_seed_lines.empty()) {
            seed_points = getSeedsFromRandomSeedLine();
        } else {
            seed_points = getSeedsFromOverlappingSeedLine(overlapping_seed_lines);
        }
        if (is_random_filling_enabled) {
            std::shuffle(seed_points.begin(), seed_points.end(), random_engine);
        }
        current_seed_line_index++;
        return;
    }


//    if (search_stage == SplayFilling) {
//        if (zero_splay_seeds.empty()) {
//            search_stage = RemainingFilling;
//        } else {
//            seed_points = zero_splay_seeds.back();
//            zero_splay_seeds.pop_back();
//            return;
//        }
//    }
//
//    if (search_stage == PerimeterFilling) {
//        if (perimeter_seeds.empty()) {
//            search_stage = RemainingFilling;
//        } else {
//            seed_points = perimeter_seeds.back();
//            perimeter_seeds.pop_back();
//            return;
//        }
//    }

    if (is_reseeding_enabled) {
        vali root_point = findRemainingRootPoint();
        if (root_point[0] == -1) {
            seed_points = {INVALID_SEED};
            return;
        }
        std::vector<vali> dual_line = findDualLine(root_point);
        std::vector<SeedPoint> spaced_dual_line = getSpacedLine(dual_line, current_seed_line_index,
                                                                (int)getPrintRadius());
        current_seed_line_index++;
        if (is_random_filling_enabled) {
            std::shuffle(spaced_dual_line.begin(), spaced_dual_line.end(), random_engine);
        }
        seed_points = spaced_dual_line;
    } else {
        seed_points = {INVALID_SEED};
        return;
    }
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


vald FilledPattern::getDirector(const vald &coordinates) const {
    return desired_pattern.get().getDirector(coordinates);
}

vald FilledPattern::getDirector(const vali &coordinates) const {
    return desired_pattern.get().getDirector(coordinates);
}

vald FilledPattern::getNewStep(vald &real_coordinates, int &length, vald &previous_move) const {
    vald new_move = getDirector(real_coordinates) * length;
    double is_opposite_to_previous_step = dot(new_move, previous_move);

    if (is_opposite_to_previous_step >= 0) {
        return new_move;
    } else {
        return -new_move;
    }
}

bool isUnchanged(const vali &first_position, const vali &second_position) {
    return first_position[0] == second_position[0] && first_position[1] == second_position[1];
}

bool isReversed(const vald &first_step, const vald &second_step) {
    return dot(first_step, second_step) <= 0;
}

vald FilledPattern::calculateNextPosition(vald &positions, vald &previous_step, int length) {
    vali current_coordinates = dtoi(positions);
    vald new_step = getNewStep(positions, length, previous_step);
    vald new_positions = positions + new_step;

    if (getRepulsion() > 0) {
        vald repulsion = getLineBasedRepulsion(desired_pattern.get().getShapeMatrix(), number_of_times_filled,
                                               new_step, getPrintRadius(),
                                               new_positions, desired_pattern.get().getDimensions(),
                                               getRepulsion(), getRepulsionAngle());
        new_positions += repulsion;
        new_step += repulsion;
    }

    vali new_coordinates = dtoi(new_positions);
    // Check if newly generated position is valid
    if (isTerminable(new_coordinates, new_step)) {
        return INVALID_POSITION;
    } else {
        return new_positions;
    }
}

bool FilledPattern::isInRange(const vali &index) const {
    return ::isInRange(index, desired_pattern.get().getDimensions());
}

bool FilledPattern::isInRange(const vald &index) const {
    return ::isInRange(dtoi(index), desired_pattern.get().getDimensions());
}

bool FilledPattern::isDirectorContinuous(const vali &previous_coordinates, const vali &new_coordinates) const {
    vald previous_director = getDirector(previous_coordinates);
    vald new_director = getDirector(new_coordinates);
    double product = dot(previous_director, new_director) / (norm(previous_director) * norm(new_director));
    if (!desired_pattern.get().isVectorFilled()) { product = fabs(product); }

    return product >= desired_pattern.get().getDiscontinuityThresholdCos();
}

/// Returns bool depending whether propagation was successful.
bool FilledPattern::propagatePath(Path &current_path, vald &positions, vald &previous_step, int length) {
    vali current_coordinates = dtoi(positions);
    if (!isInRange(current_coordinates)) { return false; }

    // Try creating the longest possible step
    vald new_positions = INVALID_POSITION;
    vald potential_positions = INVALID_POSITION;
    /// Keeps last valid position, even if it is discontinuous.
    vald previous_positions = INVALID_POSITION;

    while (--length > 0 && !isValid(new_positions)) {
        previous_positions = potential_positions;
        potential_positions = calculateNextPosition(positions, previous_step, length);

        if (isValid(potential_positions)) {
            switch (desired_pattern.get().getDiscontinuityBehaviour()) {
                case DISCONTINUITY_IGNORE:
                    new_positions = potential_positions;
                    break;
                case DISCONTINUITY_STICK:
                    if (isDirectorContinuous(current_coordinates, dtoi(potential_positions))) {
                        if (isValid(previous_positions)) {
                            new_positions = previous_positions;
                        } else {
                            new_positions = potential_positions;
                        }
                    }
                    break;
                case DISCONTINUITY_TERMINATE:
                    if (isDirectorContinuous(current_coordinates, dtoi(potential_positions))) {
                        new_positions = potential_positions;
                    }
                    break;
            }
        }
    }

    if (!isValid(new_positions)) {
        return false;
    }

    previous_step = new_positions - positions;
    positions = new_positions;
    vali new_coordinates = dtoi(new_positions);

    vali new_step_int = new_coordinates - current_coordinates;
    vald tangent = normalisedResultant(previous_step, getDirector(new_coordinates));
    vald normal = perpendicular(tangent) * getPrintRadius();

    current_path.addPoint(new_coordinates, new_positions + normal, new_positions - normal);
    std::vector<vali> current_points_to_fill = current_path.findPointsToFill(isFilled(current_coordinates));
    fillPointsFromList(current_points_to_fill, new_step_int, 1);
    return true;
}

double FilledPattern::getOverlap(const std::vector<vali> &points_to_check) {
    int points_count = points_to_check.size();
    int overlap = 0;
    for (auto &point: points_to_check) {
        if (isInRange(point)) {
            overlap += number_of_times_filled[point[0]][point[1]] - 1;
        }
    }
    return (double) overlap / (double) points_count;
}

void FilledPattern::updatePathOverlap(Path &path) {
    std::vector<double> overlap_array;
    overlap_array.reserve(path.size());
    std::vector<vali> first_segment = path.findPointsToFill(1, false);

    double current_edge_overlap = getOverlap(first_segment);
    // Overlap is defined on edges, while paths are defined on nodes, so first and last nodes will have overlaps
    // corresponding to first and last edges, while the middle one will have n[i] = (e[i] + e[i - 1]) / 2
    overlap_array.emplace_back(current_edge_overlap);
    for (int i = 2; i < path.size(); i++) {
        double previous_edge_overlap = current_edge_overlap;
        std::vector<vali> current_segment = path.findPointsToFill(i, false);

        current_edge_overlap = getOverlap(current_segment);
        overlap_array.emplace_back((previous_edge_overlap + current_edge_overlap) / 2);
    }
    overlap_array.emplace_back(current_edge_overlap);
    path.setOverlap(overlap_array);
}

void FilledPattern::updatePathsOverlap() {
    for (auto &path: sequence_of_paths) {
        updatePathOverlap(path);
    }
}

Path FilledPattern::generateNewPathForDirection(const SeedPoint &seed_point, const vald &starting_step) {
    Path path(seed_point, getPrintRadius());
    vald current_positions = itod(seed_point.getCoordinates());
    vald current_step = starting_step;

    while (propagatePath(path, current_positions, current_step, getStepLength())) {}

    return path;
}


Path FilledPattern::generateNewPath(const SeedPoint &seed_point) {
    vald starting_step = getDirector(seed_point.getCoordinates());

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
    current_points_to_fill = path.findPointsToFill(1, !isFilled(previous_coordinates));

    vali new_step_int = current_coordinates - previous_coordinates;
    fillPointsFromList(current_points_to_fill, new_step_int, -1);

    for (int i = 2; i < path.size(); i++) {
        current_coordinates = path.position(i);
        previous_coordinates = path.position(i - 1);
        vali second_previous_coordinates = path.position(i - 2);
        current_points_to_fill = path.findPointsToFill(i, !isFilled(previous_coordinates));

        new_step_int = current_coordinates - previous_coordinates;
        fillPointsFromList(current_points_to_fill, new_step_int, -1);
    }
    fillPointsInHalfCircle(path, -1, true);
    fillPointsInHalfCircle(path, -1, false);
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
FilledPattern::fillPointsInHalfCircle(const vali &last_coordinate, const vali &previous_coordinate, int value) {
    std::vector<vali> half_circle_points = findHalfCircleCentres(last_coordinate, previous_coordinate, getPrintRadius(),
                                                                 isFilled(last_coordinate),
                                                                 getDirector(last_coordinate));
    fillPointsFromList(half_circle_points, last_coordinate - previous_coordinate, value);
}

void
FilledPattern::fillPointsInHalfCircle(const Path &path, int value, bool is_front) {
    vali last_coordinate = path.last();
    vali previous_coordinate = path.secondToLast();
    vald corner_first = path.getNegativePathEdge().back();
    vald corner_second = path.getPositivePathEdge().back();
    if (is_front) {
        last_coordinate = path.first();
        double distance = 0;
        int i = 0;
        while (distance == 0 && i != path.size()) {
            previous_coordinate = path.getPositionSequence()[i++];
            distance = norm(last_coordinate - previous_coordinate);
        }
        corner_first = path.getNegativePathEdge().front();
        corner_second = path.getPositivePathEdge().front();
    } else {
        double distance = 0;
        int i = path.size();
        while (distance == 0 && i != 0) {
            previous_coordinate = path.getPositionSequence()[--i];
            distance = norm(last_coordinate - previous_coordinate);
        }
    }

    vald last_move_direction = itod(last_coordinate - previous_coordinate);

    std::vector<vali> half_circle_points = findHalfCircleEdges(last_coordinate, corner_first, corner_second,
                                                               getPrintRadius(),
                                                               isFilled(last_coordinate),
                                                               last_move_direction);
    fillPointsFromList(half_circle_points, last_coordinate - previous_coordinate, value);
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

bool isEqual(const vali &first, const vali &second) {
    return first[0] == second[0] && first[1] == second[1];
}

vali coord_to_vali(const coord &x) {
    return vali{x.first, x.second};
}

vald coord_to_vald(const coord &x) {
    return itod(coord_to_vali(x));
}

coord vali_to_coord(const vali &x) {
    return {x[0], x[1]};
}

coord vald_to_coord(const vald &x) {
    return vali_to_coord(dtoi(x));
}


void
FilledPattern::updateDualLineInDirection(coord_set &line_elements, coord_vector &line, vald previous_dual_director) {
    coord current_coord = line.back();
    vald position = coord_to_vald(current_coord);

    while (
            isFillable(coord_to_vali(current_coord)) &&
            (line_elements.find(current_coord) == line_elements.end() ||
             (line.back() == current_coord))
            ) {
        line_elements.insert(current_coord);
        // Avoid doubling the entries.
        if (line.empty() || current_coord != line.back()) {
            line.emplace_back(current_coord);
        }
        vald dual_director = normalizedDualVector(getDirector(position));
        if (dot(dual_director, previous_dual_director) < 0) {
            dual_director *= -1;
        }
        previous_dual_director = dual_director;
        position += dual_director;
        current_coord = vald_to_coord(position);
    }
}

std::vector<vali> FilledPattern::findDualLine(const vali &start) {
    coord_set line_elements = {vali_to_coord(start)};
    coord_vector line_sequence = {vali_to_coord(start)};
    vald starting_dual_director = normalizedDualVector(getDirector(start));

    updateDualLineInDirection(line_elements, line_sequence, starting_dual_director);
    std::reverse(line_sequence.begin(), line_sequence.end());
    updateDualLineInDirection(line_elements, line_sequence, -starting_dual_director);

    std::vector<vali> line_vector;
    line_vector.reserve(line_sequence.size());
    for (auto &coordinate: line_sequence) {
        line_vector.emplace_back(coord_to_vali(coordinate));
    }

    return line_vector;
}


matrix_d FilledPattern::getDualTensor(const vali &coordinates) const {
    vald dual_director = normalizedDualVector(getDirector(coordinates));
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
        separated_starting_points.emplace_back(current_position, getDirector(current_position), line_index,
                                               point_index);
        previous_position = current_position;
        is_filled_coordinate_encountered = false;
    }
}

std::vector<SeedPoint> FilledPattern::getSpacedLineRandom(const std::vector<vali> &line, int line_index) {
    std::uniform_int_distribution<> index_distribution(0, line.size() - 1);
    int starting_index = index_distribution(random_engine);
    return getSpacedLine(line, line_index, starting_index);
}

std::vector<SeedPoint> FilledPattern::getSpacedLineOverlapping(const std::vector<vali> &line, int line_index) {
    std::vector<int> i_overlapping;
    for (int i = 0; i < line.size(); i++) {
        if (isFilled(line[i])) {
            i_overlapping.emplace_back(i);
        }
    }
    if (i_overlapping.empty()) {
        throw std::runtime_error(
                "No overlapping coordinates were found during spacing the path. This should not happen.");
    }
    std::uniform_int_distribution<> index_distribution(0, i_overlapping.size() - 1);
    int starting_index = i_overlapping[index_distribution(random_engine)];
    return getSpacedLine(line, line_index, starting_index);
}


std::vector<SeedPoint> FilledPattern::getSpacedLine(const std::vector<vali> &line, int line_index, int starting_index) {
    double separation = getSeedSpacing();
    std::vector<SeedPoint> separated_starting_points = {{line[starting_index], getDirector(line[starting_index]),
                                                         line_index, starting_index}};
    bool is_looped = isLooped(line);
    bool is_filled_coordinate_encountered = false;

    vali previous_position = line[starting_index];
    for (int i = starting_index + 1; i < line.size(); i++) {
        tryAddingPointToSpacedLine(line[i], previous_position, is_filled_coordinate_encountered, separation,
                                   separated_starting_points, line_index, i);
    }

    if (is_looped) {
        for (int i = 0; i < starting_index; i++) {
            tryAddingPointToSpacedLine(line[i], previous_position, is_filled_coordinate_encountered, separation,
                                       separated_starting_points, line_index, i);
        }
    } else {
        std::reverse(separated_starting_points.begin(), separated_starting_points.end());

        is_filled_coordinate_encountered = false;
        previous_position = line[starting_index];
        for (int i = starting_index - 1; i >= 0; i--) {
            tryAddingPointToSpacedLine(line[i], previous_position, is_filled_coordinate_encountered, separation,
                                       separated_starting_points, line_index, i);
        }
    }
    return separated_starting_points;
}


bool FilledPattern::isFilled(const vali &coordinates) const {
    if (desired_pattern.get().isInShape(coordinates)) {
        return number_of_times_filled[coordinates[0]][coordinates[1]];
    } else {
        return true;
    }
}


bool FilledPattern::isFillable(const vali &point) const {
    return desired_pattern.get().isInShape(point) &&
           !isFilled(point) &&
           isPerimeterFree(number_of_times_filled, desired_pattern.get().getShapeMatrix(),
                           collision_list, point, desired_pattern.get().getDimensions());
}

/// Tests if the coordinate is within the pattern and unfilled.
bool FilledPattern::isFree(const vali &coordinate) const {
    return desired_pattern.get().isInShape(coordinate) && !isFilled(coordinate);
}

bool FilledPattern::isTerminable(const vali &coordinate, const vald &direction) {
    if (getTerminationRadius() <= 0) {
        return !isFree(coordinate);
    }
    vald tangent = normalize(direction) * getTerminationRadius();
    vald normal = perpendicular(tangent);

    for (auto &displacement: collision_list) {
        if (isLeftOfEdge(itod(displacement), normal, -normal, true) &&
            !isFree(coordinate + displacement)) {
            return true;
        }
    }
    return false;
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

std::vector<coord> FilledPattern::getSeedCoordinates() {
    std::vector<coord> seed_coordinates;
    seed_coordinates.reserve(sequence_of_paths.size());
    for (auto &path: sequence_of_paths) {
        vali coordinates_vali = path.getSeedPoint().getCoordinates();
        seed_coordinates.emplace_back(coordinates_vali[0], coordinates_vali[1]);
    }
    return seed_coordinates;
}

#pragma clang diagnostic pop